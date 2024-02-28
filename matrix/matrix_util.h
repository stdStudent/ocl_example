#ifndef MATRIX_UTIL_H
#define MATRIX_UTIL_H


void matrixGen(const cl_int size, cl_double *matrix1, cl_double *matrix2);
void matrixGenAndWriteToFile(const char *filename, const cl_int rows, const cl_int cols);
void readMatrixFromFile(const char *filename, cl_double *matrix, const int size);


#endif //MATRIX_UTIL_H
