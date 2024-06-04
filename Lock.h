#ifndef LOCK_H
#define LOCK_H

#include "Thread.h"
#include "SharedVariable.h"
#include <set>

class Thread;
class SharedVariable;

class Lock
{
public:
    Lock(int id);
    void acquire(Thread *t, bool writeMode, SharedVariable *v);
    void release(Thread *t);
    bool isLocked() const;
    Thread *getHoldingThread() const;
    SharedVariable *getSharedVariable() const;
    void setSharedVariable(SharedVariable *v);
    int getId() const;

private:
    int id;
    bool is_locked;
    Thread *holding_thread;
    SharedVariable *shared_variable;
};

#endif
