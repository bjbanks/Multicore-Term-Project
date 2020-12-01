/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#ifndef _WSDS_TASK_DEFINE
#define _WSDS_TASK_DEFINE

#include <vector>
#include <mutex>
#include <condition_variable>
#include "worker.h"

namespace WSDS {

namespace internal { class Worker; } // forward declaration, defined elsewhere

/*
 * A singular task to be processed sequentially by a worker; however, execution
 * of the task can spawn "child" tasks which will be added to the worker's pool
 * of ready tasks, where it could possibly be "stolen" by another worker and
 * computed in parallel with the "parent" task.
 *
 * User applications should extend this Task class in order to define their
 * own computive tasks. The execute() function is the computation to be done,
 * and must be defined by the extending class.
 */
class Task {

public:
    Task();
    virtual ~Task();

    // execute() is task computation function that must be
    // defined by extending class
    virtual void execute() = 0;

    // function worker will call this in order to process the task
    void process(internal::Worker* worker);

    // spawns a new "child" task
    void spawn(Task* task);

    // this task shall wait for all "children" tasks to finish computation;
    // in the meantime, this task's worker may choose to process another
    // ready tasks waiting to be processed, which may or may not be a "child"
    // task
    void wait(void);

    // is the task in a ready state for processing?
    bool is_ready(void);

    // is the task computation finished?
    bool is_finished(void);

    // indicate task computation is finished
    void finish_task(void);

    // get the parent of the current task
    Task* get_parent(void);

    std::mutex finishedMutex;
    std::condition_variable finishedCV;

private:
    internal::Worker* worker;
    Task* parent;
    std::vector<Task*> children;
    bool finished;
    bool ready;

    void set_parent(Task* task) { this->parent = task; }

}; // class Task

} // namespace WSDS

#endif // _WSDS_TASK_DEFINE
