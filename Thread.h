#ifndef THREAD_H
#define THREAD_H

#include <set>

// Forward declaration of the Lock class
class Lock;

class Thread {
public:
    Thread() : id(0) {}  // Default constructor
    Thread(int id) : id(id) {}

    int getId() const;
    const std::set<Lock*>& getLockset() const;
    const std::set<Lock*>& getWriteLockset() const;

    void acquireLock(Lock* lock, bool writeMode);
    void releaseLock(Lock* lock);

private:
    int id;
    std::set<Lock*> locksHeld;     
    std::set<Lock*> writeLocksHeld; 
};

#endif // THREAD_H
