/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#define _UNIT_TESTING

#include "task.h"
#include "increment-task.h"

// Google Unit Testing Framework
#include <gtest/gtest.h>

TEST(Task, creating_and_deleting) {
    int in = 2;
    int out;
    IncrementTask* task = new IncrementTask(in, &out);

    delete task;
}
