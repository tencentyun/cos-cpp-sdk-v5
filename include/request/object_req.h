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
#include <map>

#include "cos_defines.h"
#include "cos_sys_config.h"
#include "trsf/transfer_handler.h"

#include "Poco/SharedPtr.h"


namespace qcloud_cos {

class TransferHandler;

class ObjectReq : public BaseReq {
public:
    ObjectReq(const std::string& bucket_name, const std::string& object_name)
        : m_bucket_name(bucket_name) {
        SetObjectName(object_name);
    }

    virtual ~ObjectReq() { }

    // getter and setter
    std::string GetBucketName() const { return m_bucket_name; }
    void SetBucketName(const std::string& bucket_name) { m_bucket_name = bucket_name; }
    std::string GetObjectName() const { return m_object_name; }
    void SetObjectName(const std::string& object_name) {
        if (StringUtil::StringStartsWith(object_name, "/")) {
            m_object_name = object_name.substr(1);
        } else {
            m_object_name = object_name;
        }
        m_path = "/" + m_object_name;
    }

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

    /// x-cos-storage-class 设置 Object 的存储级别，枚举值：STANDARD,STANDARD_IA，
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
    void SetXCosGrantRead(const std::string& str) {
        AddHeader("x-cos-grant-read", str);
    }

    /// 赋予被授权者写的权限,格式：x-cos-grant-write: id=" ",id=" "./
    /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>",
    /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
    void SetXCosGrantWrite(const std::string& str) {
        AddHeader("x-cos-grant-write", str);
    }

    /// 赋予被授权者读写权限.格式：x-cos-grant-full-control: id=" ",id=" ".
    /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>",
    /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
    void SetXCosGrantFullControl(const std::string& str) {
        AddHeader("x-cos-grant-full-control", str);
    }

    /// 设置Server端加密使用的算法, 目前支持AES256
    void SetXCosServerSideEncryption(const std::string& str) {
        AddHeader("x-cos-server-side-encryption", str);
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
        m_need_compute_contentmd5 = true;
    }

    virtual ~PutObjectByStreamReq() {}

    std::istream& GetStream() const { return m_in_stream; }
    // 默认开启MD5上传校验
    void TurnOnComputeConentMd5() {
        m_need_compute_contentmd5 = true;
    }

    void TurnOffComputeConentMd5() {
        m_need_compute_contentmd5 = false;
    }

    bool ShouldComputeContentMd5() const {
        return m_need_compute_contentmd5;
    }

private:
    std::istream& m_in_stream;
    bool m_need_compute_contentmd5;
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
        m_need_compute_contentmd5 = true;
    }

    virtual ~PutObjectByFileReq() {}

    void SetLocalFilePath(const std::string& local_file_path) {
        m_local_file_path = local_file_path;
    }

    std::string GetLocalFilePath() const { return m_local_file_path; }
    // 默认开启MD5上传校验
    void TurnOnComputeConentMd5() {
        m_need_compute_contentmd5 = true;
    }

    void TurnOffComputeConentMd5() {
        m_need_compute_contentmd5 = false;
    }

    bool ShouldComputeContentMd5() const {
        return m_need_compute_contentmd5;
    }

private:
    std::string m_local_file_path;
    bool m_need_compute_contentmd5;
};

class DeleteObjectReq : public ObjectReq {
public:
    DeleteObjectReq(const std::string& bucket_name,
                    const std::string& object_name)
        : ObjectReq(bucket_name, object_name) {
        SetMethod("DELETE");
    }

    DeleteObjectReq(const std::string& bucket_name,
                    const std::string& object_name,
                    const std::string& version_id)
        : ObjectReq(bucket_name, object_name) {
        SetMethod("DELETE");
        AddParam("versionId", version_id);
    }

    ~DeleteObjectReq() {}

    /// \brief 删除指定版本号的对象
    void SetXCosVersionId(const std::string& version_id) {
        AddParam("versionId", version_id);
    }
};

class DeleteObjectsReq : public BaseReq {
public:
    DeleteObjectsReq(const std::string& bucket_name)
        : m_is_quiet(false), m_bucket_name(bucket_name) {
        SetMethod("POST");
        AddParam("delete", "");
    }

