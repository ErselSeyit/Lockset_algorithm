#include "Lock.h"

Lock::Lock(int id) : id(id), is_locked(false), holding_thread(nullptr), shared_variable(nullptr) {}

void Lock::acquire(Thread* t) {
    is_locked = true;
    holding_thread = t;

    t->acquireLock(this);
}

void Lock::release(Thread* t) {
    if (holding_thread == t) {
        is_locked = false;
        holding_thread = nullptr;
    }
}

bool Lock::isLocked() const {
    return is_locked;
}

Thread* Lock::getHoldingThread() const {
    return holding_thread;
}

SharedVariable* Lock::getSharedVariable() const {
    return shared_variable;
}

void Lock::setSharedVariable(SharedVariable* v) {
    shared_variable = v;
}