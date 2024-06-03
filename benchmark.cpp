#include <iostream>
#include <thread>
#include "Lock.h"
#include "SharedVariable.h"
#include "Thread.h"
#include "Accesstype.h"
#include "DataRaceDetector.h"

DataRaceDetector drd;
Lock lock1(1);
SharedVariable var1("var1");

void threadFunctionCorrect(int threadId)
{
    Thread thread(threadId);

    // Register thread and shared variable
    drd.registerThread(&thread);

    std::cout << "ON MAIN variable " << var1.getName() << " registered." << "With state  " << var1.stateToString(var1.getState()) << std::endl;
    drd.registerSharedVariable(&var1);

    // Acquire lock
    drd.onLockAcquire(&thread, &lock1, true, &var1);

    // Access shared variable (this call will update the state)
    drd.onSharedVariableAccess(&thread, &var1, AccessType::WRITE);

    // Release lock
    drd.onLockRelease(&thread, &lock1, &var1);

    // Unregister thread
    drd.unregisterThread(&thread);
}

void threadFunctionIncorrect(int threadId)
{
    Thread thread(threadId);

    // Register thread and shared variable
    drd.registerThread(&thread);
    drd.registerSharedVariable(&var1);

    // Access shared variable without acquiring lock
    drd.onSharedVariableAccess(&thread, &var1, AccessType::WRITE);
    var1.access(&thread, AccessType::WRITE); // Simulate access

    // Unregister thread
    drd.unregisterThread(&thread);
}

int main()
{
    drd.locksetMainStart();

    // Scenario 1: Correct usage of locks
    std::cout << "Scenario 1: Correct usage of locks preventing data races\n";
    std::thread t1(threadFunctionCorrect, 1);
    std::thread t2(threadFunctionCorrect, 2);
    t1.join();
    t2.join();

    // Reset the state of var1 for the next scenario
    var1.reset();

    // Scenario 2: Incorrect usage of locks
    std::cout << "\nScenario 2: Incorrect usage of locks leading to data races\n";
    std::thread t3(threadFunctionIncorrect, 3);
    std::thread t4(threadFunctionIncorrect, 4);
    t3.join();
    t4.join();

    drd.locksetMainEnd();

    return 0;
}
