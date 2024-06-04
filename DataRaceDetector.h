#ifndef DATARACEDETECTOR_HPP
#define DATARACEDETECTOR_HPP

#include <set>
#include <vector>
#include <pthread.h>
#include "Thread.h"
#include "Lock.h"
#include "SharedVariable.h"
#include "Accesstype.h"

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
        // Tracking data
    int numAccesses;
    int numLockAcquisitions;
    int numLockReleases;
    int numDataRaces;

private:
    pthread_barrier_t barrier;
    int barrierCount;
    bool dataRaceDetected;

    



    std::set<Thread *> threads;
    std::vector<SharedVariable *> sharedVariables;
    std::vector<pthread_mutex_t *> mutexes;
    std::set<Lock *> intersect(const std::set<Lock *> &set1, const std::set<Lock *> &set2);
    bool hasCommonLocks(const Thread *t1, const Thread *t2);
};

#endif // DATARACEDETECTOR_HPP