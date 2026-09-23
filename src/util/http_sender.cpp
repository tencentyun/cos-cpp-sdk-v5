// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/18/17
// Description:

#include "util/http_sender.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>

#include "Poco/DigestStream.h"
#include "Poco/MD5Engine.h"
#include "Poco/Net/Context.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/NetException.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"
#if !defined(_WIN32)
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif

#include "connection_pool.h"
#include "cos_config.h"
#include "cos_defines.h"
#include "cos_sys_config.h"
#include "util/codec_util.h"
#include "util/string_util.h"

namespace qcloud_cos {
namespace {

// 拼接path_query字符串
std::string BuildRequestPathAndQueryParams(const Poco::URI& uri, const std::map<std::string, std::string>& req_params) {
  std::string path = uri.getPath();
  if (path.empty()) {
    path += "/";
  }

  std::string query_str;
  bool first = true;
  for (const auto & req_param : req_params) {
    if (!first) {
      query_str += "&";
    }
    first = false;
    query_str += CodecUtil::UrlEncode(req_param.first);
    if (!req_param.second.empty()) {
      query_str += "=" + CodecUtil::UrlEncode(req_param.second);
    }
  }

  if (!query_str.empty()) {
    query_str = "?" + query_str;
  }
  return CodecUtil::EncodeKey(path) + query_str;
}

// 获取响应中body长度, 没有返回-1
int64_t GetResponseContentLength(const std::map<std::string, std::string>* resp_headers) {
  auto it = resp_headers->find("Content-Length");
  if (it != resp_headers->end() && !it->second.empty()) {
    return StringUtil::StringToUint64(it->second);
  }
  return -1;
}

void LogResponseMessage(const std::map<std::string, std::string>* resp_headers, const int& status_code,
  const Poco::Net::HTTPResponse& resp, const std::string err_msg) {
  std::ostringstream oss;
  oss << "response header :\n";
  for (const auto& resp_header : *resp_headers) {
      oss << resp_header.first << ": " << resp_header.second << "\n";
  }

  oss << "Send request over, ret=" << status_code << ", status_code=" << resp.getStatus() << ", reason=" << resp.getReason();
  if (!err_msg.empty()) {
      oss << ", error_message=" << err_msg;
  }
  SDK_LOG_DBG("%s", oss.str().c_str());
}

// 大于100KB才计算速率
void PrintRate(std::chrono::time_point<std::chrono::steady_clock> start_ts,
    std::chrono::time_point<std::chrono::steady_clock> end_ts, std::streamsize copy_size, const std::string& action) {
    int64_t time_consumed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_ts - start_ts).count();
    if (time_consumed_ms > 1 && copy_size > 100 * 1024) {
        float rate = ((float)copy_size / 1024 / 1024) / ((float)time_consumed_ms / 1000);
        SDK_LOG_DBG("%s_size: %" PRIu64 ", time_consumed: %" PRIu64 " ms, rate: %.2f MB/s", action.c_str(), copy_size,
            time_consumed_ms, rate);
    }
}

// GET 请求时, 如果头域 content-length 不为0, 则检查是否与实际接收长度一致
int CheckResponseBodyLength(const std::string& http_method, const int64_t expect_length, const int64_t actual_length, std::string* err_msg) {
  if (http_method != "GET") {
    return 0;
  }
  if (expect_length <= 0) {
    return 0;
  }
  SDK_LOG_DBG("Check response body length, content_length=%ld, actual_length=%ld", expect_length, actual_length);
  if (expect_length != actual_length) {
    *err_msg = "response body incomplete: recv-len=" + std::to_string(actual_length) +
                ", content-length=" + std::to_string(expect_length);
    SDK_LOG_ERR("Check response body fail: %s", err_msg->c_str());
    return -1;
  }
  return 0;
}

// 构建连接池 key: "host:port:scheme[:verify:ca]"
// 注意: HTTPS 连接的安全属性由 is_verify_cert / ca_location 决定(二者均可
// 按请求粒度设置), 必须纳入 key。否则 VERIFY_NONE 建立的连接会被要求校验
// 证书的请求复用, 导致证书校验被绕过。
std::string MakePoolKey(const std::string& host, uint16_t port,
                        const std::string& scheme, bool is_verify_cert,
                        const std::string& ca_location) {
    std::string key = host + ":" + std::to_string(port) + ":" + scheme;
    if (scheme == "https") {
        key += is_verify_cert ? ":v1:" : ":v0:";
        key += ca_location;  // 空表示使用系统默认 CA
    }
    return key;
}

// RAII 包装，确保 session 在函数退出时正确归还连接池或丢弃
struct SessionHandle {
    std::unique_ptr<Poco::Net::HTTPClientSession> session;
    std::string pool_key;     // 非空表示应归还到连接池
    bool is_healthy = true;   // false 表示连接异常，应丢弃
    // 复用连接的真实建立时间; 默认值(epoch)表示本次为新建连接
    std::chrono::steady_clock::time_point created;
    // 服务端是否允许保持连接, 由调用方在 receiveResponse 后写入。
    //
    // 切勿改用 session->getKeepAlive() 判断: 那是基类 HTTPSession 上我们
    // 自己 setKeepAlive() 写入的标志位(内联实现直接 return _keepAlive),
    // 池化开启时恒为 true, 与服务端响应无关; 真正反映服务端意图的是响应
    // 对象上的 HTTPMessage::getKeepAlive()(同名不同类), 而 Poco 内部的
    // mustReconnect() 是 protected, 外部不可调用。
    bool server_keep_alive = true;

