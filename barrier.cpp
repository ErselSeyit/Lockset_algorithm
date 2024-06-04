#include <iostream>
#include <vector>
#include <tuple>
#include <pthread.h>
#include "DataRaceDetector.h"
#include "Lock.h"
#include "SharedVariable.h"
#include "Thread.h"
#include "Accesstype.h"

// ... (Your existing SharedVariable, Lock, Thread classes)

void* threadFunction(void* arg);
void runBarrierScenario(DataRaceDetector& drd, SharedVariable& var1, Lock& lock1, pthread_barrier_t& barrier, int numThreads, bool mixedAccess);

int main() {
    DataRaceDetector drd;
    Lock lock1(1);
    SharedVariable var1("var1");

    std::vector<int> threadCounts = {2, 4, 8}; // Test different numbers of threads

    for (int numThreads : threadCounts) {
        drd.locksetMainStart();

        pthread_barrier_t barrier;
        pthread_barrier_init(&barrier, NULL, numThreads);
        drd.initializeBarrier(&barrier, NULL, numThreads);

        // Scenario 1: All threads write in Phase 1, read in Phase 2
        runBarrierScenario(drd, var1, lock1, barrier, numThreads, false); 

        // Scenario 2: Mixed read/write access in both phases
        runBarrierScenario(drd, var1, lock1, barrier, numThreads, true);

        pthread_barrier_destroy(&barrier);
        drd.locksetMainEnd();

        std::cout << "\n-----------------------------------------\n";
    }
    return 0;
}

void runBarrierScenario(DataRaceDetector& drd, SharedVariable& var1, Lock& lock1, pthread_barrier_t& barrier, int numThreads, bool mixedAccess) {
    std::vector<Thread> threads(numThreads);
    std::vector<pthread_t> pthreads(numThreads);

    for (int i = 0; i < numThreads; ++i) {
        threads[i] = Thread(i + 1); // Create threads
        auto threadArg = std::make_tuple(&drd, &var1, &lock1, &barrier, &threads[i], mixedAccess); // Include mixedAccess flag
        pthread_create(&pthreads[i], NULL, threadFunction, &threadArg);
    }

    for (auto& pthread : pthreads) {
        pthread_join(pthread, NULL);
    }
}

void* threadFunction(void* arg) {
    auto [drd, var1, lock1, barrier, thread, mixedAccess] = *reinterpret_cast<std::tuple<DataRaceDetector*, SharedVariable*, Lock*, pthread_barrier_t*, Thread*, bool>*>(arg);
    // ... (Obtain references to drd, var1, lock1, barrier, thread, mixedAccess)

    // Phase 1: Write access (or mixed if mixedAccess is true)
    drd->onLockAcquire(thread, lock1, (mixedAccess || thread->getId() % 2 == 0), var1); // Write if mixedAccess or even thread ID
    drd->onSharedVariableAccess(thread, var1, (mixedAccess || thread->getId() % 2 == 0) ? AccessType::WRITE : AccessType::READ);
    drd->onLockRelease(thread, lock1, var1);
   
    // Wait at barrier
    drd->barrierWait();

    // Phase 2: Mixed access
    drd->onLockAcquire(thread, lock1, (mixedAccess || thread->getId() % 2 == 1), var1); // Write if mixedAccess or odd thread ID
    drd->onSharedVariableAccess(thread, var1, (mixedAccess || thread->getId() % 2 == 1) ? AccessType::WRITE : AccessType::READ);
    drd->onLockRelease(thread, lock1, var1);
  
    return NULL;
}
