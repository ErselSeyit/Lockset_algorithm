#include <set>
#include "Lock.h"

class SharedVariable {
public:
    SharedVariable(int id);
    void addLock(Lock* lock);
    void removeLock(Lock* lock);
    bool isGuardedBy(Lock* lock) const;
    std::set<Lock*> getLockset() const;
    void setLockset(const std::set<Lock*>& locks); // Declaration of setLockset
    int getId() const; // Declaration of getId

private:
    int id;
    std::set<Lock*> lockset;
};