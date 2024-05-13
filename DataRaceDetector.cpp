// DataRaceDetector.cpp
#include "DataRaceDetector.h"
#include <algorithm>
#include <iostream>
#include "Accesstype.h"

void DataRaceDetector::onLockAcquire(Thread* t, Lock* l) {
    // Acquire the lock
    l->acquire(t);
}

void DataRaceDetector::onLockRelease(Thread* t, Lock* l) {
    // Release the lock
    l->release(t);
}

void DataRaceDetector::onSharedVariableAccess(Thread* t, SharedVariable* v, AccessType type) {
    // Check if the variable is already accessed by another thread
    if (v->isAccessed()) {
        // Get the thread that currently accesses the variable
        Thread* accessingThread = v->getAccessingThread();
        // Check if there is a potential data race between the current thread and the accessing thread
        if (accessingThread != t) {
            // Report the data race
            reportDataRace(t, v);
        }
    }

    // Access the variable
    v->access(t, type);
}

void DataRaceDetector::reportDataRace(Thread* t, SharedVariable* v) {
    // method implementation
    std::cout << "Data race detected between thread " << t->getId() << " and thread " << v->getAccessingThread()->getId() << " on shared variable " << v->getName() << std::endl;
}

std::set<Lock*> DataRaceDetector::intersect(const std::set<Lock*>& set1, const std::set<Lock*>& set2) {
    std::set<Lock*> result;
    // Compute the intersection of set1 and set2 and store it in result
    std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(result, result.begin()));
    return result;
}