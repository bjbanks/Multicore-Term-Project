/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#define _UNIT_TESTING

#include "scheduler.h"
#include "increment-task.h"
#include "fib-task.h"

// Google Unit Testing Framework
#include <gtest/gtest.h>

TEST(Scheduler, creating_and_deleting) {
    int nworkers = 4;
    WSDS::Scheduler* scheduler = new WSDS::Scheduler(nworkers);

    ASSERT_EQ(WSDS::WORK_STEALING, scheduler->get_workerAlg());

    ASSERT_EQ(nworkers, scheduler->get_nworkers());
    ASSERT_NE(nullptr, scheduler->get_workers());
    ASSERT_EQ(nullptr, scheduler->get_rootTask(0));

    delete scheduler;
}

TEST(Scheduler, spawn_and_wait_increment_task) {
    int nworkers = 4;
    WSDS::Scheduler* scheduler = new WSDS::Scheduler(nworkers);

    ASSERT_EQ(WSDS::WORK_STEALING, scheduler->get_workerAlg());

    int in = 2;
    int out;
    IncrementTask* task = new IncrementTask(in, &out);

    scheduler->spawn(task);
    scheduler->wait();

    ASSERT_EQ(3, out);

    delete task;
    delete scheduler;
}

TEST(Scheduler, spawn_and_wait_fib_task_work_stealing) {
    int nworkers = 4;
    WSDS::Scheduler* scheduler = new WSDS::Scheduler(nworkers);

    ASSERT_EQ(WSDS::WORK_STEALING, scheduler->get_workerAlg());

    int in = 10;
    long out;
    FibTask* task = new FibTask(in, &out);

    scheduler->spawn(task);
    scheduler->wait();

    ASSERT_EQ(55, out);

    delete task;
    delete scheduler;
}

TEST(Scheduler, spawn_and_wait_fib_task_round_robin) {
    int nworkers = 4;
    int workerAlg = WSDS::ROUND_ROBIN;
    WSDS::Scheduler* scheduler = new WSDS::Scheduler(nworkers, workerAlg);

    ASSERT_EQ(workerAlg, scheduler->get_workerAlg());

    int in = 10;
    long out;
    FibTask* task = new FibTask(in, &out);

    scheduler->spawn(task);
    scheduler->wait();

    ASSERT_EQ(55, out);

    delete task;
    delete scheduler;
}

TEST(Scheduler, spawn_and_wait_fib_task_random) {
    int nworkers = 4;
    int workerAlg = WSDS::RANDOM;
    WSDS::Scheduler* scheduler = new WSDS::Scheduler(nworkers, workerAlg);

    ASSERT_EQ(workerAlg, scheduler->get_workerAlg());

    int in = 10;
    long out;
    FibTask* task = new FibTask(in, &out);

    scheduler->spawn(task);
    scheduler->wait();

    ASSERT_EQ(55, out);

    delete task;
    delete scheduler;
}

TEST(Scheduler, spawn_and_wait_fib_task_smallest_deque) {
    int nworkers = 4;
    int workerAlg = WSDS::SMALLEST_DEQUE;
    WSDS::Scheduler* scheduler = new WSDS::Scheduler(nworkers, workerAlg);

    ASSERT_EQ(workerAlg, scheduler->get_workerAlg());

    int in = 10;
    long out;
    FibTask* task = new FibTask(in, &out);

    scheduler->spawn(task);
    scheduler->wait();

    ASSERT_EQ(55, out);

    delete task;
    delete scheduler;
}

TEST(Scheduler, spawn_and_wait_2_root_tasks) {
    int nworkers = 4;
    WSDS::Scheduler* scheduler = new WSDS::Scheduler(nworkers);

    ASSERT_EQ(WSDS::WORK_STEALING, scheduler->get_workerAlg());

    int in1 = 2;
    int out1;
    IncrementTask* incTask = new IncrementTask(in1, &out1);

    int in2 = 10;
    long out2;
    FibTask* fibTask = new FibTask(in2, &out2);

    scheduler->spawn(incTask);
    scheduler->spawn(fibTask);
    scheduler->wait();

    ASSERT_EQ(3, out1);
    ASSERT_EQ(55, out2);

    delete incTask;
    delete fibTask;
    delete scheduler;
}

TEST(Scheduler, spawn_and_wait_8_root_fib_tasks_work_stealing) {
    int nworkers = 8;
    int workerAlg = WSDS::WORK_STEALING;
    WSDS::Scheduler* scheduler = new WSDS::Scheduler(nworkers, workerAlg);

    ASSERT_EQ(workerAlg, scheduler->get_workerAlg());

    std::vector<int> in(nworkers);
    std::vector<long> out(nworkers);
    std::vector<FibTask*> tasks(nworkers);

    for (int i = 0; i < nworkers; i++) {
        in[i] = 10;
        tasks[i] = new FibTask(in[i], &out[i]);
        scheduler->spawn(tasks[i]);
    }

    scheduler->wait();

    for (int i = 0; i < nworkers; i++) {
        ASSERT_EQ(55, out[i]);

        delete tasks[i];
    }

    delete scheduler;
}

TEST(Scheduler, spawn_and_wait_8_root_fib_tasks_round_robin) {
    int nworkers = 8;
    int workerAlg = WSDS::ROUND_ROBIN;
    WSDS::Scheduler* scheduler = new WSDS::Scheduler(nworkers, workerAlg);

    ASSERT_EQ(workerAlg, scheduler->get_workerAlg());

    std::vector<int> in(nworkers);
    std::vector<long> out(nworkers);
    std::vector<FibTask*> tasks(nworkers);

    for (int i = 0; i < nworkers; i++) {
        in[i] = 10;
        tasks[i] = new FibTask(in[i], &out[i]);
        scheduler->spawn(tasks[i]);
    }

    scheduler->wait();

    for (int i = 0; i < nworkers; i++) {
        ASSERT_EQ(55, out[i]);

        delete tasks[i];
    }

    delete scheduler;
}

