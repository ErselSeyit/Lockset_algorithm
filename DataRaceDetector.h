#ifndef DATARACEDETECTOR_H
#define DATARACEDETECTOR_H

#include "Thread.h"
#include "Lock.h"
#include "SharedVariable.h"

class DataRaceDetector {
public:
    void onLockAcquire(Thread* t, Lock* l);
    void onLockRelease(Thread* t, Lock* l);
    void onSharedVariableAccess(Thread* t, SharedVariable* v);
};

#endif // DATARACEDETECTOR_H
