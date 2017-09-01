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

    /// \brief 获取Object 的存储级别，枚举值：STANDARD，STANDARD_IA，NEARLINE
    std::string GetXCosStorageClass() const { return m_x_cos_storage_class; }
    void SetXCosStorageClass(const std::string& x_cos_storage_class) {
        m_x_cos_storage_class = x_cos_storage_class;
    }

    /// \brief 获取自定义的元数据
    std::map<std::string, std::string> GetXCosMetas() const { return m_x_cos_metas; }
    /// \brief 获取自定义的元数据
    std::string GetXCosMeta(const std::string& key) const {
        std::map<std::string, std::string>::const_iterator itr = m_x_cos_metas.begin();
        if (itr != m_x_cos_metas.end()) {
            return itr->second;
        }
        return "";
    }
    void SetXCosMeta(const std::string& key, const std::string& value) {
        m_x_cos_metas[key] = value;
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
};

class PutObjectResp : public BaseResp {
protected:
    PutObjectResp() {}
    virtual ~PutObjectResp() {}
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

    /// \brief 获取Object 的存储级别，枚举值：STANDARD，STANDARD_IA，NEARLINE
    std::string GetXCosStorageClass() const { return m_x_cos_storage_class; }
    void SetXCosStorageClass(const std::string& x_cos_storage_class) {
        m_x_cos_storage_class = x_cos_storage_class;
    }

    /// \brief 获取自定义的元数据
    std::map<std::string, std::string> GetXCosMetas() const { return m_x_cos_metas; }
    /// \brief 获取自定义的元数据
    std::string GetXCosMeta(const std::string& key) const {
        std::map<std::string, std::string>::const_iterator itr = m_x_cos_metas.begin();
        if (itr != m_x_cos_metas.end()) {
            return itr->second;
        }
        return "";
    }
    void SetXCosMeta(const std::string& key, const std::string& value) {
        m_x_cos_metas[key] = value;
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

private:
    std::string m_bucket;
    std::string m_key; // object名称
    std::string m_upload_id;
};

class UploadPartDataResp : public BaseResp {
public:
    UploadPartDataResp() {}
    virtual ~UploadPartDataResp() {}
};

class CompleteMultiUploadResp : public BaseResp {
public:
    CompleteMultiUploadResp() {}
    virtual ~CompleteMultiUploadResp() {}

    virtual bool ParseFromXmlString(const std::string& body);

    std::string GetLocation() const { return m_location; }
    std::string GetKey() const { return m_key; }
    std::string GetBucket() const { return m_bucket; }

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

private:
    void InternalCopyFrom(const BaseResp& resp);

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

} // namespace qcloud_cos
#endif // OBJECT_RESP_H
