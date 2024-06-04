#include "Thread.h"
#include "Lock.h"  // Include the Lock class header (if not already in Thread.h)
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
    locksHeld.insert(lock);
    if (writeMode) {
        writeLocksHeld.insert(lock);
    }

    //Optional logging of the event.
    std::cout << "Thread " << this->getId() << " acquired lock " << lock->getId();
    if(writeMode){
        std::cout << " (Write Mode)" << std::endl;
    } else{
        std::cout << " (Read Mode)" << std::endl;
    }
}

void Thread::releaseLock(Lock* lock) {
    locksHeld.erase(lock);
    writeLocksHeld.erase(lock);

    //Optional logging of the event
    std::cout << "Thread " << this->getId() << " released lock " << lock->getId() << std::endl;

}
