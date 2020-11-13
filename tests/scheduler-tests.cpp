/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#define _UNIT_TESTING

#include "scheduler.h"
#include "increment-task.h"

// Google Unit Testing Framework
#include <gtest/gtest.h>

TEST(Scheduler, creating_and_deleting) {
    int nworkers = 4;
    WSDS::Scheduler* scheduler = new WSDS::Scheduler(nworkers);

    ASSERT_EQ(nworkers, scheduler->get_nworkers());
    ASSERT_NE(nullptr, scheduler->get_workers());
    ASSERT_NE(nullptr, scheduler->get_masterWorker());
    ASSERT_EQ(nullptr, scheduler->get_rootTask());

    delete scheduler;
}

// TEST(Scheduler, spawn_and_wait_increment_task) {
//     int nworkers = 4;
//     WSDS::Scheduler* scheduler = new WSDS::Scheduler(nworkers);
//
//     int in = 2;
//     int out;
//     IncrementTask* task = new IncrementTask(in, &out);
//
//     scheduler->spawn(task);
//     scheduler->wait();
//
//     ASSERT_EQ(3, out);
//
//     delete task;
//     delete scheduler;
// }
