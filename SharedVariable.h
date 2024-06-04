#ifndef SHAREDVARIABLE_H
#define SHAREDVARIABLE_H

#include <string>
#include <set>
#include "Thread.h"
#include "Accesstype.h"
#include "Lock.h"

class Thread;
class Lock;

enum class State
{
    Virgin,
    Initializing,
    Exclusive,
    Shared,
    SharedModified,
    Clean,
    Empty
};

class SharedVariable
{
private:
    std::string name;
    bool is_accessed;
    Thread *accessing_thread;
    State state;
    std::set<Lock *> candidateLocks;

public:
    SharedVariable(const std::string &name);
    SharedVariable(int id) : SharedVariable(std::to_string(id)) {}
    bool isAccessed() const;
    Thread *getAccessingThread() const;
    Thread *releaseThread(Thread *t);
    void reset();
    std::string getName() const;
    void access(Thread *t, AccessType type);
    State getState() const;
    void setState(State newState);
    std::set<Lock *> &getCandidateLocks();
    static std::string stateToString(State state);
    static std::string stateToStringShort(State state);
    void addCandidateLock(Lock *lock);
    void removeCandidateLock(Lock *lock);
    void clearCandidateLocks();
    void printCandidateLocks();
};

#endif
