/**
 * @file SharedVariable.cpp
 * @brief Implementation of the SharedVariable class
 */

#include "../include/SharedVariable.h"
#include "../include/Accesstype.h"
#include "../include/Thread.h"
#include "../include/Lock.h"
#include <iostream>

SharedVariable::SharedVariable(const std::string &name)
    : name(name), is_accessed(false), accessing_thread(nullptr), state(State::Virgin) {}

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
    return t;
}
void SharedVariable::reset()
{
    is_accessed = false;
    accessing_thread = nullptr;
    state = State::Virgin;
}
void SharedVariable::access(Thread *t, AccessType type)
{
    if (!t)
    {
        std::cerr << "Error: Null thread pointer passed to access" << std::endl;
        return;
    }
    
    is_accessed = true;
    accessing_thread = t;

    std::cout << "Thread " << t->getId() << " accessed variable " << name
              << " with " << (type == AccessType::READ ? "READ" : "WRITE") << " access. State: " << SharedVariable::stateToString(state) << std::endl;

    switch (state)
    {
    case State::Virgin:
        if (type == AccessType::WRITE)
        {
            state = State::Initializing;
        }
        else
        {
            state = State::Exclusive;
        }
        break;

    case State::Initializing:
        if (type == AccessType::WRITE)
        {
            state = State::SharedModified;
        }
        else
        {
            state = State::Shared;
        }
        break;

    case State::Exclusive:
        if (type == AccessType::WRITE)
        {
            state = State::SharedModified;
        }
        else if (type == AccessType::READ)
        {
            state = State::Shared;
        }
        break;

    case State::Shared:
        if (type == AccessType::WRITE)
        {
            state = State::SharedModified;
        }
        break;

    case State::SharedModified:
        // No state change for further accesses in SharedModified state
        break;

    default:
        break;
    }

    std::cout << "State after access: " << SharedVariable::stateToString(state) << std::endl;
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
        return "EM";
    default:
        return "?";
    }
}

std::set<Lock *> &SharedVariable::getCandidateLocks()
{
    return candidateLocks;
}

void SharedVariable::addCandidateLock(Lock *lock)
{
    if (lock)
    {
        candidateLocks.insert(lock);
    }
}

void SharedVariable::removeCandidateLock(Lock *lock)
{
    if (lock)
    {
        candidateLocks.erase(lock);
    }
}

void SharedVariable::clearCandidateLocks()
{
    candidateLocks.clear();
}

void SharedVariable::printCandidateLocks()
{
    std::cout << "Candidate locks for variable " << name << ": ";
    if (candidateLocks.empty())
    {
        std::cout << "none";
    }
    else
    {
        for (auto it = candidateLocks.begin(); it != candidateLocks.end(); ++it)
        {
            std::cout << (*it)->getId();
            if (std::next(it) != candidateLocks.end())
            {
                std::cout << ", ";
            }
        }
    }
    std::cout << std::endl;
}