    DeleteObjectsReq(const std::string& bucket_name,
                     const std::vector<ObjectVersionPair>& objects)
        : m_is_quiet(false), m_bucket_name(bucket_name) {
        SetMethod("POST");
        AddParam("delete", "");
        m_objvers = objects;
    }

    ~DeleteObjectsReq() {}

    bool GenerateRequestBody(std::string* body) const;

    void SetQuiet() {
        m_is_quiet = true;
    }

    // getter and setter
    std::string GetBucketName() const {
        return m_bucket_name;
    }

    void SetBucketName(const std::string& bucket_name) {
        m_bucket_name = bucket_name;
    }

    std::vector<ObjectVersionPair> GetObjectVersions() const {
        return m_objvers;
    }

    size_t GetObjectVerionsSize() const {
        return m_objvers.size();
    }

    void AddObjectVersion(const std::string& object, const std::string& version) {
        ObjectVersionPair pair;
        pair.m_object_name = object;
        pair.m_version_id = version;
        m_objvers.push_back(pair);
    }

    void AddObject(const std::string& object) {
        ObjectVersionPair pair;
        pair.m_object_name = object;
        pair.m_version_id = "";
        m_objvers.push_back(pair);
    }

private:
    bool m_is_quiet;
    std::string m_bucket_name;
    std::vector<ObjectVersionPair> m_objvers;
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

    /// x-cos-storage-class 设置 Object 的存储级别，枚举值：STANDARD,STANDARD_IA，
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
    void SetXCosGrantRead(const std::string& str) {
        AddHeader("x-cos-grant-read", str);
    }

    /// 赋予被授权者写的权限,格式：x-cos-grant-write: id=" ",id=" "./
    /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>",
    /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
    void SetXCosGrantWrite(const std::string& str) {
        AddHeader("x-cos-grant-write", str);
    }

    /// 赋予被授权者读写权限.格式：x-cos-grant-full-control: id=" ",id=" ".
    /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>",
    /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
    void SetXCosGrantFullControl(const std::string& str) {
        AddHeader("x-cos-grant-full-control", str);
    }

    /// 设置Server端加密使用的算法, 目前支持AES256
    void SetXCosServerSideEncryption(const std::string& str) {
        AddHeader("x-cos-server-side-encryption", str);
    }
};

class UploadPartDataReq : public ObjectReq {
public:
    UploadPartDataReq(const std::string& bucket_name,
                      const std::string& object_name, const std::string& upload_id,
                      std::istream& in_stream)
        : ObjectReq(bucket_name, object_name),
          m_in_stream(in_stream), m_upload_id(upload_id), m_part_number(1)  {
        m_method = "PUT";
        m_need_compute_contentmd5 = true;
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
    // 默认开启MD5上传校验
    void TurnOnComputeConentMd5() {
        m_need_compute_contentmd5 = true;
    }

    void TurnOffComputeConentMd5() {
        m_need_compute_contentmd5 = false;
    }

    bool ShouldComputeContentMd5() const {
        return m_need_compute_contentmd5;
    }

private:
    std::istream& m_in_stream;
    std::string m_upload_id;
    uint64_t m_part_number;
    bool m_need_compute_contentmd5;
};

class UploadPartCopyDataReq : public ObjectReq {
public:
    UploadPartCopyDataReq(const std::string& bucket_name,
                          const std::string& object_name,
                          const std::string& upload_id)
        : ObjectReq(bucket_name, object_name),
          m_upload_id(upload_id), m_part_number(1) {
        m_method = "PUT";
    }

    UploadPartCopyDataReq(const std::string& bucket_name,
                          const std::string& object_name,
                          const std::string& upload_id,
                          uint64_t part_number)
        : ObjectReq(bucket_name, object_name),
          m_upload_id(upload_id), m_part_number(part_number) {
        m_method = "PUT";
    }

    /// \brief 设置本次分块复制的ID
    void SetUploadId(const std::string& upload_id) {
        m_upload_id = upload_id;
    }
    /// \brief 设置本次分块复制的编号
    void SetPartNumber(uint64_t part_number) { m_part_number = part_number; }

    std::string GetUploadId() const { return m_upload_id; }

    uint64_t GetPartNumber() const { return m_part_number; }

