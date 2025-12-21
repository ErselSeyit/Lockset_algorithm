#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <functional> // Include functional header for std::function
#include <fstream>    // Include fstream for file operations
#include "../include/Lock.h"
#include "../include/SharedVariable.h"
#include "../include/Thread.h"
#include "../include/Accesstype.h"
#include "../include/DataRaceDetector.h"

DataRaceDetector drd;
Lock lock1(1);
Lock lock2(2);
SharedVariable var1("var1");

std::ofstream logFile("output_log.txt");

void threadFunctionCorrect(int threadId)
{
    auto threadStart = std::chrono::high_resolution_clock::now();
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
    auto threadEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> threadElapsed = threadEnd - threadStart;
    logFile << "Thread " << threadId << " execution time: " << threadElapsed.count() << " seconds\n";
}

void threadFunctionIncorrect(int threadId)
{
    auto threadStart = std::chrono::high_resolution_clock::now();
    Thread thread(threadId);

    // Register thread and shared variable
    drd.registerThread(&thread);
    drd.registerSharedVariable(&var1);

    // Access shared variable without acquiring lock
    drd.onSharedVariableAccess(&thread, &var1, AccessType::WRITE);

    // Unregister thread
    drd.unregisterThread(&thread);
    auto threadEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> threadElapsed = threadEnd - threadStart;
    logFile << "Thread " << threadId << " execution time: " << threadElapsed.count() << " seconds\n";
}

void threadFunctionMultipleLocks(int threadId)
{
    auto threadStart = std::chrono::high_resolution_clock::now();
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
    auto threadEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> threadElapsed = threadEnd - threadStart;
    logFile << "Thread " << threadId << " execution time: " << threadElapsed.count() << " seconds\n";
}

void threadFunctionReadWriteLocks(int threadId, bool write)
{
    auto threadStart = std::chrono::high_resolution_clock::now();
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
    auto threadEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> threadElapsed = threadEnd - threadStart;
    logFile << "Thread " << threadId << " execution time: " << threadElapsed.count() << " seconds\n";
}

void threadFunctionHighContention(int threadId)
{
    auto threadStart = std::chrono::high_resolution_clock::now();
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
    auto threadEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> threadElapsed = threadEnd - threadStart;
    logFile << "Thread " << threadId << " execution time: " << threadElapsed.count() << " seconds\n";
}

void runScenario(const std::string &scenarioName, const std::vector<std::function<void()>> &threads)
{
    logFile << "\n"
            << scenarioName << "\n";
    auto start = std::chrono::high_resolution_clock::now();

    // Run threads
    std::vector<std::thread> threadObjects;
    for (const auto &threadFunc : threads)
    {
        threadObjects.emplace_back(threadFunc);
    }

    // Join threads
    for (auto &threadObj : threadObjects)
    {
        threadObj.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    logFile << scenarioName << " execution time: " << elapsed.count() << " seconds\n";

    // Individual scenario summary
    logFile << scenarioName << " - Total accesses: " << drd.getNumAccesses() << "\n";
    logFile << scenarioName << " - Total lock acquisitions: " << drd.getNumLockAcquisitions() << "\n";
    logFile << scenarioName << " - Total lock releases: " << drd.getNumLockReleases() << "\n";
    logFile << scenarioName << " - Total data races detected: " << drd.getNumDataRaces() << "\n";

    // Reset the state of var1 for the next scenario
    var1.reset();
}

int main()
{
    drd.locksetMainStart();

    // Scenario 1: Correct usage of locks
    runScenario("Scenario 1: Correct usage of locks preventing data races", {[]()
                                                                             { threadFunctionCorrect(1); },
                                                                             []()
                                                                             { threadFunctionCorrect(2); }});

    // Scenario 2: Incorrect usage of locks
    runScenario("Scenario 2: Incorrect usage of locks leading to data races", {[]()
                                                                               { threadFunctionIncorrect(3); },
                                                                               []()
                                                                               { threadFunctionIncorrect(4); }});

    // Scenario 3: Multiple locks protecting the same variable
    runScenario("Scenario 3: Multiple locks protecting the same variable", {[]()
                                                                            { threadFunctionMultipleLocks(5); },
                                                                            []()
                                                                            { threadFunctionMultipleLocks(6); }});

    // Scenario 4: Read-Write locks
    runScenario("Scenario 4: Read-Write locks", {[]()
                                                 { threadFunctionReadWriteLocks(7, true); },
                                                 []()
                                                 { threadFunctionReadWriteLocks(8, false); }});

    // Scenario 5: High contention
    std::vector<std::function<void()>> highContentionThreads;
    for (int i = 9; i < 19; ++i)
    {
        highContentionThreads.emplace_back([i]()
                                           { threadFunctionHighContention(i); });
    }
    runScenario("Scenario 5: High contention", highContentionThreads);

    drd.locksetMainEnd();

    // Print overall summary report
    logFile << "\nOverall Summary Report:\n";
    logFile << "Total accesses: " << drd.getNumAccesses() << "\n";
    logFile << "Total lock acquisitions: " << drd.getNumLockAcquisitions() << "\n";
    logFile << "Total lock releases: " << drd.getNumLockReleases() << "\n";
    logFile << "Total data races detected: " << drd.getNumDataRaces() << "\n";

    logFile.close();
    return 0;
}
