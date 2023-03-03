// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:
#pragma once

#include <map>
#include <sstream>
#include <vector>
#if defined(_WIN32)
#include <codecvt>
#endif
#include "cos_sys_config.h"
#include "request/base_req.h"
#include "trsf/async_context.h"
#include "util/file_util.h"

namespace qcloud_cos {

class ObjectReq : public BaseReq {
 public:
  ObjectReq(const std::string& bucket_name, const std::string& object_name)
      : m_bucket_name(bucket_name), m_progress_cb(NULL), m_done_cb(NULL),
        m_user_data(NULL) {
    SetObjectName(object_name);
  }

  ObjectReq() {}

  virtual ~ObjectReq() {}

  // getter and setter
  std::string GetBucketName() const { return m_bucket_name; }
  void SetBucketName(const std::string& bucket_name) {
    m_bucket_name = bucket_name;
  }
  std::string GetObjectName() const { return m_object_name; }
  void SetObjectName(const std::string& object_name) {
    if (StringUtil::StringStartsWith(object_name, "/")) {
      m_object_name = object_name.substr(1);
    } else {
      m_object_name = object_name;
    }
    m_path = "/" + m_object_name;
  }
#if 1
  /// @brief  设置进度回调函数
  void SetTransferProgressCallback(const TransferProgressCallback& process_cb) {
    m_progress_cb = process_cb;
  }
  /// @brief  设置完成回调函数
  void SetDoneCallback(const DoneCallback& done_cb) { m_done_cb = done_cb; }
  /// @brief 设置回调私有数据
  void SetUserData(void* user_data) { m_user_data = user_data; }

  TransferProgressCallback GetTransferProgressCallback() const {
    return m_progress_cb;
  }
  DoneCallback GetDoneCallback() const { return m_done_cb; }
  void* GetUserData() const { return m_user_data; }

  //virtual uint64_t GetLocalFileSize() const { return 0; }
  virtual std::string GetLocalFilePath() const { return ""; }
#endif
 private:
  std::string m_bucket_name;
  std::string m_object_name;
  TransferProgressCallback m_progress_cb;  // 进度回调
  DoneCallback m_done_cb;                  // 完成回调
  void* m_user_data;                       // 私有数据
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

  /// \brief 设置单链接限速
  /// 参考https://cloud.tencent.com/document/product/436/40140
  void SetTrafficLimit(const uint64_t traffic_limit) {
    AddHeader("x-cos-traffic-limit", std::to_string(traffic_limit));
  }

  /// \brief 设置单链接限速
  /// 参考https://cloud.tencent.com/document/product/436/40140
  void SetTrafficLimit(const std::string& traffic_limit) {
    AddHeader("x-cos-traffic-limit", traffic_limit);
  }

 protected:
  GetObjectReq(const std::string& bucket_name, const std::string& object_name)
      : ObjectReq(bucket_name, object_name) {
    m_method = "GET";
  }

  virtual ~GetObjectReq() {}
};

class GetObjectByStreamReq : public GetObjectReq {
 public:
  GetObjectByStreamReq(const std::string& bucket_name,
                       const std::string& object_name, std::ostream& os)
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
  GetObjectByFileReq(const std::string& bucket_name,
                     const std::string& object_name,
                     const std::string& local_file_path = "")
      : GetObjectReq(bucket_name, object_name),
        m_local_file_path(local_file_path) {
    if (m_local_file_path.empty()) {
      m_local_file_path = "./" + object_name;
    }
  }

  virtual ~GetObjectByFileReq() {}

  void SetLocalFilePath(const std::string& local_file_path) {
    m_local_file_path = local_file_path;
  }

  std::string GetLocalFilePath() const { return m_local_file_path; }

  uint64_t GetLocalFileSize() const { return 0; }

#if defined(_WIN32)
  /// \brief 如果本地文件路径为宽字符（中文，韩文等）,需要调用该函数
  void SetWideCharPath() { mb_is_widechar_path = true; }

  bool IsWideCharPath() const { return mb_is_widechar_path; }

  std::wstring GetWideCharLocalFilePath() const {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(m_local_file_path);
  }
#endif

 private:
#if defined(_WIN32)
  bool mb_is_widechar_path;
#endif
  std::string m_local_file_path;
};

#if 0
class MultiGetObjectReq : public GetObjectReq {
 public:
  MultiGetObjectReq(const std::string& bucket_name,
                    const std::string& object_name,
                    const std::string& local_file_path = "")
      : GetObjectReq(bucket_name, object_name) {
    // 默认使用配置文件配置的分块大小和线程池大小
    m_slice_size = CosSysConfig::GetDownSliceSize();
    m_thread_pool_size = CosSysConfig::GetDownThreadPoolSize();

    if (local_file_path.empty()) {
      m_local_file_path = "./" + object_name;
    } else {
      m_local_file_path = local_file_path;
    }
  }

  virtual ~MultiGetObjectReq() {}

  void SetLocalFilePath(const std::string& local_file_path) {
    m_local_file_path = local_file_path;
  }

  std::string GetLocalFilePath() const { return m_local_file_path; }

  /// \brief 设置分片大小
  void SetSliceSize(uint64_t bytes) { m_slice_size = bytes; }

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
#endif
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

  /// Content-Type    RFC 2616 中定义的内容类型（MIME），将作为 Object
  /// 元数据保存
  void SetContentType(const std::string& str) {
    AddHeader("Content-Type", str);
  }

  /// Expect  当使用 Expect: 100-continue
  /// 时，在收到服务端确认后，才会发送请求内容
  void SetExpect(const std::string& str) { AddHeader("Expect", str); }

  /// Expires RFC 2616 中定义的过期时间，将作为 Object 元数据保存
  void SetExpires(const std::string& str) { AddHeader("Expires", str); }

  /// 允许用户自定义的头部信息,将作为 Object 元数据返回.大小限制2K
  void SetXCosMeta(const std::string& key, const std::string& value) {
    AddHeader("x-cos-meta-" + key, value);
  }

