/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#include "scheduler.h"

namespace WSDS {

Scheduler::Scheduler(int nworkers) {
    this->nworkers = nworkers;
    if (this->nworkers == 0) {
        // match nworkers to available hardware
        this->nworkers = std::thread::hardware_concurrency();
    }
    this->rootTask = nullptr;

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
void Scheduler::spawn(Task* rootTask) {
    this->rootTask = rootTask;

    // assign root task to master worker
    this->masterWorker->assign_root_task(rootTask);

    // TODO - While this is fine for now, only 1 root task can be scheduled
    //        at any given time. It would be better to be able to handle
    //        multiple root task simultaneously.

    // start workers (if not already started)
    this->start_workers();
}

// called by the user application to wait for computation of the
// root task to finish
void Scheduler::wait(void) {
    // wait for computation of the root task to complete
    while (!this->rootTask->is_finished()) {
        std::this_thread::yield();
    }

    // TODO - above yielding loop should probably be changed to utilize a
    //        mutex and condition variable, where this thread will get
    //        notified when the root task is finished
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
    this->create_worker(0, this->nworkers - 1, true);
    this->masterWorker = this->workers[0].worker;

    // prepare non-master workers
    for (int i = 1; i < this->nworkers; i++) {
        this->create_worker(i, this->nworkers - 1, false);
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
void Scheduler::create_worker(int id, int nvictims, bool master) {
    this->workers[id].thr = nullptr;
	this->workers[id].worker = new internal::Worker(id, nvictims, master);
	this->workers[id].started = false;
	this->workers[id].ready = true;
}

} // namespace WSDS
