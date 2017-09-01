// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:

#ifndef OBJECT_REQ_H
#define OBJECT_REQ_H
#pragma once

#include "request/base_req.h"

#include <vector>
#include <sstream>

#include "cos_defines.h"
#include "cos_sys_config.h"

namespace qcloud_cos {

class ObjectReq : public BaseReq {
public:
    ObjectReq(const std::string& bucket_name, const std::string& object_name)
        : m_bucket_name(bucket_name) {
        if (StringUtil::StringStartsWith(object_name, "/")) {
            m_object_name = object_name.substr(1);
        } else {
            m_object_name = object_name;
        }
        m_path = "/" + m_object_name;
    }

    virtual ~ObjectReq() { }

    // getter and setter
    std::string GetBucketName() const { return m_bucket_name; }
    void SetBucketName(const std::string& bucket_name) { m_bucket_name = bucket_name; }
    std::string GetObjectName() const { return m_object_name; }
    void SetObjectName(const std::string& object_name) { m_object_name = object_name; }

private:
    std::string m_bucket_name;
    std::string m_object_name;
};

class GetObjectReq : public ObjectReq {
public:
    /// \brief 设置响应头部中的 Content-Type 参数
    void SetResponseContentType(const std::string& str) {
        AddParam("response-content-type", str);
    }

    /// \brief 设置响应头部中的 Content-Language 参数
    void SetResponseContentLang(const std::string& str) {
        AddParam("response-content-language", str);
    }

    /// \brief 设置响应头部中的 Content-Expires 参数
    void SetResponseExpires(const std::string& str) {
        AddParam("response-expires", str);
    }

    /// \brief 设置响应头部中的 Cache-Control 参数
    void SetResponseCacheControl(const std::string& str) {
        AddParam("response-cache-control", str);
    }

    /// \brief 设置响应头部中的 Content-Disposition 参数
    void SetResponseContentEncoding(const std::string& str) {
        AddParam("response-content-encoding", str);
    }

    /// \brief 设置响应头部中的 Content-Encoding 参数
    void SetResponseContentDisposition(const std::string& str) {
        AddParam("response-content-disposition", str);
    }

protected:
    GetObjectReq(const std::string& bucket_name,
                 const std::string& object_name)
        : ObjectReq(bucket_name, object_name) {
        m_method = "GET";
    }

    virtual ~GetObjectReq() {}
};

class GetObjectByStreamReq : public GetObjectReq {
public:
    GetObjectByStreamReq(const std::string& bucket_name, const std::string& object_name,
                         std::ostream& os)
        : GetObjectReq(bucket_name, object_name), m_os(os) {
        m_method = "GET";
    }

    virtual ~GetObjectByStreamReq() {}

    std::ostream& GetStream() const { return m_os; }

private:
    std::ostream& m_os;
};

class GetObjectByFileReq : public GetObjectReq {
public:
    GetObjectByFileReq(const std::string& bucket_name, const std::string& object_name,
                       const std::string& local_file_path = "")
        : GetObjectReq(bucket_name, object_name) {
        if (local_file_path.empty()) {
            m_local_file_path = "./" + object_name;
        } else {
            m_local_file_path = local_file_path;
        }
    }

    virtual ~GetObjectByFileReq() {}

    void SetLocalFilePath(const std::string& local_file_path) {
        m_local_file_path = local_file_path;
    }

    std::string GetLocalFilePath() const { return m_local_file_path; }

private:
    std::string m_local_file_path;
};

class MultiGetObjectReq : public GetObjectReq {
public:
    MultiGetObjectReq(const std::string& bucket_name, const std::string& object_name,
                      const std::string& local_file_path = "")
        : GetObjectReq(bucket_name, object_name) {
        // 默认使用配置文件配置的分块大小和线程池大小
        m_slice_size = CosSysConfig::GetDownSliceSize();
        m_thread_pool_size = CosSysConfig::GetDownThreadPoolMaxSize();

        if (local_file_path.empty()) {
            m_local_file_path = "./" + object_name;
        } else {
            m_local_file_path = local_file_path;
        }
    }

    virtual ~MultiGetObjectReq() {
    }

    void SetLocalFilePath(const std::string& local_file_path) {
        m_local_file_path = local_file_path;
    }