    ~SessionHandle() {
        if (!pool_key.empty() && session) {
            // 服务端(或中间网关)要求关闭、或连接已断开时不得入池,
            // 否则下一个复用者会在 sendRequest 时踩到 RST/EOF。
            bool reusable =
                is_healthy && server_keep_alive && session->connected();
            ConnectionPool::GetInstance().Release(pool_key, std::move(session),
                                                  reusable, created);
        }
    }
};

// 从连接池取连接, 并剔除已被对端静默关闭的陈旧连接。
// 返回后 handle.session 为空表示需要新建连接。
void AcquirePooledSession(SessionHandle* handle) {
    handle->session = ConnectionPool::GetInstance().Acquire(handle->pool_key,
                                                            &handle->created);
    if (!handle->session) {
        return;
    }
    // 探测陈旧连接: 空闲连接可能已被对端静默关闭。连接已断开, 或 poll 可读
    // (本端尚未发请求, 通常意味着收到 FIN/RST 或残留数据), 一律视为不可复用。
    //
    // poll()/connected() 在 fd 失效时会抛 NetException。本函数在 SendRequest
    // 的 try 块内被调用, 若任其逸出会让整个用户请求失败 —— 而这里的本意就是
    // "丢弃坏连接、回退到新建连接", 故异常一律吞掉并按陈旧处理。
    bool stale = true;
    try {
        stale = !handle->session->connected() ||
                handle->session->socket().poll(
                    0, Poco::Net::Socket::SELECT_READ);
    } catch (const Poco::Exception& ex) {
        SDK_LOG_WARN("Probe pooled connection failed, treat as stale: %s",
                     ex.displayText().c_str());
        stale = true;
    }
    if (stale) {
        handle->session.reset();
        handle->created = std::chrono::steady_clock::time_point();
        // 必须回报, 否则 Hit 计数包含"取出即丢弃"的连接, 无法反映真实复用率
        ConnectionPool::GetInstance().ReportStaleDiscard();
    }
}

// 判断响应体是否已从 socket 上读尽（连接是否干净、可复用）。
//
// 依据: 各 copy 循环均以 "读到 0 字节" 作为结束条件, 读尽时 istream 必定
// 被置上 eofbit; 若写出流失败或拷贝提前中断(handleCopyStream 中的
// `if (istr && ostr)` 分支), eofbit 不会被置位, 说明 socket 上仍残留未读
// 字节 —— 此时归还连接, 下一个复用者会把残留数据当成自己的响应来解析。
//
// 该判断只读 iostream 状态位, 不触碰 socket, 因此不会阻塞(不能用 peek(),
// 它在未读尽时会去 socket 上取数据, 可能阻塞到接收超时)。
//
// 注意: 小 body 在单次 read 中被读尽时, 即使随后写出流失败 eofbit 也已置位,
// 此时判定为"干净"是正确的 —— socket 确实已排空, 连接可以安全复用。
bool IsResponseDrained(const std::istream& recv_stream) {
    return recv_stream.eof();
}

// 应用 TCP keepalive 探活参数（必须在连接建立后调用）。
// 未设置时依赖系统默认(Linux 默认 7200s), 半开连接无法及时发现。
void ApplyTcpKeepAliveOptions(Poco::Net::StreamSocket& ss) {
    try {
        ss.setKeepAlive(true);
        const int keep_idle = static_cast<int>(CosSysConfig::GetKeepIdle());
        const int keep_intvl = static_cast<int>(CosSysConfig::GetKeepIntvl());
#if defined(__linux__)
        if (keep_idle > 0) {
            ss.setOption(IPPROTO_TCP, TCP_KEEPIDLE, keep_idle);
        }
        if (keep_intvl > 0) {
            ss.setOption(IPPROTO_TCP, TCP_KEEPINTVL, keep_intvl);
        }
#elif defined(__APPLE__)
        if (keep_idle > 0) {
            ss.setOption(IPPROTO_TCP, TCP_KEEPALIVE, keep_idle);
        }
        (void)keep_intvl;
#else
        (void)keep_idle;
        (void)keep_intvl;
#endif
    } catch (const Poco::Exception& ex) {
        // keepalive 探活属优化项, 设置失败不影响请求本身
        SDK_LOG_WARN("Set tcp keepalive option failed: %s",
                     ex.displayText().c_str());
    }
}
} // namespace

