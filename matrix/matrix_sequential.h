#ifndef MATRIX_SEQUENTIAL_H
#define MATRIX_SEQUENTIAL_H

#include <CL/cl.h>

void matrixMultSequential(const int size,const cl_double *matrix1_in,const cl_double *matrix2_in,cl_double *matrix_out);

#endif //MATRIX_SEQUENTIAL_H
