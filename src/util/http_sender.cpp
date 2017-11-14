// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/18/17
// Description:

#include "util/http_sender.h"

#include <sys/time.h>

#include <iostream>
#include <sstream>

#include "boost/scoped_ptr.hpp"
#include "Poco/Net/Context.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/NetException.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"

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
                            std::string* err_msg) {
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
                          err_msg);
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
                            std::string* err_msg) {
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
                          err_msg);
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
                            std::string* err_msg) {
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
                          err_msg);
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
                            std::string* err_msg) {
    Poco::Net::HTTPResponse res;
    try {
        Poco::URI url(url_str);
        boost::scoped_ptr<Poco::Net::HTTPClientSession> session;
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
                part = c_itr->first + "&";
            } else {
                part = c_itr->first + "=" + c_itr->second + "&";
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

#ifdef __COS_DEBUG__
        std::ostringstream debug_os;
        req.write(debug_os);
        SDK_LOG_DBG("request=[%s]", debug_os.str().c_str());
#endif

        // 4. 发送请求
        std::ostream& os = session->sendRequest(req);
        Poco::StreamCopier::copyStream(is, os);

        // 5. 接收返回
        Poco::Net::StreamSocket& ss = session->socket();
        ss.setReceiveTimeout(Poco::Timespan(0, recv_timeout_in_ms * 1000));
        std::istream& recv_stream = session->receiveResponse(res);

        // 6. 处理返回
        // Poco::StreamCopier::copyToString(recv_stream, *resp_body);
        Poco::StreamCopier::copyStream(recv_stream, resp_stream);
        resp_headers->insert(res.begin(), res.end());
#ifdef __COS_DEBUG__
        SDK_LOG_DBG("response header :\n");
        for (std::map<std::string, std::string>::const_iterator itr = resp_headers->begin();
             itr != resp_headers->end(); ++itr) {
            SDK_LOG_DBG("key=[%s], value=[%s]\n", itr->first.c_str(), itr->second.c_str());
        }
#endif
        SDK_LOG_INFO("Send request over, status=%d, reason=%s",
                res.getStatus(), res.getReason().c_str());
        return res.getStatus();
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
                            std::string* err_msg) {
    Poco::Net::HTTPResponse res;
    try {
        Poco::URI url(url_str);
        boost::scoped_ptr<Poco::Net::HTTPClientSession> session;
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
                part = c_itr->first + "&";
            } else {
                part = c_itr->first + "=" + c_itr->second + "&";
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
        req.add("Content-Length", StringUtil::Uint64ToString(req_body.size()));

#ifdef __COS_DEBUG__
        std::ostringstream debug_os;
        req.write(debug_os);
        SDK_LOG_DBG("request=[%s]", debug_os.str().c_str());
#endif

        // 3. 发送请求
        std::ostream& os = session->sendRequest(req);
        if (!req_body.empty()) {
            os << req_body;
        }

        // 4. 接收返回
        Poco::Net::StreamSocket& ss = session->socket();
        ss.setReceiveTimeout(Poco::Timespan(0, recv_timeout_in_ms * 1000));
        std::istream& recv_stream = session->receiveResponse(res);

        // 6. 处理返回
        if (res.getStatus() != 200 && res.getStatus() != 206) {
            Poco::StreamCopier::copyToString(recv_stream, *xml_err_str);
        } else {
            Poco::StreamCopier::copyStream(recv_stream, resp_stream);
        }

        resp_headers->insert(res.begin(), res.end());
#ifdef __COS_DEBUG__
        SDK_LOG_DBG("response header :\n");
        for (std::map<std::string, std::string>::const_iterator itr = resp_headers->begin();
             itr != resp_headers->end(); ++itr) {
            SDK_LOG_DBG("key=[%s], value=[%s]\n", itr->first.c_str(), itr->second.c_str());
        }
#endif
        SDK_LOG_INFO("Send request over, status=%d, reason=%s",
                res.getStatus(), res.getReason().c_str());
        return res.getStatus();
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
// TODO(sevenyou) 挪走
uint64_t HttpSender::GetTimeStampInUs() {
    // 构造时间
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

} // namespace qcloud_cos
