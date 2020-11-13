/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#ifndef _INCREMENT_TASK_DEFINE
#define _INCREMENT_TASK_DEFINE

#include "task.h"

/*
 * This is a basic example of a user application task that incrments
 * an integer by 1.
 */
class IncrementTask : public WSDS::Task {

public:
    IncrementTask(int in, int* out) {
        this->in = in;
        this->out = out;
    }

    // WSDS Worker will call execute() to carry out computation of the task
    void execute() {
        // increment the value of "in" by 1 and return by storing in "out"
        *this->out = this->in + 1;
    }

private:
    int in;
    int* out;

};

#endif // _INCREMENT_TASK_DEFINE
