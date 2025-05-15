#ifndef COS_CPP_SDK_V5_INCLUDE_COS_PARAMS_H_
#define COS_CPP_SDK_V5_INCLUDE_COS_PARAMS_H_

#include <string>

namespace qcloud_cos {

/// http header中的Authorization字段
const char kHttpHeaderAuthorizatio[] = "Authorization";
//const std::string kParaCustomHeaders = "custom_headers";
const char kHttpHeaderCacheControl[] = "Cache-Control";
const char kHttpHeaderContentType[] = "Content-Type";
const char kHttpHeaderContentLength[] = "Content-Length";
const char kHttpHeaderContentDisposition[] = "Content-Disposition";
const char kHttpHeaderContentLanguage[] = "Content-Language";
const char kHttpHeaderContentEncoding[] = "Content-Encoding";
const char kHttpHeaderContentRange[] = "Content-Range";
const char kHttpHeaderExpires[] = "Expires";
const char kHttpHeaderLastModified[] = "Last-Modified";
const char kHttpHeaderConnection[] = "Connection";
const char kHttpHeaderDate[] = "Date";
const char kHttpHeaderServer[] = "Server";
const char kHttpHeaderEtag[] = "ETag";
const char kHttpHeaderLowerCaseEtag[] = "Etag";

const char kParaXCosMetaPrefix[] = "x-cos-meta-";

const char kParaMoveDstFileid[] = "dest_fileid";
const char kParaMoveOverWrite[] = "to_over_write";

const char kParaListNum[] = "num";
const char kParaListFlag[] = "list_flag";
const char kParaListContext[] = "context";

// const std::string kParaErrorDesc = "parameter error";
// const std::string kNetworkErrorDesc = "network error";
// const std::string kLocalFileNotExistDesc = "local file not exist";
// const std::string kParaPathIleagel = "path ileagel error";
// const std::string kCanNotOpRootPath = "can not operator root folder";

// x-cos-meta-前缀
const char kXCosMetaPrefix[] = "x-cos-meta-";

// Request Header
const char kReqHeaderEtag[] = "ETag";
const char kReqHeaderLowerCaseEtag[] = "Etag";
const char kReqHeaderContentLen[] = "Content-Length";
const char kReqHeaderContentType[] = "Content-Type";
const char kReqHeaderConnection[] = "Connection";
const char kReqHeaderDate[] = "Date";
const char kReqHeaderServer[] = "Server";
const char kReqHeaderXCosReqId[] = "x-cos-request-id";
const char kReqHeaderXCosTraceId[] = "x-cos-trace-id";

// Response Header
const char kRespHeaderLastModified[] = "Last-Modified";
const char kRespHeaderXCosObjectType[] = "x-cos-object-type";
const char kRespHeaderXCosStorageClass[] = "x-cos-storage-class";
const char kRespHeaderXCosHashCrc64Ecma[] = "x-cos-hash-crc64ecma";
const char kRespHeaderXCosStorageTier[] = "x-cos-storage-tier";
const char kRespHeaderXCosReqId[] = "x-cos-request-id";
const char kRespHeaderXCosTraceId[] = "x-cos-trace-id";
const char kRespHeaderXCosNextAppendPosition[] = "x-cos-next-append-position";
const char kRespHeaderXCosContentSha1[] = "x-cos-content-sha1";
const char kRespHeaderXCosTaggingCount[] = "x-cos-tagging-count";

// doc preview response header
const char kRespHeaderXTotalPage[] = "X-Total-Page";
const char kRespHeaderXErrNo[] = "X-ErrNo";
const char kRespHeaderXTotalSheet[] = "X-Total-Sheet";
const char kRespHeaderXSheetName[] = "X-Sheet-Name";


// V5 返回错误信息的xml node名
const char kErrorRoot[] = "Error";
const char kErrorCode[] = "Code";
const char kErrorMessage[] = "Message";
const char kErrorResource[] = "Resource";
const char kErrorTraceId[] = "TraceId";
const char kErrorRequestId[] = "RequestId";
const char kErrorServerTime[] = "ServerTime";

// GetBucketResponse XML node
const char kGetBucketRoot[] = "ListBucketResult";
const char kGetBucketName[] = "Name";
const char kGetBucketDelimiter[] = "Delimiter";
const char kGetBucketEncodingType[] = "EncodingType";
const char kGetBucketNextMarker[] = "NextMarker";
const char kGetBucketPrefix[] = "Prefix";
const char kGetBucketMarker[] = "Marker";
const char kGetBucketMaxKeys[] = "MaxKeys";
const char kGetBucketIsTruncated[] = "IsTruncated";
const char kGetBucketCommonPrefixes[] = "CommonPrefixes";
const char kGetBucketContents[] = "Contents";
const char kGetBucketContentsKey[] = "Key";
const char kGetBucketContentsLastModified[] = "LastModified";
const char kGetBucketContentsETag[] = "ETag";
const char kGetBucketContentsSize[] = "Size";
const char kGetBucketContentsStorageClass[] = "StorageClass";
const char kGetBucketContentsOwner[] = "Owner";
const char kGetBucketContentsOwnerID[] = "ID";

// ListMultipartUpload XML node
const char kListMultipartUploadRoot[] = "ListMultipartUploadsResult";
const char kListMultipartUploadBucket[] = "Bucket";
const char kListMultipartUploadMarker[] = "KeyMarker";
const char kListMultipartUploadIdMarker[] = "UploadIdMarker";
const char kListMultipartUploadNextKeyMarker[] = "NextKeyMarker";
const char kListMultipartUploadNextUploadIdMarker[] = "NextUploadIdMarker";
const char kListMultipartUploadMaxUploads[] = "MaxUploads";
const char kListMultipartUploadUpload[] = "Upload";
const char kListMultipartUploadKey[] = "Key";
const char kListMultipartUploadId[] = "UploadId";
const char kListMultipartUploadStorageClass[] = "StorageClass";
const char kListMultipartUploadInitiator[] = "Initiator";
const char kListMultipartUploadOwner[] = "Owner";
const char kListMultipartUploadInitiated[] = "Initiated";
const char kListMultipartUploadID[] = "ID";
const char kListMultipartUploadDisplayName[] = "DisplayName";

// BucketReplicationResponse XML node
const char kBucketReplicationRoot[] = "ReplicationConfiguration";
const char kBucketReplicationRule[] = "Rule";
const char kBucketReplicationID[] = "ID";
const char kBucketReplicationRole[] = "Role";
const char kBucketReplicationPrefix[] = "Prefix";
const char kBucketReplicationStatus[] = "Status";
const char kBucketReplicationDestination[] = "Destination";
const char kBucketReplicationBucket[] = "Bucket";
const char kBucketReplicationStorageClass[] = "StorageClass";

// InitMultiUploadResp XML node
const char kInitiateMultipartUploadRoot[] = "InitiateMultipartUploadResult";
const char kInitiateMultipartUploadBucket[] = "Bucket";
const char kInitiateMultipartUploadKey[] = "Key";
const char kInitiateMultipartUploadId[] = "UploadId";

// CompleteMultiUploadResp XML node
const char kCompleteMultiUploadRoot[] = "CompleteMultipartUploadResult";
const char kCompleteMultiUploadLocation[] = "Location";
const char kCompleteMultiUploadBucket[] = "Bucket";
const char kCompleteMultiUploadKey[] = "Key";
const char kCompleteMultiUploadETag[] = "ETag";

// StorageClass
const char kStorageClassStandard[] = "STANDARD";
const char kStorageClassStandardIA[] = "STANDARD_IA";
const char kStorageClassMAZStandard[] = "MAZ_STANDARD";
const char kStorageClassMAZStandardIA[] = "MAZ_STANDARD_IA";
const char kStorageClassIntelligentTiering[] = "INTELLIGENT_TIERING";
const char kStorageClassArchive[] = "ARCHIVE";
const char kStorageClassDeepArchive[] = "DEEP_ARCHIVE";

// Resumable download
const char kResumableDownloadTaskFileSuffix[] = ".cosresumabletask";
const char kResumableDownloadFileName[] = "fileName";
const char kResumableDownloadTaskLastModified[] = "lastModified";
const char kResumableDownloadTaskContentLength[] = "contentLength";
const char kResumableDownloadTaskEtag[] = "eTag";
const char kResumableDownloadTaskCrc64ecma[] = "crc64ecma";
const char kResumableDownloadResumeOffset[] = "resumeOffset";

// 预设ACL
const char kAclDefault[] = "default";
const char kAclPrivate[] = "private";
const char kAclPublicRead[] = "public-read";
const char kAclPublicReadWrite[] = "public-read-write";
const char kAclAuthenticatedRead[] = "authenticated-read";
const char kAclBucketOwnerRead[] = "bucket-owner-read";

// object type
const char kObjectTypeAppendable[] = "appendable";
const char kObjectTypeNormal[] = "normal";
} // namespace qcloud_cos
#endif  // COS_CPP_SDK_V5_INCLUDE_COS_PARAMS_H_
