// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/18/17
// Description:

#include "util/http_sender.h"

#if defined(WIN32)
#include <time.h>
#else
#include <sys/time.h>
#endif

#include <iostream>
#include <sstream>

#include "boost/scoped_ptr.hpp"
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

#include "cos_config.h"
#include "cos_sys_config.h"
#include "util/string_util.h"
#include "util/codec_util.h"

namespace qcloud_cos {

// Trsf handler
int HttpSender::SendRequest(const MultiUploadObjectReq *req,
                            const std::string& http_method,
                            const std::string& url_str,
                            const std::map<std::string, std::string>& req_params,
                            const std::map<std::string, std::string>& req_headers,
                            const std::string& req_body,
                            Poco::SharedPtr<TransferHandler>& handler,
                            uint64_t conn_timeout_in_ms,
                            uint64_t recv_timeout_in_ms,
                            std::map<std::string, std::string>* resp_headers,
                            std::string* resp_body,
                            std::string* err_msg,
                            bool is_check_md5) {
    std::istringstream is(req_body);
    std::ostringstream oss;
    int ret = SendRequest(req,
                          http_method,
                          url_str,
                          req_params,
                          req_headers,
                          is,
                          conn_timeout_in_ms,
                          recv_timeout_in_ms,
                          resp_headers,
                          oss,
                          err_msg,
                          handler,
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
        int ret = res.getStatus();
        resp_headers->insert(res.begin(), res.end());

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
            Poco::StreamCopier::copyStream(io_tmp, resp_stream);
        }else {
            Poco::StreamCopier::copyStream(recv_stream, resp_stream);
        }

#ifdef __COS_DEBUG__
        SDK_LOG_DBG("response header :\n");
        for (std::map<std::string, std::string>::const_iterator itr = resp_headers->begin();
             itr != resp_headers->end(); ++itr) {
            SDK_LOG_DBG("key=[%s], value=[%s]\n", itr->first.c_str(), itr->second.c_str());
        }
#endif
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
                            bool is_check_md5) {
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
        int ret = res.getStatus();
        resp_headers->insert(res.begin(), res.end());
        if (ret != 200 && ret != 206) {
            Poco::StreamCopier::copyToString(recv_stream, *xml_err_str);
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
                Poco::StreamCopier::copyStream(io_tmp, resp_stream);
            }else { // other way direct use the recv_stream
                Poco::StreamCopier::copyStream(recv_stream, resp_stream);
            }

        }

#ifdef __COS_DEBUG__
        SDK_LOG_DBG("response header :\n");
        for (std::map<std::string, std::string>::const_iterator itr = resp_headers->begin();
             itr != resp_headers->end(); ++itr) {
            SDK_LOG_DBG("key=[%s], value=[%s]\n", itr->first.c_str(), itr->second.c_str());
        }
#endif
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

// Real trsf handler process
int HttpSender::SendRequest(const MultiUploadObjectReq *objreq,
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
                            Poco::SharedPtr<TransferHandler>& handler,
                            bool is_check_md5) {
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

#ifdef __COS_DEBUG__
        std::ostringstream debug_os;
        req.write(debug_os);
        SDK_LOG_DBG("request=[%s]", debug_os.str().c_str());
#endif

        // 4. 发送请求
        std::ostream& os = session->sendRequest(req);
        // According to the copyStream to handle the process
        // TODO overwrite the copyStream insider to record
        // Poco::StreamCopier::copyStream(is, os);
        HandleStreamCopier::handleCopyStream(objreq, is, os, handler);

        // 5. 接收返回
        Poco::Net::StreamSocket& ss = session->socket();
        ss.setReceiveTimeout(Poco::Timespan(0, recv_timeout_in_ms * 1000));
        std::istream& recv_stream = session->receiveResponse(res);

        // 6. 处理返回
        int ret = res.getStatus();
        resp_headers->insert(res.begin(), res.end());

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
            Poco::StreamCopier::copyStream(io_tmp, resp_stream);
        }else {
            Poco::StreamCopier::copyStream(recv_stream, resp_stream);
        }

#ifdef __COS_DEBUG__
        SDK_LOG_DBG("response header :\n");
        for (std::map<std::string, std::string>::const_iterator itr = resp_headers->begin();
             itr != resp_headers->end(); ++itr) {
            SDK_LOG_DBG("key=[%s], value=[%s]\n", itr->first.c_str(), itr->second.c_str());
        }
#endif
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

// Must notice for now the sign auth time is second.
// the time function in windows can only support to second,
// also can use the GetSystemTimeAsFileTime which the resolution is only 15625 microseconds
// (if you make successive calls to gettimeofday() until it changes value).
// It's not much better than calling GetLocalTime() which is accurate to between 15000 and 31000 microseconds.
// This differs significantly from the unix implementations which are accurate close to the microsecond.
uint64_t HttpSender::GetTimeStampInUs() {
    // 构造时间
#if defined(WIN32)
	time_t ltime;
	time(&ltime);
	return (uint64_t)(ltime * 1000000);
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
#endif
}

} // namespace qcloud_cos
