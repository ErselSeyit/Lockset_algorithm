#include <stdexcept>
#include "Thread.h"
#include "Lock.h"

Thread::Thread(int id) : id(id) {}

void Thread::acquireLock(Lock* lock) {
    // Check if lock is already held by the thread
    if(lockset.find(lock) != lockset.end()) {
        throw std::runtime_error("Lock already held by the thread");
    }
    lockset.insert(lock);
}

void Thread::releaseLock(Lock* lock) {
    // Check if lock is held by the thread
    if(lockset.find(lock) == lockset.end()) {
        throw std::runtime_error("Lock not held by the thread");
    }
    lockset.erase(lock);
}

bool Thread::holdsLock(Lock* lock) const {
    return lockset.find(lock) != lockset.end();
}

std::set<Lock*> Thread::getLockset() const {
    return lockset;
}

int Thread::getId() const {
    return id;
}