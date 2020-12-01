
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
    bool useStealing = true;
    WSDS::internal::Worker* worker = new WSDS::internal::Worker(id, nvictims, useStealing);

    ASSERT_EQ(id, worker->get_id());
    ASSERT_EQ(0, worker->get_nvictims());
    ASSERT_EQ(useStealing, worker->get_useStealing());

    delete worker;
}
