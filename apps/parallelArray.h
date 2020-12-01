/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#ifndef _VECTOR_ADD_TASK_DEFINE
#define _VECTOR_ADD_TASK_DEFINE

#include "task.h"
#include "scheduler.h"

namespace BENCHMARKS {


    /****************************************************************/
    /*            Library Init                                      */
    /****************************************************************/
    void parallelArrayInit(WSDS::Scheduler* sched, int task_work_size);


    /****************************************************************/
    /*            Parallel Adding                                   */
    /****************************************************************/
    void parallelAdd(int* vecOut, int* vecA, int* vecB, int size);
    
    class ParallelAddTaskPartial : public WSDS::Task {


    public:

        ParallelAddTaskPartial(int* vecOut, int* vecA, int* vecB, int size);

        void execute();

    private:

        int* vecA;
        int* vecB;
        int* vecOut;
        int size;

    };
    /****************************************************************/
    /*            Parallel Multiplying                              */
    /****************************************************************/
    void parallelMultiply(int* vecOut, int* vecA, int* vecB, int size);
    
    class ParallelMultiplyTaskPartial : public WSDS::Task {


    public:

        ParallelMultiplyTaskPartial(int* vecOut, int* vecA, int* vecB, int size);

        void execute();

    private:

        int* vecA;
        int* vecB;
        int* vecOut;
        int size;

    };

    /****************************************************************/
    /*            Parallel Copying                                  */
    /****************************************************************/

    void parallelCopy(int* out, int* in, int size);

    class ParallelCopyTaskPartial : public WSDS::Task {


    public:

        ParallelCopyTaskPartial(int* vecOut, int* vecIn, int size);

        void execute();

    private:
        
        int* vecOut;
        int* vecIn;
        int size;
    };
        
        
    /****************************************************************/
    /*            Parallel Reduce                                   */
    /****************************************************************/

    int parallelReduce(int* in, int size);


    class ParallelReduceTaskPartial : public WSDS::Task {


    public:

        ParallelReduceTaskPartial(int* arr, int start_idx, int size, int step);

        void execute();

    private:

        int* arr;
        int size;
        int step;
        int start_idx;
    };
    

}

#endif
