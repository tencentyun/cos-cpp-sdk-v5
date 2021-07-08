#ifndef __TRSF_HANDLER_H__
#define __TRSF_HANDLER_H__

#include <map>
#include <istream>
#include <ostream>
#include <memory>
#include <mutex> 
#include <condition_variable>
#include "op/cos_result.h"

namespace qcloud_cos {

/// @brief 进度回调函数
typedef void (*TransferProgressCallback)(uint64_t transferred_size, uint64_t total_size, void *user_data);

/// @brief 状态回调函数
typedef void (*TransferStatusCallback)(const std::string& status, void *user_data);

class PartState {
public:
    PartState();
    PartState(int part_num, std::string& etag, size_t size, bool last_part = false);

    void SetPartNum(int number) { m_part_num = number; }
    int GetPartNum() const { return m_part_num; }

    void SetEtag(const std::string& etag) { m_etag = etag; }
    std::string GetEtag() const { return m_etag; }

    void SetSize(size_t size) { m_size_inbytes = size; }
    size_t GetSize() const { return m_size_inbytes; }

    void SetLastPart(bool lastpart) { m_lastpart = lastpart; }
    bool IsLastPart() { return m_lastpart; }

private:
    int m_part_num;
    // current use the md5
    std::string m_etag;

    size_t m_size_inbytes;

    // TODO for now just care about the whole progress
    /* size_t m_current_progress_inbytes; */
    /* size_t m_range_begin; */

    bool m_lastpart;
};

typedef std::shared_ptr<PartState> PartPointer;
// Key is partnumber
typedef std::map<int, PartPointer> PartStateMap;

enum class TransferStatus
{
    NOT_START,
    //Operation is now running
    IN_PROGRESS,
    //Operation was canceled.
    CANCELED,
    //Operation failed
    FAILED,
    //Operation was successful
    COMPLETED,
    //Operation either failed or was canceled and a user deleted the multi-part upload .
    ABORTED
};

// For now support the multiupload
class TransferHandler {
public:
    // Upload
    TransferHandler(const std::string& bucket_name, const std::string& object_name,
                    uint64_t total_size, const std::string& file_path="");

    void SetBucketName(const std::string& bucket_name) { m_bucket_name = bucket_name; }
    std::string GetBucketName() const { return m_bucket_name; }

    void SetObjectName(const std::string& object_name) { m_object_name = object_name; }
    std::string GetObjectName() const { return m_object_name; }

    void SetLocalFilePath(const std::string& local_file_path) { m_local_file_path = local_file_path; }
    std::string GetLocalFilePath() const { return m_local_file_path; }

    void SetTotalSize(uint64_t total_size) { m_total_size = total_size; }
    uint64_t GetTotalSize() const { return m_total_size; }

    // Notice there can not backwards
    void UpdateProgress(uint64_t update_prog);
    // Get the current upload size(B).
    uint64_t GetProgress() const;

    void UpdateStatus(const TransferStatus& status);

    void UpdateStatus(const TransferStatus& status, const CosResult& result);
    // Get the current status of process, detail see the enum TransferStatus.
    TransferStatus GetStatus() const;

    std::string GetStatusString() const;

    void SetUploadID(const std::string& uploadid) { m_uploadid = uploadid; }
    // Get the init or resumed uploadid.
    std::string GetUploadID() const { return m_uploadid; }

    // Cancel the process of interface the uploadid can reuse.
    void Cancel();

    bool ShouldContinue() const;

    bool IsFinishStatus(TransferStatus status) const;

    bool IsAllowTransition(TransferStatus org, TransferStatus dst) const;

    // Block until finish.
    void WaitUntilFinish();

    /// @brief  设置进度回调函数
    void SetTransferProgressCallback(TransferProgressCallback callback) {
        m_progress_cb = callback;
    }
    /// @brief  设置状态回调函数
    void SetTransferStatusCallback(TransferStatusCallback callback) {
        m_status_cb = callback;
    }
    /// @brief 设置回调私有数据
    void SetTransferCallbackUserData(void *user_data) {
        m_user_data = user_data;
    }

public:
    // Origin result
    CosResult m_result;

private:
    std::string m_bucket_name;
    std::string m_object_name;
    std::string m_local_file_path;
    uint64_t m_total_size;
    // The m_current_progress best to use the atomic. but can not support c11 for now, so use the mutex.
    uint64_t m_current_progress;
    TransferStatus m_status;
    std::string m_uploadid;
    // Is cancel
    bool m_cancel;

    PartStateMap m_part_map;

    // Mutex lock for the progress
    mutable std::mutex m_lock_prog;
    // Mutex lock for the status
    mutable std::mutex m_lock_stat;
    // Condition
    mutable std::condition_variable m_cond;

    // callback function
    TransferProgressCallback m_progress_cb;
    TransferStatusCallback m_status_cb;
    void *m_user_data;

    // Mutex lock for the part map
    // mutable boost::mutex m_lock_parts;
};

/// @异步回调Hanlder
typedef std::shared_ptr<TransferHandler> SharedTransferHandler;

class HandleStreamCopier {
public:
    static std::streamsize handleCopyStream(const SharedTransferHandler& handler, std::istream& istr, std::ostream& ostr,
                                            std::size_t bufferSize = 8192);
};

}

#endif
