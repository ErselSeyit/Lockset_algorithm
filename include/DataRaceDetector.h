/**
 * @file DataRaceDetector.h
 * @brief Header file for the DataRaceDetector class implementing the Eraser lockset algorithm
 * 
 * This class implements a dynamic data race detector for multi-threaded programs
 * using the lockset algorithm. It tracks thread access patterns to shared variables
 * and detects potential data races.
 */

#ifndef DATARACEDETECTOR_H
#define DATARACEDETECTOR_H

#include <set>
#include <vector>
#include <pthread.h>
#include "Thread.h"
#include "Lock.h"
#include "SharedVariable.h"
#include "Accesstype.h"

/**
 * @class DataRaceDetector
 * @brief Implements the Eraser lockset algorithm for data race detection
 * 
 * This detector monitors thread access to shared variables and verifies that
 * all accesses are properly protected by locks. It maintains locksets for each
 * thread and detects when concurrent accesses occur without common locks.
 */
class DataRaceDetector
{
public:
    DataRaceDetector();
    ~DataRaceDetector();
    void onLockAcquire(Thread *t, Lock *l, bool writeMode, SharedVariable *v);
    void onLockRelease(Thread *t, Lock *l, SharedVariable *v);
    void onSharedVariableAccess(Thread *t, SharedVariable *v, AccessType type);
    void registerThread(Thread *t);
    void unregisterThread(Thread *t);
    void registerSharedVariable(SharedVariable *v);
    void initializeBarrier(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, int count);
    void barrierWait();
    void locksetMainStart();
    void locksetMainEnd();
    void locksetThreadStart();
    void locksetThreadEnd();
    void reportDataRace(Thread *t, SharedVariable *v);
    
    // Statistics getters
    int getNumAccesses() const;
    int getNumLockAcquisitions() const;
    int getNumLockReleases() const;
    int getNumDataRaces() const;

private:
    pthread_barrier_t barrier;
    int barrierCount;
    bool dataRaceDetected;
    
    // Tracking data
    int numAccesses;
    int numLockAcquisitions;
    int numLockReleases;
    int numDataRaces;

    



    std::set<Thread *> threads;
    std::vector<SharedVariable *> sharedVariables;
    std::vector<pthread_mutex_t *> mutexes;
    std::set<Lock *> intersect(const std::set<Lock *> &set1, const std::set<Lock *> &set2);
    bool hasCommonLocks(const Thread *t1, const Thread *t2);
};

#endif // DATARACEDETECTOR_H