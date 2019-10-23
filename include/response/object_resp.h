// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:

#ifndef OBJECT_RESP_H
#define OBJECT_RESP_H
#pragma once

#include <vector>

#include "cos_config.h"
#include "cos_params.h"
#include "response/base_resp.h"
#include <iostream>

namespace qcloud_cos {

class GetObjectResp : public BaseResp {
public:
    /// \brief 获取object最后被修改的时间, 字符串格式Date, 类似"Wed, 28 Oct 2014 20:30:00 GMT"
    std::string GetLastModified() const { return m_last_modified; }
    void SetLastModified(const std::string& last_modified) {
        m_last_modified = last_modified;
    }

    /// \brief 获取object type, 表示object是否可以被追加上传，枚举值：normal 或者 appendable
    std::string GetXCosObjectType() const { return m_x_cos_object_type; }
    void SetXCosObjectType(const std::string& x_cos_object_type) {
        m_x_cos_object_type = x_cos_object_type;
    }

    /// \brief 获取Object 的存储级别，枚举值：STANDARD，STANDARD_IA
    std::string GetXCosStorageClass() const { return m_x_cos_storage_class; }
    void SetXCosStorageClass(const std::string& x_cos_storage_class) {
        m_x_cos_storage_class = x_cos_storage_class;
    }

    /// \brief 获取自定义的元数据
    std::map<std::string, std::string> GetXCosMetas() const { return m_x_cos_metas; }
    /// \brief 获取自定义的元数据
    std::string GetXCosMeta(const std::string& key) const {
        std::map<std::string, std::string>::const_iterator itr = m_x_cos_metas.find(key);
        if (itr != m_x_cos_metas.end()) {
            return itr->second;
        }
        return "";
    }
    void SetXCosMeta(const std::string& key, const std::string& value) {
        m_x_cos_metas[key] = value;
    }

    /// \brief Server端加密使用的算法
    std::string GetXCosServerSideEncryption() const {
        return GetHeader("x-cos-server-side-encryption");
    }

    void ParseFromHeaders(const std::map<std::string, std::string>& headers);

protected:
    GetObjectResp() {}
    virtual ~GetObjectResp() {}

private:
    std::string m_last_modified;
    std::string m_x_cos_object_type;
    std::string m_x_cos_storage_class;
    std::map<std::string, std::string> m_x_cos_metas;
};

class GetObjectByStreamResp : public GetObjectResp {
public:
    GetObjectByStreamResp() {}
    virtual ~GetObjectByStreamResp() {}
};

class GetObjectByFileResp : public GetObjectResp {
public:
    GetObjectByFileResp() {}
    virtual ~GetObjectByFileResp() {}
};

class MultiGetObjectResp : public GetObjectResp {
public:
    MultiGetObjectResp() {}
    virtual ~MultiGetObjectResp() {}

    /// Server端加密使用的算法
    std::string GetXCosServerSideEncryption() const {
        return GetHeader("x-cos-server-side-encryption");
    }
};

class PutObjectResp : public BaseResp {
protected:
    PutObjectResp() {}
    virtual ~PutObjectResp() {}

public:
    /// 获取Object的版本号, 如果Bucket未开启多版本, 返回空字符串
    std::string GetVersionId() const { return GetHeader("x-cos-version-id"); }

    /// Server端加密使用的算法
    std::string GetXCosServerSideEncryption() const {
        return GetHeader("x-cos-server-side-encryption");
    }
};

class PutObjectByStreamResp : public PutObjectResp {
public:
    PutObjectByStreamResp() {}
    virtual ~PutObjectByStreamResp() {}
};

class PutObjectByFileResp : public PutObjectResp {
public:
    PutObjectByFileResp() {}
    virtual ~PutObjectByFileResp() {}
};

class DeleteObjectResp : public BaseResp {
public:
    DeleteObjectResp() {}
    virtual ~DeleteObjectResp() {}
};

class DeleteObjectsResp : public BaseResp {
public:
    DeleteObjectsResp() {}
    virtual ~DeleteObjectsResp() {}

    std::vector<DeletedInfo> GetDeletedInfos() const {
        return m_deleted_infos;
    }

    std::vector<ErrorInfo> GetErrorinfos() const {
        return m_error_infos;
    }

    virtual bool ParseFromXmlString(const std::string& body);

private:
    std::vector<DeletedInfo> m_deleted_infos;
    std::vector<ErrorInfo> m_error_infos;
};

class HeadObjectResp : public BaseResp {
public:
    HeadObjectResp() : m_x_cos_storage_class(kStorageClassStandard) {}
    virtual ~HeadObjectResp() {}

    /// \brief 获取object最后被修改的时间, 字符串格式Date, 类似"Wed, 28 Oct 2014 20:30:00 GMT"
    std::string GetLastModified() const { return m_last_modified; }
    void SetLastModified(const std::string& last_modified) {
        m_last_modified = last_modified;
    }

