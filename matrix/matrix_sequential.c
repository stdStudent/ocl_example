#include <CL/cl.h>

#include "matrix_sequential.h"

void matrixMultSequential(
    const int size,
    const cl_double *matrix1_in,
    const cl_double *matrix2_in,
    cl_double *matrix_out
) {
    int rows, cols;
    int j;

    // For cycle added to implement a sequential, matrix by matrix,
    // multiplication that repeats the code given for the vector by matrix multipication
    // for every row of the first operand's matrix
    for (rows = 0; rows < size; rows++) {
        for (cols = 0; cols < size; cols++) {
            matrix_out[rows * size + cols] = 0.0;
            for (j = 0; j < size; j++)
                matrix_out[rows * size + cols] += matrix1_in[rows * size + j] * matrix2_in[j * size + cols];
        }
    }
}
