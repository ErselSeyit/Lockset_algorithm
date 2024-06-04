#include "Lock.h"
#include "Thread.h"
#include <iostream>

Lock::Lock(int id) : id(id), is_locked(false), holding_thread(nullptr), shared_variable(nullptr) {}

void Lock::acquire(Thread *t, bool writeMode, SharedVariable *v)
{
    is_locked = true;
    holding_thread = t;
    shared_variable = v;
    t->acquireLock(this, writeMode);
}

void Lock::release(Thread *t)
{
    is_locked = false;
    holding_thread = nullptr;
    shared_variable = nullptr;
    t->releaseLock(this);
}

bool Lock::isLocked() const
{
    return is_locked;
}

Thread *Lock::getHoldingThread() const
{
    return holding_thread;
}

SharedVariable *Lock::getSharedVariable() const
{
    return shared_variable;
}

void Lock::setSharedVariable(SharedVariable *v)
{
    shared_variable = v;
}

int Lock::getId() const
{
    return id;
}
