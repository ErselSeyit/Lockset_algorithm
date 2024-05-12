#ifndef THREAD_H
#define THREAD_H

#include <set>
#include "Lock.h"

class Thread {
private:
    std::set<Lock*> lockset;
    int id;

public:
    Thread(int id);
    void acquireLock(Lock* lock);
    void releaseLock(Lock* lock);
    bool holdsLock(Lock *lock) const;
    std::set<Lock*> getLockset() const;
    int getId() const;
};

#endif // THREAD_H