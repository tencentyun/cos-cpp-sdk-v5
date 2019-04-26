#ifndef SIMPLE_MUTEX_H
#define SIMPLE_MUTEX_H

#if defined(_WIN32)
#include <winsock2.h>
#include <synchapi.h>
#else
#include <pthread.h>
#endif

/**
 * Notice SimpleMutext and SimpleRWLock are not used in current project, 
 * now use the boost mutex and shared_mutex instead. 
 * when want to reuse this header need notice the windows header included order.
 */
class SimpleMutex {
public:
    SimpleMutex() {
#if defined(_WIN32)
		m_mutex = CreateMutexA(NULL, FALSE, NULL);
#else
		pthread_mutex_init(&m_mutex, NULL);
#endif
    }

    ~SimpleMutex() {
#if defined(_WIN32)
		CloseHandle(m_mutex);
#else
		pthread_mutex_destroy(&m_mutex);
#endif
    }

    void Lock() {
#if defined(_WIN32)
		DWORD d = WaitForSingleObject(m_mutex, INFINITE);
#else
		pthread_mutex_lock(&m_mutex);
#endif    }

    void Unlock() {
#if defined(_WIN32)
		ReleaseMutex(m_mutex);
#else
		pthread_mutex_unlock(&m_mutex);
#endif
    }

private:
#if defined(_WIN32)
	HANDLE m_mutex;
#else
	pthread_mutex_t m_mutex;
#endif
};

// mutex holder
class SimpleMutexLocker {
public:
    SimpleMutexLocker(SimpleMutex& mutex) : m_mutex(&mutex) {
        m_mutex->Lock();
    }

    ~SimpleMutexLocker() {
        m_mutex->Unlock();
    }

private:
    SimpleMutex* m_mutex;
};


// Scoped rwlock for cos_config
// Window's rwlock some version can not support, so there now use the mutex lock, 
// then think about changing into the boost::shared_lock and boost::shared_mutex.
class SimpleRWLock {
public:
    SimpleRWLock() {
#if defined(_WIN32)
		m_lock = CreateMutexA(NULL, FALSE, NULL);
#else
		pthread_rwlock_init(&m_lock, NULL);
#endif
    }

    ~SimpleRWLock() {
#if defined(_WIN32)
		CloseHandle(m_lock);
#else
		pthread_rwlock_destroy(&m_lock);
#endif
    }

    void WriteLock() {
#if defined(_WIN32)
		DWORD d = WaitForSingleObject(m_lock, INFINITE);
#else
		pthread_rwlock_wrlock(&m_lock);
#endif
        
    }

    void ReadLock() {
#if defined(_WIN32)
		DWORD d = WaitForSingleObject(m_lock, INFINITE);
#else
		pthread_rwlock_rdlock(&m_lock);
#endif
        
    }

    void Unlock() {
#if defined(_WIN32)
		ReleaseMutex(m_lock);
#else
		pthread_rwlock_unlock(&m_lock);
#endif
    }

private:
#if defined(_WIN32)
	HANDLE m_lock;
#else
	pthread_rwlock_t m_lock;
#endif
   
};

class SimpleWLocker {
public:
    SimpleWLocker(SimpleRWLock& lock) : m_lock(&lock) {
        m_lock->WriteLock();
    }
    ~SimpleWLocker() {
        m_lock->Unlock();
    }

private:
    SimpleRWLock* m_lock;
};

class SimpleRLocker {
public:
    SimpleRLocker(SimpleRWLock& lock) : m_lock(&lock) {
        m_lock->ReadLock();
    }
    ~SimpleRLocker() {
        m_lock->Unlock();
    }

private:
    SimpleRWLock* m_lock;
};

#endif
