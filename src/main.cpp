/**
 * @file main.cpp
 * @brief Main program demonstrating the Eraser lockset algorithm for data race detection
 * 
 * This program demonstrates two scenarios:
 * 1. Correct usage of locks to prevent data races
 * 2. Incorrect usage leading to detected data races
 */

#include <iostream>
#include <thread>
#include <chrono>
#include "../include/Lock.h"
#include "../include/SharedVariable.h"
#include "../include/Thread.h"
#include "../include/Accesstype.h"
#include "../include/DataRaceDetector.h"

void threadFunctionCorrect(int threadId, DataRaceDetector* drd, Lock* lock1, SharedVariable* var1)
{
    Thread thread(threadId);

    // Register thread and shared variable
    drd->registerThread(&thread);
    std::cout << "Variable " << var1->getName() << " registered. With state " 
              << var1->stateToString(var1->getState()) << std::endl;
    drd->registerSharedVariable(var1);

    // Acquire lock
    drd->onLockAcquire(&thread, lock1, true, var1);

    // Access shared variable (this call will update the state)
    drd->onSharedVariableAccess(&thread, var1, AccessType::WRITE);

    // Release lock
    drd->onLockRelease(&thread, lock1, var1);

    // Unregister thread
    drd->unregisterThread(&thread);
}

void threadFunctionIncorrect(int threadId, DataRaceDetector* drd, SharedVariable* var1)
{
    Thread thread(threadId);

    // Register thread and shared variable
    drd->registerThread(&thread);
    drd->registerSharedVariable(var1);

    // Access shared variable without acquiring lock (data race scenario)
    drd->onSharedVariableAccess(&thread, var1, AccessType::WRITE);

    // Unregister thread
    drd->unregisterThread(&thread);
}

int main()
{
    // Create detector and resources
    DataRaceDetector drd;
    Lock lock1(1);
    SharedVariable var1("var1");

    drd.locksetMainStart();

    // Scenario 1: Correct usage of locks
    auto start1 = std::chrono::high_resolution_clock::now();
    std::cout << "\n=== Scenario 1: Correct usage of locks preventing data races ===\n";
    std::thread t1(threadFunctionCorrect, 1, &drd, &lock1, &var1);
    std::thread t2(threadFunctionCorrect, 2, &drd, &lock1, &var1);
    t1.join();
    t2.join();
    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed1 = end1 - start1;
    std::cout << "Scenario 1 execution time: " << elapsed1.count() << " seconds\n";

    // Reset the state of var1 for the next scenario
    var1.reset();

    // Scenario 2: Incorrect usage of locks
    auto start2 = std::chrono::high_resolution_clock::now();
    std::cout << "\n=== Scenario 2: Incorrect usage of locks leading to data races ===\n";
    std::thread t3(threadFunctionIncorrect, 3, &drd, &var1);
    std::thread t4(threadFunctionIncorrect, 4, &drd, &var1);
    t3.join();
    t4.join();
    auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed2 = end2 - start2;
    std::cout << "Scenario 2 execution time: " << elapsed2.count() << " seconds\n";

    drd.locksetMainEnd();

    // Print summary report
    std::cout << "\n=== Summary Report ===\n";
    std::cout << "Total accesses: " << drd.getNumAccesses() << "\n";
    std::cout << "Total lock acquisitions: " << drd.getNumLockAcquisitions() << "\n";
    std::cout << "Total lock releases: " << drd.getNumLockReleases() << "\n";
    std::cout << "Total data races detected: " << drd.getNumDataRaces() << "\n";

    return 0;
}