    /// \brief 设置本次分块复制的源文件 URL 路径，可以通过 versionid 子资源指定历史版本
    void SetXCosCopySource(const std::string& src) {
        AddHeader("x-cos-copy-source", src);
    }

    /// \brief 设置源文件的字节范围，
    ///        范围值必须使用 bytes=first-last 格式，first 和 last 都是基于 0 开始的偏移量。
    ///        例如 bytes=0-9 表示你希望拷贝源文件的开头10个字节的数据，如果不指定，则表示拷贝整个文件
    void SetXCosCopySourceRange(const std::string& range) {
        AddHeader("x-cos-copy-source-range", range);
    }

    /// \brief 当 Object 在指定时间后被修改，则执行操作，否则返回 412
    void SetXCosCopySourceIfModifiedSince(const std::string& date) {
        AddHeader("x-cos-copy-source-If-Modified-Since", date);
    }

    /// \brief 当 Object 在指定时间后未被修改，则执行操作，否则返回 412
    void SetXCosCopySourceIfUnmodifiedSince(const std::string& date) {
        AddHeader("x-cos-copy-source-If-Unmodified-Since", date);
    }

    /// \brief 当 Object 的 Etag 和给定一致时，则执行操作，否则返回 412
    void SetXCosCopySourceIfMatch(const std::string& etag) {
        AddHeader("x-cos-copy-source-If-Match", etag);
    }

