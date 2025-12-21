/**
 * @file SharedVariable.h
 * @brief Header file for the SharedVariable class and State enum
 */

#ifndef SHAREDVARIABLE_H
#define SHAREDVARIABLE_H

#include <string>
#include <set>
#include "Thread.h"
#include "Accesstype.h"
#include "Lock.h"

class Thread;
class Lock;

/**
 * @enum State
 * @brief Represents the state of a shared variable in the lockset algorithm
 * 
 * States track the access pattern history of a variable:
 * - Virgin: Never accessed
 * - Initializing: First write access
 * - Exclusive: Single thread has exclusive access
 * - Shared: Multiple threads reading
 * - SharedModified: Multiple threads with at least one write
 * - Clean: Reset state
 * - Empty: Uninitialized
 */
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

/**
 * @class SharedVariable
 * @brief Represents a shared variable that can be accessed by multiple threads
 * 
 * Tracks access patterns, current state, and candidate locks that protect
 * this variable according to the lockset algorithm.
 */
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
