/**
 * @file Lock.cpp
 * @brief Implementation of the Lock class
 */

#include "../include/Lock.h"
#include "../include/Thread.h"
#include <iostream>

Lock::Lock(int id) : id(id), is_locked(false), holding_thread(nullptr), shared_variable(nullptr) {}

void Lock::acquire(Thread *t, bool writeMode, SharedVariable *v)
{
    if (!t)
    {
        std::cerr << "Error: Null thread pointer passed to Lock::acquire" << std::endl;
        return;
    }
    
    is_locked = true;
    holding_thread = t;
    shared_variable = v;
    t->acquireLock(this, writeMode);
}

void Lock::release(Thread *t)
{
    if (!t)
    {
        std::cerr << "Error: Null thread pointer passed to Lock::release" << std::endl;
        return;
    }
    
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
