#ifndef COS_PARAMS_H
#define COS_PARAMS_H

#include <string>

namespace qcloud_cos{

/// http header中的Authorization字段
const std::string kHttpHeaderAuthorization = "Authorization";
//const std::string kParaCustomHeaders = "custom_headers";
const std::string kHttpHeaderCacheControl = "Cache-Control";
const std::string kHttpHeaderContentType = "Content-Type";
const std::string kHttpHeaderContentLength = "Content-Length";
const std::string kHttpHeaderContentDisposition = "Content-Disposition";
const std::string kHttpHeaderContentLanguage = "Content-Language";
const std::string kHttpHeaderContentEncoding = "Content-Encoding";
const std::string kHttpHeaderContentRange = "Content-Range";
const std::string kHttpHeaderExpires = "Expires";
const std::string kHttpHeaderLastModified = "Last-Modified";
const std::string kHttpHeaderConnection = "Connection";
const std::string kHttpHeaderDate = "Date";
const std::string kHttpHeaderServer = "Server";
const std::string kHttpHeaderEtag = "ETag";
const std::string kHttpHeaderLowerCaseEtag = "Etag";

const std::string kParaXCosMetaPrefix = "x-cos-meta-";

const std::string kParaMoveDstFileid = "dest_fileid";
const std::string kParaMoveOverWrite = "to_over_write";

const std::string kParaListNum = "num";
const std::string kParaListFlag = "list_flag";
const std::string kParaListContext = "context";

// const std::string kParaErrorDesc = "parameter error";
// const std::string kNetworkErrorDesc = "network error";
// const std::string kLocalFileNotExistDesc = "local file not exist";
// const std::string kParaPathIleagel = "path ileagel error";
// const std::string kCanNotOpRootPath = "can not operator root folder";

// x-cos-meta-前缀
const std::string kXCosMetaPrefix = "x-cos-meta-";

// Request Header
const std::string kReqHeaderEtag = "ETag";
const std::string kReqHeaderLowerCaseEtag = "Etag";
const std::string kReqHeaderContentLen = "Content-Length";
const std::string kReqHeaderContentType = "Content-Type";
const std::string kReqHeaderConnection = "Connection";
const std::string kReqHeaderDate = "Date";
const std::string kReqHeaderServer = "Server";
const std::string kReqHeaderXCosReqId = "x-cos-request-id";
const std::string kReqHeaderXCosTraceId = "x-cos-trace-id";

// Response Header
const std::string kRespHeaderLastModified = "Last-Modified";
const std::string kRespHeaderXCosObjectType = "x-cos-object-type";
const std::string kRespHeaderXCosStorageClass = "x-cos-storage-class";
const std::string kRespHeaderXCosHashCrc64Ecma = "x-cos-hash-crc64ecma";
const std::string kRespHeaderXCosStorageTier = "x-cos-storage-tier";
const std::string kRespHeaderXCosReqId = "x-cos-request-id";
const std::string kRespHeaderXCosTraceId = "x-cos-trace-id";
const std::string kRespHeaderXCosNextAppendPosition = "x-cos-next-append-position";
const std::string kRespHeaderXCosContentSha1 = "x-cos-content-sha1";

// doc preview response header
const std::string kRespHeaderXTotalPage = "X-Total-Page";
const std::string kRespHeaderXErrNo = "X-ErrNo";
const std::string kRespHeaderXTotalSheet = "X-Total-Sheet";
const std::string kRespHeaderXSheetName = "X-Sheet-Name";


// V5 返回错误信息的xml node名
const std::string kErrorRoot = "Error";
const std::string kErrorCode = "Code";
const std::string kErrorMessage = "Message";
const std::string kErrorResource = "Resource";
const std::string kErrorTraceId = "TraceId";
const std::string kErrorRequestId = "RequestId";
const std::string kErrorServerTime = "ServerTime";

// GetBucketResponse XML node
const std::string kGetBucketRoot = "ListBucketResult";
const std::string kGetBucketName = "Name";
const std::string kGetBucketDelimiter = "Delimiter";
const std::string kGetBucketEncodingType = "EncodingType";
const std::string kGetBucketNextMarker = "NextMarker";
const std::string kGetBucketPrefix = "Prefix";
const std::string kGetBucketMarker = "Marker";
const std::string kGetBucketMaxKeys = "MaxKeys";
const std::string kGetBucketIsTruncated = "IsTruncated";
const std::string kGetBucketCommonPrefixes = "CommonPrefixes";
const std::string kGetBucketContents = "Contents";
const std::string kGetBucketContentsKey = "Key";
const std::string kGetBucketContentsLastModified = "LastModified";
const std::string kGetBucketContentsETag = "ETag";
const std::string kGetBucketContentsSize = "Size";
const std::string kGetBucketContentsStorageClass = "StorageClass";
const std::string kGetBucketContentsOwner = "Owner";
const std::string kGetBucketContentsOwnerID = "ID";

// ListMultipartUpload XML node
const std::string kListMultipartUploadRoot = "ListMultipartUploadsResult";
const std::string kListMultipartUploadBucket = "Bucket";
const std::string kListMultipartUploadMarker = "KeyMarker";
const std::string kListMultipartUploadIdMarker = "UploadIdMarker";
const std::string kListMultipartUploadNextKeyMarker = "NextKeyMarker";
const std::string kListMultipartUploadNextUploadIdMarker = "NextUploadIdMarker";
const std::string kListMultipartUploadMaxUploads = "MaxUploads";
const std::string kListMultipartUploadUpload = "Upload";
const std::string kListMultipartUploadKey = "Key";
const std::string kListMultipartUploadId = "UploadId";
const std::string kListMultipartUploadStorageClass = "StorageClass";
const std::string kListMultipartUploadInitiator = "Initiator";
const std::string kListMultipartUploadOwner = "Owner";
const std::string kListMultipartUploadInitiated = "Initiated";
const std::string kListMultipartUploadID = "ID";
const std::string kListMultipartUploadDisplayName = "DisplayName";

// BucketReplicationResponse XML node
const std::string kBucketReplicationRoot = "ReplicationConfiguration";
const std::string kBucketReplicationRule = "Rule";
const std::string kBucketReplicationID = "ID";
const std::string kBucketReplicationRole = "Role";
const std::string kBucketReplicationPrefix = "Prefix";
const std::string kBucketReplicationStatus = "Status";
const std::string kBucketReplicationDestination = "Destination";
const std::string kBucketReplicationBucket = "Bucket";
const std::string kBucketReplicationStorageClass = "StorageClass";

// InitMultiUploadResp XML node
const std::string kInitiateMultipartUploadRoot = "InitiateMultipartUploadResult";
const std::string kInitiateMultipartUploadBucket = "Bucket";
const std::string kInitiateMultipartUploadKey = "Key";
const std::string kInitiateMultipartUploadId = "UploadId";

// CompleteMultiUploadResp XML node
const std::string kCompleteMultiUploadRoot = "CompleteMultipartUploadResult";
const std::string kCompleteMultiUploadLocation = "Location";
const std::string kCompleteMultiUploadBucket = "Bucket";
const std::string kCompleteMultiUploadKey = "Key";
const std::string kCompleteMultiUploadETag = "ETag";

// StorageClass
const std::string kStorageClassStandard = "STANDARD";
const std::string kStorageClassStandardIA = "STANDARD_IA";
const std::string kStorageClassMAZStandard = "MAZ_STANDARD";
const std::string kStorageClassMAZStandardIA = "MAZ_STANDARD_IA";
const std::string kStorageClassIntelligentTiering = "INTELLIGENT_TIERING";
const std::string kStorageClassArchive = "ARCHIVE";
const std::string kStorageClassDeepArchive = "DEEP_ARCHIVE";

// Resumable download
const std::string kResumableDownloadTaskFileSuffix = ".cosresumabletask";
const std::string kResumableDownloadFileName = "fileName";
const std::string kResumableDownloadTaskLastModified = "lastModified";
const std::string kResumableDownloadTaskContentLength = "contentLength";
const std::string kResumableDownloadTaskEtag = "eTag";
const std::string kResumableDownloadTaskCrc64ecma = "crc64ecma";
const std::string kResumableDownloadResumeOffset = "resumeOffset";

// 预设ACL
const std::string kAclDefault = "default";
const std::string kAclPrivate = "private";
const std::string kAclPublicRead = "public-read";
const std::string kAclPublicReadWrite = "public-read-write";
const std::string kAclAuthenticatedRead = "authenticated-read";
const std::string kAclBucketOwnerRead = "bucket-owner-read";

// object type
const std::string kObjectTypeAppendable = "appendable";
const std::string kObjectTypeNormal = "normal";
} // namespace qcloud_cos
#endif
