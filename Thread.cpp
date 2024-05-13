// Thread.cpp
#include "Thread.h"

Thread::Thread(int id) : id(id) {
    lockset = std::set<Lock*>();
}

void Thread::acquireLock(Lock* lock) {
    lockset.insert(lock);
}

void Thread::releaseLock(Lock* lock) {
    lockset.erase(lock);
}

bool Thread::holdsLock(Lock* lock) const {
    return lockset.find(lock) != lockset.end();
}

std::set<Lock*> Thread::getLockset() const {
    return lockset;
}