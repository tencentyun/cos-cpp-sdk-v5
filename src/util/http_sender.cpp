// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/18/17
// Description:

#include "util/http_sender.h"

#include <iostream>
#include <sstream>
#include <memory>
#include <chrono>

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
#include "cos_defines.h"
#include "cos_config.h"
#include "cos_sys_config.h"
#include "util/string_util.h"
#include "util/codec_util.h"

namespace qcloud_cos {
    int HttpSender::SendRequest(const std::string& http_method,
                            const std::string& url_str,
                            const std::map<std::string, std::string>& req_params,
                            const std::map<std::string, std::string>& req_headers,
                            const std::string& req_body,
                            uint64_t conn_timeout_in_ms,
                            uint64_t recv_timeout_in_ms,
                            std::map<std::string, std::string>* resp_headers,
                            std::string* resp_body,
                            std::string* err_msg,
                            bool is_check_md5) {
    std::istringstream is(req_body);
    std::ostringstream oss;
    int ret = SendRequest(http_method,
                          url_str,
                          req_params,
                          req_headers,
                          is,
                          conn_timeout_in_ms,
                          recv_timeout_in_ms,
                          resp_headers,
                          oss,
                          err_msg,
                          is_check_md5);
    *resp_body = oss.str();
    return ret;
}

int HttpSender::SendRequest(const std::string& http_method,
                            const std::string& url_str,
                            const std::map<std::string, std::string>& req_params,
                            const std::map<std::string, std::string>& req_headers,
                            const std::string& req_body,
                            uint64_t conn_timeout_in_ms,
                            uint64_t recv_timeout_in_ms,
                            std::map<std::string, std::string>* resp_headers,
                            std::ostream& resp_stream,
                            std::string* err_msg,
                            bool is_check_md5) {
    std::istringstream is(req_body);
    int ret = SendRequest(http_method,
                          url_str,
                          req_params,
                          req_headers,
                          is,
                          conn_timeout_in_ms,
                          recv_timeout_in_ms,
                          resp_headers,
                          resp_stream,
                          err_msg,
                          is_check_md5);
    return ret;
}

int HttpSender::SendRequest(const std::string& http_method,
                            const std::string& url_str,
                            const std::map<std::string, std::string>& req_params,
                            const std::map<std::string, std::string>& req_headers,
                            std::istream& is,
                            uint64_t conn_timeout_in_ms,
                            uint64_t recv_timeout_in_ms,
                            std::map<std::string, std::string>* resp_headers,
                            std::string* resp_body,
                            std::string* err_msg,
                            bool is_check_md5) {
    std::ostringstream oss;
    int ret = SendRequest(http_method,
                          url_str,
                          req_params,
                          req_headers,
                          is,
                          conn_timeout_in_ms,
                          recv_timeout_in_ms,
                          resp_headers,
                          oss,
                          err_msg,
                          is_check_md5);
    *resp_body = oss.str();
    return ret;
}

int HttpSender::SendRequest(const std::string& http_method,
                            const std::string& url_str,
                            const std::map<std::string, std::string>& req_params,
                            const std::map<std::string, std::string>& req_headers,
                            std::istream& is,
                            uint64_t conn_timeout_in_ms,
                            uint64_t recv_timeout_in_ms,
                            std::map<std::string, std::string>* resp_headers,
                            std::ostream& resp_stream,
                            std::string* err_msg,
                            bool is_check_md5) {
    Poco::Net::HTTPResponse res;
    try {
        Poco::URI url(url_str);
        std::unique_ptr<Poco::Net::HTTPClientSession> session;
        if (StringUtil::StringStartsWithIgnoreCase(url_str, "https")) {
            Poco::Net::Context::Ptr context = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE,
                                                     "", "", "", Poco::Net::Context::VERIFY_RELAXED,
                                                     9, true, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
            session.reset(new Poco::Net::HTTPSClientSession(url.getHost(), url.getPort(), context));
        } else {
            session.reset(new Poco::Net::HTTPClientSession(url.getHost(), url.getPort()));
        }

        session->setTimeout(Poco::Timespan(0, conn_timeout_in_ms * 1000));
        // 1. 拼接path_query字符串
        std::string path = url.getPath();
        if (path.empty()) {
            path += "/";
        }

        std::string query_str;
        for (std::map<std::string, std::string>::const_iterator c_itr = req_params.begin();
                c_itr != req_params.end(); ++c_itr) {
            std::string part;
            if (c_itr->second.empty()) {
                part = CodecUtil::UrlEncode(c_itr->first) + "&";
            } else {
                part = CodecUtil::UrlEncode(c_itr->first) + "=" + CodecUtil::UrlEncode(c_itr->second) + "&";
            }
            query_str += part;
        }

        if (!query_str.empty()) {
            query_str = "?" + query_str.substr(0, query_str.size() - 1);
        }
        std::string path_and_query_str = CodecUtil::EncodeKey(path) + query_str;
        //std::string path_and_query_str = CodecUtil::EncodeKey(path) + CodecUtil::UrlEncode("?response-content-type") + "= " + CodecUtil::UrlEncode("abcd\r\rnef");

        // 2. 创建http request, 并填充头部
        Poco::Net::HTTPRequest req(http_method, path_and_query_str, Poco::Net::HTTPMessage::HTTP_1_1);
        for (std::map<std::string, std::string>::const_iterator c_itr = req_headers.begin();
                c_itr != req_headers.end(); ++c_itr) {
            req.add(c_itr->first, (c_itr->second).c_str());
        }

        // 3. 计算长度
        std::streampos pos = is.tellg();
        is.seekg(0, std::ios::end);
        req.setContentLength(is.tellg());
        is.seekg(pos);

        std::ostringstream debug_os;
        req.write(debug_os);
        SDK_LOG_DBG("request=[%s]", debug_os.str().c_str());

        // 4. 发送请求
        // 统计上传速率
        std::chrono::time_point<std::chrono::steady_clock> start_ts, end_ts;
        unsigned int time_consumed_ms = 0;
        start_ts = std::chrono::steady_clock::now();
        std::ostream& os = session->sendRequest(req);
        std::streamsize copy_size = Poco::StreamCopier::copyStream(is, os);
        end_ts = std::chrono::steady_clock::now();
        time_consumed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_ts - start_ts).count();
        // 大于100KB才计算速率
        if (time_consumed_ms > 0 && copy_size > 100 * 1204) {
            float rate = ((float)copy_size / 1024 / 1024) / ((float)time_consumed_ms / 1000);
            SDK_LOG_DBG("send_size:%u, time_consumed:%u ms, rate:%0.2f MB/s", copy_size, time_consumed_ms, rate);
        }

        // 5. 接收返回
        Poco::Net::StreamSocket& ss = session->socket();
        ss.setReceiveTimeout(Poco::Timespan(0, recv_timeout_in_ms * 1000));
        std::istream& recv_stream = session->receiveResponse(res);

        // 6. 处理返回
        int ret = res.getStatus();
        resp_headers->insert(res.begin(), res.end());
        // 有些代理可能会把ETag头部修改成Etag,此处修改成ETag
        if (resp_headers->count("Etag") > 0) {
            (*resp_headers)["ETag"] = (*resp_headers)["Etag"];
            resp_headers->erase("Etag");
        }
        std::string etag = "";
        std::map<std::string, std::string>::const_iterator etag_itr
            = resp_headers->find("ETag");
        if (etag_itr != resp_headers->end()) {
            etag = StringUtil::Trim(etag_itr->second, "\"");
        }

        if (is_check_md5 && !StringUtil::IsV4ETag(etag)
            && !StringUtil::IsMultipartUploadETag(etag)) {
            SDK_LOG_DBG("Check Response Md5");
            Poco::MD5Engine md5;
            Poco::DigestOutputStream dos(md5);

            // explicit iostream (streambuf* sb);
            std::stringbuf ibuf;
            std::iostream io_tmp(&ibuf);

            // The Poco session->receiveResponse return the streambuf which dose not overload the base_iostream seekpos which is the realization of the tellg and seekg.
            // It casue the recv_stream can not relocation the begin postion, so can not reuse of the recv_stream.
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
                ret = -1;
            }
            Poco::StreamCopier::copyStream(io_tmp, resp_stream);
        } else {
            start_ts = std::chrono::steady_clock::now();
            copy_size = Poco::StreamCopier::copyStream(recv_stream, resp_stream);
            end_ts = std::chrono::steady_clock::now();
        }
        time_consumed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_ts - start_ts).count();
        // 大于100KB才计算速率
        if (time_consumed_ms > 0 && copy_size > 100 * 1204) {
            float rate = ((float)copy_size / 1024 / 1024) / ((float)time_consumed_ms / 1000);
            SDK_LOG_DBG("recv_size:%u, time_consumed:%u ms, rate:%0.2f MB/s", copy_size, time_consumed_ms, rate);
        }

        {
	        std::string resp_header_log;
	        resp_header_log.append("response header :\n");
	        for (std::map<std::string, std::string>::const_iterator itr = resp_headers->begin();
		        itr != resp_headers->end(); ++itr) {
		        resp_header_log.append(itr->first + ": " + itr->second + "\n");
	        }
	        SDK_LOG_DBG("%s", resp_header_log.c_str());
        }

        SDK_LOG_INFO("Send request over, status=%d, reason=%s",
                res.getStatus(), res.getReason().c_str());
        return ret;
    } catch (Poco::Net::NetException& ex){
        SDK_LOG_ERR("Net Exception:%s", ex.displayText().c_str());
        *err_msg = "Net Exception:" + ex.displayText();
        return -1;
    } catch (Poco::TimeoutException& ex) {
        SDK_LOG_ERR("TimeoutException:%s", ex.displayText().c_str());
        *err_msg = "TimeoutException:" + ex.displayText();
        return -1;
    } catch (const std::exception &ex) {
        SDK_LOG_ERR("Exception:%s, errno=%d", std::string(ex.what()).c_str(), errno);
        *err_msg = "Exception:" + std::string(ex.what());
        return -1;
    }

    return res.getStatus();
}

