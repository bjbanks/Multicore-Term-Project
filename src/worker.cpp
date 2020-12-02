/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#include "worker.h"
#include "scheduler.h"

namespace WSDS {

/*
 * Internal data structures and functions not expected to be used
 * by user applications utilizing the WSDS user-level scheduler.
 */
namespace internal {

Worker::Worker(int id, int nvictims, Scheduler* scheduler, int workerAlg) {
    this->id = id;
    this->stopped = false;
    this->workerAlg = workerAlg;
    this->assignedTask = nullptr;
    this->nvictims = 0;
    this->victimDeqs = new Deque*[nvictims];
    this->readyDeq = new Deque(id, 100000); // TODO - size needs to be dynamic
    this->scheduler = scheduler;
    this->distribution = std::uniform_int_distribution<>(0, nvictims-1);
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

// add a task to a worker's ready pool
void Worker::add_ready_task(Task* task, bool forceSelf, bool forceNotSelf) {
    Worker* worker = this;

    if (!forceSelf) {
        if (this->workerAlg != WORK_STEALING || forceNotSelf) {
            // determine worker based on worker algorithm via Scheduler
            worker = this->scheduler->next_worker();

            while (forceNotSelf && worker == this) {
                // force a random worker, need for this should be rare
                worker = this->scheduler->next_worker(true);
            }
        }
    }

    std::unique_lock<std::mutex> lock(worker->dequeMutex);
    worker->readyDeq->push_bottom(task);
    lock.unlock();
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
        std::unique_lock<std::mutex> lock(this->dequeMutex);
        this->assignedTask = this->readyDeq->pop_bottom();
        lock.unlock();

        // if no task, attempt to steal one if using stealing
        if (this->assignedTask == nullptr && this->workerAlg == WORK_STEALING) {
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
            this->assignedTask = nullptr;
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
        std::unique_lock<std::mutex> lock(this->dequeMutex);
        this->assignedTask = this->readyDeq->pop_bottom();
        lock.unlock();

        if (this->assignedTask == nullptr) {
            // ready deq empty, attempt to steal a task if using stealing
            if (this->workerAlg == WORK_STEALING) {
                std::this_thread::yield();
                this->assignedTask = steal_task();
            }
        }

        // if we have an assigned task, check if workable and process it
        if (this->assignedTask != nullptr) {
            if (!this->assignedTask->is_finished()) {

                // IMPORTANT NOTE!
                // In order to process the task here, it must originate from the
                // waiting task (be a child, grandchild, great-grandchild, ...),
                // otherwise a hang is possible where no worker can makes progress.

                // trace parents of chosen task looking for the waiting task
                Task* parent = this->assignedTask->get_parent();
                while (parent != nullptr && parent != waitingTask) {
                    parent = parent->get_parent();
                }

                if (parent == waitingTask) {
                    // originated from waiting task, process it to make progress
                    this->assignedTask->process(this);
                }
                else {
                    // did not originate from waiting task and is not workable,
                    // add the task to the ready deque of some other worker

                    Task* tmpTask = this->assignedTask;
                    this->add_ready_task(tmpTask, false, true); // forceNotSelf = true
                }
            }
            this->assignedTask = nullptr;
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
    if (this->nvictims == 0 || this->victimDeqs == nullptr || this->workerAlg != WORK_STEALING) {
        return nullptr;
    }

    // pick random victim
    int index = this->distribution(this->generator);
    Deque* victimDeq = this->victimDeqs[index];

    // attempt to steal task from top of victim deque
    return victimDeq->pop_top();
}

// get the current size of the ready deque (number of waiting ready tasks)
int Worker::get_ready_deque_size() {
    std::unique_lock<std::mutex> lock(this->dequeMutex);
    int ntasks = this->readyDeq->get_num_tasks();
    lock.unlock();

    return ntasks;
}

} // namespace internal

} // namespace WSDS
