/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#include <stdlib.h>
#include <iostream>
#include "task.h"
#include "scheduler.h"

#define NWORKERS 8

class FibTask : public WSDS::Task {

public:
    FibTask(int n, long* out) {
        this->n = n;
        this->out = out;
    }

    void execute() {
        // fib(1) and fib(2) are both 1
        if (n <= 2) {
            *out = 1;
            return;
        }

        // if here, spawn a task for fib(n-1) and fib(n-2)
        long x;
        WSDS::Task* task1 = new FibTask(n-1, &x);
        spawn(task1);

        long y;
        WSDS::Task* task2 = new FibTask(n-2, &y);
        spawn(task2);

        // wait for all spawned child tasks to finish
        wait();

        delete task1;
        delete task2;

        // fib(n) = fib(n-1) + fib(n-2)
        *out = x + y;
    }

private:
    int n;
    long* out;

};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./fibonacci <index>" << std::endl;
        return 0;
    }

    WSDS::Scheduler* scheduler = new WSDS::Scheduler(NWORKERS);

    int in = std::strtol(argv[1], nullptr, 10);
    long out;
    FibTask* task = new FibTask(in, &out);

    scheduler->spawn(task);
    scheduler->wait();

    std::cout << out << std::endl;

    delete task;
    delete scheduler;
}
