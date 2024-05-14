#include "SharedVariable.h"
#include "Accesstype.h"
#include "Thread.h"
#include "Lock.h"
#include <iostream>
SharedVariable::SharedVariable(const std::string &name) : name(name), is_accessed(false), accessing_thread(nullptr), state(State::Virgin) {}

bool SharedVariable::isAccessed() const
{
    return is_accessed;
}

Thread *SharedVariable::getAccessingThread() const
{
    return accessing_thread;
}

Thread *SharedVariable::releaseThread(Thread *t)
{
    if (accessing_thread == t)
    {
        is_accessed = false;
        accessing_thread = nullptr;
    }
    return accessing_thread;
}

void SharedVariable::access(Thread *t, AccessType type)
{
    is_accessed = true;
    accessing_thread = t;

    switch (state)
    {
    case State::Virgin:
        state = (type == AccessType::READ) ? State::Exclusive : State::Initializing;
        break;
    case State::Initializing:
        state = (type == AccessType::READ) ? State::Shared : State::Exclusive;
        break;
    case State::Exclusive:
        state = (type == AccessType::READ) ? State::Exclusive : State::SharedModified;
        break;
    case State::Shared:
        state = (type == AccessType::READ) ? State::Shared : State::SharedModified;
        break;
    case State::SharedModified:
        state = State::SharedModified;
        break;
    case State::Clean:
        state = State::Exclusive;
        break;
    case State::Empty:
        break;
    }
}
std::string SharedVariable::getName() const
{
    return name;
}

State SharedVariable::getState() const
{
    return state;
}

void SharedVariable::setState(State newState)
{
    state = newState;
}

std::set<Lock *> &SharedVariable::getCandidateLocks()
{

    return candidateLocks;
}

std::string SharedVariable::stateToString(State state)
{
    switch (state)
    {
    case State::Virgin:
        return "Virgin";
    case State::Initializing:
        return "Initializing";
    case State::Exclusive:
        return "Exclusive";
    case State::Shared:
        return "Shared";
    case State::SharedModified:
        return "SharedModified";
    case State::Clean:
        return "Clean";
    case State::Empty:
        return "Empty";
    default:
        return "Unknown";
    }
}

std::string SharedVariable::stateToStringShort(State state)
{
    switch (state)
    {
    case State::Virgin:
        return "V";
    case State::Initializing:
        return "I";
    case State::Exclusive:
        return "E";
    case State::Shared:
        return "S";
    case State::SharedModified:
        return "SM";
    case State::Clean:
        return "C";
    case State::Empty:
        return "E";
    default:
        return "U";
    }
}

void SharedVariable::addCandidateLock(Lock *lock)
{
    candidateLocks.insert(lock);
}

void SharedVariable::removeCandidateLock(Lock *lock)
{
    candidateLocks.erase(lock);
}

void SharedVariable::clearCandidateLocks()
{
    candidateLocks.clear();
}

void SharedVariable::printCandidateLocks()
{
    std::cout << "Candidate locks for variable " << name << ": ";
    for (Lock *lock : candidateLocks)
    {
        std::cout << lock->getId() << " ";
    }
    std::cout << std::endl;
}