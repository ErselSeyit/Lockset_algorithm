// Lock.h
#ifndef LOCK_H
#define LOCK_H

#include "Thread.h"
#include "SharedVariable.h"
#include <set>

class Lock {
public:
    Lock(int id);
    void acquire(Thread* t);
    void release(Thread* t);
    bool isLocked() const;
    Thread* getHoldingThread() const;
    SharedVariable* getSharedVariable() const;
    void setSharedVariable(SharedVariable* v);

private:
    int id;
    bool is_locked;
    Thread* holding_thread;
    SharedVariable* shared_variable;
};

#endif