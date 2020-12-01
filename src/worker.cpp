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

Worker::Worker(int id, int nvictims, bool useStealing) {
    this->id = id;
    this->stopped = false;
    this->useStealing = useStealing;
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

// add a task to the worker's ready pool
void Worker::add_ready_task(Task* task) {
    this->readyDeq->push_bottom(task); // TODO - review synchronization
}

// called by the executing task when a "child" tasks is spawned in order
// to add the task to the worker's ready pool
void Worker::add_child_task(Task* task) {
    this->readyDeq->push_bottom(task);
}

// indicate this worker should be stopped
void Worker::stop() {
    this->stopped = true;
}

// the main work loop of the worker
void Worker::work_loop() {
    // continue in work loop until a stop is indicated
    while(!this->stopped.load()) {

        // attempt to collect next ready task
        this->assignedTask = this->readyDeq->pop_bottom();

        // if no task, attempt to steal one if using stealing
        if (this->assignedTask == nullptr && this->useStealing) {
            // no local ready task, attempt to steal one after yielding
            std::this_thread::yield();
            this->assignedTask = steal_task();
        }

        // if we have an assigned task, process it
        if (this->assignedTask != nullptr) {
            // only if not already finished
            if (!this->assignedTask->is_finished()) {
                this->assignedTask->process(this);
            }
        }

    }
}

// secondary work loop for when the task being processed calls a wait()
// and can not proceed until all its children tasks have finished
void Worker::wait_loop() {
    // move current assigned task to a waiting state
    Task* waitingTask = this->assignedTask;
    bool waitingTaskReady = false;
    this->assignedTask = nullptr;

    // continue in wait loop until a stop is indicated,
    // or the waitingTask has become ready
    while (!this->stopped.load() && !waitingTaskReady) {

        // attempt to collect next ready task
        this->assignedTask = this->readyDeq->pop_bottom();

        // if no task, attempt to steal one if using stealing
        if (this->assignedTask == nullptr && this->useStealing) {
            // no local ready task, attempt to steal one after yielding
            std::this_thread::yield();
            this->assignedTask = steal_task();
        }

        // if we have an assigned task, process it
        if (this->assignedTask != nullptr) {
            // only if not already finished
            if (!this->assignedTask->is_finished()) {
                this->assignedTask->process(this);
            }
        }

        // check if waiting task has become available
        if (waitingTask->is_ready()) {
            waitingTaskReady = true;
        }

    }

    // return to execution of the now ready waiting task
    this->assignedTask = waitingTask;
}

// attempt to steal a task from a "victim"
Task* Worker::steal_task() {
    // must have victims to steal from
    if (this->nvictims == 0 || this->victimDeqs == nullptr || !this->useStealing) {
        return nullptr;
    }

    // pick random victim
    Deque* victimDeq = this->victimDeqs[rand() % this->nvictims];

    // attempt to steal task from top of victim deque
    return victimDeq->pop_top();
}

} // namespace internal

} // namespace WSDS
