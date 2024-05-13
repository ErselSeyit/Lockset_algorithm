#include <iostream>
#include "Lock.h"
#include "SharedVariable.h"
#include "Thread.h"
#include "Accesstype.h"
#include "DataRaceDetector.h"  // Include the DataRaceDetector header file

int main() {
    // Create DataRaceDetector
    DataRaceDetector drd;

    // Create some locks
    Lock lock1(1), lock2(2);

    // Create some shared variables
    SharedVariable var1(1);

    // Create some threads
    Thread* thread1 = new Thread(1);
    Thread* thread2 = new Thread(2);

    // Simulate some events
    try {
        drd.onLockAcquire(thread1, &lock1); // Thread 1 acquires lock 1
        drd.onSharedVariableAccess(thread1, &var1, AccessType::READ); // Thread 1 reads from var1
 

        drd.onLockAcquire(thread2, &lock2); // Thread 2 acquires lock 1
        drd.onSharedVariableAccess(thread2, &var1, AccessType::WRITE); // Thread 2 writes to var1
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    // Delete the Thread objects to prevent memory leaks
    delete thread1;
    delete thread2;

    return 0;
}