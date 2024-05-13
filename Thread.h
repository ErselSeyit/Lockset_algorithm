#ifndef THREAD_H
#define THREAD_H

#include <set>
#include <string>

class Lock; // Forward declaration

class Thread
{
private:
    int id;
    std::set<Lock *> locksHeld;
    std::set<Lock *> writeLocksHeld;

public:
    Thread(int id);
    int getId() const;
    const std::set<Lock *> &getLockset() const;
    const std::set<Lock *> &getWriteLockset() const;
    void acquireLock(Lock *lock, bool writeMode);
    void releaseLock(Lock *lock);
};

#endif