int HttpSender::SendRequest(
    const SharedTransferHandler& handler, const std::string& http_method,
    const std::string& url_str,
    const std::map<std::string, std::string>& req_params,
    const std::map<std::string, std::string>& req_headers,
    const std::string& req_body, uint64_t conn_timeout_in_ms,
    uint64_t recv_timeout_in_ms,
    std::map<std::string, std::string>* resp_headers, std::string* resp_body,
    std::string* err_msg, bool is_check_md5,
    bool is_verify_cert, const std::string& ca_location,
    const SSLCtxCallback& ssl_ctx_cb, void *user_data) {
  std::istringstream is(req_body);
  std::ostringstream oss;
  int ret = SendRequest(handler, http_method, url_str, req_params, req_headers,
                        is, conn_timeout_in_ms, recv_timeout_in_ms,
                        resp_headers, oss, err_msg, is_check_md5,
                        is_verify_cert, ca_location, ssl_ctx_cb, user_data);
  *resp_body = oss.str();
  return ret;
}

int HttpSender::SendRequest(
    const SharedTransferHandler& handler, const std::string& http_method,
    const std::string& url_str,
    const std::map<std::string, std::string>& req_params,
    const std::map<std::string, std::string>& req_headers, std::istream& is,
    uint64_t conn_timeout_in_ms, uint64_t recv_timeout_in_ms,
    std::map<std::string, std::string>* resp_headers, std::string* resp_body,
    std::string* err_msg, bool is_check_md5,
    bool is_verify_cert, const std::string& ca_location,
    const SSLCtxCallback& ssl_ctx_cb, void *user_data) {
  std::ostringstream oss;
  int ret = SendRequest(handler, http_method, url_str, req_params, req_headers,
                        is, conn_timeout_in_ms, recv_timeout_in_ms,
                        resp_headers, oss, err_msg, is_check_md5,
                        is_verify_cert, ca_location, ssl_ctx_cb, user_data);
  *resp_body = oss.str();
  return ret;
}