int HttpSender::SendRequest(const std::string& http_method,
                            const std::string& url_str,
                            const std::map<std::string, std::string>& req_params,
                            const std::map<std::string, std::string>& req_headers,
                            const std::string& req_body,
                            uint64_t conn_timeout_in_ms,
                            uint64_t recv_timeout_in_ms,
                            std::map<std::string, std::string>* resp_headers,
                            std::string* xml_err_str,
                            std::ostream& resp_stream,
                            std::string* err_msg,
                            uint64_t* real_byte,
                            bool is_check_md5) {
    Poco::Net::HTTPResponse res;
    try {
        Poco::URI url(url_str);
        std::unique_ptr<Poco::Net::HTTPClientSession> session;
        if (StringUtil::StringStartsWithIgnoreCase(url_str, "https")) {
            Poco::Net::Context::Ptr context = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE,
                                                     "", "", "", Poco::Net::Context::VERIFY_RELAXED,
                                                     9, true, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
            session.reset(new Poco::Net::HTTPSClientSession(url.getHost(), url.getPort(), context));
        } else {
            session.reset(new Poco::Net::HTTPClientSession(url.getHost(), url.getPort()));
        }
        session->setTimeout(Poco::Timespan(0, conn_timeout_in_ms * 1000));
        // 1. 拼接path_query字符串
        std::string path = url.getPath();
        if (path.empty()) {
            path += "/";
        }

        std::string query_str;
        for (std::map<std::string, std::string>::const_iterator c_itr = req_params.begin();
                c_itr != req_params.end(); ++c_itr) {
            std::string part;
            if (c_itr->second.empty()) {
                part = CodecUtil::UrlEncode(c_itr->first) + "&";
            } else {
                part = CodecUtil::UrlEncode(c_itr->first) + "=" + CodecUtil::UrlEncode(c_itr->second) + "&";
            }
            query_str += part;
        }

        if (!query_str.empty()) {
            query_str = "?" + query_str.substr(0, query_str.size() - 1);
        }
        // std::string path_and_query_str = CodecUtil::EncodeKey(path) + CodecUtil::UrlEncode(query_str);
        std::string path_and_query_str = CodecUtil::EncodeKey(path) + query_str;
        //std::string path_and_query_str = CodecUtil::EncodeKey(path) + "response-content-type=abcd%0A%0Def";

        // 2. 创建http request, 并填充头部
        Poco::Net::HTTPRequest req(http_method, path_and_query_str, Poco::Net::HTTPMessage::HTTP_1_1);
        for (std::map<std::string, std::string>::const_iterator c_itr = req_headers.begin();
                c_itr != req_headers.end(); ++c_itr) {
            // 有用户这这里出了堆栈，(c_itr->second).c_str() -> c_itr->second
            //req.add(c_itr->first, (c_itr->second).c_str());
            req.add(c_itr->first, c_itr->second);
        }
        req.add("Content-Length", StringUtil::Uint64ToString(req_body.size()));

        std::ostringstream debug_os;
        req.write(debug_os);
        SDK_LOG_DBG("request=[%s]", debug_os.str().c_str());

        std::chrono::time_point<std::chrono::steady_clock> start_ts, end_ts;
        unsigned int time_consumed_ms = 0;
        std::streamsize copy_size = 0;
        // 3. 发送请求
        std::ostream& os = session->sendRequest(req);
        if (!req_body.empty()) {
            // 统计上传速率
            start_ts = std::chrono::steady_clock::now();
            os << req_body;
            end_ts = std::chrono::steady_clock::now();
            time_consumed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_ts - start_ts).count();
            // 大于100KB才计算速率
            if (time_consumed_ms > 0 && req_body.size() > 100 * 1204) {
                float rate = ((float)req_body.size() / 1024 / 1024) / ((float)time_consumed_ms / 1000);
                SDK_LOG_DBG("send_size:%u, time_consumed:%u ms, rate:%0.2f MB/s", req_body.size(), time_consumed_ms, rate);
            }
        }

        // 4. 接收返回
        Poco::Net::StreamSocket& ss = session->socket();
        ss.setReceiveTimeout(Poco::Timespan(0, recv_timeout_in_ms * 1000));
        std::istream& recv_stream = session->receiveResponse(res);

        // 6. 处理返回
        int ret = res.getStatus();
        resp_headers->insert(res.begin(), res.end());
        // 有些代理可能会把ETag头部修改成Etag,此处修改成ETag
        if (resp_headers->count("Etag") > 0) {
            (*resp_headers)["ETag"] = (*resp_headers)["Etag"];
            resp_headers->erase("Etag");
        }
        if (ret != 200 && ret != 206) {
            *real_byte = Poco::StreamCopier::copyToString(recv_stream, *xml_err_str);
        } else {
            std::string etag = "";
            std::map<std::string, std::string>::const_iterator etag_itr
                = resp_headers->find("ETag");
            if (etag_itr != resp_headers->end()) {
                etag = StringUtil::Trim(etag_itr->second, "\"");
            }

            if (is_check_md5 && !StringUtil::IsV4ETag(etag)
                && !StringUtil::IsMultipartUploadETag(etag)) {
                SDK_LOG_DBG("Check Response Md5");
                Poco::MD5Engine md5;
                Poco::DigestOutputStream dos(md5);

                // explicit iostream (streambuf* sb);
                std::stringbuf ibuf;
                std::iostream io_tmp(&ibuf);

                // The Poco session->receiveResponse return the streambuf which dose not overload the base_iostream seekpos which is the realization of the tellg and seekg.
                // It casue the recv_stream can not relocation the begin postion, so can not reuse of the recv_stream.
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
                    *err_msg = "Md5 of response body is not equal to the etag in the header."
                        " Body Md5= " + md5_str + ", etag=" + etag;
                    SDK_LOG_ERR("Check Md5 fail, %s", err_msg->c_str());
                    ret = -1;
                }
                Poco::StreamCopier::copyStream(io_tmp, resp_stream);
            } else { // other way direct use the recv_stream
                start_ts = std::chrono::steady_clock::now();
                *real_byte = Poco::StreamCopier::copyStream(recv_stream, resp_stream);
                end_ts = std::chrono::steady_clock::now();
            }
            time_consumed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_ts - start_ts).count();
            // 大于100KB才计算速率
            if (time_consumed_ms > 0 && *real_byte > 100 * 1204) {
                float rate = ((float)copy_size / 1024 / 1024) / ((float)time_consumed_ms / 1000);
                SDK_LOG_DBG("recv_size:%u, time_consumed:%u ms, rate:%0.2f MB/s", *real_byte, time_consumed_ms, rate);
            }
        }

        {
	        std::string resp_header_log;
	        resp_header_log.append("response header :\n");
	        for (std::map<std::string, std::string>::const_iterator itr = resp_headers->begin();
			        itr != resp_headers->end(); ++itr) {
		        resp_header_log.append(itr->first + ": " + itr->second + "\n");
	        }
	        SDK_LOG_DBG("%s", resp_header_log.c_str());
        }
        
        SDK_LOG_INFO("Send request over, status=%d, reason=%s", ret, res.getReason().c_str());
        return ret;
    } catch (Poco::Net::NetException& ex){
        SDK_LOG_ERR("Net Exception:%s", ex.displayText().c_str());
        *err_msg = "Net Exception:" + ex.displayText();
        return -1;
    } catch (Poco::TimeoutException& ex) {
        SDK_LOG_ERR("TimeoutException:%s", ex.displayText().c_str());
        *err_msg = "TimeoutException:" + ex.displayText();
        return -1;
    } catch (const std::exception &ex) {
        SDK_LOG_ERR("Exception:%s, errno=%d", std::string(ex.what()).c_str(), errno);
        *err_msg = "Exception:" + std::string(ex.what());
        return -1;
    }

    return res.getStatus();
}

