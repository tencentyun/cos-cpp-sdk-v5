// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 08/12/17
// Description:

#ifndef MOCK_SERVER_H
#define MOCK_SERVER_H
#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/StreamCopier.h"
#include "Poco/Util/ServerApplication.h"

#include "cos_params.h"
#include "util/string_util.h"

namespace qcloud_cos {

const std::string kMockServerName = "MOCK_SERVER";
const std::string kMockObjectTypeNormal = "normal";
const std::string kMockLastModified = "Sat, 22 Jul 2017 08:42:09 GMT";

const std::string kMockHeadContentType  = "application/x-www-form-urlencoded; charset=UTF-8";
const std::string kMockHeadETag = "TEST_HEAD_ETAG";
const std::string kMockHeadReqId = "TEST_HEAD_REQUEST_ID";

const std::string kMockPutObjectContentType = "application/xml";
const std::string kMockPutObjectETag = "TEST_PUT_ETAG";
const std::string kMockPutObjectReqId = "TEST_HEAD_REQUEST_ID";

const std::string kMockGetObjectContentType = "application/octet-stream";
const std::string kMockGetObjectETag = "TEST_GET_ETAG";
const std::string kMockGetObjectReqId = "TEST_GET_OBJECT_REQUEST_ID";

const std::string kMockInitMultiUploadContentType = "application/xml";
const std::string kMockInitMultiUploadReqId = "TEST_INIT_MULTI_UPLOAD_REQUEST_ID";
const std::string kMockUploadId = "upload_id_test";

const std::string kMockUploadPartContentType = "application/xml";
const std::string kMockUploadPartReqId = "TEST_UPLOAD_PART_REQUEST_ID";
const std::string kMockUploadPartETag = "TEST_UPLOAD_PART_ETAG";

const std::string kMockCompleteETag = "TEST_COMPLETE_ETAG";
const std::string kMockCompleteReqId = "TEST_COMPLETE_REQUEST_ID";
const std::string kMockCompleteContentType = "application/xml";

const std::string kMockAbortReqId = "TEST_ABORT_REQUEST_ID";

const std::string kMockPutBucketReqId = "TEST_PUT_BUCKET_REQUEST_ID";
const std::string kMockPutBucketReplicationReqId = "TEST_PUT_BUCKET_REPLICATION_REQUEST_ID";
const std::string kMockGetBucketReplicationReqId = "TEST_GET_BUCKET_REPLICATION_REQUEST_ID";
const std::string kMockDeleteBucketReplicationReqId = "TEST_DELETE_BUCKET_REPLICATION_REQUEST_ID";

class MockRequestHandler : public Poco::Net::HTTPRequestHandler {
public:
    virtual void handleRequest(Poco::Net::HTTPServerRequest& req,
                               Poco::Net::HTTPServerResponse& resp) {
        try {
            std::string host = req.getHost();
            std::string method = req.getMethod();
            std::string uri = req.getURI();
            std::string bucket_name, region;
            uint64_t app_id;
            if (!resolveHost(host, &region, &bucket_name, &app_id)) {
                handleErrorRequest(req, resp);
                return;
            }

            // UT先这么简单判断
            if ("GET" == method) {
                if (StringUtil::StringStartsWith(uri, "/?replication")) {
                    handleGetBucketReplicationRequest(req, resp);
                } else if (StringUtil::StringStartsWith(uri, "/?lifecycle")) {
                    // TODO(sevenyou)
                    //handleGetBucketLifecycleRequest(req, resp);
                } else if ("/" == uri || StringUtil::StringStartsWith(uri, "/?")) {
                    handleGetBucketRequest(req, resp);
                } else {
                    handleGetObjectRequest(req, resp);
                }
            } else if ("POST" == method) {
                if (uri.find("uploads") != std::string::npos) {
                    handleInitMultiUploadRequest(req, resp);
                } else if (uri.find("uploadId") != std::string::npos) {
                    handleCompMultiUploadRequest(req, resp);
                }
            } else if ("PUT" == method) {
                if (uri.find("partNumber") != std::string::npos
                    && uri.find("uploadId") != std::string::npos) {
                    handleUploadPartRequest(req, resp);
                } else if (StringUtil::StringStartsWith(uri, "?replication")) {
                    handlePutBucketReplicationRequest(req, resp);
                } else if (StringUtil::StringStartsWith(uri, "?lifecycle")) {
                    // TODO(sevenyou)
                    // handlePutBucketLifecycleRequest(req, resp);
                } else {
                    handlePutObjectRequest(req, resp);
                }
            } else if ("HEAD" == method) {
                handleHeadObjectRequest(req, resp);
            } else if ("DELETE" == method) {
                if (StringUtil::StringStartsWith(uri, "?replication")) {
                    handleDeleteBucketReplicationRequest(req, resp);
                } else if (StringUtil::StringStartsWith(uri, "?lifecycle")) {
                    // TODO(sevenyou)
                    // handleDeleteBucketLifecycleRequest(req, resp);
                } else {
                    handleAbortMultiUploadRequest(req, resp);
                }
            }
        } catch (const Poco::NotFoundException& ex) {
            std::cout << "exception" << std::endl;
        } catch (const std::exception& ex) {
            std::cout << "exception" << std::endl;
        }
    }

private:

    bool resolveHost(const std::string& host, std::string* region,
                     std::string* bucket_name, uint64_t* app_id) {
        size_t pos = host.find(".");
        size_t pos2 = host.find(".", pos + 1);
        if (std::string::npos == pos) {
            return false;
        }
        std::string app_bucket = host.substr(0, pos);
        *region = host.substr(pos + 1, pos2 - pos - 1);
        pos = app_bucket.find('-');
        if(std::string::npos == pos) {
            return false;
        }

        *bucket_name = app_bucket.substr(0, pos);

        std::string app_id_str = app_bucket.substr(pos + 1);
        if (!isLegalAppId(app_id_str)) {
            return false;
        }

        *app_id = StringUtil::StringToUint64(app_id_str);
        return true;
    }

    bool isLegalAppId(const std::string& app_id) {
        for (std::string::const_iterator itr = app_id.begin(); itr < app_id.end(); ++itr) {
            if (*itr < '0' || *itr > '9') {
                return false;
            }
        }
        return true;
    }

    void handleUnkonownRequest(Poco::Net::HTTPServerRequest& req,
                               Poco::Net::HTTPServerResponse& resp) {
        std::cout << "Unknown Request" << std::endl;
    }

    void handleErrorRequest(Poco::Net::HTTPServerRequest& req,
                            Poco::Net::HTTPServerResponse& resp) {
        resp.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
        resp.setContentType("application/xml");
        resp.add("Server", kMockServerName);
        std::ostream& out = resp.send();
        out << "<Error>\n"
            << "<Code>InvalidBucketName</Code>\n"
            << "<Message>bucket name is invalid</Message>\n"
            << "<Resource>error_request_resource</Resource>\n"
            << "<RequestId>error_request_id</RequestId>\n"
            << "<TraceId>error_request_trace_id</TraceId>\n"
            << "</Error>";
        out.flush();
    }

    void handleHeadObjectRequest(Poco::Net::HTTPServerRequest& req,
                                 Poco::Net::HTTPServerResponse& resp) {
        resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        resp.setContentType(kMockHeadContentType);
        resp.add("ETag", kMockHeadETag);
        resp.add("Server", kMockServerName);
        resp.add("Last-Modified", kMockLastModified);
        resp.add("x-cos-object-type", kMockObjectTypeNormal);
        resp.add("x-cos-request-id", kMockHeadReqId);
        resp.add("Content-Length", "1048576");
        std::ostream& out = resp.send();
        out.flush();
    }

    void handleGetObjectRequest(Poco::Net::HTTPServerRequest& req,
                                Poco::Net::HTTPServerResponse& resp) {
        resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        resp.setContentType(kMockGetObjectContentType);
        resp.add("ETag", kMockGetObjectETag);
        resp.add("Server", kMockServerName);
        resp.add("x-cos-storage-class", kStorageClassStandardIA);
        resp.add("x-cos-object-type", kMockObjectTypeNormal);
        resp.add("x-cos-request-id", kMockGetObjectReqId);
        std::ostream& out = resp.send();
        for (int i = 0; i < 1024; ++i) {
            std::string temp(1024, 'a');
            out << temp;
        }
        out.flush();
    }