    std::string GetLocalFilePath() const { return m_local_file_path; }

    /// \brief 设置分片大小
    void SetSliceSize(uint64_t bytes) {
         m_slice_size = bytes;
    }

    /// \brief 获取分片大小
    uint64_t GetSliceSize() const { return m_slice_size; }

    /// \brief 设置线程池大小
    void SetThreadPoolSize(int size) {
        assert(size > 0);
        m_thread_pool_size = size;
    }

    /// \brief 获取线程池大小
    int GetThreadPoolSize() const { return m_thread_pool_size; }

private:
    std::string m_local_file_path;
    uint64_t m_slice_size;
    int m_thread_pool_size;
};

class PutObjectReq : public ObjectReq {
public:
    /// Cache-Control RFC 2616 中定义的缓存策略，将作为 Object 元数据保存
    void SetCacheControl(const std::string& str) {
        AddHeader("Cache-Control", str);
    }

    /// Content-Disposition RFC 2616 中定义的文件名称，将作为 Object 元数据保存
    void SetContentDisposition(const std::string& str) {
        AddHeader("Content-Disposition", str);
    }

    /// Content-Encoding    RFC 2616 中定义的编码格式，将作为 Object 元数据保存
    void SetContentEncoding(const std::string& str) {
        AddHeader("Content-Encoding", str);
    }

    /// Content-Type    RFC 2616 中定义的内容类型（MIME），将作为 Object 元数据保存
    void SetContentType(const std::string& str) {
        AddHeader("Content-Type", str);
    }

    /// Expect  当使用 Expect: 100-continue 时，在收到服务端确认后，才会发送请求内容
    void SetExpect(const std::string& str) {
        AddHeader("Expect", str);
    }

    /// Expires RFC 2616 中定义的过期时间，将作为 Object 元数据保存
    void SetExpires(const std::string& str) {
        AddHeader("Expires", str);
    }

    /// 允许用户自定义的头部信息,将作为 Object 元数据返回.大小限制2K
    void SetXCosMeta(const std::string& key, const std::string& value) {
        AddHeader("x-cos-meta-" + key, value);
    }

    /// x-cos-storage-class 设置 Object 的存储级别，枚举值：STANDARD,STANDARD_IA，NEARLINE，
    /// 默认值：STANDARD（目前仅支持华南园区）
    void SetXCosStorageClass(const std::string& storage_class) {
        AddHeader("x-cos-storage-class", storage_class);
    }

    /// 定义Object的ACL属性,有效值：private,public-read-write,public-read
    /// 默认值：private
    void SetXcosAcl(const std::string& str) {
        AddHeader("x-cos-acl", str);
    }

    /// 赋予被授权者读的权限.格式：x-cos-grant-read: id=" ",id=" ".
    /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>"
    /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
    void SetXcosGrantRead(const std::string& str) {
        AddHeader("x-cos-grant-read", str);
    }

    /// 赋予被授权者写的权限,格式：x-cos-grant-write: id=" ",id=" "./
    /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>",
    /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
    void SetXcosGrantWrite(const std::string& str) {
        AddHeader("x-cos-grant-write", str);
    }

    /// 赋予被授权者读写权限.格式：x-cos-grant-full-control: id=" ",id=" ".
    /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>",
    /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
    void SetXcosGrantFullControl(const std::string& str) {
        AddHeader("x-cos-grant-full-control", str);
    }

protected:
    PutObjectReq(const std::string& bucket_name,
                 const std::string& object_name)
        : ObjectReq(bucket_name, object_name) {
        m_method = "PUT";
    }

    virtual ~PutObjectReq() {}
};

class PutObjectByStreamReq : public PutObjectReq {
public:
    PutObjectByStreamReq(const std::string& bucket_name,
                         const std::string& object_name,
                         std::istream& in_stream)
        : PutObjectReq(bucket_name, object_name), m_in_stream(in_stream) {
    }

    virtual ~PutObjectByStreamReq() {}

