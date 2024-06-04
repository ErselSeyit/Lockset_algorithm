#include "DataRaceDetector.h"
#include <iostream>
#include <algorithm>

DataRaceDetector::DataRaceDetector() : dataRaceDetected(false), barrierCount(0), numAccesses(0), numLockAcquisitions(0), numLockReleases(0), numDataRaces(0) {}

DataRaceDetector::~DataRaceDetector() {}

void DataRaceDetector::registerThread(Thread *t)
{
    threads.insert(t);
    std::cout << "Thread " << t->getId() << " registered." << std::endl;
}

void DataRaceDetector::unregisterThread(Thread *t)
{
    threads.erase(t);
    std::cout << "Thread " << t->getId() << " unregistered." << std::endl;
}

void DataRaceDetector::registerSharedVariable(SharedVariable *v)
{
    sharedVariables.push_back(v);
    std::cout << "Shared variable " << v->getName() << " registered." << "With state  " << v->stateToString(v->getState()) << std::endl;
}

void DataRaceDetector::locksetMainStart()
{
    dataRaceDetected = false;
    sharedVariables.clear();
    threads.clear();
    mutexes.clear();
    numAccesses = 0;
    numLockAcquisitions = 0;
    numLockReleases = 0;
    numDataRaces = 0;
    std::cout << "Data race detector initialized." << std::endl;
}

void DataRaceDetector::locksetMainEnd()
{
    if (dataRaceDetected)
    {
        std::cout << "Warning: data race detected!" << std::endl;
    }
    else
    {
        std::cout << "Data race not detected!" << std::endl;
    }
    std::cout << "Data race detector finished." << std::endl;
}

void DataRaceDetector::onLockAcquire(Thread *t, Lock *l, bool writeMode, SharedVariable *v)
{
    
    l->acquire(t, writeMode, v);
    t->acquireLock(l, writeMode);
    numLockAcquisitions++;

    std::cout << "Thread " << t->getId() << " acquired lock " << l->getId()
              << " with " << (writeMode ? "WRITE" : "READ") << " access on variable " << v->getName() << std::endl;
}

void DataRaceDetector::onLockRelease(Thread *t, Lock *l, SharedVariable *v)
{
    // 1. Ownership Verification
    if (l->getHoldingThread() != t) { 
        std::cerr << "Error: Thread " << t->getId() << " tried to release lock " << l->getId() << " which it doesn't own." << std::endl;
        return; 
    }

    // 2. Update Lock and Thread State 
    l->release(t);
    t->releaseLock(l);

    // 3. Transition the Shared Variable
    if (v->getState() == State::Exclusive) {
        v->setState(State::Virgin); // Exclusive access complete, reset to initial state
    } else if (v->getState() == State::SharedModified) {
        v->setState(State::Shared); // Last writer released, transition to Shared
    }
    
    // 4. Release the variable
    v->releaseThread(t); 
    
    // 5. Update Statistics (optional)
    numLockReleases++;

    // 6. Logging (optional)
    std::cout << "Thread " << t->getId() << " released lock " << l->getId()
              << " on variable " << v->getName() << std::endl;
}


void DataRaceDetector::onSharedVariableAccess(Thread *t, SharedVariable *v, AccessType type)
{
    numAccesses++;

    std::cout << "Thread " << t->getId() << " is trying to access variable " << v->getName()
              << " with " << (type == AccessType::READ ? "READ" : "WRITE") << " access." << std::endl;

    std::cout << "Variable " << v->getName() << " is currently in state " << v->stateToString(v->getState()) << std::endl;

    if (v->isAccessed() && v->getAccessingThread() != t)
    {
        Thread *accessingThread = v->getAccessingThread();
        bool commonLocks = hasCommonLocks(accessingThread, t);

        std::cout << "Thread " << accessingThread->getId() << " is currently accessing variable " << v->getName()
                  << " with " << (v->getState() == State::Exclusive ? "WRITE" : "READ") << " access." << std::endl;

        if (type == AccessType::WRITE)
        {
            if (v->getState() == State::Exclusive || v->getState() == State::Initializing ||
                v->getState() == State::Shared || v->getState() == State::SharedModified)
            {
                if (!commonLocks)
                {
                    std::cout << "Thread " << t->getId() << " cannot access variable " << v->getName()
                              << " with WRITE access because it is already being accessed by thread " << accessingThread->getId() << std::endl;
                    dataRaceDetected = true;
                    numDataRaces++;
                    reportDataRace(t, v);
                }
            }
        }
        else if (type == AccessType::READ)
        {
            if (v->getState() == State::Exclusive || v->getState() == State::Initializing)
            {
                if (!commonLocks)
                {
                    std::cout << "Thread " << t->getId() << " cannot access variable " << v->getName()
                              << " with READ access because it is already being accessed by thread " << accessingThread->getId()
                              << " with WRITE access." << std::endl;
                    dataRaceDetected = true;
                    numDataRaces++;
                    reportDataRace(t, v);
                }
            }
        }
    }

    v->access(t, type);

    std::cout << "Thread " << t->getId() << " accessed variable " << v->getName()
              << ", now in state " << v->stateToString(v->getState()) << std::endl;
}

void DataRaceDetector::reportDataRace(Thread *t, SharedVariable *v)
{
    std::cout << "Data race detected between thread " << t->getId()
              << " and thread " << v->getAccessingThread()->getId() << " on shared variable " << v->getName() << std::endl;
}

std::set<Lock *> DataRaceDetector::intersect(const std::set<Lock *> &set1, const std::set<Lock *> &set2)
{
    std::set<Lock *> result;
    std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(result, result.begin()));
    return result;
}

bool DataRaceDetector::hasCommonLocks(const Thread *t1, const Thread *t2)
{
    std::set<Lock *> commonLocks = intersect(t1->getLockset(), t2->getLockset());
    if (!commonLocks.empty())
    {
        return true;
    }

    commonLocks = intersect(t1->getWriteLockset(), t2->getLockset());
    if (!commonLocks.empty())
    {
        return true;
    }

    commonLocks = intersect(t1->getLockset(), t2->getWriteLockset());
    if (!commonLocks.empty())
    {
        return true;
    }

    commonLocks = intersect(t1->getWriteLockset(), t2->getWriteLockset());
    if (!commonLocks.empty())
    {
        return true;
    }

    return false;
}

void DataRaceDetector::initializeBarrier(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, int count)
{
    pthread_barrier_init(barrier, attr, count);
    this->barrier = *barrier;
    barrierCount = count;

    std::cout << "Barrier initialized with count " << count << std::endl;
}

void DataRaceDetector::barrierWait()
{
    pthread_barrier_wait(&barrier);

    std::cout << "All threads have reached the barrier" << std::endl;
    for (auto &var : sharedVariables)
    {
        std::cout << "Resetting state of variable " << var->getName() << std::endl;
        var->setState(State::Clean);
    }
}