int HttpSender::SendRequest(
    const SharedTransferHandler& handler, const std::string& http_method,
    const std::string& url_str,
    const std::map<std::string, std::string>& req_params,
    const std::map<std::string, std::string>& req_headers, 
    std::istream& is, // 流式输入，用于传输请求正文
    uint64_t conn_timeout_in_ms, 
    uint64_t recv_timeout_in_ms,
    std::map<std::string, std::string>* resp_headers, 
    std::ostream& resp_stream, // 流式输出，用于接收响应正文
    std::string* err_msg, 
    bool is_check_md5,
    bool is_verify_cert, 
    const std::string& ca_location,
    const SSLCtxCallback& ssl_ctx_cb,
    void *user_data,
    const char *req_body_buf, // 可选的缓冲区
    size_t req_body_len) {
  SessionHandle session_handle;
  bool use_pool = CosSysConfig::GetKeepAlive() && (ssl_ctx_cb == nullptr);
  try {
    SDK_LOG_INFO("send request to [%s]", url_str.c_str());
    Poco::URI url(url_str);

    if (use_pool) {
      session_handle.pool_key =
          MakePoolKey(url.getHost(), url.getPort(), url.getScheme(),
                      is_verify_cert, ca_location);
      AcquirePooledSession(&session_handle);
    }

    if (!session_handle.session) {
      if (url.getScheme() == "https") {
        bool load_default_ca = ca_location.empty();
        Poco::Net::Context::VerificationMode verify_mode = Poco::Net::Context::VERIFY_RELAXED;
        if (!is_verify_cert) {
          verify_mode = Poco::Net::Context::VERIFY_NONE;
        }
        Poco::Net::Context::Ptr context =
            new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", ca_location,
                                   verify_mode, 9, load_default_ca,
                                   "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
        if (ssl_ctx_cb) {
          int ret = ssl_ctx_cb(context->sslContext(), user_data);
          if (ret != 0) {
            *err_msg = "SSL_Ctx Callback Exception Code: " + std::to_string(ret);
            return kHttpStatusNetError;
          }
        }
        session_handle.session.reset(new Poco::Net::HTTPSClientSession(url.getHost(), url.getPort(), context));
      } else {
        session_handle.session.reset(new Poco::Net::HTTPClientSession(url.getHost(), url.getPort()));
      }
    }

    Poco::Net::HTTPClientSession& session = *session_handle.session;
    session.setTimeout(Poco::Timespan(0, conn_timeout_in_ms * 1000));
    // 开启长连接后必须显式设置 keepAlive，否则 Poco 默认发送 Connection: Close，
    // 导致连接无法复用。
    session.setKeepAlive(use_pool);
    if (use_pool) {
      // 抬高 Poco 内部的 keepAliveTimeout, 让空闲判定只由连接池一处决定。
      //
      // 两个原因:
      // 1. 该值默认仅 8s(DEFAULT_KEEP_ALIVE_TIMEOUT), 超时后 sendRequest()
      //    会静默重建连接(完整 TCP+TLS 握手), 而连接池仍按 MaxIdleMs 判定
      //    "未过期"并计为命中 —— 白做握手, 命中率还虚高;
      // 2. Poco 的计时基准是 mustReconnect() 里的 _lastRequest, 即"上次
      //    发出请求的时刻"; 而连接池与服务端的基准都是"最后一次活动时刻"。
      //    因此响应传输本身耗时较久时(大对象下载), Poco 会把传输时间也算作
      //    空闲而提前重连, 与连接池判定背离。
      //
      // 取 MaxIdleMs + MaxAgeMs: 连接最迟在 MaxAge 处被池强制退休, 故两次
      // 请求的间隔不会超过该值, 可确保 Poco 永远不会先于连接池触发。
      const uint64_t poco_ka_ms = CosSysConfig::GetConnectionPoolMaxIdleMs() +
                                  CosSysConfig::GetConnectionPoolMaxAgeMs();
      session.setKeepAliveTimeout(Poco::Timespan(
          static_cast<long>(poco_ka_ms / 1000),
          static_cast<long>((poco_ka_ms % 1000) * 1000)));
    }
    // 1. 拼接path_query字符串
    std::string path_and_query_str = BuildRequestPathAndQueryParams(url, req_params);

    // 2. 创建http request, 并填充头部
    Poco::Net::HTTPRequest req(http_method, path_and_query_str,
                               Poco::Net::HTTPMessage::HTTP_1_1);
    for (std::map<std::string, std::string>::const_iterator c_itr =
             req_headers.begin();
         c_itr != req_headers.end(); ++c_itr) {
      req.add(c_itr->first, c_itr->second);
    }

    // 3. 打印请求信息
    std::ostringstream debug_os;
    req.write(debug_os);
    SDK_LOG_DBG("request=[%s]", debug_os.str().c_str());

    // 4. 发送请求, 统计上传速率
    std::chrono::time_point<std::chrono::steady_clock> start_ts, end_ts;
    start_ts = std::chrono::steady_clock::now();
    std::ostream& os = session.sendRequest(req);
    std::streamsize copy_size;
    if (req_body_buf != nullptr) {
      copy_size = HandleStreamCopier::handleCopyStream(handler, req_body_buf, req_body_len, os);
    } else {
      copy_size = HandleStreamCopier::handleCopyStream(handler, is, os);
    }
    end_ts = std::chrono::steady_clock::now();
    PrintRate(start_ts, end_ts, copy_size, "send");

    // 5. 接收返回
    Poco::Net::HTTPResponse res;
    Poco::Net::StreamSocket& ss = session.socket();
    ss.setReceiveTimeout(Poco::Timespan(0, recv_timeout_in_ms * 1000));
    // 连接此时已建立, 下发 TCP keepalive 探活参数(KeepIdle/KeepIntvl),
    // 使连接在池中空闲期间能及时发现半开连接。
    if (use_pool) {
      ApplyTcpKeepAliveOptions(ss);
    }
    std::istream& recv_stream = session.receiveResponse(res);
    // 记录服务端的连接保持意图(Connection 头), 供归还时判断能否入池
    session_handle.server_keep_alive = res.getKeepAlive();

    // 6. 处理返回
    int status_code = res.getStatus();
    resp_headers->insert(res.begin(), res.end());
    // 有些代理可能会把ETag头部修改成Etag,此处修改成ETag
    if (resp_headers->count("Etag") > 0) {
      (*resp_headers)["ETag"] = (*resp_headers)["Etag"];
      resp_headers->erase("Etag");
    }
    std::string etag;
    std::map<std::string, std::string>::const_iterator etag_itr =
        resp_headers->find("ETag");
    if (etag_itr != resp_headers->end()) {
      etag = StringUtil::Trim(etag_itr->second, "\"");
    }

    if (is_check_md5 && !StringUtil::IsV4ETag(etag) && !StringUtil::IsMultipartUploadETag(etag)) {
      SDK_LOG_DBG("Check Response Md5");
      Poco::MD5Engine md5;
      Poco::DigestOutputStream dos(md5);

      std::stringbuf ibuf;
      std::iostream io_tmp(&ibuf);

      // The Poco session->receiveResponse return the streambuf which dose not
      // overload the base_iostream seekpos which is the realization of the
      // tellg and seekg. It casue the recv_stream can not relocation the begin
      // postion, so can not reuse of the recv_stream.
      // FIXME it might has property issue.
      start_ts = std::chrono::steady_clock::now();
      copy_size = Poco::StreamCopier::copyStream(recv_stream, io_tmp);
      end_ts = std::chrono::steady_clock::now();

      std::streampos pos = io_tmp.tellg();
      Poco::StreamCopier::copyStream(io_tmp, dos);
      io_tmp.clear();
      io_tmp.seekg(pos);
      dos.close();

      std::string md5_str = Poco::DigestEngine::digestToHex(md5.digest());

      if (etag != md5_str) {
          *err_msg = "Md5 of response body is not equal to the etag in the header."
                     " Body Md5= " + md5_str + ", etag=" + etag;
          SDK_LOG_ERR("Check Md5 fail, %s", err_msg->c_str());
          status_code = kHttpStatusNetError;
      }
      HandleStreamCopier::handleCopyStream(handler, io_tmp, resp_stream);
    } else {
      int64_t content_length = GetResponseContentLength(resp_headers);
      start_ts = std::chrono::steady_clock::now();
      copy_size = HandleStreamCopier::handleCopyStream(handler, recv_stream, resp_stream);
      end_ts = std::chrono::steady_clock::now();
      int res = CheckResponseBodyLength(http_method, content_length, copy_size, err_msg);
      if (res < 0) {
        status_code = kHttpStatusNetError;
        // 响应体不完整: 连接上可能残留未读数据, 归还池中会污染
        // 下一个复用该连接的请求, 必须丢弃
        session_handle.is_healthy = false;
      }
    }
    // 兜底: CheckResponseBodyLength 只覆盖 "GET + 有 Content-Length" 的场景,
    // chunked 响应、非 GET 请求、用户 resp_stream 写失败导致的短读都会漏网。
    // 统一用 "响应流是否读到 EOF" 判断 socket 是否已排空。
    if (use_pool && session_handle.is_healthy &&
        !IsResponseDrained(recv_stream)) {
      SDK_LOG_ERR("Response body not fully drained, drop connection");
      session_handle.is_healthy = false;
    }
    PrintRate(start_ts, end_ts, copy_size, "recv");

    LogResponseMessage(resp_headers, status_code, res, *err_msg);
    SDK_LOG_INFO("Send request over, ret=%d, http_status=%d, reason=%s", status_code, res.getStatus(), res.getReason().c_str());
    return status_code;
  } catch (Poco::Net::NetException& ex) {
    session_handle.is_healthy = false;
    SDK_LOG_ERR("Net Exception:%s", ex.displayText().c_str());
    *err_msg = "Net Exception:" + ex.displayText();
    return kHttpStatusNetError;
  } catch (Poco::TimeoutException& ex) {
    session_handle.is_healthy = false;
    SDK_LOG_ERR("TimeoutException:%s", ex.displayText().c_str());
    *err_msg = "TimeoutException:" + ex.displayText();
    return kHttpStatusNetError;
  } catch (UserCancelException& ex) {
    session_handle.is_healthy = false;
    SDK_LOG_INFO("Request canceled by user");
    *err_msg = "Request canceled by user";
    return kHttpStatusUserCancel;
  } catch (Poco::URISyntaxException& ex) {
    session_handle.is_healthy = false;
    SDK_LOG_ERR("url:%s    URISyntaxException:%s", url_str.c_str(), ex.displayText().c_str());
    *err_msg = "url:" + url_str +  "    URISyntaxException:" + ex.displayText();
    return kHttpStatusNetError;
  } catch (const std::exception& ex) {
    session_handle.is_healthy = false;
    SDK_LOG_ERR("Exception:%s, errno=%d", std::string(ex.what()).c_str(),
                errno);
    *err_msg = "Exception:" + std::string(ex.what());
    return kHttpStatusNetError;
  }
}

