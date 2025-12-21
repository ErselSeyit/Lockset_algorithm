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

    // Create shared variables
    SharedVariable var1(1), var2(2);

    // Create threads (using stack allocation to avoid memory leaks)
    Thread thread1(1);
    Thread thread2(2);

    // Simulate some events
    try
    {
        // Simulate operations by thread1
        drd.onSharedVariableAccess(&thread1, &var1, AccessType::READ);  // Thread 1 reads from var1
        drd.onSharedVariableAccess(&thread1, &var2, AccessType::WRITE); // Thread 1 writes to var2

        // Simulate operations by thread2
        drd.onSharedVariableAccess(&thread2, &var1, AccessType::WRITE); // Thread 2 writes to var1
        drd.onSharedVariableAccess(&thread2, &var2, AccessType::READ);  // Thread 2 reads from var2
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