  /// 设置 Object 的存储类型
  /// 枚举值：MAZ_STANDARD，MAZ_STANDARD_IA，INTELLIGENT_TIERING，MAZ_INTELLIGENT_TIERING，STANDARD_IA，ARCHIVE，DEEP_ARCHIVE
  /// 默认值：STANDARD
  void SetXCosStorageClass(const std::string& storage_class) {
    AddHeader("x-cos-storage-class", storage_class);
  }

  /// 定义Object的ACL属性,有效值：private,public-read-write,public-read
  /// 默认值：private
  void SetXCosAcl(const std::string& str) { AddHeader("x-cos-acl", str); }

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

  /// \brief 设置单链接限速
  /// 参考https://cloud.tencent.com/document/product/436/40140
  void SetTrafficLimit(const uint64_t traffic_limit) {
    AddHeader("x-cos-traffic-limit", std::to_string(traffic_limit));
  }

  /// \brief 设置单链接限速
  /// 参考https://cloud.tencent.com/document/product/436/40140
  void SetTrafficLimit(const std::string& traffic_limit) {
      AddHeader("x-cos-traffic-limit", traffic_limit);
  }

  /// \brief 关闭MD5上传校验
  void TurnOffComputeConentMd5() { m_need_compute_contentmd5 = false; }

  bool ShouldComputeContentMd5() const { return m_need_compute_contentmd5; }

  bool ShouldCheckETag() const { return m_need_check_etag; }

 protected:
  PutObjectReq(const std::string& bucket_name, const std::string& object_name)
      : ObjectReq(bucket_name, object_name) {
    m_method = "PUT";
    m_need_compute_contentmd5 = true;
    m_need_check_etag = true;
  }

  PutObjectReq() {}

  virtual ~PutObjectReq() {}

  /// \brief 不检查ETag
  void TurnOffCheckETag() { m_need_check_etag = false; }

 private:
  bool m_need_compute_contentmd5;
  bool m_need_check_etag;  // 某些api，比如append操作不检查响应中的ETag
};

class PutObjectByStreamReq : public PutObjectReq {
 public:
  PutObjectByStreamReq(const std::string& bucket_name,
                       const std::string& object_name, std::istream& in_stream)
      : PutObjectReq(bucket_name, object_name), m_in_stream(in_stream) {}

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
#if defined(_WIN32)
    mb_is_widechar_path = false;
#endif
  }

  virtual ~PutObjectByFileReq() {}

  virtual uint64_t GetLocalFileSize() const {
#if defined(_WIN32)
    if (mb_is_widechar_path)
      return FileUtil::GetFileLen(GetWideCharLocalFilePath());
    else
#endif
      return FileUtil::GetFileLen(m_local_file_path);
  }

  void SetLocalFilePath(const std::string& local_file_path) {
    m_local_file_path = local_file_path;
  }

  std::string GetLocalFilePath() const { return m_local_file_path; }

#if defined(_WIN32)
  /// \brief 如果本地文件路径为宽字符（中文，韩文等）,需要调用该函数
  void SetWideCharPath() { mb_is_widechar_path = true; }

  bool IsWideCharPath() const { return mb_is_widechar_path; }

  std::wstring GetWideCharLocalFilePath() const {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(m_local_file_path);
  }
#endif

 private:
  std::string m_local_file_path;
#if defined(_WIN32)
  bool mb_is_widechar_path;  // 标识文件路径是否为宽字符
#endif
};

class DeleteObjectReq : public ObjectReq {
 public:
  DeleteObjectReq(const std::string& bucket_name,
                  const std::string& object_name)
      : ObjectReq(bucket_name, object_name) {
    SetMethod("DELETE");
  }

  DeleteObjectReq(const std::string& bucket_name,
                  const std::string& object_name, const std::string& version_id)
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

  void SetQuiet() { m_is_quiet = true; }

  // getter and setter
  std::string GetBucketName() const { return m_bucket_name; }

  void SetBucketName(const std::string& bucket_name) {
    m_bucket_name = bucket_name;
  }

  std::vector<ObjectVersionPair> GetObjectVersions() const { return m_objvers; }

  uint32_t GetObjectVerionsSize() const { return m_objvers.size(); }

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
  HeadObjectReq(const std::string& bucket_name, const std::string& object_name)
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

  /// Content-Type    RFC 2616 中定义的内容类型（MIME），将作为 Object
  /// 元数据保存
  void SetContentType(const std::string& str) {
    AddHeader("Content-Type", str);
  }

  /// Expires RFC 2616 中定义的过期时间，将作为 Object 元数据保存
  void SetExpires(const std::string& str) { AddHeader("Expires", str); }

  /// 允许用户自定义的头部信息,将作为 Object 元数据返回.大小限制2K
  void SetXCosMeta(const std::string& key, const std::string& value) {
    AddHeader("x-cos-meta-" + key, value);
  }

  /// x-cos-storage-class 设置 Object 的存储级别，枚举值：STANDARD,STANDARD_IA，
  /// 默认值：STANDARD
  void SetXCosStorageClass(const std::string& storage_class) {
    AddHeader("x-cos-storage-class", storage_class);
  }

