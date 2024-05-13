// SharedVariable.cpp
#include "SharedVariable.h"
#include "Accesstype.h"
#include "Thread.h"

SharedVariable::SharedVariable(const std::string& name) : name(name), is_accessed(false), accessing_thread(nullptr) {}

bool SharedVariable::isAccessed() const {
    return is_accessed;
}

Thread* SharedVariable::getAccessingThread() const {
    return accessing_thread;
}

void SharedVariable::access(Thread* t, AccessType type) {
    is_accessed = true;
    accessing_thread = t;
    // Add code here to handle the access type (READ or WRITE)
}

std::string SharedVariable::getName() const {
    return name;
}