int HttpSender::SendRequest(
    const SharedTransferHandler& handler, const std::string& http_method,
    const std::string& url_str,
    const std::map<std::string, std::string>& req_params,
    const std::map<std::string, std::string>& req_headers,
    const std::string& req_body, // 字符串输入，用于传输请求正文
    uint64_t conn_timeout_in_ms,
    uint64_t recv_timeout_in_ms,
    std::map<std::string, std::string>* resp_headers, 
    std::string* xml_err_str, // 额外的错误信息, 用于响应返回非 2xx 错误码时, 传输报错响应信息
    std::ostream& resp_stream,  // 流式输出, 用于传输响应正文
    std::string* err_msg, 
    uint64_t* real_byte, // 实际接收字节数
    bool is_check_md5, 
    bool is_verify_cert, 
    const std::string& ca_location,
    const SSLCtxCallback& ssl_ctx_cb,
    void *user_data) {
  SessionHandle session_handle;
  bool use_pool = CosSysConfig::GetKeepAlive() && (ssl_ctx_cb == nullptr);
  try {
    SDK_LOG_INFO("send request to [%s]", url_str.c_str());
    Poco::URI url(url_str);

    if (use_pool) {
      session_handle.pool_key =
          MakePoolKey(url.getHost(), url.getPort(), url.getScheme(),
                      is_verify_cert, ca_location);
      AcquirePooledSession(&session_handle);
    }

    if (!session_handle.session) {
      if (url.getScheme() == "https") {
        bool load_default_ca = ca_location.empty();
        Poco::Net::Context::VerificationMode verify_mode = Poco::Net::Context::VERIFY_RELAXED;
        if (!is_verify_cert) {
          verify_mode = Poco::Net::Context::VERIFY_NONE;
        }

        Poco::Net::Context::Ptr context =
            new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", ca_location,
                                   verify_mode, 9, load_default_ca,
                                   "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
        if (ssl_ctx_cb) {
          int ret = ssl_ctx_cb(context->sslContext(), user_data);
          if (ret != 0) {
            *err_msg = "SSL_Ctx Callback Exception Code: " + std::to_string(ret);
            return kHttpStatusNetError;
          }
        }
        session_handle.session.reset(new Poco::Net::HTTPSClientSession(url.getHost(), url.getPort(), context));
      } else {
        session_handle.session.reset(new Poco::Net::HTTPClientSession(url.getHost(), url.getPort()));
      }
    }
    Poco::Net::HTTPClientSession& session = *session_handle.session;
    session.setTimeout(Poco::Timespan(0, conn_timeout_in_ms * 1000));
    // 开启长连接后必须显式设置 keepAlive，否则 Poco 默认发送 Connection: Close，
    // 导致连接无法复用。
    session.setKeepAlive(use_pool);
    if (use_pool) {
      // 抬高 Poco 内部的 keepAliveTimeout, 让空闲判定只由连接池一处决定。
      //
      // 两个原因:
      // 1. 该值默认仅 8s(DEFAULT_KEEP_ALIVE_TIMEOUT), 超时后 sendRequest()
      //    会静默重建连接(完整 TCP+TLS 握手), 而连接池仍按 MaxIdleMs 判定
      //    "未过期"并计为命中 —— 白做握手, 命中率还虚高;
      // 2. Poco 的计时基准是 mustReconnect() 里的 _lastRequest, 即"上次
      //    发出请求的时刻"; 而连接池与服务端的基准都是"最后一次活动时刻"。
      //    因此响应传输本身耗时较久时(大对象下载), Poco 会把传输时间也算作
      //    空闲而提前重连, 与连接池判定背离。
      //
      // 取 MaxIdleMs + MaxAgeMs: 连接最迟在 MaxAge 处被池强制退休, 故两次
      // 请求的间隔不会超过该值, 可确保 Poco 永远不会先于连接池触发。
      const uint64_t poco_ka_ms = CosSysConfig::GetConnectionPoolMaxIdleMs() +
                                  CosSysConfig::GetConnectionPoolMaxAgeMs();
      session.setKeepAliveTimeout(Poco::Timespan(
          static_cast<long>(poco_ka_ms / 1000),
          static_cast<long>((poco_ka_ms % 1000) * 1000)));
    }
    // 1. 拼接path_query字符串
    std::string path_and_query_str = BuildRequestPathAndQueryParams(url, req_params);

    // 2. 创建http request, 并填充头部
    Poco::Net::HTTPRequest req(http_method, path_and_query_str,
                               Poco::Net::HTTPMessage::HTTP_1_1);
    for (auto c_itr = req_headers.begin(); c_itr != req_headers.end(); ++c_itr) {
      // 有用户这这里出了堆栈，(c_itr->second).c_str() -> c_itr->second
      // req.add(c_itr->first, (c_itr->second).c_str());
      req.add(c_itr->first, c_itr->second);
    }

    std::ostringstream debug_os;
    req.write(debug_os);
    SDK_LOG_DBG("request=[%s]", debug_os.str().c_str());

    std::chrono::time_point<std::chrono::steady_clock> start_ts, end_ts;
    unsigned int time_consumed_ms = 0;
    std::streamsize copy_size = 0;
    // 3. 发送请求
    std::ostream& os = session.sendRequest(req);
    if (!req_body.empty()) {
      // 统计上传速率
      start_ts = std::chrono::steady_clock::now();
      os << req_body;
      end_ts = std::chrono::steady_clock::now();
      PrintRate(start_ts, end_ts, req_body.size(), "send");
    }

    // 4. 接收返回
    Poco::Net::HTTPResponse res;
    Poco::Net::StreamSocket& ss = session.socket();
    ss.setReceiveTimeout(Poco::Timespan(0, recv_timeout_in_ms * 1000));
    // 连接此时已建立, 下发 TCP keepalive 探活参数(KeepIdle/KeepIntvl),
    // 使连接在池中空闲期间能及时发现半开连接。
    if (use_pool) {
      ApplyTcpKeepAliveOptions(ss);
    }
    std::istream& recv_stream = session.receiveResponse(res);
    // 记录服务端的连接保持意图(Connection 头), 供归还时判断能否入池
    session_handle.server_keep_alive = res.getKeepAlive();

    // 6. 处理返回
    int status_code = res.getStatus();
    resp_headers->insert(res.begin(), res.end());
    // 有些代理可能会把ETag头部修改成Etag,此处修改成ETag
    if (resp_headers->count("Etag") > 0) {
      (*resp_headers)["ETag"] = (*resp_headers)["Etag"];
      resp_headers->erase("Etag");
    }
    if (status_code != 200 && status_code != 206) {
      *real_byte = Poco::StreamCopier::copyToString(recv_stream, *xml_err_str);
    } else {
      std::string etag;
      std::map<std::string, std::string>::const_iterator etag_itr =
          resp_headers->find("ETag");
      if (etag_itr != resp_headers->end()) {
        etag = StringUtil::Trim(etag_itr->second, "\"");
      }

      // 获取响应中body长度
      int64_t content_length = GetResponseContentLength(resp_headers);
      if (handler && content_length > 0) {
        handler->SetTotalSize(content_length);
      }
      if (is_check_md5 && !StringUtil::IsV4ETag(etag) && !StringUtil::IsMultipartUploadETag(etag)) {
        SDK_LOG_DBG("Check Response Md5");
        Poco::MD5Engine md5;
        Poco::DigestOutputStream dos(md5);

        // explicit iostream (streambuf* sb);
        std::stringbuf ibuf;
        std::iostream io_tmp(&ibuf);

        // The Poco session->receiveResponse return the streambuf which dose not
        // overload the base_iostream seekpos which is the realization of the
        // tellg and seekg. It casue the recv_stream can not relocation the
        // begin postion, so can not reuse of the recv_stream.
        // FIXME it might has property issue.
        start_ts = std::chrono::steady_clock::now();
        *real_byte = Poco::StreamCopier::copyStream(recv_stream, io_tmp);
        end_ts = std::chrono::steady_clock::now();

        std::streampos pos = io_tmp.tellg();
        Poco::StreamCopier::copyStream(io_tmp, dos);
        io_tmp.clear();
        io_tmp.seekg(pos);
        dos.close();
        std::string md5_str = Poco::DigestEngine::digestToHex(md5.digest());

        if (etag != md5_str) {
          *err_msg = "Md5 of response body is not equal to the etag in the header. Body Md5= " + md5_str +
                      ", etag=" + etag + ", recv-len=" + StringUtil::Uint64ToString(*real_byte) +
                      ", content-length=" + std::to_string(content_length);
          SDK_LOG_ERR("Check Md5 fail, %s", err_msg->c_str());
          status_code = kHttpStatusNetError;
        }
        HandleStreamCopier::handleCopyStream(handler, io_tmp, resp_stream);
      } else {  // other way direct use the recv_stream
        start_ts = std::chrono::steady_clock::now();
        *real_byte = HandleStreamCopier::handleCopyStream(handler, recv_stream, resp_stream);
        end_ts = std::chrono::steady_clock::now();
        int res = CheckResponseBodyLength(http_method, content_length, *real_byte, err_msg);
        if (res < 0) {
            status_code = kHttpStatusNetError;
            // 响应体不完整: 连接上可能残留未读数据, 不得归还池中
            session_handle.is_healthy = false;
        }
      }
      PrintRate(start_ts, end_ts, *real_byte, "recv");
    }
    // 兜底: CheckResponseBodyLength 只覆盖 "GET + 有 Content-Length" 的场景,
    // chunked 响应、非 GET 请求、用户 resp_stream 写失败导致的短读都会漏网。
    // 统一用 "响应流是否读到 EOF" 判断 socket 是否已排空。
    if (use_pool && session_handle.is_healthy &&
        !IsResponseDrained(recv_stream)) {
      SDK_LOG_ERR("Response body not fully drained, drop connection");
      session_handle.is_healthy = false;
    }

    LogResponseMessage(resp_headers, status_code, res, *err_msg);
    SDK_LOG_INFO("Send request over, ret=%d, http_status=%d, reason=%s", status_code, res.getStatus(), res.getReason().c_str());
    return status_code;
  } catch (Poco::Net::NetException& ex) {
    session_handle.is_healthy = false;
    SDK_LOG_ERR("Net Exception:%s", ex.displayText().c_str());
    *err_msg = "Net Exception:" + ex.displayText();
    return kHttpStatusNetError;
  } catch (Poco::TimeoutException& ex) {
    session_handle.is_healthy = false;
    SDK_LOG_ERR("TimeoutException:%s", ex.displayText().c_str());
    *err_msg = "TimeoutException:" + ex.displayText();
    return kHttpStatusNetError;
  } catch(UserCancelException & ex) {
    session_handle.is_healthy = false;
    SDK_LOG_INFO("Request canceled by user");
    *err_msg = "Request canceled by user";
    return kHttpStatusUserCancel;
  } catch (Poco::URISyntaxException& ex) {
    session_handle.is_healthy = false;
    SDK_LOG_ERR("url:%s    URISyntaxException:%s", url_str.c_str(), ex.displayText().c_str());
    *err_msg = "url:" + url_str +  "    URISyntaxException:" + ex.displayText();
    return kHttpStatusNetError;
  } catch (const std::exception& ex) {
    session_handle.is_healthy = false;
    SDK_LOG_ERR("Exception:%s, errno=%d", std::string(ex.what()).c_str(),
                errno);
    *err_msg = "Exception:" + std::string(ex.what());
    return kHttpStatusNetError;
  }
}

}  // namespace qcloud_cos
