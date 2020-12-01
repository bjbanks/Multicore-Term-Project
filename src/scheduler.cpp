/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#include "scheduler.h"

namespace WSDS {

Scheduler::Scheduler(int nworkers, int workerAlg) {
    this->nworkers = nworkers;
    if (this->nworkers == 0) {
        // match nworkers to available hardware
        this->nworkers = std::thread::hardware_concurrency();
    }
    this->rootTasks = std::vector<Task*>();
    this->workerAlg = workerAlg;

    // only needed for ROUND_ROBIN alg
    this->roundRobinIndex = 0;

    this->distribution = std::uniform_int_distribution<>(0, nworkers-1);

    // create all workers
    this->create_workers();

    // start all workers
    this->start_workers();
}

Scheduler::~Scheduler() {
    // stop all workers
    this->stop_workers();

    // delete objects
    for (int i = 0; i < this->nworkers; i++) {
        delete this->workers[i].thr;
        delete this->workers[i].worker;
    }
    delete []this->workers;
}

// schedules the root task for computation by the workers
void Scheduler::spawn(Task* rootTask, int workerIndex) {
    // add task to collection of root tasks
    this->rootTasks.push_back(rootTask);

    // add root task to ready deque of worker with index workerIndex
    this->workers[workerIndex].worker->add_ready_task(rootTask, true);

    // update round robin index if using round robin alg
    if (this->workerAlg == ROUND_ROBIN) {
        std::unique_lock<std::mutex> lock(this->roundRobinMutex);
        this->roundRobinIndex = workerIndex + 1;
        if (this->roundRobinIndex >= this->nworkers) {
            this->roundRobinIndex = 0;
        }
        lock.unlock();
    }
}

// called by the user application to wait for computation of all
// root tasks to finish
void Scheduler::wait(void) {
    // wait for computation of all root tasks to complete
    int ntasks = this->rootTasks.size();
    for (int i = 0; i < ntasks; i++) {

        // aquire lock on finishedMutex of task
        std::unique_lock<std::mutex> lock(this->rootTasks[i]->finishedMutex);

        // use CV to wait until task is finished
        while (!this->rootTasks[i]->is_finished()) {
            this->rootTasks[i]->finishedCV.wait(lock);
        }

        // release lock
        lock.unlock();
    }
}

// choose the next worker to get a task based on worker algorithm,
// not needed when using work stealing
internal::Worker* Scheduler::next_worker() {
    internal::Worker* worker;

    switch(this->workerAlg) {
        case ROUND_ROBIN:
            {
                std::unique_lock<std::mutex> lock(this->roundRobinMutex);
                worker = this->workers[this->roundRobinIndex].worker;
                this->roundRobinIndex++;
                if (this->roundRobinIndex >= this->nworkers) {
                    this->roundRobinIndex = 0;
                }
                lock.unlock();
            }
            break;

        case SMALLEST_DEQUE:
            {
                worker = this->worker_with_smallest_deque();
            }
            break;

        default:
        case RANDOM:
            {
                int index = this->distribution(this->generator);
                worker = this->workers[index].worker;
            }
            break;
    }

    return worker;
}

// create and start all worker threads if not already started
void Scheduler::start_workers() {
    // start non-master work loops first
    for (int i = 1; i < this->nworkers; i++) {
        this->start_worker(i);
    }

    // now start master work loop
    this->start_worker(0);
}

// create and start worker thread with given worker id
void Scheduler::start_worker(int id) {
    // yield if worker not yet ready
    while (!this->workers[id].ready) {
        std::this_thread::yield();
    }

    // only start if not already started
    if (!this->workers[id].started) {
        this->workers[id].started = true;
        this->workers[id].thr = new std::thread([=] {
            this->workers[id].worker->work_loop();
        });
    }
}

// stop all worker threads if "running"
void Scheduler::stop_workers() {
    for (int i = 0; i < this->nworkers; i++) {
        this->stop_worker(i);
    }

    // wait for all worker threads to terminate
    for (int i = 0; i < this->nworkers; i++) {
        // thread only exists if it was started
        if (this->workers[i].started && this->workers[i].thr->joinable()) {
            this->workers[i].thr->join();
        }
    }
}

// indicate worker thread with given worker id should be stopped
void Scheduler::stop_worker(int id) {
    // allow worker to complete ready prep
    while (!this->workers[id].ready) {
        std::this_thread::yield();
    }

    // indicate worker should be stopped
    this->workers[id].worker->stop();
}

// generate and prepare all workers
void Scheduler::create_workers() {
    // initialize non-ready worker data
    workers = new internal::WorkerData[this->nworkers];
    for (int i = 0; i < this->nworkers; i++) {
        this->workers[i].ready = false;
    }

    // prepare worker 0 (the master worker)
    this->create_worker(0, this->nworkers - 1);

    // prepare non-master workers
    for (int i = 1; i < this->nworkers; i++) {
        this->create_worker(i, this->nworkers - 1);
    }

    // yield until all workers are ready
    for (int i = 0; i < this->nworkers; i++) {
        while (!this->workers[i].ready) {
            std::this_thread::yield();
        }
    }

    // create a cache of victim references within each worker
    // all other workers are potential victims
    for (int i = 0; i < this->nworkers; i++) {
        for (int k = 0; k < this->nworkers; k++) {
            if (i != k) {
                this->workers[i].worker->add_victim(this->workers[k].worker);
            }
        }
    }
}

// prepare worker with given worker id
void Scheduler::create_worker(int id, int nvictims) {
    this->workers[id].thr = nullptr;
	this->workers[id].worker = new internal::Worker(id, nvictims, this, this->workerAlg);
	this->workers[id].started = false;
	this->workers[id].ready = true;
}

// determine worker with smallest number of waiting ready tasks
internal::Worker* Scheduler::worker_with_smallest_deque() {
    int workerIndex = 0;
    int smallestSize = INT_MAX;

    for (int i = 0; i < this->nworkers; i++) {
        internal::Worker* worker = this->workers[i].worker;
        int dequeSize = worker->get_ready_deque_size();
        if (dequeSize < smallestSize) {
            smallestSize = dequeSize;
            workerIndex = i;

            // cannot get smaller than an empty deque
            if (dequeSize == 0) {
                break;
            }
        }
    }

    return this->workers[workerIndex].worker;
}

} // namespace WSDS
