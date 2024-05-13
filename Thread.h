// Thread.h
#ifndef THREAD_H
#define THREAD_H
#include "Lock.h"
#include <set>
#include <string> 

class Lock;

class Thread {
public:
    Thread(int id);
    int getId() const { return id; }
    void acquireLock(Lock* lock);
    void releaseLock(Lock* lock);
    bool holdsLock(Lock* lock) const;
    std::set<Lock*> getLockset() const;

private:
    int id;
    std::set<Lock*> lockset;
};

#endif