int HttpSender::TransferSendRequest(const SharedTransferHandler& handler,
                       const std::string& http_method,
                       const std::string& url_str,
                       const std::map<std::string, std::string>& req_params,
                       const std::map<std::string, std::string>& req_headers,
                       std::istream& is,
                       uint64_t conn_timeout_in_ms,
                       uint64_t recv_timeout_in_ms,
                       std::map<std::string, std::string>* resp_headers,
                       std::ostream& resp_stream,
                       std::string* err_msg,
                       bool is_check_md5) {

    if (http_method != "GET" && http_method != "PUT") {
        *err_msg = "Invalid http_method:" + http_method;
        SDK_LOG_ERR("%s", err_msg->c_str());
        return -1;
    }
    
    Poco::Net::HTTPResponse res;
    try {
        Poco::URI url(url_str);
        std::unique_ptr<Poco::Net::HTTPClientSession> session;
        if (StringUtil::StringStartsWithIgnoreCase(url_str, "https")) {
            Poco::Net::Context::Ptr context = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE,
                                                                     "", "", "", Poco::Net::Context::VERIFY_RELAXED,
                                                                     9, true, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
            session.reset(new Poco::Net::HTTPSClientSession(url.getHost(), url.getPort(), context));
        } else {
            session.reset(new Poco::Net::HTTPClientSession(url.getHost(), url.getPort()));
        }

        session->setTimeout(Poco::Timespan(0, conn_timeout_in_ms * 1000));

        // 1. 拼接path_query字符串
        std::string path = url.getPath();
        if (path.empty()) {
            path += "/";
        }

        std::string query_str;
        for (std::map<std::string, std::string>::const_iterator c_itr = req_params.begin();
             c_itr != req_params.end(); ++c_itr) {
            std::string part;
            if (c_itr->second.empty()) {
                part = CodecUtil::UrlEncode(c_itr->first) + "&";
            } else {
                part = CodecUtil::UrlEncode(c_itr->first) + "=" + CodecUtil::UrlEncode(c_itr->second) + "&";
            }
            query_str += part;
        }

        if (!query_str.empty()) {
            query_str = "?" + query_str.substr(0, query_str.size() - 1);
        }
        std::string path_and_query_str = CodecUtil::EncodeKey(path) + query_str;

        // 2. 创建http request, 并填充头部
        Poco::Net::HTTPRequest req(http_method, path_and_query_str, Poco::Net::HTTPMessage::HTTP_1_1);
        for (std::map<std::string, std::string>::const_iterator c_itr = req_headers.begin();
             c_itr != req_headers.end(); ++c_itr) {
            req.add(c_itr->first, (c_itr->second).c_str());
        }

        // 3. 计算长度
        std::streampos pos = is.tellg();
        is.seekg(0, std::ios::end);
        req.setContentLength(is.tellg());
        is.seekg(pos);

        std::ostringstream debug_os;
        req.write(debug_os);
        SDK_LOG_DBG("request=[%s]", debug_os.str().c_str());

        // 4. 发送请求
        std::ostream& os = session->sendRequest(req);
        if (http_method == "PUT") {
            // 更新上传进度
            HandleStreamCopier::handleCopyStream(handler, is, os);
        } else {
            Poco::StreamCopier::copyStream(is, os);
        }
        // 5. 接收返回
        Poco::Net::StreamSocket& ss = session->socket();
        ss.setReceiveTimeout(Poco::Timespan(0, recv_timeout_in_ms * 1000));
        std::istream& recv_stream = session->receiveResponse(res);

        // 6. 处理返回
        int ret = res.getStatus();
        resp_headers->insert(res.begin(), res.end());
        // 有些代理可能会把ETag头部修改成Etag,此处修改成ETag
        if (resp_headers->count("Etag") > 0) {
            (*resp_headers)["ETag"] = (*resp_headers)["Etag"];
            resp_headers->erase("Etag");
        }

        std::string etag = "";
        std::map<std::string, std::string>::const_iterator etag_itr
                = resp_headers->find("ETag");
        if (etag_itr != resp_headers->end()) {
            etag = StringUtil::Trim(etag_itr->second, "\"");
        }

        if (is_check_md5 && !StringUtil::IsV4ETag(etag)
            && !StringUtil::IsMultipartUploadETag(etag)) {
            SDK_LOG_DBG("Check Response Md5");
            Poco::MD5Engine md5;
            Poco::DigestOutputStream dos(md5);

            // explicit iostream (streambuf* sb);
            std::stringbuf ibuf;
            std::iostream io_tmp(&ibuf);

            // The Poco session->receiveResponse return the streambuf which dose not overload the base_iostream seekpos which is the realization of the tellg and seekg.
            // It casue the recv_stream can not relocation the begin postion, so can not reuse of the recv_stream.
            // FIXME it might has property issue.
            Poco::StreamCopier::copyStream(recv_stream, io_tmp);

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
                ret = -1;
            }
            
            if (http_method == "GET") {
                // 更新下载进度
                HandleStreamCopier::handleCopyStream(handler, io_tmp, resp_stream);
            } else {
                Poco::StreamCopier::copyStream(io_tmp, resp_stream);
            }
        }else {
            if (http_method == "GET") {
                // 更新下载进度
                HandleStreamCopier::handleCopyStream(handler, recv_stream, resp_stream);
            } else {
                Poco::StreamCopier::copyStream(recv_stream, resp_stream);
            }
        }

        {
            std::string resp_header_log;
            resp_header_log.append("response header :\n");
            for (std::map<std::string, std::string>::const_iterator itr = resp_headers->begin();
                   itr != resp_headers->end(); ++itr) {
               resp_header_log.append(itr->first + ": " + itr->second + "\n");
            }
            SDK_LOG_DBG("%s", resp_header_log.c_str());
        }

        SDK_LOG_INFO("Send request over, status=%d, reason=%s",
                     res.getStatus(), res.getReason().c_str());
        return ret;
    } catch (Poco::Net::NetException& ex){
        SDK_LOG_ERR("Net Exception:%s", ex.displayText().c_str());
        *err_msg = "Net Exception:" + ex.displayText();
        return -1;
    } catch (Poco::TimeoutException& ex) {
        SDK_LOG_ERR("TimeoutException:%s", ex.displayText().c_str());
        *err_msg = "TimeoutException:" + ex.displayText();
        return -1;
    } catch (Poco::AssertionViolationException& ex) {
        // handle the cancel way or other violation exception
        *err_msg = "AssertionViolationException:%s" + ex.displayText();
        return -1;
    } catch (const std::exception &ex) {
        SDK_LOG_ERR("Exception:%s, errno=%d", std::string(ex.what()).c_str(), errno);
        *err_msg = "Exception:" + std::string(ex.what());
        return -1;
    }

    return res.getStatus();
}

} // namespace qcloud_cos
