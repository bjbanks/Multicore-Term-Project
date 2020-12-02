/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#include "task.h"

namespace WSDS {

std::atomic<int> next_task_id{0}; // used to id tasks for debugging

Task::Task() {
    this->worker = nullptr;
    this->parent = nullptr;
    this->children = std::vector<Task*>();
    this->finished = false;
    this->id = next_task_id++;
}

Task::~Task() {}

// function worker will call this in order to process the task
void Task::process(internal::Worker* worker) {
    this->worker = worker;

    // only process if not already finished
    if (!this->is_finished()) {
        // execute task computation
        this->execute();

        // task computation done, finish the task
        this->finish_task();
    }
}

// spawns a new "child" task
void Task::spawn(Task* task) {
    // mark self as parent of child task
    task->parent = this;

    // add child task to list of children
    this->children.push_back(task);

    // add child task to a worker's ready deque
    this->worker->add_ready_task(task);
}

// this task shall wait for all "children" tasks to finish computation;
// in the meantime, this task's worker may choose to process another
// ready tasks waiting to be processed, which may or may not be a "child"
// task
void Task::wait(void) {
    // check if ready, if not start a wait_loop
    if (!this->is_ready()) {
        this->worker->wait_loop();
    }
}

// is the task in a ready state for processing?
bool Task::is_ready(void) {
    // ready if and only if all children tasks are finished
    int nchildren = this->children.size();
    for (int i = 0; i < nchildren; i++) {
        if (!this->children[i]->is_finished()) {
            return false;
        }
    }
    return true;
}

// is the task computation finished?
bool Task::is_finished() {
    return this->finished;
}

// indicate task computation is finished
void Task::finish_task() {
    // aquire lock on finishedMutex of task
    std::unique_lock<std::mutex> lock(this->finishedMutex);

    this->finished = true;

    // release lock
    lock.unlock();

    // notify any waiting tasks
    this->finishedCV.notify_all();
}

// get the parent of the current task
Task* Task::get_parent() {
    return this->parent;
}

// returns unique task id
int Task::get_id() {
    return this->id;
}

} // namespace WSDS
