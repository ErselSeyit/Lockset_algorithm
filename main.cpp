#include <iostream>
#include "Lock.h"
#include "SharedVariable.h"
#include "Thread.h"
#include "Accesstype.h"
#include "DataRaceDetector.h" // Include the DataRaceDetector header file

int main()
{
    // Create DataRaceDetector
    DataRaceDetector drd;

    // Create additional locks
    Lock lock3(3), lock4(4);

    // Create additional shared variables
    SharedVariable var2(2), var3(3);

    // Create additional threads
    Thread *thread3 = new Thread(3);
    Thread *thread4 = new Thread(4);

    // Simulate some events
    try
    {
        // Simulate operations by thread3
        drd.onLockAcquire(thread3, &lock3, true);                      // Thread 3 acquires lock 3 in write mode
        drd.onSharedVariableAccess(thread3, &var2, AccessType::READ);  // Thread 3 reads from var2
        drd.onSharedVariableAccess(thread3, &var3, AccessType::WRITE); // Thread 3 writes to var3
        drd.onLockRelease(thread3, &lock3);                            // Thread 3 releases lock 3

        // Simulate operations by thread4
        drd.onLockAcquire(thread4, &lock4, true);                      // Thread 4 acquires lock 4 in write mode
        drd.onSharedVariableAccess(thread4, &var2, AccessType::WRITE); // Thread 4 writes to var2
        drd.onLockRelease(thread4, &lock4);                            // Thread 4 releases lock 4
    }
    catch (const std::exception &e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    // Cleanup to prevent memory leaks
    delete thread3;
    delete thread4;

    return 0;
}
