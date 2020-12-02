/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#include "parallelArray.h"
#include "math.h"
#include <iostream>


namespace BENCHMARKS {

    WSDS::Scheduler* parSched;
    int work_per_subtask;

    void print_arr(int* arr, int size){

        for (int i = 0; i < size; i ++){

            std::cout << arr[i] << " ";

        }

        std::cout << std::endl;


    }

    //if parenttask is NULL default to sheduler spawning.  If non-null, the user wants
    //task to be tracked as a subtask of a pre-existing task
    void Spawn(WSDS::Task* newTask, WSDS::Task* parentTask){

        if (parentTask == NULL){
            parSched->spawn(newTask);
        } else {
            parentTask->spawn(newTask);
        }

    }

    void Wait(WSDS::Task* parentTask){

        if (parentTask == NULL){
            parSched->wait();
        } else {
            parentTask->wait();
        }

    }


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
    void parallelAdd(int* vecOut, int* vecA, int* vecB, int size, WSDS::Task* parentTask) {


        int num_sub_tasks = size / work_per_subtask;
        int partial_size = size  / num_sub_tasks;
        int i;

        ParallelAddTaskPartial* tasks[num_sub_tasks];


        /*Spawn a bunch of congruent work to divide up array to do vector adds*/
        for (i = 0; i < num_sub_tasks; i++){

            int offset = i*partial_size;

            tasks[i] = new ParallelAddTaskPartial(&vecOut[offset], &vecA[offset], &vecB[offset], partial_size);

            Spawn(tasks[i], parentTask);

        }

        /*wait until all child tasks are done*/
        Wait(parentTask);

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
    void parallelMultiply(int* vecOut, int* vecA, int* vecB, int size, WSDS::Task* parentTask) {

        int i;
        int num_sub_tasks = size / work_per_subtask;
        int partial_size = size  / num_sub_tasks;

        ParallelMultiplyTaskPartial* tasks[num_sub_tasks];

        //        std::cout << "Task " << parentTask->getId() << " running parallel multiply" << std:: endl;


        /*Spawn a bunch of congruent work to divide up array to do vector adds*/
        for (i = 0; i < num_sub_tasks; i++){

            int offset = i*partial_size;

            tasks[i] = new ParallelMultiplyTaskPartial(&vecOut[offset], &vecA[offset], &vecB[offset], partial_size);

            //            std::cout << "Task " << parentTask->getId() << " Spawning " << tasks[i]->getId() << std::endl;
            Spawn(tasks[i], parentTask);

        }

        /*wait until all child tasks are done*/
        //        std::cout << "Task " << parentTask->getId() << " waiting" << std::endl;
        Wait(parentTask);
        //        std::cout << "Task " << parentTask->getId() << " done waiting" << std::endl;

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

        //        std::cout << "Task " << this->getId() << "in parallel multiply" << std:: endl;

        for(int i = 0; i < this->size; i++) {
            vecOut[i] = vecA[i] * vecB[i];
        }

    }


    /****************************************************************/
    /*            Parallel Copying                                  */
    /****************************************************************/

    void parallelCopy(int* out, int* in, int size, WSDS::Task* parentTask){

        //        std::cout << "Task " << parentTask->getId() << "running parallel copy" << std:: endl;

        int i;
        int num_sub_tasks = size / work_per_subtask;
        int partial_size = size  / num_sub_tasks;

        ParallelCopyTaskPartial* tasks[num_sub_tasks];


        /*Spawn a bunch of congruent work to divide up array to do vector adds*/
        for (i = 0; i < num_sub_tasks; i++){

            int offset = i*partial_size;

            tasks[i] = new ParallelCopyTaskPartial(&out[offset], &in[offset], partial_size);

            Spawn(tasks[i], parentTask);

        }

        /*wait until all child tasks are done*/
        Wait(parentTask);

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

        //        std::cout << "Task " << this->getId() << "in parallel copy" << std:: endl;
        for (int i = 0; i < this->size; i++){
            this->vecOut[i] = this->vecIn[i];
        }

    }


    /****************************************************************/
    /*            Parallel Reduce                                   */
    /****************************************************************/


    int parallelReduce(int* in, int size, WSDS::Task* parentTask){

        //        std::cout << "Task " << parentTask->getId() << "running parallel reduce" << std:: endl;

        int arrIn[size];
        int out[size];

        parallelCopy(arrIn, in, size, parentTask);

        int num_steps = (int)log2((double)size);
        int num_sub_tasks = size / work_per_subtask;

        ParallelReduceTaskPartial* tasks[size];

        for ( int step = 1; step <= num_steps; step++){

            for (int task = 0; task < num_sub_tasks; task++){

                tasks[task] = new ParallelReduceTaskPartial(out, arrIn, task*work_per_subtask+1, size, step);
                //                std::cout << "Task " << parentTask->getId() << " Spawning " << tasks[task]->getId() << std::endl;
                Spawn(tasks[task], parentTask);

            }


            //            std::cout << "Task " << parentTask->getId() << " waiting" << std::endl;
            Wait(parentTask);
            //            std::cout << "Task " << parentTask->getId() << " done waiting" << std::endl;

            parallelCopy(arrIn, out, size, parentTask);

        }
        return out[0];

    }

    ParallelReduceTaskPartial::ParallelReduceTaskPartial(int* arrOut, int* arrIn, int start_idx, int size, int step){
        this->arrOut = arrOut;
        this->arrIn = arrIn;
        this->size = size;
        this->step = step;
        this->start_idx = start_idx;
    }


    void ParallelReduceTaskPartial::execute(){

        //        std::cout << "Task " << this->getId() << "in parallel reduce" << std:: endl;


        if (start_idx > ( size / (1<<step))) {
            return; //nothing to do
        }

        for (int i = start_idx; i <= start_idx + work_per_subtask; i++){
            if (i <= ( size / (1<<step) ) ) { //i <= N/2^h

                arrOut[i-1] = arrIn[2*i-2] + arrIn[2*i-1];

            }
        }

    }


}
