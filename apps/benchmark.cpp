#include <iostream>
#include <sys/time.h>
#include <time.h>
#include "scheduler.h"
#include <stdlib.h>
#include "parallelArray.h"
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define NWORKERS 1

void init_arr(int* arr, int size){

    for (int i = 1; i <= size; i++){
        arr[i-1] = i;
    }


}

void print_arr(int* arr, int size){

    for (int i = 0; i < size; i ++){

        std::cout << arr[i] << " ";

    }

    std::cout << std::endl;


}

double t2d(struct timeval *t) {
    return t->tv_sec*1000000.0 + t->tv_usec;
}

double do_timed_run(const char* app, int size, int iterations){

    struct timeval before, after;
    int i;
    double time;
    int *arr1, *arr2, *arr3;
    int result;


    /************************************************************/
    /*                 Parallel Add                             */
    /************************************************************/
    if (!strcmp(app, "parallelAdd")) {
        
        arr1 = new int[size];
        arr2 = new int[size];
        arr3 = new int[size];

        init_arr(arr1, size);
        init_arr(arr2, size);
        

        gettimeofday(&before, NULL);
        for (i = 0; i < iterations; i++){
            BENCHMARKS::parallelAdd(arr3, arr1, arr2, size);
        }


    }

    /************************************************************/
    /*                 Parallel Multiply                        */
    /************************************************************/    
    if (!strcmp(app, "parallelMultiply")) {
        
        arr1 = new int[size];
        arr2 = new int[size];
        arr3 = new int[size];

        init_arr(arr1, size);
        init_arr(arr2, size);
        

        gettimeofday(&before, NULL);
        for (i = 0; i < iterations; i++){
            BENCHMARKS::parallelMultiply(arr3, arr1, arr2, size);
        }


    }

    /************************************************************/
    /*                 Parallel Copy                            */
    /************************************************************/    
    if (!strcmp(app, "parallelCopy")) {
        
        arr1 = new int[size];
        arr2 = new int[size];

        init_arr(arr1, size);        

        gettimeofday(&before, NULL);
        for (i = 0; i < iterations; i++){
            BENCHMARKS::parallelCopy(arr2, arr1, size);
        }

    }

    /************************************************************/
    /*                 Parallel Reduce                          */
    /************************************************************/    
    if (!strcmp(app, "parallelReduce")){

        arr1 = new int[size];

        init_arr(arr1, size);

        gettimeofday(&before, NULL);
        for (i = 0; i < iterations; i++){
            result = BENCHMARKS::parallelReduce(arr1, size);
            std::cout << result << " " << ((size*(size+1))/2) << std::endl;
            assert(result == ((size*(size+1))/2));
        }



    }



    gettimeofday(&after, NULL);



    time = t2d(&after) - t2d(&before);

    return time / iterations;   

}


int main(int argc, char* argv[]){

    double runtime;
    int task_work_size = atoi(argv[1]);
    int datasize = atoi(argv[2]);
    int iterations = atoi(argv[3]);

    WSDS::Scheduler* scheduler = new WSDS::Scheduler(NWORKERS, true);


    //initialize the parallel array library by registering the scheduler
    BENCHMARKS::parallelArrayInit(scheduler, task_work_size);
   

    std::cout << "Running Parallel Add: ";
    runtime = do_timed_run("parallelAdd", datasize, iterations);
    std::cout << "Result: " << runtime << " ns" << std::endl << std::endl;

    std::cout << "Running Parallel Multiply:" << std::endl;
    runtime = do_timed_run("parallelMultiply", datasize, iterations);
    std::cout << "Result: " << runtime << " ns" << std::endl << std::endl;

    std::cout << "Running Parallel Copy:" << std::endl;
    runtime = do_timed_run("parallelCopy", datasize, iterations);
    std::cout << "Result: " << runtime << " ns" << std::endl << std::endl;

    std::cout << "Running Parallel Reduce:" << std::endl;
    runtime = do_timed_run("parallelReduce", datasize, iterations);
    std::cout << "Result: " << runtime << " ns" << std::endl << std::endl;

    
    return 0;

}
