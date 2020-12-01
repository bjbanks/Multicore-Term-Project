/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

/*
 * Deque implementation is originally based on reference pseudocode provided
 * by Arora, Blumofe, and Plaxton in their paper on "Thread Scheduling for
 * Multiprogrammed Multiprocessors".
 */

#ifndef _WSDS_DEQUE_DEFINE
#define _WSDS_DEQUE_DEFINE

#include <atomic>
#include "task.h"

namespace WSDS {

class Task; // forward declaration, defined elsewhere

/*
 * Internal data structures and functions not expected to be used
 * by user applications utilizing the WSDS user-level scheduler.
 */
namespace internal {

/*
 * Age struct is required by the current Deque implementation in order
 * to atomically update both tag and top via a compare and swap operation.
 */
typedef struct _Age {
    unsigned int tag;
    int top;
} Age;

/*
 * A work-stealing deque (i.e. "double-ended queue") implementation to be used
 * by workers to store the pool of waiting "ready" tasks. Worker's use their
 * own deques in a LIFO style, only pushing to and popping from the "bottom"
 * of the deque. However, when attempting to steal from another worker's deque,
 * work stealers will always pop from the top.
 */
class Deque {

public:
    Deque(int id, size_t size);
    ~Deque();

    // remove and return a task from the "top" of the deque,
    // only called by work stealers
    Task* pop_top(void);

    // add a task to the "bottom" of the deque,
    // only called by the deque owner
    void push_bottom(Task* task);

    // remove and return a task from the "bottom" of the deque,
    // only called by the deque owner
    Task* pop_bottom(void);

    // get the current number of tasks in the deque
    int get_num_tasks(void);

private:
    int id;
    size_t size;
    Task** collection;
    std::atomic<internal::Age> age;
    std::atomic<int> bottom;

#ifdef _UNIT_TESTING
public:
    int get_id(void) { return this->id; }
    size_t get_size(void) { return this->size; }
    Task** get_collection(void) { return this->collection; }
    unsigned int get_tag(void) { return this->age.load().tag; }
    int get_top(void) { return this->age.load().top; }
    int get_bottom(void) { return this->bottom.load(); }
#endif

}; // class Deque

} // namespace internal

} // namespace WSDS

#endif // _WSDS_DEQUE_DEFINE
