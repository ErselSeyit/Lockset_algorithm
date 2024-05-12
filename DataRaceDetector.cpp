
#include <iostream>
#include <algorithm>
#include <iterator>
#include <set>
#include "Thread.h"
#include "Lock.h"

enum class AccessType { READ, WRITE };


class Lockset {
private:
    std::set<Lock*> locks;

public:
    void addLock(Lock* lock) {
        locks.insert(lock);
    }

    void removeLock(Lock* lock) {
        locks.erase(lock);
    }

    bool containsLock(Lock* lock) {
        return locks.find(lock) != locks.end();
    }

    std::set<Lock*> getLocks() const {
        return locks;
    }
     void setLocks(const std::set<Lock*>& newLocks) { // Add this method
        locks = newLocks;
    }
};

class SharedVariable {
private:
    Lockset lockset;
    int id;

public:
    SharedVariable(int id) : id(id) {}

    void addLock(Lock* lock) {
        lockset.addLock(lock);
    }

    void removeLock(Lock* lock) {
        lockset.removeLock(lock);
    }

    bool isGuardedBy(Lock* lock) {
        return lockset.containsLock(lock);
    }

    std::set<Lock*> getLockset() const {
        return lockset.getLocks();   
    }

    void setLockset(const std::set<Lock*>& locks) { // Add this method
        lockset.setLocks(locks);
    }
       int getId() const { // Add this method
        return id;}
};

class DataRaceDetector {
public:
    void onLockAcquire(Thread* t, Lock* l) {
        t->acquireLock(l);
    }

    void onLockRelease(Thread* t, Lock* l) {
        t->releaseLock(l);
    }

    void onSharedVariableAccess(Thread* t, SharedVariable* v, AccessType type) {
        if (type == AccessType::READ || type == AccessType::WRITE) {
            std::set<Lock*> threadLockset = t->getLockset();
            std::set<Lock*> variableLockset = v->getLockset();

            // If the thread's lockset and the variable's lockset don't intersect,
            // report a potential data race.
            if (!intersects(threadLockset, variableLockset)) {
                reportDataRace(t, v);
            }
        }

        if (type == AccessType::WRITE) {
            // If the thread writes to the variable, update the variable's lockset
            // to be the intersection of its current lockset and the thread's lockset.
            v->setLockset(intersect(v->getLockset(), t->getLockset()));
        }
    }

private:
    void reportDataRace(Thread* t, SharedVariable* v) {
        std::cout << "Potential data race: thread " << t->getId()
                  << " accessed variable " << v->getId()
                  << " without holding the necessary locks.\n";
    }

    std::set<Lock*> intersect(const std::set<Lock*>& set1, const std::set<Lock*>& set2) {
        std::set<Lock*> intersection;
        std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(),
                              std::inserter(intersection, intersection.begin()));
        return intersection;
    }

    bool intersects(const std::set<Lock*>& set1, const std::set<Lock*>& set2) {
        std::set<Lock*> intersection;
        std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(),
                              std::inserter(intersection, intersection.begin()));
        return !intersection.empty();
    }
};