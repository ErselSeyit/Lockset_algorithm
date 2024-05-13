#ifndef SHAREDVARIABLE_H
#define SHAREDVARIABLE_H
#include <string>
#include "Accesstype.h"

class Thread;

class SharedVariable {
public:
    SharedVariable(const std::string& name);
    SharedVariable(int id) : SharedVariable(std::to_string(id)) {}  // New constructor
    bool isAccessed() const;
    Thread* getAccessingThread() const;
    void access(Thread* t, AccessType type);
    std::string getName() const;

private:
    std::string name;
    bool is_accessed;
    Thread* accessing_thread;
};
#endif 