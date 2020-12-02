/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#ifndef _WSDS_SCHEDULER_DEFINE
#define _WSDS_SCHEDULER_DEFINE

#include <iostream>
#include <random>
#include <chrono>
#include <limits.h>
#include "worker.h"

namespace WSDS {

/*
 * Internal data structures and functions not expected to be used
 * by user applications utilizing the WSDS user-level scheduler.
 */
namespace internal {

/*
 * WorkerData struct required by the current Scheduler implementation in order
 * to manage the state of workers and their data.
 */
typedef struct _WorkerData  {
    std::thread* thr;
    Worker* worker;
    std::atomic_bool ready;
    std::atomic_bool started;
} WorkerData;

} // namespace internal

/*
 * A work-stealing user-level task scheduler. User applications should utilize
 * the scheduler by first creating an instance of the scheduler which accepts
 * as input the desired number of worker threads. A value of 0 (i.e. zero) will
 * result in a number of worker threads equivalent to the maximum available
 * hardware. Once setup, user applications should use the scheduler's
 * spawn(Task* task) and wait() functions to "schedule" and process a root task.
 */
class Scheduler {

public:
    Scheduler(int nworkers, int workerAlg = WORK_STEALING);
    ~Scheduler();

    // schedules the root task for computation by the workers
    void spawn(Task* rootTask);

    // called by the user application to wait for computation of all
    // root tasks to finish
    void wait(void);

    // choose the next worker to get a task based on worker algorithm,
    // not needed when using work stealing
    internal::Worker* next_worker(bool forceRandom = false);

    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution;

private:
    int nworkers;
    internal::WorkerData* workers;
    internal::Worker* masterWorker;
    std::vector<Task*> rootTasks;
    int workerAlg;
    int roundRobinIndex;
    std::mutex roundRobinMutex;

    // create and start all worker threads if not already started
    void start_workers(void);

    // create and start worker thread with given worker id
    void start_worker(int id);

    // stop all worker threads if "running"
    void stop_workers(void);

    // indicate worker thread with given worker id should be stopped
    void stop_worker(int id);

    // generate and prepare all workers
    void create_workers(void);

    // prepare worker with given worker id
    void create_worker(int id, int nvictims);

    // determine worker with smallest number of waiting ready tasks
    internal::Worker* worker_with_smallest_deque(void);

#ifdef _UNIT_TESTING
public:
    int get_nworkers() { return this->nworkers; }
    internal::WorkerData* get_workers() { return this->workers; }
    int get_workerAlg() { return this->workerAlg; }
    Task* get_rootTask(unsigned int i) {
        if (i < this->rootTasks.size()) {
            return this->rootTasks[i];
        }
        else {
            return nullptr;
        }
    }
#endif

}; // class Scheduler

} // namespace WSDS

#endif // _WSDS_SCHEDULER_DEFINE