    std::istream& GetStream() const { return m_in_stream; }

private:
    std::istream& m_in_stream;
};

class PutObjectByFileReq : public PutObjectReq {
public:
    PutObjectByFileReq(const std::string& bucket_name,
                       const std::string& object_name,
                       const std::string& local_file_path = "")
        : PutObjectReq(bucket_name, object_name) {
        if (local_file_path.empty()) {
            m_local_file_path = "./" + object_name;
        } else {
            m_local_file_path = local_file_path;
        }
    }

    virtual ~PutObjectByFileReq() {}

    void SetLocalFilePath(const std::string& local_file_path) {
        m_local_file_path = local_file_path;
    }

    std::string GetLocalFilePath() const { return m_local_file_path; }

private:
    std::string m_local_file_path;
};

class HeadObjectReq : public ObjectReq {
public:
    HeadObjectReq(const std::string& bucket_name,
                  const std::string& object_name)
        : ObjectReq(bucket_name, object_name) {
        m_method = "HEAD";
    }

    virtual ~HeadObjectReq() {}
};

class InitMultiUploadReq : public ObjectReq {
public:
    InitMultiUploadReq(const std::string& bucket_name,
                       const std::string& object_name)
        : ObjectReq(bucket_name, object_name) {
        m_method = "POST";
    }
    virtual ~InitMultiUploadReq() {}

    /// Cache-Control RFC 2616 中定义的缓存策略，将作为 Object 元数据保存
    void SetCacheControl(const std::string& str) {
        AddHeader("Cache-Control", str);
    }

    /// Content-Disposition RFC 2616 中定义的文件名称，将作为 Object 元数据保存
    void SetContentDisposition(const std::string& str) {
        AddHeader("Content-Disposition", str);
    }

    /// Content-Encoding    RFC 2616 中定义的编码格式，将作为 Object 元数据保存
    void SetContentEncoding(const std::string& str) {
        AddHeader("Content-Encoding", str);
    }

    /// Content-Type    RFC 2616 中定义的内容类型（MIME），将作为 Object 元数据保存
    void SetContentType(const std::string& str) {
        AddHeader("Content-Type", str);
    }

    /// Expires RFC 2616 中定义的过期时间，将作为 Object 元数据保存
    void SetExpires(const std::string& str) {
        AddHeader("Expires", str);
    }

    /// 允许用户自定义的头部信息,将作为 Object 元数据返回.大小限制2K
    void SetXCosMeta(const std::string& key, const std::string& value) {
        AddHeader("x-cos-meta-" + key, value);
    }

    /// x-cos-storage-class 设置 Object 的存储级别，枚举值：STANDARD,STANDARD_IA，NEARLINE，
    /// 默认值：STANDARD（目前仅支持华南园区）
    void SetXCosStorageClass(const std::string& storage_class) {
        AddHeader("x-cos-storage-class", storage_class);
    }

    /// 定义Object的ACL属性,有效值：private,public-read-write,public-read
    /// 默认值：private
    void SetXCosAcl(const std::string& str) {
        AddHeader("x-cos-acl", str);
    }

    /// 赋予被授权者读的权限.格式：x-cos-grant-read: id=" ",id=" ".
    /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>"
    /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
    void SetXcosGrantRead(const std::string& str) {
        AddHeader("x-cos-grant-read", str);
    }

    /// 赋予被授权者写的权限,格式：x-cos-grant-write: id=" ",id=" "./
    /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>",
    /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
    void SetXcosGrantWrite(const std::string& str) {
        AddHeader("x-cos-grant-write", str);
    }

    /// 赋予被授权者读写权限.格式：x-cos-grant-full-control: id=" ",id=" ".
    /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>",
    /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
    void SetXcosGrantFullControl(const std::string& str) {
        AddHeader("x-cos-grant-full-control", str);
    }
};

class UploadPartDataReq : public ObjectReq {
public:
    UploadPartDataReq(const std::string& bucket_name,
                      const std::string& object_name, const std::string& upload_id,
                      std::istream& in_stream)
        : ObjectReq(bucket_name, object_name),
          m_upload_id(upload_id), m_part_number(1), m_in_stream(in_stream) {
        m_method = "PUT";
    }

    /// \brief 设置本次分块上传的ID
    void SetUploadId(const std::string& upload_id) {
        m_upload_id = upload_id;
    }
    /// \brief 设置本次分块上传的编号
    void SetPartNumber(uint64_t part_number) { m_part_number = part_number; }