    void handlePutObjectRequest(Poco::Net::HTTPServerRequest& req,
                                Poco::Net::HTTPServerResponse& resp) {
        // 象征性接收一下,免得client SIGPIPE
        std::istream& is = req.stream();
        std::fstream ofs("/dev/null");
        Poco::StreamCopier::copyStream(is, ofs, req.getContentLength());

        resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        resp.setContentType(kMockPutObjectContentType);
        resp.add("ETag", kMockPutObjectETag);
        resp.add("Server", kMockServerName);
        std::ostream& out = resp.send();
        out.flush();
    }

    void handleInitMultiUploadRequest(Poco::Net::HTTPServerRequest& req,
                                      Poco::Net::HTTPServerResponse& resp) {
        resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        resp.setContentType(kMockInitMultiUploadContentType);
        resp.add("ETag", kMockPutObjectETag);
        resp.add("Server", kMockServerName);
        resp.add("x-cos-request-id", kMockInitMultiUploadReqId);
        std::ostream& out = resp.send();
        out << "<InitiateMultipartUploadResult>\n"
            << "<Bucket>bucket_test</Bucket>\n"
            << "<Key>object_test_multi</Key>\n"
            << "<UploadId>" << kMockUploadId << "</UploadId>\n"
            << "</InitiateMultipartUploadResult>";
        out.flush();
    }

    void handleUploadPartRequest(Poco::Net::HTTPServerRequest& req,
                                 Poco::Net::HTTPServerResponse& resp) {
        // 象征性接收一下,免得client SIGPIPE
        std::istream& is = req.stream();
        std::fstream ofs("/dev/null");
        Poco::StreamCopier::copyStream(is, ofs, req.getContentLength());

        resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        resp.setContentType(kMockUploadPartContentType);
        resp.add("ETag", kMockUploadPartETag);
        resp.add("Server", kMockServerName);
        resp.add("x-cos-request-id", kMockUploadPartReqId);
        std::ostream& out = resp.send();
        out.flush();
    }

    void handleCompMultiUploadRequest(Poco::Net::HTTPServerRequest& req,
                                      Poco::Net::HTTPServerResponse& resp) {
        resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        resp.setContentType(kMockCompleteContentType);
        resp.add("Server", kMockServerName);
        resp.add("x-cos-request-id", kMockCompleteReqId);
        std::ostream& out = resp.send();
        out << "<CompleteMultipartUploadResult>\n"
            << "<Location>bucket_test-7777.cn-north.myqcloud.com/object_test_multi</Location>\n"
            << "<Bucket>bucket_test</Bucket>\n"
            << "<Key>object_test_multi</Key>\n"
            << "<ETag>" << kMockCompleteETag << "</ETag>\n"
            << "</CompleteMultipartUploadResult>";
        out.flush();
    }

    void handleAbortMultiUploadRequest(Poco::Net::HTTPServerRequest& req,
                                       Poco::Net::HTTPServerResponse& resp) {
        resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        resp.setContentType(kMockCompleteContentType);
        resp.add("Server", kMockServerName);
        resp.add("x-cos-request-id", kMockAbortReqId);
        std::ostream& out = resp.send();
        out.flush();
    }