    /// \brief 获取object type, 表示object是否可以被追加上传，枚举值：normal 或者 appendable
    std::string GetXCosObjectType() const { return m_x_cos_object_type; }
    void SetXCosObjectType(const std::string& x_cos_object_type) {
        m_x_cos_object_type = x_cos_object_type;
    }

    /// \brief 获取Object 的存储级别，枚举值：STANDARD，STANDARD_IA
    std::string GetXCosStorageClass() const { return m_x_cos_storage_class; }
    void SetXCosStorageClass(const std::string& x_cos_storage_class) {
        m_x_cos_storage_class = x_cos_storage_class;
    }

    /// \brief 获取自定义的元数据
    std::map<std::string, std::string> GetXCosMetas() const { return m_x_cos_metas; }
    /// \brief 获取自定义的元数据
    std::string GetXCosMeta(const std::string& key) const {
        std::map<std::string, std::string>::const_iterator itr = m_x_cos_metas.find(key);
        if (itr != m_x_cos_metas.end()) {
            return itr->second;
        }
        return "";
    }
    void SetXCosMeta(const std::string& key, const std::string& value) {
        m_x_cos_metas[key] = value;
    }

    /// \brief 获得 archive 类型对象的当前恢复状态
    std::string GetXCosRestore() const {
        return GetHeader("x-cos-restore");
    }

    /// \brief Server端加密使用的算法
    std::string GetXCosServerSideEncryption() const {
        return GetHeader("x-cos-server-side-encryption");
    }

    void ParseFromHeaders(const std::map<std::string, std::string>& headers);

private:
    std::string m_last_modified;
    std::string m_x_cos_object_type;
    std::string m_x_cos_storage_class;
    std::map<std::string, std::string> m_x_cos_metas;
};

class InitMultiUploadResp : public BaseResp {
public:
    InitMultiUploadResp() {}
    virtual ~InitMultiUploadResp() {}

    virtual bool ParseFromXmlString(const std::string& body);

    /// \brief 获取uploadId, 用于后续上传
    std::string GetUploadId() const { return m_upload_id; }

    /// \brief 获取Object的名称
    std::string GetKey() const { return m_key; }

    /// \brief 分片上传的目标Bucket
    std::string GetBucket() const { return m_bucket; }

    /// \brief Server端加密使用的算法
    std::string GetXCosServerSideEncryption() const {
        return GetHeader("x-cos-server-side-encryption");
    }

private:
    std::string m_bucket;
    std::string m_key; // object名称
    std::string m_upload_id;
};

class UploadPartDataResp : public BaseResp {
public:
    UploadPartDataResp() {}
    virtual ~UploadPartDataResp() {}

    /// \brief Server端加密使用的算法
    std::string GetXCosServerSideEncryption() const {
        return GetHeader("x-cos-server-side-encryption");
    }
};

class UploadPartCopyDataResp : public BaseResp {
public:
    UploadPartCopyDataResp() {}
    virtual ~UploadPartCopyDataResp() {}

    virtual bool ParseFromXmlString(const std::string& body);

    /// \brief 获取返回文件的MD5算法校验值。
    ///        ETag 的值可以用于检查 Object 的内容是否发生变化。
    std::string GetEtag() const { return m_etag; }

    /// \brief 返回文件最后修改时间，GMT 格式
    std::string GetLastModified() const { return m_last_modified; }

    /// \brief Server端加密使用的算法
    std::string GetXCosServerSideEncryption() const {
        return GetHeader("x-cos-server-side-encryption");
    }

private:
    std::string m_etag;
    std::string m_last_modified;
};

class CompleteMultiUploadResp : public BaseResp {
public:
    CompleteMultiUploadResp() {}
    virtual ~CompleteMultiUploadResp() {}

    virtual bool ParseFromXmlString(const std::string& body);

    std::string GetLocation() const { return m_location; }
    std::string GetKey() const { return m_key; }
    std::string GetBucket() const { return m_bucket; }
    std::string GetVersionId() const { return GetHeader("x-cos-version-id"); }

    /// \brief Server端加密使用的算法
    std::string GetXCosServerSideEncryption() const {
        return GetHeader("x-cos-server-side-encryption");
    }

private:
    std::string m_location; // Object的外网访问域名
    std::string m_bucket;
    std::string m_key;
};

class MultiUploadObjectResp : public BaseResp {
public:
    MultiUploadObjectResp() {}

    virtual ~MultiUploadObjectResp() {}

    std::string GetRespTag() { return m_resp_tag; }

    std::string GetLocation() const { return m_location; }

    std::string GetKey() const { return m_key; }

    std::string GetBucket() const { return m_bucket; }

    void SetLocation(const std::string& location) { m_location = location; }

    void SetKey(const std::string& key) { m_key = key; }

    void SetBucket(const std::string& bucket) { m_bucket = bucket; }

    void CopyFrom(const InitMultiUploadResp& resp);

    void CopyFrom(const UploadPartDataResp& resp);

