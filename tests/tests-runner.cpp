/*
* Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
*/

#define _UNIT_TESTING

// Google Unit Testing Framework
#include <gtest/gtest.h>

/*
* Unit Test Runner.
* Runs all linked unit tests.
*/
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
