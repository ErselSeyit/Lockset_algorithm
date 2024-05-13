// DataRaceDetector.h
#ifndef DATA_RACE_DETECTOR_H
#define DATA_RACE_DETECTOR_H

#include "Thread.h"
#include "Lock.h"
#include "SharedVariable.h"
#include <set>

class DataRaceDetector {
public:
    void onLockAcquire(Thread* t, Lock* l);
    void onLockRelease(Thread* t, Lock* l);
    void onSharedVariableAccess(Thread* t, SharedVariable* v, AccessType type);
    void reportDataRace(Thread* t, SharedVariable* v);
    std::set<Lock*> intersect(const std::set<Lock*>& set1, const std::set<Lock*>& set2);
};

#endif