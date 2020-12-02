/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */


#ifndef _PARALLEL_MATRIX_DEFINE
#define _PARAKLLELMATRIX_DEFINE

#include "task.h"
#include "scheduler.h"

namespace BENCHMARKS {


#define GET_IDX(row, col, size) (row*size + col)

    /******************************************************************/
    /*                    Library Initialization                      */
    /******************************************************************/

    //initializes the parallel matrix library
    void parallelMatrixInit(WSDS::Scheduler* sched, int task_work_size);


    /******************************************************************/
    /*                    Matrix Transpose                            */
    /******************************************************************/


    //will transpose a matrix that is a (size x size) square matrix.  X should be a 2-D array
    void parallelMatrixTranspose(int* x, int size);


    class ParallelMatrixTransposePartial : public WSDS::Task {
        
    public:

        

        ParallelMatrixTransposePartial(int* x, int size, int block_width, int block_idx);

        //will spawn thread that will divide and conquer on matrix transpose
        void execute();

    private:

        int * x;
        int size;
        int block_width;
        int block_idx;

    };


    /******************************************************************/
    /*                    Matrix Multiply                             */
    /******************************************************************/

    //will transpose a matrix that is a (size x size) square matrix.  A/B/OUT should all be 2-D arrays
    void parallelMatrixMultiply(int* out, int* A, int* B, int size);


    class ParallelMatrixMultiplyPartial : public WSDS::Task {

    public:

        ParallelMatrixMultiplyPartial(int* out, int* A, int* B, int size, int i, int j);
        
        void execute();

    private:

        int * out;
        int * A;
        int * B;
        int size;
        
        int i; //A row operating on
        int j; //B row operating on
        
        
    };

    


}

#endif