  /// 定义Object的ACL属性,有效值：private,public-read-write,public-read
  /// 默认值：private
  void SetXCosAcl(const std::string& str) { AddHeader("x-cos-acl", str); }

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
                    const std::string& object_name,
                    const std::string& upload_id, std::istream& in_stream)
      : ObjectReq(bucket_name, object_name), m_in_stream(in_stream),
        m_upload_id(upload_id), m_part_number(1) {
    m_method = "PUT";
    m_need_compute_contentmd5 = true;
  }

  /// \brief 设置本次分块上传的ID
  void SetUploadId(const std::string& upload_id) { m_upload_id = upload_id; }
  /// \brief 设置本次分块上传的编号
  void SetPartNumber(uint64_t part_number) { m_part_number = part_number; }

  std::string GetUploadId() const { return m_upload_id; }

  uint64_t GetPartNumber() const { return m_part_number; }

  std::istream& GetStream() const { return m_in_stream; }
  // 默认开启MD5上传校验
  void TurnOnComputeConentMd5() { m_need_compute_contentmd5 = true; }

  void TurnOffComputeConentMd5() { m_need_compute_contentmd5 = false; }

  bool ShouldComputeContentMd5() const { return m_need_compute_contentmd5; }

  /// \brief 设置单链接限速
  /// 参考https://cloud.tencent.com/document/product/436/40140
  void SetTrafficLimit(const uint64_t traffic_limit) {
    AddHeader("x-cos-traffic-limit", std::to_string(traffic_limit));
  }

  /// \brief 设置单链接限速
  /// 参考https://cloud.tencent.com/document/product/436/40140
  void SetTrafficLimit(const std::string& traffic_limit) {
    AddHeader("x-cos-traffic-limit", traffic_limit);
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
      : ObjectReq(bucket_name, object_name), m_upload_id(upload_id),
        m_part_number(1) {
    m_method = "PUT";
  }

  UploadPartCopyDataReq(const std::string& bucket_name,
                        const std::string& object_name,
                        const std::string& upload_id, uint64_t part_number)
      : ObjectReq(bucket_name, object_name), m_upload_id(upload_id),
        m_part_number(part_number) {
    m_method = "PUT";
  }

  /// \brief 设置本次分块复制的ID
  void SetUploadId(const std::string& upload_id) { m_upload_id = upload_id; }
  /// \brief 设置本次分块复制的编号
  void SetPartNumber(uint64_t part_number) { m_part_number = part_number; }

  std::string GetUploadId() const { return m_upload_id; }

  uint64_t GetPartNumber() const { return m_part_number; }

  /// \brief 设置本次分块复制的源文件 URL 路径，可以通过 versionid
  /// 子资源指定历史版本
  void SetXCosCopySource(const std::string& src) {
    AddHeader("x-cos-copy-source", src);
  }

  /// \brief 设置源文件的字节范围，
  ///        范围值必须使用 bytes=first-last 格式，first 和 last 都是基于 0
  ///        开始的偏移量。 例如 bytes=0-9
  ///        表示你希望拷贝源文件的开头10个字节的数据，如果不指定，则表示拷贝整个文件
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
                         const std::string& object_name,
                         const std::string& upload_id)
      : ObjectReq(bucket_name, object_name), m_upload_id(upload_id) {
    m_method = "POST";
  }

  virtual ~CompleteMultiUploadReq() {}

  std::string GetUploadId() const { return m_upload_id; }
  /// \brief 设置上传的ID
  void SetUploadId(const std::string& upload_id) { m_upload_id = upload_id; }

  std::vector<uint64_t> GetPartNumbers() const { return m_part_numbers; }
  /// \brief 设置上传的所有分块编号
  void SetPartNumbers(const std::vector<uint64_t>& part_numbers) {
    m_part_numbers.clear();
    m_part_numbers.insert(m_part_numbers.end(), part_numbers.begin(),
                          part_numbers.end());
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

/*
class PutObjectByFile : public ObjectReq {
 public:
  PutObjectByFile(const std::string& bucket_name,
                    const std::string& object_name,
                    const std::string& local_file_path = "")
      : ObjectReq(bucket_name, object_name)
#if defined(_WIN32)
        ,
        mb_is_widechar_path(false)
#endif
  {
    // 默认使用配置文件配置的分块大小和线程池大小
    m_part_size = CosSysConfig::GetUploadPartSize();
    m_thread_pool_size = CosSysConfig::GetUploadThreadPoolSize();
    mb_set_meta = false;

    // 默认打开当前路径下object的同名文件
    if (local_file_path.empty()) {
      m_local_file_path = "./" + object_name;
    } else {
      m_local_file_path = local_file_path;
    }
    // 分块上传默认检查crc64
    SetCheckCRC64(true);
  }
  virtual ~PutObjectByFile() {}

  void SetLocalFilePath(const std::string& local_file_path) {
    m_local_file_path = local_file_path;
  }

  std::string GetLocalFilePath() const { return m_local_file_path; }

  /// \brief 设置分块大小,若小于1M,则按1M计算;若大于5G,则按5G计算
  void SetPartSize(uint64_t bytes) {
    if (bytes <= kPartSize1M) {
      m_part_size = kPartSize1M;
    } else if (bytes >= kPartSize5G) {
      m_part_size = kPartSize5G;
    } else {
      m_part_size = bytes;
    }
  }

  /// \brief 获取分块大小
  uint64_t GetPartSize() const { return m_part_size; }

  void SetThreadPoolSize(int size) {
    assert(size > 0);
    m_thread_pool_size = size;
  }

  int GetThreadPoolSize() const { return m_thread_pool_size; }

  /// \brief 设置Server端加密使用的算法, 目前支持AES256
  void SetXCosServerSideEncryption(const std::string& str) {
    AddHeader("x-cos-server-side-encryption", str);
  }

  /// \brief 允许用户自定义的头部信息,将作为 Object 元数据返回.大小限制2K
  void SetXCosMeta(const std::string& key, const std::string& value) {
    mb_set_meta = true;
    m_xcos_meta.insert(std::pair<std::string, std::string>(key, value));
  }

  bool IsSetXCosMeta() const { return mb_set_meta; }

  std::map<std::string, std::string> GetXCosMeta() const { return m_xcos_meta; }

  /// \brief 请求参数中设置单链接限速, 参考
  /// https://cloud.tencent.com/document/product/436/40140
  void SetTrafficLimitByParam(const std::string& str) {
    if (GetHeader("x-cos-traffic-limit") == "") {
      AddParam("x-cos-traffic-limit", str);
    }
  }

  /// \brief 请求头中参数中设置单链接限速
  void SetTrafficLimitByHeader(const std::string& str) {
    if (GetParam("x-cos-traffic-limit") == "") {
      AddHeader("x-cos-traffic-limit", str);
    }
  }

  /// \brief 设置 Object 的存储类型
  ///
枚举值：MAZ_STANDARD，MAZ_STANDARD_IA，INTELLIGENT_TIERING，MAZ_INTELLIGENT_TIERING，STANDARD_IA，ARCHIVE，DEEP_ARCHIVE
  /// 默认值：STANDARD
  void SetXCosStorageClass(const std::string& storage_class) {
    AddHeader("x-cos-storage-class", storage_class);
  }

  /// \brief 设置Object的ACL属性
  void SetXCosAcl(const std::string& str) { AddHeader("x-cos-acl", str); }

  /// \briefCache-Control RFC 2616 中定义的缓存策略，将作为 Object 元数据保存
  void SetCacheControl(const std::string& str) {
    AddHeader("Cache-Control", str);
  }

  /// \brief Content-Disposition RFC 2616 中定义的文件名称，将作为 Object
  /// 元数据保存
  void SetContentDisposition(const std::string& str) {
    AddHeader("Content-Disposition", str);
  }

  /// \brief Content-Encoding    RFC 2616 中定义的编码格式，将作为 Object
  /// 元数据保存
  void SetContentEncoding(const std::string& str) {
    AddHeader("Content-Encoding", str);
  }

  /// \brief Content-Type    RFC 2616 中定义的内容类型（MIME），将作为 Object
  /// 元数据保存
  void SetContentType(const std::string& str) {
    AddHeader("Content-Type", str);
  }

  /// \brief Expect  当使用 Expect: 100-continue
  /// 时，在收到服务端确认后，才会发送请求内容
  void SetExpect(const std::string& str) { AddHeader("Expect", str); }

  /// \brief Expires RFC 2616 中定义的过期时间，将作为 Object 元数据保存
  void SetExpires(const std::string& str) { AddHeader("Expires", str); }

  void SetUploadID(const std::string& uploadid) {
    if (!uploadid.empty()) {
      m_uploadid = uploadid;
    }
  }

  std::string GetUploadID() const { return m_uploadid; }
#if defined(_WIN32)
  /// \brief 如果本地文件路径为宽字符（中文，韩文等）,需要调用该函数
  void SetWideCharPath() { mb_is_widechar_path = true; }

  bool IsWideCharPath() const { return mb_is_widechar_path; }

  std::wstring GetWideCharLocalFilePath() const {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(m_local_file_path);
  }
#endif
 private:
  std::string m_local_file_path;
  uint64_t m_part_size;
  int m_thread_pool_size;
  std::map<std::string, std::string> m_xcos_meta;
  bool mb_set_meta;
  std::string m_uploadid;
#if defined(_WIN32)
  bool mb_is_widechar_path;  // 标识文件路径是否为宽字符
#endif
};  // namespace qcloud_cos
*/
class AbortMultiUploadReq : public ObjectReq {
 public:
  AbortMultiUploadReq(const std::string& bucket_name,
                      const std::string& object_name,
                      const std::string& upload_id)
      : ObjectReq(bucket_name, object_name), m_upload_id(upload_id) {
    m_method = "DELETE";
  }

  virtual ~AbortMultiUploadReq() {}

  std::string GetUploadId() const { return m_upload_id; }

  /// \brief 设置本次终止上传的uploadId
  void SetUploadId(const std::string& upload_id) { m_upload_id = upload_id; }

 private:
  std::string m_upload_id;
};

class ListPartsReq : public ObjectReq {
 public:
  ListPartsReq(const std::string& bucket_name, const std::string& object_name,
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
  void SetXCosAcl(const std::string& str) { AddHeader("x-cos-acl", str); }

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
  void SetOwner(const Owner& owner) { m_owner = owner; }

  /// 设置被授权者信息与权限信息
  void SetAccessControlList(const std::vector<Grant>& grants) {
    m_acl = grants;
  }

  /// 添加单个 Bucket 的授权信息
  void AddAccessControlList(const Grant& grant) { m_acl.push_back(grant); }

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
  /// 可与 x-cos-copy-source-If-None-Match
  /// 一起使用，与其他条件联合使用返回冲突。
  void SetXCosCopySourceIfModifiedSince(const std::string& str) {
    AddHeader("x-cos-copy-source-If-Modified-Since", str);
  }

  /// 当 Object 在指定时间后未被修改，则执行操作，否则返回 412。
  /// 可与 x-cos-copy-source-If-Match 一起使用，与其他条件联合使用返回冲突。
  void SetXCosCopySourceIfUnmodifiedSince(const std::string& str) {
    AddHeader("x-cos-copy-source-If-Unmodified-Since", str);
  }

  /// 当 Object 的 Etag 和给定一致时，则执行操作，否则返回 412。
  /// 可与x-cos-copy-source-If-Unmodified-Since
  /// 一起使用，与其他条件联合使用返回冲突
  void SetXCosCopySourceIfMatch(const std::string& str) {
    AddHeader("x-cos-copy-source-If-Match", str);
  }

  /// 当 Object 的 Etag 和给定不一致时，则执行操作，否则返回 412。
  /// 可与 x-cos-copy-source-If-Modified-Since
  /// 一起使用，与其他条件联合使用返回冲突。
  void SetXCosCopySourceIfNoneMatch(const std::string& str) {
    AddHeader("x-cos-copy-source-If-None-Match", str);
  }

  /// 设置 Object 的存储类型
  /// 枚举值：MAZ_STANDARD，MAZ_STANDARD_IA，INTELLIGENT_TIERING，MAZ_INTELLIGENT_TIERING，STANDARD_IA，ARCHIVE，DEEP_ARCHIVE
  /// 默认值：STANDARD
  void SetXCosStorageClass(const std::string& storage_class) {
    AddHeader("x-cos-storage-class", storage_class);
  }

  /// 定义Object的ACL属性,有效值：private,public-read-write,public-read
  /// 默认值：private
  void SetXCosAcl(const std::string& str) { AddHeader("x-cos-acl", str); }

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
  CopyReq(const std::string& bucket_name, const std::string& object_name)
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
  /// 可与 x-cos-copy-source-If-None-Match
  /// 一起使用，与其他条件联合使用返回冲突。
  void SetXCosCopySourceIfModifiedSince(const std::string& str) {
    AddHeader("x-cos-copy-source-If-Modified-Since", str);
  }

  /// 当 Object 在指定时间后未被修改，则执行操作，否则返回 412。
  /// 可与 x-cos-copy-source-If-Match 一起使用，与其他条件联合使用返回冲突。
  void SetXCosCopySourceIfUnmodifiedSince(const std::string& str) {
    AddHeader("x-cos-copy-source-If-Unmodified-Since", str);
  }

  /// 当 Object 的 Etag 和给定一致时，则执行操作，否则返回 412。
  /// 可与x-cos-copy-source-If-Unmodified-Since
  /// 一起使用，与其他条件联合使用返回冲突
  void SetXCosCopySourceIfMatch(const std::string& str) {
    AddHeader("x-cos-copy-source-If-Match", str);
  }

  /// 当 Object 的 Etag 和给定不一致时，则执行操作，否则返回 412。
  /// 可与 x-cos-copy-source-If-Modified-Since
  /// 一起使用，与其他条件联合使用返回冲突。
  void SetXCosCopySourceIfNoneMatch(const std::string& str) {
    AddHeader("x-cos-copy-source-If-None-Match", str);
  }

  /// 设置 Object 的存储类型
  /// 枚举值：MAZ_STANDARD，MAZ_STANDARD_IA，INTELLIGENT_TIERING，MAZ_INTELLIGENT_TIERING，STANDARD_IA，ARCHIVE，DEEP_ARCHIVE
  /// 默认值：STANDARD
  void SetXCosStorageClass(const std::string& storage_class) {
    AddHeader("x-cos-storage-class", storage_class);
  }

  /// 定义Object的ACL属性,有效值：private,public-read-write,public-read
  /// 默认值：private
  void SetXCosAcl(const std::string& str) { AddHeader("x-cos-acl", str); }

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

  void SetThreadPoolSize(unsigned size) {
    assert(size > 0);
    m_thread_pool_size = size;
  }

  unsigned GetThreadPoolSize() const { return m_thread_pool_size; }

 private:
  uint64_t m_part_size;
  unsigned m_thread_pool_size;
};

class PostObjectRestoreReq : public ObjectReq {
 public:
  PostObjectRestoreReq(const std::string& bucket_name,
                       const std::string& object_name)
      : ObjectReq(bucket_name, object_name), m_expiry_days(0),
        m_tier("Standard") {
    m_method = "POST";
    AddParam("restore", "");
  }

  PostObjectRestoreReq(const std::string& bucket_name,
                       const std::string& object_name,
                       const std::string& version_id)
      : ObjectReq(bucket_name, object_name), m_expiry_days(0),
        m_tier("Standard") {
    m_method = "POST";
    AddParam("restore", "");
    if (!version_id.empty()) {
      AddHeader("x-cos-version-id", version_id);
    }
  }

  PostObjectRestoreReq(const std::string& bucket_name,
                       const std::string& object_name,
                       const std::string& version_id, const std::string& tier)
      : ObjectReq(bucket_name, object_name), m_expiry_days(0), m_tier(tier) {
    m_method = "POST";
    AddParam("restore", "");
    if (!version_id.empty()) {
      AddHeader("x-cos-version-id", version_id);
    }
  }

  virtual ~PostObjectRestoreReq() {}

  /// \brief  设置临时副本的过期时间
  void SetExiryDays(uint64_t days) { m_expiry_days = days; }

  /// \brief 指定版本号的对象
  void SetXCosVersionId(const std::string& version_id) {
    AddHeader("x-cos-version-id", version_id);
  }

  // \brief 具体复原过程类型，枚举值： Expedited ，Standard
  // ，Bulk；默认值：Standard
  void SetTier(const std::string& tier) { m_tier = tier; }

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
        m_expired_time_in_s(0), m_use_https(true) {
    m_path = "/" + object_name;
    m_method = StringUtil::HttpMethodToString(http_method);
  }

  virtual ~GeneratePresignedUrlReq() {}

  uint64_t GetStartTimeInSec() const { return m_start_time_in_s; }
  uint64_t GetExpiredTimeInSec() const { return m_expired_time_in_s; }
  void SetStartTimeInSec(uint64_t start_time) {
    m_start_time_in_s = start_time;
  }
  void SetExpiredTimeInSec(uint64_t expired_time) {
    m_expired_time_in_s = expired_time;
  }

  /// \brief 是否生成https链接，默认为true
  void SetUseHttps(bool use_https) { m_use_https = use_https; }
  bool UseHttps() const { return m_use_https; }


 private:
  uint64_t m_start_time_in_s;
  uint64_t m_expired_time_in_s;
  bool m_use_https;         // 是否启用https
};

class OptionsObjectReq : public ObjectReq {
 public:
  OptionsObjectReq(const std::string& bucket_name,
                   const std::string& object_name)
      : ObjectReq(bucket_name, object_name) {
    m_method = "OPTIONS";
    m_path = "/" + object_name;
  }

  virtual ~OptionsObjectReq() {}

  /// \brief
  /// 添加Origin头部，参考官网https://cloud.tencent.com/document/product/436/8288
  void SetOrigin(const std::string& origin) { AddHeader("Origin", origin); }

  /// \brief 添加跨域访问的请求HTTP方法的头部
  void SetAccessControlRequestMethod(const std::string& method) {
    AddHeader("Access-Control-Request-Method", method);
  }

  /// \brief 添加跨域访问的请求头部的头部
  void SetAccessControlRequestHeaders(const std::string& headers) {
    AddHeader("Access-Control-Request-Headers", headers);
  }
};

class SelectObjectContentReq : public ObjectReq {
 public:
  /// \brief
  /// input_file_type: 待检索对象的格式为csv或者json
  /// out_file_type: 输出格式为csv或者json
  SelectObjectContentReq(const std::string& bucket_name,
                         const std::string& object_name,
                         int input_file_type = CSV,
                         int input_compress_type = COMPRESS_NONE,
                         int out_file_type = CSV)
      : ObjectReq(bucket_name, object_name), m_input_file_type(input_file_type),
        m_input_compress_type(input_compress_type),
        m_output_file_type(out_file_type), m_expression_type("SQL"),
        m_request_progress(false) {
    m_method = "POST";
    m_path = "/" + object_name;
    AddParam("select", "");
    AddParam("select-type", "2");
    GenDefaultInputSerialization();
    GenDefaultOutputSerialization();
  }

  virtual ~SelectObjectContentReq() {}

  /// \brief
  /// 设置SQL表达式，参考官网https://cloud.tencent.com/document/product/436/37641
  void SetSqlExpression(const std::string& sql_expression) {
    m_sql_expression = sql_expression;
  }

  /// \brief 设置SQL表达式类型，目前只支持SQL
  void SetExpressionType(const std::string& expression_type) {
    m_sql_expression = expression_type;
  }

  /// \brief input描述待检索对象的格式
  void SetInputSerialization(const std::string& input) {
    m_inputserialization = input;
  }

  /// \brief output描述检索结果的输出格式
  void SetOutputSerialization(const std::string& output) {
    m_outputserialization = output;
  }

  /// \brief 是否需要返回查询进度 QueryProgress 信息，如果选中 COS Select
  /// 将周期性返回查询进度
  void SetRequestProgress(bool progress) { m_request_progress = progress; }

  bool GenerateRequestBody(std::string* body) const;

 private:
  /// \brief 根据m_input_csv产生默认的待检索对象的格式
  void GenDefaultInputSerialization() {
    std::ostringstream input_xml;
    input_xml << "<InputSerialization>";
    if (m_input_compress_type == COMPRESS_GZIP) {
      input_xml << "<CompressionType>GZIP</CompressionType>";
    } else if (m_input_compress_type == COMPRESS_BZIP2) {
      input_xml << "<CompressionType>BZIP2</CompressionType>";
    } else {
      input_xml << "<CompressionType>NONE</CompressionType>";
    }
    if (m_input_file_type == CSV) {
      input_xml << "<CSV>";
      input_xml << "<FileHeaderInfo>NONE</FileHeaderInfo>";
      input_xml << "<RecordDelimiter>\\n</RecordDelimiter>";
      input_xml << "<FieldDelimiter>,</FieldDelimiter>";
      input_xml << "<QuoteCharacter>\"</QuoteCharacter>";
      input_xml << "<QuoteEscapeCharacter>\"</QuoteEscapeCharacter>";
      input_xml << "<Comments>#</Comments>";
      input_xml
          << "<AllowQuotedRecordDelimiter>FALSE</AllowQuotedRecordDelimiter>";
      input_xml << "</CSV>";
      input_xml << "</InputSerialization>";
    } else if (m_input_file_type == JSON) {
      input_xml << "<JSON>";
      input_xml << "<Type>DOCUMENT</Type>";
      input_xml << "</JSON>";
      input_xml << "</InputSerialization>";
    }
    m_inputserialization = input_xml.str();
  }

  /// \brief 根据m_out_csv产生默认的检索结果的输出格式
  void GenDefaultOutputSerialization() {
    std::ostringstream output_xml;
    if ((m_output_file_type == CSV)) {
      output_xml << "<OutputSerialization>";
      output_xml << "<CSV>";
      output_xml << "<QuoteFields>ASNEEDED</QuoteFields>";
      output_xml << "<RecordDelimiter>\\n</RecordDelimiter>";
      output_xml << "<FieldDelimiter>,</FieldDelimiter>";
      output_xml << "<QuoteCharacter>\"</QuoteCharacter>";
      output_xml << "<QuoteEscapeCharacter>\"</QuoteEscapeCharacter>";
      output_xml << "</CSV>";
      output_xml << "</OutputSerialization>";
    } else if (m_output_file_type == JSON) {
      output_xml << "<OutputSerialization>";
      output_xml << "<JSON>";
      output_xml << "<RecordDelimiter>\\n</RecordDelimiter>";
      output_xml << "</JSON>";
      output_xml << "</OutputSerialization>";
    }
    m_outputserialization = output_xml.str();
  }

  int m_input_file_type;              // csv or json
  int m_input_compress_type;          // NONE,GZIP,BZIP2
  int m_output_file_type;             // csv or json
  std::string m_sql_expression;       // like "Select * from COSObject"
  std::string m_expression_type;      // default is "SQL"
  std::string m_inputserialization;   // xml format
  std::string m_outputserialization;  // xml format
  bool m_request_progress;            // default is false
};

class AppendObjectReq : public PutObjectByStreamReq {
 public:
  AppendObjectReq(const std::string& bucket_name,
                  const std::string& object_name, std::istream& in_stream)
      : PutObjectByStreamReq(bucket_name, object_name, in_stream) {
    m_method = "POST";
    m_path = "/" + object_name;
    AddParam("append", "");
    AddParam("position", "0");
    // append返回的ETag不是body的MD5，不需要检查Etag
    TurnOffCheckETag();
  }

  virtual ~AppendObjectReq() {}

  /// \brief
  /// 添加position参数，参考官网https://cloud.tencent.com/document/product/436/7741
  void SetPosition(const std::string& position) {
    AddParam("position", position);
  }
};

/// \brief: 创建直播通道请求
class PutLiveChannelReq : public ObjectReq {
 public:
  /// \brief
  /// channel_name: 通道名
  PutLiveChannelReq(const std::string& bucket_name,
                    const std::string& channel_name)
      : ObjectReq(bucket_name, channel_name) {
    m_method = "PUT";
    m_path = "/" + channel_name;
    AddParam("live", "");
    m_expire_sec = 3600;
  }

  virtual ~PutLiveChannelReq() {}

  void SetLiveChannelConfig(const LiveChannelConfiguration& config) {
    m_config = config;
  }

  /// brief: 设置url参数
  void SetUrlParams(const std::map<std::string, std::string>& url_params) {
    m_url_params = url_params;
  }

  const std::map<std::string, std::string>& GetUrlParams() const {
    return m_url_params;
  }

  /// brief: 设置推流签名过期时间
  void SetExpire(int expire) { m_expire_sec = expire; }

  int GetExpire() const { return m_expire_sec; }

  bool GenerateRequestBody(std::string* body) const;

 private:
  LiveChannelConfiguration m_config;
  std::map<std::string, std::string> m_url_params;
  int m_expire_sec;
};

/// \brief: 启用或者禁用直播通道请求
class PutLiveChannelSwitchReq : public ObjectReq {
 public:
  /// \brief
  /// channel_name: 通道名
  PutLiveChannelSwitchReq(const std::string& bucket_name,
                          const std::string& channel_name)
      : ObjectReq(bucket_name, channel_name) {
    m_method = "PUT";
    m_path = "/" + channel_name;
    AddParam("live", "");
    AddParam("switch", "enabled");
  }

  virtual ~PutLiveChannelSwitchReq() {}

  /// \brief 启用通道
  void SetEnabled() { AddParam("switch", "enabled"); }

  /// \brief 禁用通道
  void SetDisabled() { AddParam("switch", "disabled"); }
};

/// \brief: 获取直播通道配置
class GetLiveChannelReq : public ObjectReq {
 public:
  GetLiveChannelReq(const std::string& bucket_name,
                    const std::string& channel_name)
      : ObjectReq(bucket_name, channel_name) {
    m_method = "GET";
    m_path = "/" + channel_name;
    AddParam("live", "");
  }
  virtual ~GetLiveChannelReq() {}
};

/// \brief: 获取直播通道推流历史
class GetLiveChannelHistoryReq : public ObjectReq {
 public:
  GetLiveChannelHistoryReq(const std::string& bucket_name,
                           const std::string& channel_name)
      : ObjectReq(bucket_name, channel_name) {
    m_method = "GET";
    m_path = "/" + channel_name;
    AddParam("live", "");
    AddParam("comp", "history");
  }
  virtual ~GetLiveChannelHistoryReq() {}
};

/// \brief: 获取直播通道推流状态
class GetLiveChannelStatusReq : public ObjectReq {
 public:
  GetLiveChannelStatusReq(const std::string& bucket_name,
                          const std::string& channel_name)
      : ObjectReq(bucket_name, channel_name) {
    m_method = "GET";
    m_path = "/" + channel_name;
    AddParam("live", "");
    AddParam("comp", "status");
  }
  virtual ~GetLiveChannelStatusReq() {}
};

/// \brief: 删除直播通道
class DeleteLiveChannelReq : public ObjectReq {
 public:
  DeleteLiveChannelReq(const std::string& bucket_name,
                       const std::string& channel_name)
      : ObjectReq(bucket_name, channel_name) {
    m_method = "DELETE";
    m_path = "/" + channel_name;
    AddParam("live", "");
  }
  virtual ~DeleteLiveChannelReq() {}
};

/// \brief: 查询指定通道在指定时间段推流生成的播放列表
class GetLiveChannelVodPlaylistReq : public ObjectReq {
 public:
  GetLiveChannelVodPlaylistReq(const std::string& bucket_name,
                               const std::string& channel_name)
      : ObjectReq(bucket_name, channel_name) {
    m_method = "GET";
    m_path = "/" + channel_name;
    AddParam("vod", "");
  }
  virtual ~GetLiveChannelVodPlaylistReq() {}
  /// \brief time格式为unix时间戳，必须调用该接口设置开始时间和结束时间
  void SetTime(uint64_t starttime, uint64_t endtime) {
    AddParam("starttime", StringUtil::Uint64ToString(starttime));
    AddParam("endtime", StringUtil::Uint64ToString(endtime));
  }
};

/// \brief: 为指定通道生成一个可供点播例用的播放列表
class PostLiveChannelVodPlaylistReq : public ObjectReq {
 public:
  PostLiveChannelVodPlaylistReq(const std::string& bucket_name,
                                const std::string& channel_name)
      : ObjectReq(bucket_name, channel_name) {
    m_method = "POST";
    m_path = "/" + channel_name;
    AddParam("vod", "");
  }
  virtual ~PostLiveChannelVodPlaylistReq() {}

  /// \brief time格式为unix时间戳，必须调用该接口设置开始时间和结束时间
  void SetTime(uint64_t starttime, uint64_t endtime) {
    AddParam("starttime", StringUtil::Uint64ToString(starttime));
    AddParam("endtime", StringUtil::Uint64ToString(endtime));
  }

  /// \brief 必须调用该接口设置播放列表名称
  void SetPlaylistName(const std::string& playlist_name) {
    m_path.append("/" + playlist_name);
  }
};

/* Multithread接口 */

class MultiPutObjectReq : public PutObjectByFileReq {
 public:
  MultiPutObjectReq(const std::string& bucket_name,
                    const std::string& object_name,
                    const std::string& local_file_path)
      : PutObjectByFileReq(bucket_name, object_name, local_file_path) {
        SetCheckCRC64(true);
      }
  virtual ~MultiPutObjectReq() {}
};

class MultiGetObjectReq : public GetObjectByFileReq {
 public:
  MultiGetObjectReq(const std::string& bucket_name,
                    const std::string& object_name,
                    const std::string& local_file_path)
      : GetObjectByFileReq(bucket_name, object_name, local_file_path) {}
  virtual ~MultiGetObjectReq() {}
};

/* Async接口 */

#if 0
class AsyncReq {
 public:
  AsyncReq(const std::string& bucket_name, const std::string& object_name,
           const std::string& local_file_path, bool wide_char_path = false)
      : m_bucket_name(bucket_name), m_object_name(object_name),
        m_local_file_path(local_file_path) {}

  virtual ~AsyncReq() {}
  std::string GetBucketName() const { return m_bucket_name; }
  std::string GetObjectName() const { return m_object_name; }
  std::string GetLocalFilePath() const { return m_local_file_path; }
  virtual uint64_t GetLocalFileSize() const { return 0; }
#if defined(_WIN32)
  /// \brief 如果本地文件路径为宽字符（中文，韩文等）,需要调用该函数
  void SetWideCharPath() { mb_is_widechar_path = true; }

  bool IsWideCharPath() const { return mb_is_widechar_path; }
  /// \brief 获取宽字符路径
  std::wstring GetWideCharLocalFilePath() const {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(m_local_file_path);
  }
#endif
  /// @brief  设置进度回调函数
  void SetTransferProgressCallback(const TransferProgressCallback& process_cb) {
    m_progress_cb = process_cb;
  }
  /// @brief  设置完成回调函数
  void SetDoneCallback(const DoneCallback& done_cb) { m_done_cb = done_cb; }
  /// @brief 设置回调私有数据
  void SetUserData(void* user_data) { m_user_data = user_data; }

  TransferProgressCallback GetTransferProgressCallback() const {
    return m_progress_cb;
  }
  DoneCallback GetDoneCallback() const { return m_done_cb; }
  void* GetUserData() const { return m_user_data; }

 protected:
  std::string m_bucket_name;
  std::string m_object_name;
  bool mb_is_widechar_path;
  std::string m_local_file_path;
  TransferProgressCallback m_progress_cb;  // 进度回调
  DoneCallback m_done_cb;                  // 完成回调
  void* m_user_data;
};
#endif

class AsyncPutObjectReq : public PutObjectByFileReq {
 public:
  AsyncPutObjectReq(const std::string& bucket_name, const std::string& object_name,
              const std::string& local_file_path)
      : PutObjectByFileReq(bucket_name, object_name, local_file_path) {}

  virtual ~AsyncPutObjectReq() {}
};

class AsyncPutObjectByStreamReq : public PutObjectByStreamReq {
 public:
  AsyncPutObjectByStreamReq(const std::string& bucket_name, const std::string& object_name,
              std::istream& in_stream)
      : PutObjectByStreamReq(bucket_name, object_name, in_stream) {}

  virtual ~AsyncPutObjectByStreamReq() {}
};

class AsyncMultiPutObjectReq : public PutObjectByFileReq {
 public:
  AsyncMultiPutObjectReq(const std::string& bucket_name, const std::string& object_name,
              const std::string& local_file_path)
      : PutObjectByFileReq(bucket_name, object_name, local_file_path) {}

  virtual ~AsyncMultiPutObjectReq() {}
};

class AsyncGetObjectReq : public GetObjectByFileReq {
 public:
  AsyncGetObjectReq(const std::string& bucket_name, const std::string& object_name,
              const std::string& local_file_path)
      : GetObjectByFileReq(bucket_name, object_name, local_file_path) {}

  virtual ~AsyncGetObjectReq() {}
};

class AsyncMultiGetObjectReq : public GetObjectByFileReq {
 public:
  AsyncMultiGetObjectReq(const std::string& bucket_name, const std::string& object_name,
              const std::string& local_file_path)
      : GetObjectByFileReq(bucket_name, object_name, local_file_path) {}

  virtual ~AsyncMultiGetObjectReq() {}
};
/* 批量及目录操作接口 */

class PutObjectsByDirectoryReq : public PutObjectReq {
 public:
  PutObjectsByDirectoryReq(const std::string& bucket_name,
                           const std::string& directory_name)
      : m_bucket_name(bucket_name), m_directory_name(directory_name),
        m_cos_path("") {
    m_need_compute_contentmd5 = true;  // 默认打开
  }

  PutObjectsByDirectoryReq(const std::string& bucket_name,
                           const std::string& directory_name,
                           const std::string& cos_path)
      : m_bucket_name(bucket_name), m_directory_name(directory_name),
        m_cos_path(cos_path) {
    m_need_compute_contentmd5 = true;  // 默认打开
  }

  virtual ~PutObjectsByDirectoryReq() {}

  std::string GetBucketName() const { return m_bucket_name; }

  std::string GetDirectoryName() const { return m_directory_name; }

  // 设置上传到cos的路径
  void SetCosPath(const std::string& cos_path) { m_cos_path = cos_path; }

  std::string GetCosPath() const { return m_cos_path; }

  void TurnOnComputeConentMd5() { m_need_compute_contentmd5 = true; }

  void TurnOffComputeConentMd5() { m_need_compute_contentmd5 = false; }

  bool ShouldComputeContentMd5() const { return m_need_compute_contentmd5; }

 private:
  // TODO 支持前缀和后缀
  std::string m_bucket_name;
  std::string m_directory_name;
  std::string m_cos_path;
  bool m_need_compute_contentmd5;
};

class PutDirectoryReq : public PutObjectReq {
 public:
  PutDirectoryReq(const std::string& bucket_name,
                  const std::string& directory_name)
      : PutObjectReq(bucket_name, directory_name) {}

  virtual ~PutDirectoryReq() {}
};

class MoveObjectReq {
 public:
  MoveObjectReq(const std::string& bucket_name,
                const std::string& src_object_name,
                const std::string& dst_object_name)
      : m_bucket_name(bucket_name), m_src_object_name(src_object_name),
        m_dst_object_name(dst_object_name) {}
  virtual ~MoveObjectReq() {}

  std::string GetBucketName() const { return m_bucket_name; }

  std::string GetSrcObjectName() const { return m_src_object_name; }

  std::string GetDstObjectName() const { return m_dst_object_name; }

 private:
  std::string m_bucket_name;
  std::string m_src_object_name;
  std::string m_dst_object_name;
};

class DeleteObjectsByPrefixReq {
 public:
  DeleteObjectsByPrefixReq(const std::string& bucket_name,
                           const std::string& prefix)
      : m_bucket_name(bucket_name), m_prefix(prefix) {}
  virtual ~DeleteObjectsByPrefixReq() {}

  std::string GetBucketName() const { return m_bucket_name; }

  std::string GetPrefix() const { return m_prefix; }

 private:
  std::string m_bucket_name;
  std::string m_prefix;
};

}  // namespace qcloud_cos