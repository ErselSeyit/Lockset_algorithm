#include "DataRaceDetector.h"
#include <algorithm>
#include <iostream>

void DataRaceDetector::onLockAcquire(Thread *t, Lock *l, bool writeMode)
{
    l->acquire(t);
    t->acquireLock(l, writeMode);
}

void DataRaceDetector::onLockRelease(Thread *t, Lock *l)
{
    l->release(t);
    t->releaseLock(l);
}

void DataRaceDetector::onSharedVariableAccess(Thread *t, SharedVariable *v, AccessType type)
{
    std::cout << "Thread " << t->getId() << " is trying to access variable " << v->getName() << " with "
              << (type == AccessType::READ ? "read" : "write") << " access." << " Current state: " << SharedVariable::stateToString(v->getState()) << std::endl;
    
    // Check if the variable is already accessed
    if (v->isAccessed())
    {
        Thread *accessingThread = v->getAccessingThread();
        if (accessingThread != t)
        {
            std::set<Lock *> locksHeldByCurrentThread = t->getLockset();
            std::set<Lock *> locksHeldByAccessingThread = accessingThread->getLockset();
            std::set<Lock *> commonLocks = intersect(locksHeldByCurrentThread, locksHeldByAccessingThread);

            if (commonLocks.empty())
            {
                std::cout << "No common locks held by threads " << t->getId() << " and " << accessingThread->getId() << std::endl;
                reportDataRace(t, v);
            }
            else
            {
                std::cout << "Common lock(s) found between threads " << t->getId() << " and " << accessingThread->getId() << std::endl;
            }
        }
    }

    // Perform state transition
    State currentState = v->getState();
    if (currentState == State::Virgin)
    {
        v->setState(State::Exclusive);
    }
    else if (currentState == State::Exclusive)
    {
        if (v->getAccessingThread() != t)
        {
            if (type == AccessType::READ)
            {
                v->setState(State::Shared);
            }
            else
            {
                v->setState(State::SharedModified);
            }
        }
    }
    else if (currentState == State::Shared)
    {
        if (type == AccessType::WRITE)
        {
            v->setState(State::SharedModified);
        }
    }

    v->access(t, type);
    std::cout << "Thread " << t->getId() << " accessed variable " << v->getName() << ", now in state " << SharedVariable::stateToString(v->getState()) << std::endl;
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
