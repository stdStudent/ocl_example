#include <stdio.h>
#include <stdlib.h>

#include <CL/cl.h>

#include "../helper/opencl_check.h"
#include "../helper/opencl_util.h"
#include "../helper/read_file.h"
#include "../helper/timestamp.h"

#include "matrix_util.h"
#include "matrix_sequential.h"
#include "matrix_comparison.h"

size_t getOptimalLocalSize(const size_t size, const size_t preffered_local_size, const size_t max_local_size) {
    if (size % preffered_local_size == 0) {
        return preffered_local_size;
    }

    for (size_t i = preffered_local_size; i < max_local_size; ++i) {
        if (size % i == 0) {
            return i;
        }
    }

    for (size_t i = 1; i < preffered_local_size; ++i) {
        if (size % i == 0) {
            return i;
        }
    }

    // If no suitable local work size is found, return 1 as local size
    return 1;
}

void matrixComparison(const cl_int size, cl_int localSize) {
    if (size <= 0) {
        printf("incorrect argument (size: %d) must greater than zero\n", size);
        exit(-1);
    }

    if (localSize > 0 && size % localSize != 0) {
        printf("size should be a multiple of localSize\n");
        exit(-1);
    }

    // Data structures allocated to store the operand and resulting matrixes of size "size*size"
    cl_double *matrix1 = malloc(sizeof(cl_double) * size * size);
    cl_double *matrix2 = malloc(sizeof(cl_double) * size * size);
    cl_double *result_sq = malloc(sizeof(cl_double) * size * size);
    cl_double *result_pl = malloc(sizeof(cl_double) * size * size);

    //matrixGen(size, matrix1, matrix2);
    matrixGenAndWriteToFile("../input/matrix1.txt", size, size);
    matrixGenAndWriteToFile("../input/matrix2.txt", size, size);
    readMatrixFromFile("../input/matrix1.txt", matrix1, size);
    readMatrixFromFile("../input/matrix2.txt", matrix2, size);

    // Variables used to individually calculate the inititalization, copy and compilation times (that form the overhead) and the kernel runtime
    double time_opencl, time_opencl_init, time_opencl_comp, time_opencl_cpy;
    double time1, time2, time3, time4;

    cl_event mulDone;

    double time_sq = getTimestamp();
    matrixMultSequential(size, matrix1, matrix2, result_sq);
    time_sq = getTimestamp() - time_sq;

    cl_int status;
    time_opencl = getTimestamp();

    //-----------------------------------------------------
    // STEP 1: Discover and initialize the platforms
    //-----------------------------------------------------
    cl_uint numPlatforms = 0;
    cl_platform_id *platforms = NULL;

    // Retrieve the number of platforms
    status = clGetPlatformIDs(0, NULL, &numPlatforms);
    CHECK(status);

    // Allocate enough space for each platform
    platforms = (cl_platform_id *) malloc(numPlatforms * sizeof(cl_platform_id));

    // Fill in platforms with clGetPlatformIDs()
    status = clGetPlatformIDs(numPlatforms, platforms, NULL);
    CHECK(status);

    //-----------------------------------------------------
    // STEP 2: Discover and initialize the devices
    //-----------------------------------------------------
    cl_uint numDevices = 0;
    cl_device_id *devices = NULL;

    // Use clGetDeviceIDs() to retrieve the number of devices present
    status = clGetDeviceIDs(
        platforms[0],
        CL_DEVICE_TYPE_GPU,
        0,
        NULL,
        &numDevices
    );
    CHECK(status);

    // Allocate enough space for each device
    devices = (cl_device_id *) malloc(numDevices * sizeof(cl_device_id));

    // Fill in devices with clGetDeviceIDs()
    status |= clGetDeviceIDs(
        platforms[0],
        CL_DEVICE_TYPE_GPU,
        numDevices,
        devices,
        NULL
    );
    CHECK(status);

    // Select the device which will be used
    const int device_id = 0;

    //-----------------------------------------------------
    // STEP 3: Create a context
    //-----------------------------------------------------
    cl_context context = NULL;

    // Create a context using clCreateContext() and
    // associate it with the devices
    context = clCreateContext(
        NULL,
        1,
        &devices[device_id],
        NULL,
        NULL,
        &status
    );
    CHECK(status);

    //-----------------------------------------------------
    // STEP 4: Create a command queue
    //-----------------------------------------------------
    cl_command_queue cmdQueue;

    // Create a command queue using clCreateCommandQueue(),
    // and associate it with the device you want to execute on
    cmdQueue = clCreateCommandQueue(
        context,
        devices[device_id],
        0,
        &status
    );
    CHECK(status);


    //-----------------------------------------------------
    // STEP 5: Create device buffers and copy data to them
    //-----------------------------------------------------
    cl_mem bufferMatrixIn1, bufferMatrixIn2, bufferMatrixOut;

    bufferMatrixIn1 = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY,
        size * size * sizeof(cl_double),
        NULL,
        &status
    );
    CHECK(status);

    bufferMatrixIn2 = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY,
        size * size * sizeof(cl_double),
        NULL,
        &status
    );
    CHECK(status);

    bufferMatrixOut = clCreateBuffer(
        context,
        CL_MEM_WRITE_ONLY,
        size * size * sizeof(cl_double),
        NULL,
        &status
    );
    CHECK(status);

    // Timer to calculate the initialization time
    time_opencl_init = getTimestamp();

    status = clEnqueueWriteBuffer(
        cmdQueue,
        bufferMatrixIn1,
        CL_FALSE,
        0,
        size * size * sizeof(cl_double),
        matrix1,
        0,
        NULL,
        NULL
    );
    CHECK(status);

    status |= clEnqueueWriteBuffer(
        cmdQueue,
        bufferMatrixIn2,
        CL_FALSE,
        0,
        size * size * sizeof(cl_double),
        matrix2,
        0,
        NULL,
        NULL
    );
    CHECK(status);

    //-----------------------------------------------------
    // STEP 6: Create and compile the program
    //-----------------------------------------------------
    unsigned char *programBuffer;
    size_t programSize;
    const char *filename = "../matrix/matrix_parallel.cl";

    if (read_file(&programBuffer, &programSize, filename) != 0) {
        printf("Error reading file: %s\n", filename);
    }

    // Timer to calculate the copytime
    time_opencl_cpy = getTimestamp();

    const cl_program program = clCreateProgramWithSource(
        context,
        1,
        (const char **) &programBuffer,
        &programSize,
        &status
    );
    CHECK(status);

    free(programBuffer);

    // Build (compile) the program for the devices
    const char options[] = "-cl-std=CL1.2";
    status |= clBuildProgram(
        program,
        1,
        &devices[device_id],
        options,
        NULL,
        NULL
    );

    if (status != CL_SUCCESS) {
        printBuildLogs(devices, device_id, program);
        exit(1);
    }

    //-----------------------------------------------------
    // STEP 7: Create the kernel
    //-----------------------------------------------------
    const cl_kernel mulKernel = clCreateKernel(
        program,
        "mul_kernel",
        &status
    );
    CHECK(status);

    //-----------------------------------------------------
    // STEP 8: Set the kernel arguments
    //-----------------------------------------------------

    // Associate the input and output buffers with the kernel
    status = clSetKernelArg(
        mulKernel,
        0,
        sizeof(cl_mem),
        &bufferMatrixIn1
    );
    CHECK(status);

    status |= clSetKernelArg(
        mulKernel,
        1,
        sizeof(cl_mem),
        &bufferMatrixIn2
    );
    CHECK(status);

    status |= clSetKernelArg(
        mulKernel,
        2,
        sizeof(cl_mem),
        &bufferMatrixOut
    );
    CHECK(status);

    status |= clSetKernelArg(
        mulKernel,
        3,
        sizeof(cl_int),
        &size
    );
    CHECK(status);

    //-----------------------------------------------------
    // OPTIONAL STEP: calculate optimal local size
    //-----------------------------------------------------
    if (localSize <= 0) {
        size_t preffered_local_size;
        size_t max_local_size;
        cl_int err;

        // Get the preffered work-group size for the kernel on the device
        err = clGetKernelWorkGroupInfo(
            mulKernel,
            devices[device_id],
            CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,
            sizeof(size_t),
            &preffered_local_size,
            NULL
        );
        CHECK(err);

        // Get the maximum work-group size for the kernel on the device
        err = clGetDeviceInfo(
            devices[device_id],
            CL_DEVICE_MAX_WORK_GROUP_SIZE,
            sizeof(size_t),
            &max_local_size, NULL
        );
        CHECK(err);

        localSize = getOptimalLocalSize(size, preffered_local_size, max_local_size);
    }

    //-----------------------------------------------------
    // STEP 9: Configure the work-item structure
    //-----------------------------------------------------
    // Define an index space (global work size) of work items for
    // execution. A workgroup size (local work size) is not required,
    // but outgh to be used for optimal performace.

    size_t globalWorkSize[1];
    globalWorkSize[0] = size * size;

    size_t localWorkSize[1];
    localWorkSize[0] = localSize;

    // Timer to calculate the computation time
    time_opencl_comp = getTimestamp();

    status |= clEnqueueNDRangeKernel(
        cmdQueue,
        mulKernel,
        1,
        NULL,
        globalWorkSize,
        localWorkSize,
        0,
        NULL,
        &mulDone
    );

    if (status != CL_SUCCESS) {
        clWaitForEvents(1, &mulDone);
        CHECK(status);
    }

    clEnqueueReadBuffer(
        cmdQueue,
        bufferMatrixOut,
        CL_TRUE,
        0,
        size * size * sizeof(cl_double),
        result_pl,
        1,
        &mulDone,
        NULL
    );
    CHECK(status);

    // Computation time
    time1 = getTimestamp() - time_opencl_comp;

    // Inicialization time
    time2 = time_opencl_init - time_opencl;

    // Copytime
    time3 = time_opencl_cpy - time_opencl_init;

    // Compilation time
    time4 = time_opencl_comp - time_opencl_cpy;

    printf("Sequential execution: %f (sec)\n", time_sq / 1000000);
    printf("Parallel execution (local work group size: %d): %f (sec)\n"
           "Kernel runtime: %f (sec).\n"
           "Overhead %f (sec):"
           "\n\tInit time: %f (sec),"
           "\n\tCopy time: %f (sec),"
           "\n\tCompilation time: %f (sec)\n",
           localSize,
           (time1 + time2 + time3 + time4) / 1000000,
           time1 / 1000000,
           (time2 + time3 + time4) / 1000000,
           time2 / 1000000,
           time3 / 1000000,
           time4 / 1000000
    );

    // Check if the matrices are identical
    for (int i = 0; i < size; i++) {
        if ((int) result_sq[i] != (int) result_pl[i]) {
            printf("wrong at position %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    //-----------------------------------------------------
    // STEP 10: Release OpenCL resources
    //-----------------------------------------------------

    // Free OpenCL resources
    clReleaseKernel(mulKernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(cmdQueue);
    clReleaseMemObject(bufferMatrixIn1);
    clReleaseMemObject(bufferMatrixIn2);
    clReleaseMemObject(bufferMatrixOut);
    clReleaseContext(context);

    // Free up memory and close files
    free(matrix1);
    free(matrix2);
    free(result_sq);
    free(result_pl);
}
