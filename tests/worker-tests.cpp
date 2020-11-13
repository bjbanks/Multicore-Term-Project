
/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#define _UNIT_TESTING

#include "worker.h"

// Google Unit Testing Framework
#include <gtest/gtest.h>

TEST(Worker, creating_and_deleting) {
    int id = 0;
    int nvictims = 2;
    bool master = true;
    WSDS::internal::Worker* worker = new WSDS::internal::Worker(id, nvictims, master);

    ASSERT_EQ(id, worker->get_id());
    ASSERT_EQ(0, worker->get_nvictims());
    ASSERT_EQ(master, worker->get_master());

    delete worker;
}