    void handleGetBucketRequest(Poco::Net::HTTPServerRequest& req,
                                Poco::Net::HTTPServerResponse& resp) {
        resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        resp.setContentType(kMockCompleteContentType);
        resp.add("Server", kMockServerName);
        resp.add("x-cos-request-id", kMockCompleteReqId);
        std::ostream& out = resp.send();
        out << "<ListBucketResult>\n"
            << "<Name>bucket_test</Name>\n"
            << "<Encoding-Type>url</Encoding-Type>\n"
            << "<Prefix>test/</Prefix>\n"
            << "<Marker/>\n"
            << "<MaxKeys>100</MaxKeys>\n"
            << "<Delimiter>/</Delimiter>\n"
            << "<IsTruncated>false</IsTruncated>\n"
            << "<NextMarker>1234.txt</NextMarker>\n"
            << "<Contents>\n"
            << "<Key>sevenyoutest01</Key>\n"
            << "<LastModified>2017-06-23T12:33:26.000Z</LastModified>\n"
            << "<ETag>39bfb88c11c65ed6424d2e1cd4db1826</ETag>\n"
            << "<Size>10485760</Size>\n"
            << "<Owner>\n"
            << "<ID>77777</ID>\n"
            << "</Owner>\n"
            << "<StorageClass>STANDARD</StorageClass>\n"
            << "</Contents>\n"
            << "<Contents>\n"
            << "<Key></Key>\n"
            << "<LastModified>2017-06-23T12:33:26.000Z</LastModified>\n"
            << "<ETag>fb31459ad10289ff49327fd91a3e1f6a</ETag>\n"
            << "<Size>4</Size>\n"
            << "<Owner>\n"
            << "<ID>88888</ID>\n"
            << "</Owner>\n"
            << "<StorageClass>STANDARD</StorageClass>\n"
            << "</Contents>\n"
            << "</ListBucketResult>";
        out.flush();
    }

    void handlePutBucketRequest(Poco::Net::HTTPServerRequest& req,
                                Poco::Net::HTTPServerResponse& resp) {
        resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        resp.add("Server", kMockServerName);
        resp.add("x-cos-request-id", kMockPutBucketReqId);
        std::ostream& out = resp.send();
        out.flush();
    }

    void handlePutBucketReplicationRequest(Poco::Net::HTTPServerRequest& req,
                                           Poco::Net::HTTPServerResponse& resp) {
        resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        resp.add("Server", kMockServerName);
        resp.add("x-cos-request-id", kMockPutBucketReplicationReqId);
        std::ostream& out = resp.send();
        out.flush();
    }

    void handleGetBucketReplicationRequest(Poco::Net::HTTPServerRequest& req,
                                            Poco::Net::HTTPServerResponse& resp) {
        resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        resp.add("Server", kMockServerName);
        resp.add("x-cos-request-id", kMockGetBucketReplicationReqId);
        std::ostream& out = resp.send();
        out << "<ReplicationConfiguration>\n"
            << "<Role>qcs::cam::uin/12345:uin/12345</Role>\n"
            << "<Rule>\n"
            << "<ID>rule_00</ID>\n"
            << "<Status>Enabled</Status>\n"
            << "<Prefix>prefix_00</Prefix>\n"
            << "<Destination>\n"
            << "<Bucket>qcs:id/0:cos:cn-south:appid/56789:dest_bucket_00</Bucket>\n"
            << "<StorageClass>Standard</StorageClass>\n"
            << "</Destination>\n"
            << "</Rule>\n"
            << "<Rule>\n"
            << "<ID>rule_01</ID>\n"
            << "<Status>Disabled</Status>\n"
            << "<Prefix>prefix_01</Prefix>\n"
            << "<Destination>\n"
            << "<Bucket>qcs:id/0:cos:cn-south:appid/19456:sevenyousouthtest</Bucket>\n"
            << "<StorageClass>Standard_IA</StorageClass>\n"
            << "</Destination>\n"
            << "</Rule>\n"
            << "</ReplicationConfiguration>";
        out.flush();
    }

    void handleDeleteBucketReplicationRequest(Poco::Net::HTTPServerRequest& req,
                                               Poco::Net::HTTPServerResponse& resp) {
        resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        resp.add("Server", kMockServerName);
        resp.add("x-cos-request-id", kMockDeleteBucketReplicationReqId);
        std::ostream& out = resp.send();
        out.flush();
    }
};

class MockRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
public:
    virtual Poco::Net::HTTPRequestHandler* createRequestHandler(
            const Poco::Net::HTTPServerRequest &) {
        return new MockRequestHandler();
    }
};

} // namespace qcloud_cos
#endif
