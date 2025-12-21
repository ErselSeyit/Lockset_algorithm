/**
 * @file Thread.cpp
 * @brief Implementation of the Thread class
 */

#include "../include/Thread.h"
#include "../include/Lock.h"
#include <iostream>

// Method implementations
int Thread::getId() const {
    return id;
}

const std::set<Lock*>& Thread::getLockset() const {
    return locksHeld;
}

const std::set<Lock*>& Thread::getWriteLockset() const {
    return writeLocksHeld;
}

void Thread::acquireLock(Lock* lock, bool writeMode) {
    if (!lock)
    {
        std::cerr << "Error: Null lock pointer passed to Thread::acquireLock" << std::endl;
        return;
    }
    
    locksHeld.insert(lock);
    if (writeMode) {
        writeLocksHeld.insert(lock);
    }

    // Optional logging of the event
    std::cout << "Thread " << this->getId() << " acquired lock " << lock->getId();
    if (writeMode) {
        std::cout << " (Write Mode)" << std::endl;
    } else {
        std::cout << " (Read Mode)" << std::endl;
    }
}

void Thread::releaseLock(Lock* lock) {
    if (!lock)
    {
        std::cerr << "Error: Null lock pointer passed to Thread::releaseLock" << std::endl;
        return;
    }
    
    locksHeld.erase(lock);
    writeLocksHeld.erase(lock);

    // Optional logging of the event
    std::cout << "Thread " << this->getId() << " released lock " << lock->getId() << std::endl;
}
