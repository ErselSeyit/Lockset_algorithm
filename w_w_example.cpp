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

    // Create a shared variable
    SharedVariable var1(1);

    // Create two threads
    Thread *thread1 = new Thread(1);
    Thread *thread2 = new Thread(2);

    // Simulate some events
    try
    {
        // Simulate operations by thread1
        drd.onSharedVariableAccess(thread1, &var1, AccessType::WRITE);  // Thread 1 writes to var1

        // Simulate operations by thread2
        drd.onSharedVariableAccess(thread2, &var1, AccessType::WRITE); // Thread 2 writes to var1
    }
    catch (const std::exception &e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    // Cleanup to prevent memory leaks
    delete thread1;
    delete thread2;

    return 0;
}
