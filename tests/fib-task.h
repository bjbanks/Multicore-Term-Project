/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#ifndef _FIB_TASK_DEFINE
#define _FIB_TASK_DEFINE

#include "task.h"

/*
 * This is a basic example of a user application task that computes
 * the fibonacci sequence.
 */
class FibTask : public WSDS::Task {

public:
    FibTask(int n, long* out) {
        this->n = n;
        this->out = out;
    }

    // WSDS Worker will call execute() to process the task
    void execute() {
        // fib(1) and fib(2) are both 1
        if (n <= 2) {
            *out = 1;
            return;
        }

        // if here, spawn a task for fib(n-1) and fib(n-2)
        long x;
        WSDS::Task* task1 = new FibTask(n-1, &x);
        spawn(task1);

        long y;
        WSDS::Task* task2 = new FibTask(n-2, &y);
        spawn(task2);

        // wait for all spawned child tasks to finish
        wait();

        delete task1;
        delete task2;

        // fib(n) = fib(n-1) + fib(n-2)
        *out = x + y;
    }

private:
    int n;
    long* out;

};

#endif // _FIB_TASK_DEFINE
