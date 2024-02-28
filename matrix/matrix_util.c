#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <CL/cl.h>

#include "matrix_util.h"

void matrixGen(const cl_int size, cl_double *matrix1, cl_double *matrix2) {
    if (matrix1 == NULL || matrix2 == NULL) {
        puts("matrixGen(): you must initialize matrices first.");
        exit(1);
    }

    for (int i = 0; i < size * size; i++)
        matrix1[i] = ((double) rand()) / 65535.0;

    for (int i = 0; i < size * size; i++)
        matrix2[i] = ((double) rand()) / 5307.0;
}

void matrixGenAndWriteToFile(const char *filename, const cl_int rows, const cl_int cols) {
    double lower_bound = -1000;
    double upper_bound = 1000;

    // Seed the random number generator
    srand(time(NULL));

    // Open the file for writing
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Cannot open file %s\n", filename);
        exit(1);
    }

    // Generate the matrix and write it to the file
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            double value = lower_bound + ((double) rand()) / RAND_MAX * (upper_bound - lower_bound); // This will generate a number between lower_bound and upper_bound
            fprintf(file, "%f ", value);
        }
        fprintf(file, "\n");
    }

    // Close the file
    fclose(file);
}

void readMatrixFromFile(const char *filename, cl_double *matrix, const int size) {
    srand(time(NULL));

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Cannot open file %s\n", filename);
        exit(1);
    }

    for (int i = 0; i < size * size; i++) {
        if (fscanf(file, "%lf", &matrix[i]) != 1) {
            printf("Failed to read number at position %d\n", i);
            exit(1);
        }
    }

    fclose(file);
}