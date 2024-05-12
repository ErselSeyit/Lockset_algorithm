#include "SharedVariable.h"

SharedVariable::SharedVariable(int id) : id(id) {}

void SharedVariable::addLock(Lock* lock) {
    lockset.insert(lock);
}

void SharedVariable::removeLock(Lock* lock) {
    lockset.erase(lock);
}

bool SharedVariable::isGuardedBy(Lock* lock) const {
    return lockset.find(lock) != lockset.end();
}

std::set<Lock*> SharedVariable::getLockset() const {
    return lockset;
}

void SharedVariable::setLockset(const std::set<Lock*>& locks) { // Add this method
    lockset = locks;
}

int SharedVariable::getId() const { // Add this method
    return id;
}