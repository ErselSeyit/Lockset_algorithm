#include <iostream>
#include "Lock.h"
#include "SharedVariable.h"
#include "Thread.h"
#include "Accesstype.h"
#include "DataRaceDetector.h" // Include the DataRaceDetector header file

void runScenario1(DataRaceDetector &drd, SharedVariable &var1)
{
    // Create threads for scenario 1
    Thread thread1(1);
    Thread thread2(2);

    std::cout << "\nScenario 1: Data race detected" << std::endl;
    try
    {
        drd.onSharedVariableAccess(&thread1, &var1, AccessType::WRITE); // Thread 1 writes var1 without holding a lock
        drd.onSharedVariableAccess(&thread2, &var1, AccessType::READ);  // Thread 2 reads var1 without holding a lock
    }
    catch (const std::exception &e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    std::cout << "End of scenario 1" << std::endl;
}
void runScenario2(DataRaceDetector &drd, SharedVariable &var1, Lock &lock1)
{
    // Create threads for scenario 2
    Thread thread1(1);
    Thread thread2(2);

    std::cout << "\nScenario 2: Correct usage of locks preventing data races" << std::endl;
    try
    {
        drd.onLockAcquire(&thread1, &lock1, true, &var1);               // Thread 1 acquires lock1 in write mode
        drd.onSharedVariableAccess(&thread1, &var1, AccessType::WRITE); // Thread 1 writes var1
        drd.onLockRelease(&thread1, &lock1, &var1);                     // Thread 1 releases lock1

        drd.onLockAcquire(&thread2, &lock1, false, &var1);             // Thread 2 acquires lock1 in read mode
        drd.onSharedVariableAccess(&thread2, &var1, AccessType::READ); // Thread 2 reads var1
        drd.onLockRelease(&thread2, &lock1, &var1);                    // Thread 2 releases lock1
    }
    catch (const std::exception &e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    std::cout << "End of scenario 2" << std::endl;
}
void runScenario3(DataRaceDetector &drd, SharedVariable &var1, Lock &lock1)
{
    // Create threads for scenario 3
    Thread thread1(1);
    Thread thread2(2);

    std::cout << "\nScenario 3: Mixed read/write access with locks" << std::endl;
    try
    {
        drd.onLockAcquire(&thread1, &lock1, true, &var1);               // Thread 1 acquires lock1 in write mode
        drd.onSharedVariableAccess(&thread1, &var1, AccessType::WRITE); // Thread 1 writes var1

        drd.onLockAcquire(&thread2, &lock1, false, &var1);             // Thread 2 acquires lock1 in read mode
        drd.onSharedVariableAccess(&thread2, &var1, AccessType::READ); // Thread 2 reads var1

        drd.onLockRelease(&thread1, &lock1, &var1); // Thread 1 releases lock1
        drd.onLockRelease(&thread2, &lock1, &var1); // Thread 2 releases lock1
    }
    catch (const std::exception &e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    std::cout << "End of scenario 3" << std::endl;
}
void runScenario4(DataRaceDetector &drd, SharedVariable &var1, Lock &lock1, Lock &lock2)
{
    // Create threads for scenario 4
    Thread thread1(1);
    Thread thread2(2);

    std::cout << "\nScenario 4: Nested locks" << std::endl;
    try
    {
        drd.onLockAcquire(&thread1, &lock1, true, &var1);               // Thread 1 acquires lock1 in write mode
        drd.onLockAcquire(&thread1, &lock2, true, &var1);               // Thread 1 acquires lock2 in write mode
        drd.onSharedVariableAccess(&thread1, &var1, AccessType::WRITE); // Thread 1 writes var1

        drd.onLockAcquire(&thread2, &lock1, false, &var1);             // Thread 2 acquires lock1 in read mode
        drd.onSharedVariableAccess(&thread2, &var1, AccessType::READ); // Thread 2 reads var1

        drd.onLockRelease(&thread1, &lock2, &var1); // Thread 1 releases lock2
        drd.onLockRelease(&thread1, &lock1, &var1); // Thread 1 releases lock1
        drd.onLockRelease(&thread2, &lock1, &var1); // Thread 2 releases lock1
    }
    catch (const std::exception &e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    std::cout << "End of scenario 4" << std::endl;
}

int main()
{
    DataRaceDetector drd;

    Lock lock1(1);
    Lock lock2(2);

    SharedVariable var1("var1");

    drd.locksetMainStart();

    // Scenario 1
    {
        var1.setState(State::Virgin);
        drd.registerSharedVariable(&var1);
        drd.locksetThreadStart();
        runScenario1(drd, var1);
        drd.locksetThreadEnd();
    }

    // Scenario 2
    {
        var1.setState(State::Virgin);
        drd.registerSharedVariable(&var1);
        drd.locksetThreadStart();
        runScenario2(drd, var1, lock1);
        drd.locksetThreadEnd();
    }

    // Scenario 3
    {
        var1.setState(State::Virgin);
        drd.registerSharedVariable(&var1);
        drd.locksetThreadStart();
        runScenario3(drd, var1, lock1);
        drd.locksetThreadEnd();
    }

    // Scenario 4
    {
        var1.setState(State::Virgin);
        drd.registerSharedVariable(&var1);
        drd.locksetThreadStart();
        runScenario4(drd, var1, lock1, lock2);
        drd.locksetThreadEnd();
    }

    drd.locksetMainEnd();

    return 0;
}