    /// \brief 当 Object 的 Etag 和给定不一致时，则执行操作，否则返回 412
    void SetXCosCopySourceIfNoneMatch(const std::string& etag) {
        AddHeader("x-cos-copy-source-If-None-Match", etag);
    }

private:
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
    typedef void (*UploadProgressCallback)(const MultiUploadObjectReq *req, Poco::SharedPtr<TransferHandler>& handler);
public:
    MultiUploadObjectReq(const std::string& bucket_name,
                   const std::string& object_name, const std::string& local_file_path = "")
        : ObjectReq(bucket_name, object_name) {
        // 默认使用配置文件配置的分块大小和线程池大小
        m_part_size = CosSysConfig::GetUploadPartSize();
        m_thread_pool_size = CosSysConfig::GetUploadThreadPoolSize();
        mb_set_meta = false;
        m_progress_callback = NULL;

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

    /// 设置Server端加密使用的算法, 目前支持AES256
    void SetXCosServerSideEncryption(const std::string& str) {
        AddHeader("x-cos-server-side-encryption", str);
    }

    /// 允许用户自定义的头部信息,将作为 Object 元数据返回.大小限制2K
    void SetXCosMeta(const std::string& key, const std::string& value) {
        mb_set_meta = true;
        m_xcos_meta.insert(std::pair<std::string, std::string>(key, value));
    }

    bool IsSetXCosMeta() const{
        return mb_set_meta;
    }

    std::map<std::string, std::string> GetXCosMeta() const{
        return m_xcos_meta;
    }

    void SetUploadID(const std::string& uploadid) {
        if (!uploadid.empty()) {
            m_uploadid = uploadid;
        }
    }

    std::string GetUploadID() const {
        return m_uploadid;
    }

    void SetUploadProgressCallback(UploadProgressCallback callback) {
        m_progress_callback = callback;
    }
   
    void TriggerUploadProgressCallback(Poco::SharedPtr<TransferHandler>& handler) const {
        if(m_progress_callback) {
            m_progress_callback(this, handler); 
        }
    }


private:
    std::string m_local_file_path;
    uint64_t m_part_size;
    int m_thread_pool_size;
    std::map<std::string, std::string> m_xcos_meta;
    bool mb_set_meta;
    std::string m_uploadid;
    
public:
    UploadProgressCallback m_progress_callback;

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

class ListPartsReq : public ObjectReq {
public:
    ListPartsReq(const std::string& bucket_name,
                 const std::string& object_name,
                 const std::string& upload_id)
        : ObjectReq(bucket_name, object_name) {
        m_method = "GET";
        AddParam("uploadId", upload_id);
    }

    virtual ~ListPartsReq() {}

    /// \brief 规定返回值的编码方式
    void SetEncodingType(const std::string& encoding_type) {
        AddParam("encoding-type", encoding_type);
    }

    /// \brief 单次返回最大的条目数量，若不设置，默认 1000
    void SetMaxParts(uint64_t max_parts) {
        AddParam("max-parts", StringUtil::Uint64ToString(max_parts));
    }

    /// \brief 默认以 UTF-8 二进制顺序列出条目，所有列出条目从 marker 开始
    void SetPartNumberMarker(const std::string& part_number_marker) {
        AddParam("part-number-marker", part_number_marker);
    }
};

class GetObjectACLReq : public ObjectReq {
public:
    GetObjectACLReq(const std::string& bucket_name,
                    const std::string& object_name)
        : ObjectReq(bucket_name, object_name) {
        m_method = "GET";
        AddParam("acl", "");
    }

    virtual ~GetObjectACLReq() {}
};

class PutObjectACLReq : public ObjectReq {
public:
    PutObjectACLReq(const std::string& bucket_name,
                    const std::string& object_name)
        : ObjectReq(bucket_name, object_name) {
        m_method = "PUT";
        AddParam("acl", "");
    }

    virtual ~PutObjectACLReq() {}

    /// 定义Object的ACL属性,有效值：private,public-read-write,public-read
    /// 默认值：private
    void SetXCosAcl(const std::string& str) {
        AddHeader("x-cos-acl", str);
    }

    /// 赋予被授权者读的权限.格式：x-cos-grant-read: id=" ",id=" ".
    /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>"
    /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
    void SetXCosGrantRead(const std::string& str) {
        AddHeader("x-cos-grant-read", str);
    }

    /// 赋予被授权者写的权限,格式：x-cos-grant-write: id=" ",id=" ".
    /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>",
    /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
    void SetXCosGrantWrite(const std::string& str) {
        AddHeader("x-cos-grant-write", str);
    }

    /// 赋予被授权者读写权限.格式：x-cos-grant-full-control: id=" ",id=" ".
    /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>",
    /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
    void SetXCosGrantFullControl(const std::string& str) {
        AddHeader("x-cos-grant-full-control", str);
    }

    /// Object Owner设置
    void SetOwner(const Owner& owner) {
        m_owner = owner;
    }

    /// 设置被授权者信息与权限信息
    void SetAccessControlList(const std::vector<Grant>& grants) {
        m_acl = grants;
    }

    /// 添加单个 Bucket 的授权信息
    void AddAccessControlList(const Grant& grant) {
        m_acl.push_back(grant);
    }

    /// 清空权限信息
    void ClearAccessControlList() {
        std::vector<Grant> tmp;
        m_acl.swap(tmp);
    }

    bool GenerateRequestBody(std::string* body) const;

private:
    Owner m_owner;
    std::vector<Grant> m_acl;
};

class PutObjectCopyReq : public ObjectReq {
public:
    PutObjectCopyReq(const std::string& bucket_name,
                     const std::string& object_name)
        : ObjectReq(bucket_name, object_name) {
        m_method = "PUT";
    }

    virtual ~PutObjectCopyReq() {}

    /// 源文件 URL 路径，可以通过 versionid 子资源指定历史版本
    void SetXCosCopySource(const std::string& str) {
        AddHeader("x-cos-copy-source", str);
    }

    /// 是否拷贝元数据，枚举值：Copy, Replaced，默认值 Copy。
    /// 假如标记为 Copy，忽略 Header 中的用户元数据信息直接复制；
    /// 假如标记为 Replaced，按 Header 信息修改元数据。
    /// 当目标路径和原路径一致，即用户试图修改元数据时，必须为 Replaced
    void SetXCosMetadataDirective(const std::string& str) {
        AddHeader("x-cos-metadata-directive", str);
    }

    /// 当 Object 在指定时间后被修改，则执行操作，否则返回 412。
    /// 可与 x-cos-copy-source-If-None-Match 一起使用，与其他条件联合使用返回冲突。
    void SetXCosCopySourceIfModifiedSince(const std::string& str) {
        AddHeader("x-cos-copy-source-If-Modified-Since", str);
    }

    /// 当 Object 在指定时间后未被修改，则执行操作，否则返回 412。
    /// 可与 x-cos-copy-source-If-Match 一起使用，与其他条件联合使用返回冲突。
    void SetXCosCopySourceIfUnmodifiedSince(const std::string& str) {
        AddHeader("x-cos-copy-source-If-Unmodified-Since", str);
    }

    /// 当 Object 的 Etag 和给定一致时，则执行操作，否则返回 412。
    /// 可与x-cos-copy-source-If-Unmodified-Since 一起使用，与其他条件联合使用返回冲突
    void SetXCosCopySourceIfMatch(const std::string& str) {
        AddHeader("x-cos-copy-source-If-Match", str);
    }

    /// 当 Object 的 Etag 和给定不一致时，则执行操作，否则返回 412。
    /// 可与 x-cos-copy-source-If-Modified-Since 一起使用，与其他条件联合使用返回冲突。
    void SetXCosCopySourceIfNoneMatch(const std::string& str) {
        AddHeader("x-cos-copy-source-If-None-Match", str);
    }

    /// x-cos-storage-class 设置 Object 的存储级别，枚举值：STANDARD,STANDARD_IA，
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
    void SetXCosGrantRead(const std::string& str) {
        AddHeader("x-cos-grant-read", str);
    }

    /// 赋予被授权者写的权限,格式：x-cos-grant-write: id=" ",id=" "./
    /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>",
    /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
    void SetXCosGrantWrite(const std::string& str) {
        AddHeader("x-cos-grant-write", str);
    }

    /// 赋予被授权者读写权限.格式：x-cos-grant-full-control: id=" ",id=" ".
    /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>",
    /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
    void SetXCosGrantFullControl(const std::string& str) {
        AddHeader("x-cos-grant-full-control", str);
    }

    /// 允许用户自定义的头部信息,将作为 Object 元数据返回.大小限制2K
    void SetXCosMeta(const std::string& key, const std::string& value) {
        AddHeader("x-cos-meta-" + key, value);
    }

    /// 设置Server端加密使用的算法, 目前支持AES256
    void SetXCosServerSideEncryption(const std::string& str) {
        AddHeader("x-cos-server-side-encryption", str);
    }
};

class CopyReq : public ObjectReq {
public:
    CopyReq(const std::string& bucket_name,
            const std::string& object_name)
        : ObjectReq(bucket_name, object_name) {
        // 默认使用配置文件配置的分块大小和线程池大小
        m_part_size = CosSysConfig::GetUploadCopyPartSize();
        m_thread_pool_size = CosSysConfig::GetUploadThreadPoolSize();
    }

    virtual ~CopyReq() {}

    /// 源文件 URL 路径，可以通过 versionid 子资源指定历史版本
    void SetXCosCopySource(const std::string& str) {
        AddHeader("x-cos-copy-source", str);
    }

    /// 是否拷贝元数据，枚举值：Copy, Replaced，默认值 Copy。
    /// 假如标记为 Copy，忽略 Header 中的用户元数据信息直接复制；
    /// 假如标记为 Replaced，按 Header 信息修改元数据。
    /// 当目标路径和原路径一致，即用户试图修改元数据时，必须为 Replaced
    void SetXCosMetadataDirective(const std::string& str) {
        AddHeader("x-cos-metadata-directive", str);
    }

    /// 当 Object 在指定时间后被修改，则执行操作，否则返回 412。
    /// 可与 x-cos-copy-source-If-None-Match 一起使用，与其他条件联合使用返回冲突。
    void SetXCosCopySourceIfModifiedSince(const std::string& str) {
        AddHeader("x-cos-copy-source-If-Modified-Since", str);
    }

    /// 当 Object 在指定时间后未被修改，则执行操作，否则返回 412。
    /// 可与 x-cos-copy-source-If-Match 一起使用，与其他条件联合使用返回冲突。
    void SetXCosCopySourceIfUnmodifiedSince(const std::string& str) {
        AddHeader("x-cos-copy-source-If-Unmodified-Since", str);
    }

    /// 当 Object 的 Etag 和给定一致时，则执行操作，否则返回 412。
    /// 可与x-cos-copy-source-If-Unmodified-Since 一起使用，与其他条件联合使用返回冲突
    void SetXCosCopySourceIfMatch(const std::string& str) {
        AddHeader("x-cos-copy-source-If-Match", str);
    }

    /// 当 Object 的 Etag 和给定不一致时，则执行操作，否则返回 412。
    /// 可与 x-cos-copy-source-If-Modified-Since 一起使用，与其他条件联合使用返回冲突。
    void SetXCosCopySourceIfNoneMatch(const std::string& str) {
        AddHeader("x-cos-copy-source-If-None-Match", str);
    }

    /// x-cos-storage-class 设置 Object 的存储级别，枚举值：STANDARD,STANDARD_IA
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
    void SetXCosGrantRead(const std::string& str) {
        AddHeader("x-cos-grant-read", str);
    }

    /// 赋予被授权者写的权限,格式：x-cos-grant-write: id=" ",id=" "./
    /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>",
    /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
    void SetXCosGrantWrite(const std::string& str) {
        AddHeader("x-cos-grant-write", str);
    }

    /// 赋予被授权者读写权限.格式：x-cos-grant-full-control: id=" ",id=" ".
    /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>",
    /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
    void SetXCosGrantFullControl(const std::string& str) {
        AddHeader("x-cos-grant-full-control", str);
    }

    /// 允许用户自定义的头部信息,将作为 Object 元数据返回.大小限制2K
    void SetXCosMeta(const std::string& key, const std::string& value) {
        AddHeader("x-cos-meta-" + key, value);
    }

    std::map<std::string, std::string> GetPartCopyHeader() const;
    std::map<std::string, std::string> GetInitHeader() const;

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
    uint64_t m_part_size;
    int m_thread_pool_size;
};

class PostObjectRestoreReq : public ObjectReq {
public:
    PostObjectRestoreReq(const std::string& bucket_name,
                         const std::string& object_name)
        : ObjectReq(bucket_name, object_name), m_expiry_days(0), m_tier("Standard") {
        m_method = "POST";
        AddParam("restore", "");
    }

    PostObjectRestoreReq(const std::string& bucket_name,
                         const std::string& object_name,
                         const std::string& version_id)
        : ObjectReq(bucket_name, object_name), m_expiry_days(0), m_tier("Standard") {
        m_method = "POST";
        AddParam("restore", "");
        if (!version_id.empty()) {
            AddHeader("x-cos-version-id", version_id);
        }
    }

    PostObjectRestoreReq(const std::string& bucket_name,
                         const std::string& object_name,
                         const std::string& version_id,
                         const std::string& tier)
        : ObjectReq(bucket_name, object_name), m_expiry_days(0), m_tier(tier) {
        m_method = "POST";
        AddParam("restore", "");
        if (!version_id.empty()) {
            AddHeader("x-cos-version-id", version_id);
        }
    }

    virtual ~PostObjectRestoreReq() {}

    /// \brief  设置临时副本的过期时间
    void SetExiryDays(uint64_t days) {
        m_expiry_days = days;
    }

    /// \brief 指定版本号的对象
    void SetXCosVersionId(const std::string& version_id) {
        AddHeader("x-cos-version-id", version_id);
    }

    // \brief 具体复原过程类型，枚举值： Expedited ，Standard ，Bulk；默认值：Standard
    void SetTier(const std::string& tier) {
        m_tier = tier;
    }

    bool GenerateRequestBody(std::string* body) const;

private:
    uint64_t m_expiry_days;
    std::string m_tier;
};

class GeneratePresignedUrlReq : public ObjectReq {
public:
    GeneratePresignedUrlReq(const std::string& bucket_name,
                            const std::string& object_name,
                            HTTP_METHOD http_method)
        : ObjectReq(bucket_name, object_name), m_start_time_in_s(0),
          m_expired_time_in_s(0) {
        m_path = "/" + object_name;
        m_method = StringUtil::HttpMethodToString(http_method);
    }

    virtual ~GeneratePresignedUrlReq() {}

    uint64_t GetStartTimeInSec() const { return m_start_time_in_s; }
    uint64_t GetExpiredTimeInSec() const { return m_expired_time_in_s; }
    void SetStartTimeInSec(uint64_t start_time) { m_start_time_in_s = start_time; }
    void SetExpiredTimeInSec(uint64_t expired_time) { m_expired_time_in_s = expired_time; }

private:
    uint64_t m_start_time_in_s;
    uint64_t m_expired_time_in_s;
};

} // namespace qcloud_cos
#endif // OBJECT_REQ_H
