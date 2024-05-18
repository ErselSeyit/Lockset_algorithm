#include <iostream>
#include "Lock.h"
#include "SharedVariable.h"
#include "Thread.h"
#include "Accesstype.h"
#include "DataRaceDetector.h" // Include the DataRaceDetector header file
#include <pthread.h>
#include <tuple>
#include "../../usr/include/c++/9/tr1/tuple"

void *threadFunction(void *arg);
void runBarrierScenario(DataRaceDetector &drd, SharedVariable &var1, Lock &lock1, pthread_barrier_t &barrier);

int main()
{
    DataRaceDetector drd;

    Lock lock1(1);
    Lock lock2(2);

    SharedVariable var1("var1");

    drd.locksetMainStart();

    // Barrier Scenario
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, 2); // Initialize barrier for 2 threads

    drd.initializeBarrier(&barrier, NULL, 2);

    runBarrierScenario(drd, var1, lock1, barrier);

    pthread_barrier_destroy(&barrier); // Destroy the barrier

    drd.locksetMainEnd();

    return 0;
}

void runBarrierScenario(DataRaceDetector &drd, SharedVariable &var1, Lock &lock1, pthread_barrier_t &barrier)
{
    Thread thread1(1);
    Thread thread2(2);

    pthread_t pthread1, pthread2;

    auto threadArg1 = std::make_tuple(&drd, &var1, &lock1, &barrier, &thread1);
    auto threadArg2 = std::make_tuple(&drd, &var1, &lock1, &barrier, &thread2);

    pthread_create(&pthread1, NULL, threadFunction, &threadArg1);
    pthread_create(&pthread2, NULL, threadFunction, &threadArg2);

    pthread_join(pthread1, NULL);
    pthread_join(pthread2, NULL);
}

void *threadFunction(void *arg)
{
    auto tuple = *reinterpret_cast<std::tuple<DataRaceDetector *, SharedVariable *, Lock *, pthread_barrier_t *, Thread *> *>(arg);
    DataRaceDetector *drd = std::get<0>(tuple);
    SharedVariable *var1 = std::get<1>(tuple);
    Lock *lock1 = std::get<2>(tuple);
    pthread_barrier_t *barrier = std::get<3>(tuple);
    Thread *thread = std::get<4>(tuple);
    // Phase 1: Acquire lock and write to shared variable
    drd->onLockAcquire(thread, lock1, true, var1);
    drd->onSharedVariableAccess(thread, var1, AccessType::WRITE);
    drd->onLockRelease(thread, lock1, var1);

    // Wait at barrier
    drd->barrierWait();

    // Phase 2: Acquire lock and read from shared variable
    drd->onLockAcquire(thread, lock1, false, var1);
    drd->onSharedVariableAccess(thread, var1, AccessType::READ);
    drd->onLockRelease(thread, lock1, var1);

    return NULL;
}
