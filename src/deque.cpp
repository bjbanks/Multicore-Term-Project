/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

/*
 * Deque implementation is originally based on reference pseudocode provided
 * by Arora, Blumofe, and Plaxton in their paper on "Thread Scheduling for
 * Multiprogrammed Multiprocessors".
 */

#include "deque.h"

namespace WSDS {

/*
 * Internal data structures and functions not expected to be used
 * by user applications utilizing the WSDS user-level scheduler.
 */
namespace internal {

Deque::Deque(int id, size_t size) {
    this->id = id;
    this->size = size; // initial size
    this->collection = new Task*[size];
    internal::Age newAge;
    newAge.tag = 0;
    newAge.top = 0;
    this->age.store(newAge);
    this->bottom.store(0);
}

Deque::~Deque() {
    delete[] this->collection;
}

// remove and return a task from the "top" of the deque,
// only called by work stealers
Task* Deque::pop_top() {
    // load original index values
    internal::Age oldAge;
    oldAge = this->age.load();
    int localBot = this->bottom.load();

    // check if deque is empty
    if (localBot <= oldAge.top) {
        return nullptr; // EMPTY
    }

    // collect task from "top" of deque
    Task* task = this->collection[oldAge.top];

    // attempt to atomically update top with a compare and swap of age
    internal::Age newAge = oldAge;
    newAge.top++;
    if (this->age.compare_exchange_strong(oldAge, newAge)) {
        // atomic compare and swap success
        return task;
    }

    // atomic update failed, age was modified by another work stealer
    return nullptr; // ABORT
}

// add a task to the "bottom" of the deque,
// only called by the deque owner
void Deque::push_bottom(Task* task) {
    // load original bottom index value
    int localBot = this->bottom.load();

    // add task to deque
    this->collection[localBot] = task;

    // atomically update bottom index value
    localBot++;
    this->bottom.store(localBot);
}

// remove and return a task from the "bottom" of the deque,
// only called by the deque owner
Task* Deque::pop_bottom() {
    // load original bottom index value
    int localBot = this->bottom.load();

    // check if deque is empty
    if (localBot == 0) {
        return nullptr;
    }

    // atomically update bottom index value,
    // effectively removes bottom most task from deque
    localBot--;
    this->bottom.store(localBot);

    // collecte removed task
    Task* task = this->collection[localBot];

    // load original age value, and check if we made deque empty with pop
    internal::Age oldAge = this->age.load();
    if (localBot > oldAge.top) {
        // deque not empty, proceed with return of popped task
        return task;
    }

    // deque is now empty after the pop, reset top and bottom, and increment tag
    this->bottom.store(0);
    internal::Age newAge;
    newAge.top = 0;
    newAge.tag = oldAge.tag + 1;

    // because the deque is now empty localBot should equal old top, unless
    // the top (and only) task happen to have been stolen during execution
    // of this function
    if (localBot == oldAge.top) {
        if (this->age.compare_exchange_strong(oldAge, newAge)) {
            // atomic compare and swap success
            return task;
        }
    }

    // if here, "bottom" task was also the "top" task, and was stolen before
    // we could "atomically" pop it
    this->age.store(newAge);
    return nullptr;
}

} // namespace internal

} // namespace WSDS
