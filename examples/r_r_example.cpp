#include <iostream>
#include "../include/Lock.h"
#include "../include/SharedVariable.h"
#include "../include/Thread.h"
#include "../include/Accesstype.h"
#include "../include/DataRaceDetector.h"

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
        drd.onSharedVariableAccess(thread1, &var1, AccessType::READ); // Thread 1 reads from var1

        // Simulate operations by thread2
        drd.onSharedVariableAccess(thread2, &var1, AccessType::READ); // Thread 2 reads from var1
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
