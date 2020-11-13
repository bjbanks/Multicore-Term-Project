/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#include "worker.h"

namespace WSDS {

/*
 * Internal data structures and functions not expected to be used
 * by user applications utilizing the WSDS user-level scheduler.
 */
namespace internal {

Worker::Worker(int id, int nvictims, bool master) {
    this->id = id;
    this->stopped = false;
    this->master = master;
    this->rootTask = nullptr;
    this->assignedTask = nullptr;
    this->nvictims = 0;
    this->victimDeqs = new Deque*[nvictims];
    this->readyDeq = new Deque(id, 100); // TODO - size needs to be dynamic somehow
}

Worker::~Worker() {
    delete[] this->victimDeqs;
    delete this->readyDeq;
}

// add a "victim" worker to cache of potential victims
void Worker::add_victim(Worker* victim) {
    this->victimDeqs[this->nvictims] = victim->readyDeq;
    this->nvictims++;
}

// indicate this worker should be stopped
void Worker::stop() {
    this->stopped = true;
}

// the main work loop of the worker
void Worker::work_loop() {
    // TODO
}

// secondary work loop for when the task being processed calls a wait()
// and can not proceed until all its children tasks have finished
void Worker::wait_loop() {
    // TODO
}

// called by the scheduler to assign a "root" task to the master worker
void Worker::assign_root_task(Task* task) {
    // can only assign root task to master worker
    if (!this->master) {
        return;
    }

    // TODO - While this is fine for now, only 1 root task can be scheduled
    //        at any given time. It would be better to be able to handle
    //        multiple root task simultaneously.

    this->rootTask = task;
}

// called by the executing task when a "child" tasks is spawned in order
// to add the task to the worker's ready pool
void Worker::new_child_task(Task* task) {
    this->readyDeq->push_bottom(task);
}

// attempt to steal a task from a "victim"
Task* Worker::steal_task() {
    // must have victims to steal from
    if (this->nvictims == 0 || this->victimDeqs == nullptr) {
        return nullptr;
    }

    // pick random victim
    Deque* victimDeq = this->victimDeqs[rand() % this->nvictims];

    // attempt to steal task from top of victim deque
    return victimDeq->pop_top();
}

} // namespace internal

} // namespace WSDS
