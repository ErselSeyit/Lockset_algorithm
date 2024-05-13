#include "SharedVariable.h"
#include "Accesstype.h"
#include "Thread.h"

SharedVariable::SharedVariable(const std::string &name) : name(name), is_accessed(false), accessing_thread(nullptr), state(State::Virgin) {}

bool SharedVariable::isAccessed() const
{
    return is_accessed;
}

Thread *SharedVariable::getAccessingThread() const
{
    return accessing_thread;
}

void SharedVariable::access(Thread *t, AccessType type)
{
    is_accessed = true;
    accessing_thread = t;
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
