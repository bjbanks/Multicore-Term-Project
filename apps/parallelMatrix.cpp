
#include "parallelMatrix.h"
#include "parallelArray.h"

#define WORK_PER_SUBTASK (1 << 10)

namespace BENCHMARKS {


    WSDS::Scheduler* parSched;

    void parallelMatrixInit(WSDS::Scheduler* sched){
        parSched = sched;

    }


    void parallelMatrixTranspose(int** x, int size){

        int num_blocks_x = size / WORK_PER_SUBTASK;
        int num_blocks_y = num_blocks_x; //square matrix
        int num_blocks_total = num_blocks_x * num_blocks_y;

        int num_sub_tasks = num_blocks_total;


        ParallelMatrixTranspostPartial* tasks[num_sub_tasks];

        for (int i = 0; i < num_sub_tasks; i++){

            tasks[i] = new ParallelMatrixTransposePartial(x, size, WORK_PER_SUBTASK, i);

            parSched->spawn(tasks[i]);
        }

        parSched->wait();

        for (i = 0; i < num_sub_tasks; i++){
            delete tasks[i];
        }


    }


    ParallelMatrixTransposePartial::ParallelMatrixTransposePartial(int** x, int size, int block_width, int block_idx){

            this->x = x;
            this->size = size;
            this->block_width = width;
            this->block_idx = block_idx;

    }


    ParallelMatrixTransposePartial::execute(){

        int num_blocks_x = (size / block_width);
        int num_blocks_y = num_blocks_x;
        int start_idx_x = (block_idx % num_blocks_x) * block_width;
        int start_idx_y = (block_idx / num_blocks_y) * block_width;

        int idx_half = size / 2;

        //top corner is on half of transpose that isn't doing work
        if ((start_idx_x + block_width) > (start_idx_y + block_width)){
            return;
        }


        for( int i = start_idx_x; i < start_idx_x + block_width; i++) {
            for (int j = start_idx_y; j < start_idx_y + block_width; j++) {

                //only swap elements on one size of diagonal of matrix
                if (i <= j){
                    temp = block_idx[i][j];
                    block_idx[i][j] = block_idx[j][i];
                    block_idx[j][i] = temp;
                }
                
            }
        }                          

    }


    void parallelMatrixMultiply(int** out, int** A, int** B, int size){


        int num_sub_tasks = size; //task for each i, j

        //transpose B to create sequential array
        parallelMatrixTranspose(B, size);

        ParallelMatrixTranspostPartial* tasks[num_sub_tasks][num_sub_tasks];

        for (int i = 0; i < num_sub_tasks; i++){
            for (int j = 0; j < num_sub_tasks; j++){

                tasks[i][j] = new ParallelMatrixMultiplyPartial(out, A, B, size, i, j);

                parSched->spawn(tasks[i][j]);
            }
        }

        parSched->wait();

        for (i = 0; i < num_sub_tasks; i++){
            for (j = 0; j < num_sub_tasks; j++){
                delete tasks[i][j];
            }
        }

        //convert B back
        parallelMatrixTranspose(B, size);
    }


    void ParallelMatrixMultiplyPartial::ParallelMatrixMultiplyPartial(int** out, int** A, int** B, int size, int i, int j){
        this->out = out;
        this->A = A;
        this->B = B;
        this->size = size;
        this->i = i;
        this->j = j;
    }

    void ParallelMatrixMultiplyPartial::execute(){

        //multiply the elements of the array together
        int temp[size];
        parallelMultiply(temp, &A[i], &B[j], size);
        parallelReduce(temp, size);
        out[i][j] = temp[0];


    }

    
     
}
