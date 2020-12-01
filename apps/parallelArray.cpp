/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#include "parallelArray.h"
#include "math.h"

#define WORK_PER_SUBTASK (1 << 10)

namespace BENCHMARKS {

    WSDS::Scheduler* parSched;


    /****************************************************************/
    /*            Library Init                                      */
    /****************************************************************/
    void parallelArrayInit(WSDS::Scheduler* sched){
        parSched = sched;
    }

    /****************************************************************/
    /*            Parallel Adding                                   */
    /****************************************************************/
    void parallelAdd(int* vecOut, int* vecA, int* vecB, int size) {

        int num_sub_tasks = size / WORK_PER_SUBTASK;
        int partial_size = size  / num_sub_tasks;

        ParallelAddTaskPartial* tasks[num_sub_tasks];


        /*Spawn a bunch of congruent work to divide up array to do vector adds*/
        for (int i = 0; i < num_sub_tasks; i++){

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

        int num_sub_tasks = size / WORK_PER_SUBTASK;
        int partial_size = size  / num_sub_tasks;

        ParallelAddTaskPartial* tasks[num_sub_tasks];


        /*Spawn a bunch of congruent work to divide up array to do vector adds*/
        for (int i = 0; i < num_sub_tasks; i++){

            int offset = i*partial_size;
            
            tasks[i] = new ParallelMultiplyTaskPartial(&vecOut[offset], &vecA[offset], &vecB[offset], partial_size);

            parSched->spawn(tasks[i]);
            
        }

        /*wait until all child tasks are done*/
        parSched->wait();

        /*clean up the sub tasks*/
        for (i = 0; i < this->num_sub_tasks; i++){
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

        int num_sub_tasks = size / WORK_PER_SUBTASK;
        int partial_size = size  / num_sub_tasks;

        ParallelAddTaskPartial* tasks[num_sub_tasks];


        /*Spawn a bunch of congruent work to divide up array to do vector adds*/
        for (int i = 0; i < this->num_sub_tasks; i++){

            int offset = i*partial_size;
            
            tasks[i] = new ParallelCopyTaskPartial(&out[offset], &in[offset], partial_size);

            parSched->spawn(tasks[i]);
            
        }

        /*wait until all child tasks are done*/
        parSched->wait();

        /*clean up the sub tasks*/
        for (i = 0; i < this->num_sub_tasks; i++){
            delete tasks[i];
        }



    }

    
    void ParallelCopyTaskPartial::ParallelCopyTaskPartial(int* vecOut, int* vecIn, int size){

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

    int parallelReduce(int* in, int size){

        int out[size];
        
        parallelCopy(out, in, size);

        int num_steps = (int)log2((double)size);
        int num_sub_tasks = size / WORK_PER_SUBTASK;
        
        ParallelReduceTaskPartial* tasks[size];

        for ( int step = 1; i <= num_steps; i++){

            for (int task = 0; i < num_sub_tasks; i++){

                tasks[i] = new ParallelReduceTaskPartial(out, task*WORK_PER_SUBTASK+1, size, step);
                parSched->spawn(tasks[i]);
                
            }
            
            parSched->wait();
            
        }       
        
    }

    ParallelReduceTaskPartial::ParallelReduceTaskPartial(int* arr, int start_idx, int size, int step){       
        this->arr = arr;
        this->size = size;
        this->step = step;
        this->start_idx = start_idx;
    }

    void ParallelReduceTaskParial::execute(){

        if (start_idx <=  ( size / (1<<step))) {
            return; //nothing to do
        }

        for (int i = start_idx; i <= start_idx + WORK_PER_SUBTASK; i++){
            if (i <= ( size / (1<<step) ) ) { //i <= N/2^h
                arr[i-1] = arr[2*i-2] + arr[2*1-1];
            }
        }

    }    

}


}
