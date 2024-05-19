#include "SharedVariable.h"
#include "Accesstype.h"
#include "Thread.h"
#include "Lock.h"
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

void SharedVariable::access(Thread *t, AccessType type)
{
    is_accessed = true;
    accessing_thread = t;

    // std out state

    std::cout << " ************Thread " << t->getId() << " accessed variable " << name
              << " with " << (type == AccessType::READ ? "READ" : "WRITE") << " access. State  " << SharedVariable::stateToString(state) << std::endl;

    switch (state)
    {
    case State::Virgin:
        state = (type == AccessType::WRITE) ? State::Initializing : State::Exclusive;
        break;
    case State::Initializing:
        state = (type == AccessType::WRITE) ? State::SharedModified : State::Shared;
        break;
    case State::Exclusive:
        if (type == AccessType::WRITE)
        {
            state = State::SharedModified;
        }
        break;
    case State::Shared:
    case State::SharedModified:
        if (type == AccessType::WRITE)
        {
            state = State::SharedModified;
        }
        break;
    default:
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