    std::string GetUploadId() const { return m_upload_id; }

    uint64_t GetPartNumber() const { return m_part_number; }

    std::istream& GetStream() const { return m_in_stream; }

private:
    std::istream& m_in_stream;
    std::string m_upload_id;
    uint64_t m_part_number;
};

class CompleteMultiUploadReq : public ObjectReq {
public:
    CompleteMultiUploadReq(const std::string& bucket_name,
                           const std::string& object_name, const std::string& upload_id)
        : ObjectReq(bucket_name, object_name), m_upload_id(upload_id) {
        m_method = "POST";
    }

    virtual ~CompleteMultiUploadReq() {}

    std::string GetUploadId() const { return m_upload_id; }
    /// \brief 设置上传的ID
    void SetUploadId(const std::string& upload_id) {
        m_upload_id = upload_id;
    }

    std::vector<uint64_t> GetPartNumbers() const { return m_part_numbers; }
    /// \brief 设置上传的所有分块编号
    void SetPartNumbers(const std::vector<uint64_t>& part_numbers) {
        m_part_numbers.clear();
        m_part_numbers.insert(m_part_numbers.end(), part_numbers.begin(), part_numbers.end());
    }

    std::vector<std::string> GetEtags() const { return m_etags; }
    /// \brief 设置上传的所有分块etag
    void SetEtags(const std::vector<std::string>& etags) {
        m_etags.clear();
        m_etags.insert(m_etags.end(), etags.begin(), etags.end());
    }

    void AddPartEtagPair(uint64_t part_number, const std::string& etag) {
        m_part_numbers.push_back(part_number);
        m_etags.push_back(etag);
    }

    bool GenerateRequestBody(std::string* body) const;

private:
    std::string m_upload_id;
    std::vector<uint64_t> m_part_numbers;
    std::vector<std::string> m_etags;
};

class MultiUploadObjectReq : public ObjectReq {
public:
    MultiUploadObjectReq(const std::string& bucket_name,
                   const std::string& object_name, const std::string& local_file_path = "")
        : ObjectReq(bucket_name, object_name) {
        // 默认使用配置文件配置的分块大小和线程池大小
        m_part_size = CosSysConfig::GetUploadPartSize();
        m_thread_pool_size = CosSysConfig::GetUploadThreadPoolSize();

        // 默认打开当前路径下object的同名文件
        if (local_file_path.empty()) {
            m_local_file_path = "./" + object_name;
        } else {
            m_local_file_path = local_file_path;
        }
    }
    virtual ~MultiUploadObjectReq() {}

    void SetLocalFilePath(const std::string& local_file_path) {
        m_local_file_path = local_file_path;
    }

    std::string GetLocalFilePath() const { return m_local_file_path; }

    // 设置分块大小,若小于1M,则按1M计算;若大于5G,则按5G计算
    void SetPartSize(uint64_t bytes) {
        if (bytes <= kPartSize1M) {
            m_part_size = kPartSize1M;
        } else if (bytes >= kPartSize5G) {
            m_part_size = kPartSize5G;
        } else {
            m_part_size = bytes;
        }
    }

    // 获取分块大小
    uint64_t GetPartSize() const { return m_part_size; }

    void SetThreadPoolSize(int size) {
        assert(size > 0);
        m_thread_pool_size = size;
    }

    int GetThreadPoolSize() const { return m_thread_pool_size; }

private:
    std::string m_local_file_path;
    uint64_t m_part_size;
    int m_thread_pool_size;
};

class AbortMultiUploadReq : public ObjectReq {
public:
    AbortMultiUploadReq(const std::string& bucket_name,
                        const std::string& object_name, const std::string& upload_id)
        : ObjectReq(bucket_name, object_name), m_upload_id(upload_id) {
        m_method = "DELETE";
    }

    virtual ~AbortMultiUploadReq() {}

    std::string GetUploadId() const { return m_upload_id; }
    /// \brief 设置本次终止上传的uploadId
    void SetUploadId(const std::string& upload_id) {
        m_upload_id = upload_id;
    }

private:
    std::string m_upload_id;
};

} // namespace qcloud_cos
#endif // OBJECT_REQ_H
