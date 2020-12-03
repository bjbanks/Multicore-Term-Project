
#include "parallelMatrix.h"
#include "parallelArray.h"

namespace BENCHMARKS {


    WSDS::Scheduler* parSchedMat;
    int work_per_subtaskm;


    void parallelMatrixInit(WSDS::Scheduler* sched, int task_work_size){
        parSchedMat = sched;
        work_per_subtaskm = task_work_size;

    }


    void parallelMatrixTranspose(int* x, int size){

        int i;
        int num_blocks_x = size / work_per_subtaskm;
        int num_blocks_y = num_blocks_x; //square matrix
        int num_blocks_total = num_blocks_x * num_blocks_y;

        int num_sub_tasks = num_blocks_total;


        ParallelMatrixTransposePartial* tasks[num_sub_tasks];

        for (i = 0; i < num_sub_tasks; i++){

            tasks[i] = new ParallelMatrixTransposePartial(x, size, work_per_subtaskm, i);

            parSchedMat->spawn(tasks[i]);
        }

        parSchedMat->wait();

        for (i = 0; i < num_sub_tasks; i++){
            delete tasks[i];
        }


    }


    ParallelMatrixTransposePartial::ParallelMatrixTransposePartial(int* x, int size, int block_width, int block_idx){

            this->x = x;
            this->size = size;
            this->block_width = block_width;
            this->block_idx = block_idx;

    }


    void ParallelMatrixTransposePartial::execute(){

        int num_blocks_x = (size / block_width);
        int num_blocks_y = num_blocks_x;
        int start_idx_x = (block_idx % num_blocks_x) * block_width;
        int start_idx_y = (block_idx / num_blocks_y) * block_width;

        int idxA, idxB, temp;

        //top corner is on half of transpose that isn't doing work
        if ((start_idx_x + block_width) > (start_idx_y + block_width)){
            return;
        }


        for( int i = start_idx_x; i < start_idx_x + block_width; i++) {
            for (int j = start_idx_y; j < start_idx_y + block_width; j++) {

                //only swap elements on one side of diagonal of matrix
                if (i <= j){

                    idxA = GET_IDX(i, j, size);
                    idxB = GET_IDX(j, i, size);

                    temp = x[idxA];
                    x[idxA] = x[idxB];
                    x[idxB] = temp;
                }

            }
        }

    }


    void parallelMatrixMultiply(int* out, int* A, int* B, int size){


        int i,j;
        int num_sub_tasks = size; //task for each i, j


        //transpose B to create sequential array
        parallelMatrixTranspose(B, size);

        ParallelMatrixMultiplyPartial* tasks[num_sub_tasks][num_sub_tasks];


        for (i = 0; i < num_sub_tasks; i++){
            for (j = 0; j < num_sub_tasks; j++){

                tasks[i][j] = new ParallelMatrixMultiplyPartial(out, A, B, size, i, j);
                parSchedMat->spawn(tasks[i][j]);
            }
        }

        parSchedMat->wait();

        for (i = 0; i < num_sub_tasks; i++){
            for (j = 0; j < num_sub_tasks; j++){
                delete tasks[i][j];
            }
        }

        //convert B back
        parallelMatrixTranspose(B, size);
    }


    ParallelMatrixMultiplyPartial::ParallelMatrixMultiplyPartial(int* out, int* A, int* B, int size, int i, int j){
        this->out = out;
        this->A = A;
        this->B = B;
        this->size = size;
        this->i = i;
        this->j = j;
    }

    void ParallelMatrixMultiplyPartial::execute(){

        //multiply the elements of the array together
        int* temp = new int[size];
        parallelMultiply(temp, &A[i*size], &B[j*size], size, this);
        out[GET_IDX(i, j, size)] = parallelReduce(temp, size, this);
        delete temp;


    }



}
