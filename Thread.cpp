#include "Thread.h"
#include "Lock.h"

Thread::Thread(int id) : id(id) {}

int Thread::getId() const
{
    return id;
}

const std::set<Lock *> &Thread::getLockset() const
{
    return locksHeld;
}

const std::set<Lock *> &Thread::getWriteLockset() const
{
    return writeLocksHeld;
}

void Thread::acquireLock(Lock *lock, bool writeMode)
{
    locksHeld.insert(lock);
    if (writeMode)
    {
        writeLocksHeld.insert(lock);
    }
}

void Thread::releaseLock(Lock *lock)
{
    locksHeld.erase(lock);
    writeLocksHeld.erase(lock);
}