TEST(Scheduler, spawn_and_wait_8_root_fib_tasks_random) {
    int nworkers = 8;
    int workerAlg = WSDS::RANDOM;
    WSDS::Scheduler* scheduler = new WSDS::Scheduler(nworkers, workerAlg);

    ASSERT_EQ(workerAlg, scheduler->get_workerAlg());

    std::vector<int> in(nworkers);
    std::vector<long> out(nworkers);
    std::vector<FibTask*> tasks(nworkers);

    for (int i = 0; i < nworkers; i++) {
        in[i] = 10;
        tasks[i] = new FibTask(in[i], &out[i]);
        scheduler->spawn(tasks[i]);
    }

    scheduler->wait();

    for (int i = 0; i < nworkers; i++) {
        ASSERT_EQ(55, out[i]);

        delete tasks[i];
    }

    delete scheduler;
}

TEST(Scheduler, spawn_and_wait_8_root_fib_tasks_smallest_deque) {
    int nworkers = 8;
    int workerAlg = WSDS::SMALLEST_DEQUE;
    WSDS::Scheduler* scheduler = new WSDS::Scheduler(nworkers, workerAlg);

    ASSERT_EQ(workerAlg, scheduler->get_workerAlg());

    std::vector<int> in(nworkers);
    std::vector<long> out(nworkers);
    std::vector<FibTask*> tasks(nworkers);

    for (int i = 0; i < nworkers; i++) {
        in[i] = 10;
        tasks[i] = new FibTask(in[i], &out[i]);
        scheduler->spawn(tasks[i]);
    }

    scheduler->wait();

    for (int i = 0; i < nworkers; i++) {
        ASSERT_EQ(55, out[i]);

        delete tasks[i];
    }

    delete scheduler;
}

TEST(Scheduler, spawn_and_wait_100_root_fib_tasks_work_stealing) {
    int nworkers = 4;
    int workerAlg = WSDS::WORK_STEALING;
    WSDS::Scheduler* scheduler = new WSDS::Scheduler(nworkers, workerAlg);

    ASSERT_EQ(workerAlg, scheduler->get_workerAlg());

    int ntasks = 100;
    std::vector<int> in(ntasks);
    std::vector<long> out(ntasks);
    std::vector<FibTask*> tasks(ntasks);

    for (int i = 0; i < ntasks; i++) {
        in[i] = 10;
        tasks[i] = new FibTask(in[i], &out[i]);
        scheduler->spawn(tasks[i]);
    }

    scheduler->wait();

    for (int i = 0; i < ntasks; i++) {
        ASSERT_EQ(55, out[i]);

        delete tasks[i];
    }

    delete scheduler;
}

TEST(Scheduler, spawn_and_wait_100_root_fib_tasks_round_robin) {
    int nworkers = 4;
    int workerAlg = WSDS::ROUND_ROBIN;
    WSDS::Scheduler* scheduler = new WSDS::Scheduler(nworkers, workerAlg);

    ASSERT_EQ(workerAlg, scheduler->get_workerAlg());

    int ntasks = 100;
    std::vector<int> in(ntasks);
    std::vector<long> out(ntasks);
    std::vector<FibTask*> tasks(ntasks);

    for (int i = 0; i < ntasks; i++) {
        in[i] = 10;
        tasks[i] = new FibTask(in[i], &out[i]);
        scheduler->spawn(tasks[i]);
    }

    scheduler->wait();

    for (int i = 0; i < ntasks; i++) {
        ASSERT_EQ(55, out[i]);

        delete tasks[i];
    }

    delete scheduler;
}

TEST(Scheduler, spawn_and_wait_100_root_fib_tasks_random) {
    int nworkers = 4;
    int workerAlg = WSDS::RANDOM;
    WSDS::Scheduler* scheduler = new WSDS::Scheduler(nworkers, workerAlg);

    ASSERT_EQ(workerAlg, scheduler->get_workerAlg());

    int ntasks = 100;
    std::vector<int> in(ntasks);
    std::vector<long> out(ntasks);
    std::vector<FibTask*> tasks(ntasks);

    for (int i = 0; i < ntasks; i++) {
        in[i] = 10;
        tasks[i] = new FibTask(in[i], &out[i]);
        scheduler->spawn(tasks[i]);
    }

    scheduler->wait();

    for (int i = 0; i < ntasks; i++) {
        ASSERT_EQ(55, out[i]);

        delete tasks[i];
    }

    delete scheduler;
}

TEST(Scheduler, spawn_and_wait_100_root_fib_tasks_smallest_deque) {
    int nworkers = 4;
    int workerAlg = WSDS::SMALLEST_DEQUE;
    WSDS::Scheduler* scheduler = new WSDS::Scheduler(nworkers, workerAlg);

    ASSERT_EQ(workerAlg, scheduler->get_workerAlg());

    int ntasks = 100;
    std::vector<int> in(ntasks);
    std::vector<long> out(ntasks);
    std::vector<FibTask*> tasks(ntasks);

    for (int i = 0; i < ntasks; i++) {
        in[i] = 10;
        tasks[i] = new FibTask(in[i], &out[i]);
        scheduler->spawn(tasks[i]);
    }

    scheduler->wait();

    for (int i = 0; i < ntasks; i++) {
        ASSERT_EQ(55, out[i]);

        delete tasks[i];
    }

    delete scheduler;
}
