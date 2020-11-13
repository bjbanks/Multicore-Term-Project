/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#include "task.h"

namespace WSDS {

Task::Task() {
    this->worker = nullptr;
    this->parent = nullptr;
    this->children = std::vector<Task*>();
    this->finished = false;
    this->ready = true;
}

Task::~Task() {}

// function worker will call this in order to process the task
void Task::process(internal::Worker* worker) {
    // TODO
}

// spawns a new "child" task
void Task::spawn(Task* task) {
    // TODO
}

// this task shall wait for all "children" tasks to finish computation;
// in the meantime, this task's worker may choose to process another
// ready tasks waiting to be processed, which may or may not be a "child"
// task
void Task::wait(void) {
    // TODO
}

// is the task in a ready state for processing?
bool Task::is_ready(void) {
    // TODO
    return false;
}

// is the task computation finished?
bool Task::is_finished() {
    return this->finished;
}

// indicate task computation is finished
void Task::finish_task() {
    this->finished = true;
}

} // namespace WSDS
