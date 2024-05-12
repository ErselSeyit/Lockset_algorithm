#include "DataRaceDetector.h"
#include "Lock.h"
#include "SharedVariable.h"
#include "Thread.h"

int main() {
    // Create DataRaceDetector
    DataRaceDetector drd;

    // Create some locks
    Lock lock1(1), lock2(2);

    // Create some shared variables
    SharedVariable var1(1), var2(2);

    // Create some threads
    Thread thread1(1), thread2(2);

    // Simulate some events
    drd.onLockAcquire(&thread1, &lock1); // Thread 1 acquires lock 1
    drd.onSharedVariableAccess(&thread1, &var1); // Thread 1 accesses variable 1

    drd.onLockAcquire(&thread2, &lock2); // Thread 2 acquires lock 2
    drd.onSharedVariableAccess(&thread2, &var2); // Thread 2 accesses variable 2

    drd.onLockRelease(&thread1, &lock1); // Thread 1 releases lock 1
    drd.onSharedVariableAccess(&thread1, &var1); // Thread 1 accesses variable 1, potential data race!

    return 0;
}