    void CopyFrom(const CompleteMultiUploadResp& resp);

    /// \brief Server端加密使用的算法
    std::string GetXCosServerSideEncryption() const {
        return GetHeader("x-cos-server-side-encryption");
    }

private:
    std::string m_location; // Object的外网访问域名
    std::string m_bucket;
    std::string m_key;
    std::string m_upload_id;

    // FIXME(sevenyou) 先这么搞吧
    std::string m_resp_tag; // 用于区分是哪一种response
};

class AbortMultiUploadResp : public BaseResp {
public:
    AbortMultiUploadResp() {}
    virtual ~AbortMultiUploadResp() {}
};

class ListPartsResp : public BaseResp {
public:
    ListPartsResp() : m_bucket(""), m_encoding_type(""), m_key(""), m_upload_id(""),
        m_part_number_marker(0), m_parts(0), m_next_part_number_marker(0),
        m_storage_class(""), m_max_parts(1000), m_is_truncated(false) {}
    virtual ~ListPartsResp() {}

    virtual bool ParseFromXmlString(const std::string& body);

    std::string GetBucket() const { return m_bucket; }

    std::string GetEncodingType() const { return m_encoding_type; }

    std::string GetKey() const { return m_key; }

    std::string GetUploadId() const { return m_upload_id; }

    Initiator GetInitiator() const { return m_initiator; }

    Owner GetOwner() const { return m_owner; }

    uint64_t GetPartNumberMarker() const { return m_part_number_marker; }

    std::vector<Part> GetParts() const { return m_parts; }

    uint64_t GetNextPartNumberMarker() const { return m_next_part_number_marker; }

    std::string GetStorageClass() const { return m_storage_class; }

    uint64_t GetMaxParts() const { return m_max_parts; }

    bool IsTruncated() const { return m_is_truncated; }

private:
    std::string m_bucket;
    std::string m_encoding_type;
    std::string m_key;
    std::string m_upload_id;
    Initiator m_initiator;
    Owner m_owner;
    uint64_t m_part_number_marker;
    std::vector<Part> m_parts;
    uint64_t m_next_part_number_marker;
    std::string m_storage_class;
    uint64_t m_max_parts;
    bool m_is_truncated;
};

class GetObjectACLResp : public BaseResp {
public:
    GetObjectACLResp() {}
    virtual ~GetObjectACLResp() {}

    virtual bool ParseFromXmlString(const std::string& body);

    std::string GetOwnerID() const { return m_owner_id; }
    std::string GetOwnerDisplayName() const { return m_owner_display_name; }
    std::vector<Grant> GetAccessControlList() const { return m_acl; }

private:
    std::string m_owner_id;
    std::string m_owner_display_name;
    std::vector<Grant> m_acl;
};

class PutObjectACLResp : public BaseResp {
public:
    PutObjectACLResp() {}
    virtual ~PutObjectACLResp() {}
};

class PutObjectCopyResp : public BaseResp {
public:
    PutObjectCopyResp() {}
    virtual ~PutObjectCopyResp() {}

    virtual bool ParseFromXmlString(const std::string& body);

    std::string GetEtag() const { return m_etag; }
    std::string GetLastModified() const { return m_last_modified; }
    std::string GetVersionId() const { return m_version_id; }

    /// Server端加密使用的算法
    std::string GetXCosServerSideEncryption() const {
        return GetHeader("x-cos-server-side-encryption");
    }

private:
    std::string m_etag;
    std::string m_last_modified;
    std::string m_version_id;
};

class CopyResp : public BaseResp {
public:
    CopyResp() : m_location(""), m_bucket(""), m_key(""),
        m_etag(""), m_last_modified(""), m_version_id(""), m_resp_tag("") {}
    virtual ~CopyResp() {}

    void CopyFrom(const PutObjectCopyResp& resp);

    void CopyFrom(const CompleteMultiUploadResp& resp);

    void Clear() {
        m_location = "";
        m_bucket = "";
        m_key = "";
        m_etag = "";
        m_last_modified = "";
        m_version_id = "";
        m_resp_tag = "";
    }

    std::string GetLocation() const { return m_location; }
    std::string GetKey() const { return m_key; }
    std::string GetBucket() const { return m_bucket; }
    std::string GetEtag() const { return m_etag; }
    std::string GetLastModified() const { return m_last_modified; }
    std::string GetVersionId() const { return m_version_id; }
    std::string GetRespTag() { return m_resp_tag; }

private:
    std::string m_location; // Object的外网访问域名
    std::string m_bucket;
    std::string m_key;
    std::string m_etag;
    std::string m_last_modified;
    std::string m_version_id;

    std::string m_resp_tag; // 用于区分是哪一种response
};

class PostObjectRestoreResp : public BaseResp {
public:
    PostObjectRestoreResp() {}
    ~PostObjectRestoreResp() {}
};

} // namespace qcloud_cos
#endif // OBJECT_RESP_H
