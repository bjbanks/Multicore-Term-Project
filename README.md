# Multicore-Term-Project

`WSDS` or `Work-Stealing Deque Scheduler` is a user-level task scheduler that utilizes a work-stealing deque algorithm for task management and scheduling.

## Testing

### Google Test

Unit tests are implemented with utilization of Google's C++ unit testing library, `Google Test`. Thus, prior to running unit tests, `Google Test` must be installed on your system.

Instructions for installation on an Ubuntu (or Linux equivalent) are below. Installation for other systems are left to the user to find elsewhere online.

1) `sudo apt-get install libgtest-dev # install gtest dev package`
2) `sudo apt-get install cmake # install cmake`
3) `/usr/src/gtest`
4) `sudo cmake CMakeLists.txt`
5) `sudo make`
6) `sudo cp *.a /usr/lib`

### Unit Tests

Unit test files are located in `tests/`. To run the tests, `cd` into the `tests/` directory and build the `unit_tests` executable by executing `make`. After `make` completes successfully, run `unit_tests`.

Results of the unit tests will be output to the console, and appear similar to the below output. It is expected your output will show many more test cases than below as unit tests are added during development.

```
[==========] Running 6 tests from 1 test cases.
[----------] Global test environment set-up.
[----------] 6 tests from Deque
[ RUN      ] Deque.creating_and_deleting
[       OK ] Deque.creating_and_deleting (0 ms)
[ RUN      ] Deque.single_push
[       OK ] Deque.single_push (0 ms)
[ RUN      ] Deque.single_push_and_pop
[       OK ] Deque.single_push_and_pop (0 ms)
[ RUN      ] Deque.two_pushes_and_two_pops
[       OK ] Deque.two_pushes_and_two_pops (0 ms)
[ RUN      ] Deque.single_steal
[       OK ] Deque.single_steal (0 ms)
[ RUN      ] Deque.empty_deque_top_and_bottom_pops
[       OK ] Deque.empty_deque_top_and_bottom_pops (0 ms)
[----------] 6 tests from Deque (0 ms total)

[----------] Global test environment tear-down
[==========] 6 tests from 1 test cases ran. (0 ms total)
[  PASSED  ] 6 tests.
```

### Valgrind

While not required for running unit tests, we also utilize `Valgrind` to check for memory related errors and memory leaks.

If installed on your system, you can run the unit tests with `Valgrind` by executing the generated `vg_unit_tests` executable located in `tests/` (after running `make` in `tests/`). The `Valgrind` memory check report will be output to `valgrind-out.txt`.

Instructions for installation on an Ubuntu (or Linux equivalent) are below. Installation for other systems are left to the user to find elsewhere online.

1) `sudo apt-get install valgrind`

### Benchmarks

To be able to run various benchmarks on the created scheduler you can do the following:

```
cd apps
make benchmark
./benchmark 2 5 1 <stealing | roundrobin | random | smallest>
```

Take care to keep the second parameter less than 8.  Matrix multiply is very memory hungry and could run out of memory.  To run other benchmarks with more memory, `benchmark.cpp` can be modifies to comment out matrix multiply and matrix transpose.
