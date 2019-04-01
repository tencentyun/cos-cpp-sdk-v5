#ifndef SIMPLE_MUTEX_H
#define SIMPLE_MUTEX_H
#include <pthread.h>

class SimpleMutex {
public:
    SimpleMutex() {
        pthread_mutex_init(&m_mutex, NULL);
    }

    ~SimpleMutex() {
        pthread_mutex_destroy(&m_mutex);
    }

    void Lock() {
        pthread_mutex_lock(&m_mutex);
    }

    void Unlock() {
        pthread_mutex_unlock(&m_mutex);
    }

private:
    pthread_mutex_t m_mutex;
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


// scoped rwlock for cos_config
class SimpleRWLock {
public:
    SimpleRWLock() {
        pthread_rwlock_init(&m_lock,NULL);
    }

    ~SimpleRWLock() {
        pthread_rwlock_destroy(&m_lock);
    }

    void WriteLock() {
        pthread_rwlock_wrlock(&m_lock);
    }

    void ReadLock() {
        pthread_rwlock_rdlock(&m_lock);
    }

    void Unlock() {
        pthread_rwlock_unlock(&m_lock);
    }

private:
    pthread_rwlock_t m_lock;
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
