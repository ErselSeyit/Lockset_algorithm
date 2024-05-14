#include "DataRaceDetector.h"
#include <algorithm>
#include <iostream>
#include "Accesstype.h"
#include "Lock.h"
#include "SharedVariable.h"
#include "Thread.h"

DataRaceDetector::DataRaceDetector() : dataRaceDetected(false), barrierCount(0)
{
    pthread_mutex_init(&detectorMutex, nullptr);
}

DataRaceDetector::~DataRaceDetector()
{
    pthread_mutex_destroy(&detectorMutex);
}

void DataRaceDetector::registerThread(Thread *t)
{
    pthread_mutex_lock(&detectorMutex);
    threads.insert(t);
    pthread_mutex_unlock(&detectorMutex);
}

void DataRaceDetector::unregisterThread(Thread *t)
{
    pthread_mutex_lock(&detectorMutex);
    threads.erase(t);
    pthread_mutex_unlock(&detectorMutex);
}

void DataRaceDetector::registerSharedVariable(SharedVariable *v)
{
    pthread_mutex_lock(&detectorMutex);
    sharedVariables.push_back(v);
    pthread_mutex_unlock(&detectorMutex);
}

void DataRaceDetector::locksetMainStart()
{
    pthread_mutex_lock(&detectorMutex);
    dataRaceDetected = false;
    sharedVariables.clear();
    threads.clear();
    mutexes.clear();
    pthread_mutex_unlock(&detectorMutex);
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
}

void DataRaceDetector::locksetThreadStart()
{
    pthread_mutex_lock(&detectorMutex);
    Thread *t = new Thread(pthread_self());
    threads.insert(t);
    pthread_mutex_unlock(&detectorMutex);
}

void DataRaceDetector::locksetThreadEnd()
{
    pthread_mutex_lock(&detectorMutex);
    Thread *t = nullptr;
    for (auto &thread : threads)
    {
        if (thread->getId() == pthread_self())
        {
            t = thread;
            break;
        }
    }
    if (t)
    {
        threads.erase(t);
        delete t;
    }
    pthread_mutex_unlock(&detectorMutex);
}

void DataRaceDetector::onLockAcquire(Thread *t, Lock *l, bool writeMode, SharedVariable *v)
{
    pthread_mutex_lock(&detectorMutex);
    l->acquire(t, writeMode, v);
    t->acquireLock(l, writeMode);
    pthread_mutex_unlock(&detectorMutex);
}

void DataRaceDetector::onLockRelease(Thread *t, Lock *l, SharedVariable *v)
{
    pthread_mutex_lock(&detectorMutex);
    l->release(t);
    t->releaseLock(l);
    v->releaseThread(t);
    pthread_mutex_unlock(&detectorMutex);
}
void DataRaceDetector::onSharedVariableAccess(Thread *t, SharedVariable *v, AccessType type)
{
    pthread_mutex_lock(&detectorMutex);

    std::cout << "Thread " << t->getId() << " is trying to access variable " << v->getName() << " with " << (type == AccessType::READ ? "READ" : "WRITE") << " access." << std::endl;

    std::cout << "Variable " << v->getName() << " is currently in state " << v->stateToString(v->getState()) << std::endl;

    if (v->isAccessed() && v->getAccessingThread() != t)
    {
        Thread *accessingThread = v->getAccessingThread();
        bool commonLocks = hasCommonLocks(accessingThread, t);

        std::cout << "Thread " << accessingThread->getId() << " is currently accessing variable " << v->getName() << " with " << (v->getState() == State::Exclusive ? "WRITE" : "READ") << " access." << std::endl;

        if (type == AccessType::WRITE)
        {
            if (v->getState() == State::Exclusive || v->getState() == State::Initializing || v->getState() == State::Shared || v->getState() == State::SharedModified)
            {
                if (!commonLocks)
                {
                    std::cout << "Thread " << t->getId() << " cannot access variable " << v->getName() << " with WRITE access because it is already being accessed by thread " << accessingThread->getId() << std::endl;
                    dataRaceDetected = true;
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
                    std::cout << "Thread " << t->getId() << " cannot access variable " << v->getName() << " with READ access because it is already being accessed by thread " << accessingThread->getId() << " with WRITE access." << std::endl;
                    dataRaceDetected = true;
                    reportDataRace(t, v);
                }
            }
        }
    }

    // Call the access method to handle the state transition
    v->access(t, type);

    std::cout << "Thread " << t->getId() << " accessed variable " << v->getName() << ", now in state " << v->stateToString(v->getState()) << std::endl;

    pthread_mutex_unlock(&detectorMutex);
}

void DataRaceDetector::reportDataRace(Thread *t, SharedVariable *v)
{
    std::cout << "Data race detected between thread " << t->getId() << " and thread " << v->getAccessingThread()->getId() << " on shared variable " << v->getName() << std::endl;
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

    return false;
}

void DataRaceDetector::initializeBarrier(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, int count)
{
    pthread_barrier_init(barrier, attr, count);
    this->barrier = *barrier;
    barrierCount = count;
}

void DataRaceDetector::barrierWait()
{
    pthread_barrier_wait(&barrier);
    pthread_mutex_lock(&detectorMutex);
    for (auto &var : sharedVariables)
    {
        var->setState(State::Clean);
    }
    pthread_mutex_unlock(&detectorMutex);
}