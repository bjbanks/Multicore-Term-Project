/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#define _UNIT_TESTING

#include "deque.h"
#include "increment-task.h"

// Google Unit Testing Framework
#include <gtest/gtest.h>

TEST(Deque, creating_and_deleting) {
    int id = 2;
    size_t size = 8;
    WSDS::internal::Deque* deque = new WSDS::internal::Deque(id, size);

    ASSERT_EQ(id, deque->get_id());
    ASSERT_EQ(size, deque->get_size());
    ASSERT_NE(nullptr, deque->get_collection());
    ASSERT_EQ(0, deque->get_bottom());
    ASSERT_EQ(0, deque->get_tag());
    ASSERT_EQ(0, deque->get_top());

    delete deque;
}

TEST(Deque, single_push) {
    int id = 2;
    size_t size = 8;
    WSDS::internal::Deque* deque = new WSDS::internal::Deque(id, size);

    int in = 2;
    int out;
    IncrementTask* task = new IncrementTask(in, &out);

    deque->push_bottom(task);

    WSDS::Task** collection = deque->get_collection();
    ASSERT_NE(nullptr, collection);
    ASSERT_EQ(task, collection[0]);
    ASSERT_EQ(1, deque->get_bottom());
    ASSERT_EQ(0, deque->get_top());

    delete task;
    delete deque;
}

TEST(Deque, single_push_and_pop) {
    int id = 2;
    size_t size = 8;
    WSDS::internal::Deque* deque = new WSDS::internal::Deque(id, size);

    int in = 2;
    int out;
    IncrementTask* task = new IncrementTask(in, &out);

    deque->push_bottom(task);

    ASSERT_EQ(1, deque->get_bottom());

    WSDS::Task* task_returned = deque->pop_bottom();

    ASSERT_EQ(task, task_returned);
    ASSERT_EQ(0, deque->get_bottom());
    ASSERT_EQ(0, deque->get_top());

    delete task;
    delete deque;
}

TEST(Deque, two_pushes_and_two_pops) {
    int id = 2;
    size_t size = 8;
    WSDS::internal::Deque* deque = new WSDS::internal::Deque(id, size);

    int in1 = 2;
    int out1;
    IncrementTask* task1 = new IncrementTask(in1, &out1);

    int in2 = 4;
    int out2;
    IncrementTask* task2 = new IncrementTask(in2, &out2);

    deque->push_bottom(task1);

    ASSERT_EQ(1, deque->get_bottom());

    deque->push_bottom(task2);

    ASSERT_EQ(2, deque->get_bottom());

    WSDS::Task* task2_returned = deque->pop_bottom();

    ASSERT_EQ(task2, task2_returned);

    WSDS::Task* task1_returned = deque->pop_bottom();

    ASSERT_EQ(task1, task1_returned);
    ASSERT_EQ(0, deque->get_bottom());
    ASSERT_EQ(0, deque->get_top());

    delete task1;
    delete task2;
    delete deque;
}

TEST(Deque, single_steal) {
    int id = 2;
    size_t size = 8;
    WSDS::internal::Deque* deque = new WSDS::internal::Deque(id, size);

    int in = 2;
    int out;
    IncrementTask* task = new IncrementTask(in, &out);

    deque->push_bottom(task);

    ASSERT_EQ(1, deque->get_bottom());

    WSDS::Task* stolen_task = deque->pop_top();

    ASSERT_EQ(task, stolen_task);
    ASSERT_EQ(1, deque->get_bottom());
    ASSERT_EQ(1, deque->get_top());

    delete task;
    delete deque;
}

TEST(Deque, empty_deque_top_and_bottom_pops) {
    int id = 2;
    size_t size = 8;
    WSDS::internal::Deque* deque = new WSDS::internal::Deque(id, size);

    WSDS::Task* task_returned = deque->pop_bottom();

    ASSERT_EQ(nullptr, task_returned);

    task_returned = deque->pop_top();

    ASSERT_EQ(nullptr, task_returned);
    ASSERT_EQ(0, deque->get_bottom());
    ASSERT_EQ(0, deque->get_top());

    delete deque;
}
