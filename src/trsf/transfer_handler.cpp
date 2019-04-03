#include "trsf/transfer_handler.h"

#include "Poco/Buffer.h"
#include "boost/thread/lock_guard.hpp"

namespace qcloud_cos {
    PartState::PartState() :
        m_part_num(0),
        m_etag(""),
        m_size_inbytes(0),
        m_lastpart(false){}
    
    PartState::PartState(int part_num, std::string& etag, size_t size, bool last_part) :
        m_part_num(part_num),
        m_etag(etag),
        m_size_inbytes(size),
        m_lastpart(last_part){}


    TransferHandler::TransferHandler(const std::string& bucket_name, const std::string& object_name,
                                    uint64_t total_size, const std::string& file_path) :
        m_bucket_name(bucket_name),
        m_object_name(object_name),
        m_local_file_path(file_path),
        m_total_size(total_size),
        m_current_progress(0),
        m_status(TransferStatus::NOT_START),
        m_uploadid(""),
        m_cancel(false) {}

    void TransferHandler::UpdateProgress(uint64_t update_prog) {
        boost::lock_guard<boost::mutex> locker(m_lock_prog);

        m_current_progress += update_prog;

        // Notice the progress there can not backwards, but the each parts has retry counts,
        // should limit the progress no bigger than the total size.
        // s3 has two invariants:(1) Never lock; (2) Never go backwards. Complete me.
        if (m_current_progress > m_total_size) {
            m_current_progress = m_total_size;
        }

    }

    uint64_t TransferHandler::GetProgress() const {
        boost::lock_guard<boost::mutex> locker(m_lock_prog);
        return m_current_progress;
    }

    bool TransferHandler::IsFinishStatus(TransferStatus status) const {
        switch (status) {
            case TransferStatus::ABORTED:
            case TransferStatus::COMPLETED:
            case TransferStatus::FAILED:
            case TransferStatus::CANCELED:
                return true;
            default:
                return false;
        }
    }

    bool TransferHandler::IsAllowTransition(TransferStatus org, TransferStatus dst) const {
        if (org == dst) {
            return true;
        }

        if (IsFinishStatus(org) && IsFinishStatus(dst)) {
            return org == TransferStatus::CANCELED && dst == TransferStatus::ABORTED;
        }

        return true;
    }

    void TransferHandler::UpdateStatus(TransferStatus status) {
        // There can unlock the condition to release the waitUntilFinish
        // But need use the boost *-*
        boost::unique_lock<boost::mutex> locker(m_lock_stat);
        if (IsAllowTransition(m_status, status)) {
            m_status = status;

            if (IsFinishStatus(status)) {
                if (status == TransferStatus::COMPLETED) {
                    // Some other logic
                }
                locker.unlock();
                m_cond.notify_all();
                
            }
        }

    }

    TransferStatus TransferHandler::GetStatus() const {
        boost::lock_guard<boost::mutex> locker(m_lock_stat);
        return m_status;
    }

    void TransferHandler::Cancel() {
        boost::lock_guard<boost::mutex> locker(m_lock_stat);
        m_cancel = true;
    }

    bool TransferHandler::ShouldContinue() const {
        boost::lock_guard<boost::mutex> locker(m_lock_stat);
        return !m_cancel;
    }


    void TransferHandler::WaitUntilFinish() {
        boost::unique_lock<boost::mutex> locker(m_lock_stat);
        while (!IsFinishStatus(m_status)) {
            m_cond.wait(locker);
        }
    }
    

    std::streamsize HandleStreamCopier::handleCopyStream(std::istream& istr, std::ostream& ostr, Poco::SharedPtr<TransferHandler>& handler,
                                                         std::size_t bufferSize) {
        poco_assert (bufferSize > 0);
        
        Poco::Buffer<char> buffer(bufferSize);
        std::streamsize len = 0;
        istr.read(buffer.begin(), bufferSize);
        std::streamsize n = istr.gcount();
        while (n > 0) {
            // Throw the AssertionViolationException if the conditon is not true
            poco_assert (handler->ShouldContinue());

            len += n;
            ostr.write(buffer.begin(), n);
            // update progress
            handler->UpdateProgress(n);
            if (istr && ostr) {
                istr.read(buffer.begin(), bufferSize);
                n = istr.gcount();
            } else {
                n = 0;
            }
        }
        return len;
    }
}
