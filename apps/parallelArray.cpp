/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#include "parallelArray.h"
#include "math.h"
#include <iostream>


namespace BENCHMARKS {

    WSDS::Scheduler* parSched;
    int work_per_subtask;


    /****************************************************************/
    /*            Library Init                                      */
    /****************************************************************/
    void parallelArrayInit(WSDS::Scheduler* sched, int task_work_size){
        parSched = sched;
        work_per_subtask = task_work_size;
    }

    /****************************************************************/
    /*            Parallel Adding                                   */
    /****************************************************************/
    void parallelAdd(int* vecOut, int* vecA, int* vecB, int size) {

        int num_sub_tasks = size / work_per_subtask;
        int partial_size = size  / num_sub_tasks;
        int i;

        ParallelAddTaskPartial* tasks[num_sub_tasks];


        /*Spawn a bunch of congruent work to divide up array to do vector adds*/
        for (i = 0; i < num_sub_tasks; i++){

            int offset = i*partial_size;
            
            tasks[i] = new ParallelAddTaskPartial(&vecOut[offset], &vecA[offset], &vecB[offset], partial_size);

            parSched->spawn(tasks[i]);
            
        }

        /*wait until all child tasks are done*/
        parSched->wait();

        /*clean up the sub tasks*/
        for (i = 0; i < num_sub_tasks; i++){
            delete tasks[i];
        }

    }


    ParallelAddTaskPartial::ParallelAddTaskPartial(int* vecOut, int* vecA, int* vecB, int size) {
        this->vecA = vecA;
        this->vecB = vecB;
        this->vecOut = vecOut;
        this->size = size;
    }
    
    void ParallelAddTaskPartial::execute(){

        for(int i = 0; i < this->size; i++) {
            vecOut[i] = vecA[i] + vecB[i];
        }

    }

    /****************************************************************/
    /*            Parallel Mutliply                                 */
    /****************************************************************/
    void parallelMultiply(int* vecOut, int* vecA, int* vecB, int size) {

        int i;
        int num_sub_tasks = size / work_per_subtask;
        int partial_size = size  / num_sub_tasks;

        ParallelMultiplyTaskPartial* tasks[num_sub_tasks];


        /*Spawn a bunch of congruent work to divide up array to do vector adds*/
        for (i = 0; i < num_sub_tasks; i++){

            int offset = i*partial_size;
            
            tasks[i] = new ParallelMultiplyTaskPartial(&vecOut[offset], &vecA[offset], &vecB[offset], partial_size);

            parSched->spawn(tasks[i]);
            
        }

        /*wait until all child tasks are done*/
        parSched->wait();

        /*clean up the sub tasks*/
        for (i = 0; i < num_sub_tasks; i++){
            delete tasks[i];
        }

    }


    ParallelMultiplyTaskPartial::ParallelMultiplyTaskPartial(int* vecOut, int* vecA, int* vecB, int size) {
        this->vecA = vecA;
        this->vecB = vecB;
        this->vecOut = vecOut;
        this->size = size;
    }
    
    void ParallelMultiplyTaskPartial::execute(){

        for(int i = 0; i < this->size; i++) {
            vecOut[i] = vecA[i] * vecB[i];
        }

    }


    /****************************************************************/
    /*            Parallel Copying                                  */
    /****************************************************************/

    void parallelCopy(int* out, int* in, int size){

        int i;
        int num_sub_tasks = size / work_per_subtask;
        int partial_size = size  / num_sub_tasks;

        ParallelCopyTaskPartial* tasks[num_sub_tasks];


        /*Spawn a bunch of congruent work to divide up array to do vector adds*/
        for (i = 0; i < num_sub_tasks; i++){

            int offset = i*partial_size;
            
            tasks[i] = new ParallelCopyTaskPartial(&out[offset], &in[offset], partial_size);

            parSched->spawn(tasks[i]);
            
        }

        /*wait until all child tasks are done*/
        parSched->wait();

        /*clean up the sub tasks*/
        for (i = 0; i < num_sub_tasks; i++){
            delete tasks[i];
        }



    }

    
    ParallelCopyTaskPartial::ParallelCopyTaskPartial(int* vecOut, int* vecIn, int size){

        this->vecOut = vecOut;
        this->vecIn = vecIn;
        this->size = size;

    }

    void ParallelCopyTaskPartial::execute(){
        for (int i = 0; i < this->size; i++){
            this->vecOut[i] = this->vecIn[i];
        }
        
    }


    /****************************************************************/
    /*            Parallel Reduce                                   */
    /****************************************************************/

void print_arr(int* arr, int size){

    for (int i = 0; i < size; i ++){

        std::cout << arr[i] << " ";

    }

    std::cout << std::endl;


}

    int parallelReduce(int* in, int size){

        int out[size];
        
        parallelCopy(out, in, size);

        int num_steps = (int)log2((double)size);
        int num_sub_tasks = size / work_per_subtask;


        
        ParallelReduceTaskPartial* tasks[size];

        for ( int step = 1; step <= num_steps; step++){

            for (int task = 0; task < num_sub_tasks; task++){

                tasks[task] = new ParallelReduceTaskPartial(out, task*work_per_subtask+1, size, step);
                parSched->spawn(tasks[task]);                
                
            }
            
            parSched->wait();

            print_arr(out, size);
            
        }     
        return out[0];
        
    }

    ParallelReduceTaskPartial::ParallelReduceTaskPartial(int* arr, int start_idx, int size, int step){       
        this->arr = arr;
        this->size = size;
        this->step = step;
        this->start_idx = start_idx;
    }

    void ParallelReduceTaskPartial::execute(){

        if (start_idx > ( size / (1<<step))) {
            return; //nothing to do
        }

        for (int i = start_idx; i <= start_idx + work_per_subtask; i++){
            if (i <= ( size / (1<<step) ) ) { //i <= N/2^h
                arr[i-1] = arr[2*i-2] + arr[2*i-1];
            }
        }        

    }    


}
