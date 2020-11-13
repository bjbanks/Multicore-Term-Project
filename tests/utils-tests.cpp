/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#define _UNIT_TESTING

#include "utils.h"

// Google Unit Testing Framework
#include <gtest/gtest.h>

TEST(Utils, is_pow2) {
    ASSERT_TRUE(WSDS::internal::is_pow2(1));
    ASSERT_TRUE(WSDS::internal::is_pow2(2));
    ASSERT_TRUE(WSDS::internal::is_pow2(4));
    ASSERT_TRUE(WSDS::internal::is_pow2(8));
    ASSERT_TRUE(WSDS::internal::is_pow2(16));
    ASSERT_TRUE(WSDS::internal::is_pow2(32));
    ASSERT_TRUE(WSDS::internal::is_pow2(64));
    ASSERT_FALSE(WSDS::internal::is_pow2(3));
    ASSERT_FALSE(WSDS::internal::is_pow2(7));
    ASSERT_FALSE(WSDS::internal::is_pow2(13));
    ASSERT_FALSE(WSDS::internal::is_pow2(17));
    ASSERT_FALSE(WSDS::internal::is_pow2(30));
}

TEST(Utils, next_pow2) {
    ASSERT_EQ(1, WSDS::internal::next_pow2(1));
    ASSERT_EQ(2, WSDS::internal::next_pow2(2));
    ASSERT_EQ(4, WSDS::internal::next_pow2(3));
    ASSERT_EQ(4, WSDS::internal::next_pow2(4));
    ASSERT_EQ(8, WSDS::internal::next_pow2(5));
    ASSERT_EQ(16, WSDS::internal::next_pow2(10));
    ASSERT_EQ(32, WSDS::internal::next_pow2(31));
    ASSERT_EQ(1024, WSDS::internal::next_pow2(800));
}
