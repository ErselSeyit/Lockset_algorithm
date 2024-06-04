#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include "Lock.h"
#include "SharedVariable.h"
#include "Thread.h"
#include "Accesstype.h"
#include "DataRaceDetector.h"

DataRaceDetector drd;
Lock lock1(1);
Lock lock2(2);
SharedVariable var1("var1");

void threadFunctionCorrect(int threadId)
{
    Thread thread(threadId);

    // Register thread and shared variable
    drd.registerThread(&thread);
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

    // Unregister thread
    drd.unregisterThread(&thread);
}

void threadFunctionMultipleLocks(int threadId)
{
    Thread thread(threadId);

    // Register thread and shared variable
    drd.registerThread(&thread);
    drd.registerSharedVariable(&var1);

    // Acquire first lock
    drd.onLockAcquire(&thread, &lock1, true, &var1);

    // Acquire second lock
    drd.onLockAcquire(&thread, &lock2, true, &var1);

    // Access shared variable (this call will update the state)
    drd.onSharedVariableAccess(&thread, &var1, AccessType::WRITE);

    // Release second lock
    drd.onLockRelease(&thread, &lock2, &var1);

    // Release first lock
    drd.onLockRelease(&thread, &lock1, &var1);

    // Unregister thread
    drd.unregisterThread(&thread);
}

void threadFunctionReadWriteLocks(int threadId, bool write)
{
    Thread thread(threadId);

    // Register thread and shared variable
    drd.registerThread(&thread);
    drd.registerSharedVariable(&var1);

    // Acquire lock
    drd.onLockAcquire(&thread, &lock1, write, &var1);

    // Access shared variable (this call will update the state)
    drd.onSharedVariableAccess(&thread, &var1, write ? AccessType::WRITE : AccessType::READ);

    // Release lock
    drd.onLockRelease(&thread, &lock1, &var1);

    // Unregister thread
    drd.unregisterThread(&thread);
}

void threadFunctionHighContention(int threadId)
{
    Thread thread(threadId);

    // Register thread and shared variable
    drd.registerThread(&thread);
    drd.registerSharedVariable(&var1);

    // Acquire lock
    drd.onLockAcquire(&thread, &lock1, true, &var1);

    // Access shared variable (this call will update the state) multiple times
    for (int i = 0; i < 1000; ++i)
    {
        drd.onSharedVariableAccess(&thread, &var1, AccessType::WRITE);
    }

    // Release lock
    drd.onLockRelease(&thread, &lock1, &var1);

    // Unregister thread
    drd.unregisterThread(&thread);
}

int main()
{
    drd.locksetMainStart();

    // Scenario 1: Correct usage of locks
    auto start1 = std::chrono::high_resolution_clock::now();
    std::cout << "Scenario 1: Correct usage of locks preventing data races\n";
    std::thread t1(threadFunctionCorrect, 1);
    std::thread t2(threadFunctionCorrect, 2);
    t1.join();
    t2.join();
    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed1 = end1 - start1;
    std::cout << "Scenario 1 execution time: " << elapsed1.count() << " seconds\n";

    // Reset the state of var1 for the next scenario
    var1.reset();

    // Scenario 2: Incorrect usage of locks
    auto start2 = std::chrono::high_resolution_clock::now();
    std::cout << "\nScenario 2: Incorrect usage of locks leading to data races\n";
    std::thread t3(threadFunctionIncorrect, 3);
    std::thread t4(threadFunctionIncorrect, 4);
    t3.join();
    t4.join();
    auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed2 = end2 - start2;
    std::cout << "Scenario 2 execution time: " << elapsed2.count() << " seconds\n";

    // Reset the state of var1 for the next scenario
    var1.reset();

    // Scenario 3: Multiple locks protecting the same variable
    auto start3 = std::chrono::high_resolution_clock::now();
    std::cout << "\nScenario 3: Multiple locks protecting the same variable\n";
    std::thread t5(threadFunctionMultipleLocks, 5);
    std::thread t6(threadFunctionMultipleLocks, 6);
    t5.join();
    t6.join();
    auto end3 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed3 = end3 - start3;
    std::cout << "Scenario 3 execution time: " << elapsed3.count() << " seconds\n";

    // Reset the state of var1 for the next scenario
    var1.reset();

    // Scenario 4: Read-Write locks
    auto start4 = std::chrono::high_resolution_clock::now();
    std::cout << "\nScenario 4: Read-Write locks\n";
    std::thread t7(threadFunctionReadWriteLocks, 7, true);  // Write lock
    std::thread t8(threadFunctionReadWriteLocks, 8, false); // Read lock
    t7.join();
    t8.join();
    auto end4 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed4 = end4 - start4;
    std::cout << "Scenario 4 execution time: " << elapsed4.count() << " seconds\n";

    // Reset the state of var1 for the next scenario
    var1.reset();

    // Scenario 5: High contention
    auto start5 = std::chrono::high_resolution_clock::now();
    std::cout << "\nScenario 5: High contention\n";
    std::vector<std::thread> threads;
    for (int i = 9; i < 19; ++i)
    {
        threads.emplace_back(threadFunctionHighContention, i);
    }
    for (auto &t : threads)
    {
        t.join();
    }
    auto end5 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed5 = end5 - start5;
    std::cout << "Scenario 5 execution time: " << elapsed5.count() << " seconds\n";

    drd.locksetMainEnd();

    // Print summary report
    std::cout << "\nSummary Report:\n";
    std::cout << "Total accesses: " << drd.numAccesses << "\n";
    std::cout << "Total lock acquisitions: " << drd.numLockAcquisitions << "\n";
    std::cout << "Total lock releases: " << drd.numLockReleases << "\n";
    std::cout << "Total data races detected: " << drd.numDataRaces << "\n";

    return 0;
}
