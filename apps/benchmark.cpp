#include <iostream>
#include <sys/time.h>
#include <time.h>
#include "scheduler.h"
#include <stdlib.h>
#include "parallelArray.h"
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "parallelMatrix.h"

#define NWORKERS 1

void init_arr(int* arr, int size){

    for (int i = 1; i <= size; i++){
        arr[i-1] = i;
    }


}

void init_matrix(int * arr, int size){

    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            arr[GET_IDX(i, j, size)] = j;
        }
    }

}


void print_arr(int* arr, int size){

    for (int i = 0; i < size; i ++){

        std::cout << arr[i] << " ";

    }

    std::cout << std::endl;


}

void print_matrix(int * matrix, int size){

    for (int i = 0; i < size; i ++){
        print_arr(&matrix[i*size], size);
    }

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
        gettimeofday(&after, NULL);

        delete arr1;
        delete arr2;
        delete arr3;


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
        gettimeofday(&after, NULL);

        delete arr1;
        delete arr2;
        delete arr3;


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
        gettimeofday(&after, NULL);

        delete arr1;
        delete arr2;

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
            assert(result == ((size*(size+1))/2));
        }
        gettimeofday(&after, NULL);

        delete arr1;

    }

    /************************************************************/
    /*                 Parallel Transpose                       */
    /************************************************************/    
    if (!strcmp(app, "parallelTranspose")){

        arr1 = new int[size*size];

        init_matrix(arr1, size);

        gettimeofday(&before, NULL);
        for (i = 0; i < iterations; i++){
            BENCHMARKS::parallelMatrixTranspose(arr1, size);
        }
        gettimeofday(&after, NULL);

        delete arr1;

    }

    /************************************************************/
    /*                 Parallel Mat Multiply                    */
    /************************************************************/    
    if (!strcmp(app, "parallelMatMultiply")){


        arr1 = new int[size*size];
        arr2 = new int[size*size];
        arr3 = new int[size*size];

        init_matrix(arr1, size);
        init_matrix(arr2, size);

        BENCHMARKS::parallelMatrixTranspose(arr2, size);


        gettimeofday(&before, NULL);
        for (i = 0; i < iterations; i++){
            BENCHMARKS::parallelMatrixMultiply(arr3, arr1, arr2, size);
        }
        gettimeofday(&after, NULL);

        print_matrix(arr3, size);

        delete arr1;
        delete arr2;
        delete arr3;

    }


    time = t2d(&after) - t2d(&before);

    return time / iterations;   

}


int main(int argc, char* argv[]){

    double runtime;
    int task_work_size = 1<<atoi(argv[1]);
    int datasize = 1<<atoi(argv[2]);
    int iterations = atoi(argv[3]);
    char* policy = argv[4];
    WSDS::Scheduler* scheduler;

    //configure scheduler based on command line input
    if (!strcmp(policy, "smallest")) {
        scheduler = new WSDS::Scheduler(NWORKERS, WSDS::SMALLEST_DEQUE);
    } else if (!strcmp(policy, "stealing")) {
        scheduler = new WSDS::Scheduler(NWORKERS, WSDS::WORK_STEALING);
    } else if (!strcmp(policy, "random")) {
        scheduler = new WSDS::Scheduler(NWORKERS, WSDS::RANDOM);
    } else if (!strcmp(policy, "roundrobin")) {
        scheduler = new WSDS::Scheduler(NWORKERS, WSDS::ROUND_ROBIN);
    } else {

        std::cout << "Error: Unknown Scheduler Policy." << std:: endl;
        std::cout << " Please use one of the folllowing: smallest | stealing | random | roundrobin" << std::endl;
        exit(-1);

    }


    //initialize the parallel array library by registering the scheduler
    BENCHMARKS::parallelArrayInit(scheduler, task_work_size);
    BENCHMARKS::parallelMatrixInit(scheduler, task_work_size);
   

    /*std::cout << "Running Parallel Add: ";
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
    std::cout << "Result: " << runtime << " ns" << std::endl << std::endl;*/

    /*std::cout << "Running Parallel Transpose:" << std::endl;
    runtime = do_timed_run("parallelTranspose", datasize, iterations);
    std::cout << "Result: " << runtime << " ns" << std::endl << std::endl;*/

    std::cout << "Running Parallel Mat Multiply:" << std::endl;
    runtime = do_timed_run("parallelMatMultiply", datasize, iterations);
    std::cout << "Result: " << runtime << " ns" << std::endl << std::endl;


    return 0;

}
