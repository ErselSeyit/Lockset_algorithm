// DataRaceDetector.cpp
#include "DataRaceDetector.h"
#include <algorithm>
#include <iostream>
#include "Accesstype.h"
void DataRaceDetector::onLockAcquire(Thread* t, Lock* l) {
    // Acquire the lock
    l->acquire(t);

    t->acquireLock(l);
}

void DataRaceDetector::onLockRelease(Thread* t, Lock* l) {
    // Release the lock
    l->release(t);
}
void DataRaceDetector::onSharedVariableAccess(Thread* t, SharedVariable* v, AccessType type) {
    std::cout << "Thread " << t->getId() << " is trying to access variable " << v->getName() << std::endl;

    if (v->isAccessed()) {
        Thread* accessingThread = v->getAccessingThread();
        if (accessingThread != t) {
            std::set<Lock*> locksHeldByCurrentThread = t->getLockset();
            std::set<Lock*> locksHeldByAccessingThread = accessingThread->getLockset();
            std::set<Lock*> commonLocks = intersect(locksHeldByCurrentThread, locksHeldByAccessingThread);

            if (commonLocks.empty()) {
                std::cout << "No common locks held by threads " << t->getId() << " and " << accessingThread->getId() << std::endl;
                reportDataRace(t, v);
            } else {
                std::cout << "Common lock(s) found between threads " << t->getId() << " and " << accessingThread->getId() << std::endl;
            }
        }
    }

    v->access(t, type);
    std::cout << "Thread " << t->getId() << " accessed variable " << v->getName() << std::endl;
}

void DataRaceDetector::reportDataRace(Thread* t, SharedVariable* v) {
    std::cout << "Data race detected between thread " << t->getId() << " and thread " << v->getAccessingThread()->getId() << " on shared variable " << v->getName() << std::endl;
}

std::set<Lock*> DataRaceDetector::intersect(const std::set<Lock*>& set1, const std::set<Lock*>& set2) {
    std::set<Lock*> result;
    for (Lock* lock : set1) {
        if (set2.find(lock) != set2.end()) {
            result.insert(lock);
        }
    }
    return result;
}