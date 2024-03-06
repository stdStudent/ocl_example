#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#include "../helper/read_file.h"

#include "vector_sum.h"

#include <time.h>

static void print_vector(const char *name, double *vec, int size) {
    printf("%s: [", name);
    for (int i = 0; i < size; ++i) {
        printf("%f", vec[i]);
        if (i < size - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}


void vector_sum(int size) {
    // 1. Load the kernel code
    unsigned char *source_str;
    size_t source_size;

    if (read_file(&source_str, &source_size, "../vector/vector_sum.cl") != 0) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }

    // Generate input vectors with random values
    srand(time(NULL));
    double *vector1_in = (double*)malloc(sizeof(double)*size);
    double *vector2_in = (double*)malloc(sizeof(double)*size);
    for (int i = 0; i < size; ++i) {
        vector1_in[i] = (double)rand() / RAND_MAX;
        vector2_in[i] = (double)rand() / RAND_MAX;
    }

    // 2. Get platform and device information
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

    // 3. Create an OpenCL context
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    // 4. Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    // 5. Create memory buffers on the device for each vector
    cl_mem vec1_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, size * sizeof(double), NULL, &ret);
    cl_mem vec2_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, size * sizeof(double), NULL, &ret);
    cl_mem vec3_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size * sizeof(double), NULL, &ret);

    // Write the data from vector1_in and vector2_in to their respective device memory buffers
    ret = clEnqueueWriteBuffer(command_queue, vec1_mem_obj, CL_TRUE, 0, size * sizeof(double), vector1_in, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, vec2_mem_obj, CL_TRUE, 0, size * sizeof(double), vector2_in, 0, NULL, NULL);

    // 6. Create a program from the kernel source and clear no longer needed OpenCL source buffer
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
    free(source_str);

    // 7. Build the program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    // 8. Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "vec_sum_kernel", &ret);

    // 9. Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&vec1_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&vec2_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&vec3_mem_obj);
    ret = clSetKernelArg(kernel, 3, sizeof(cl_int), (void *)&size);

    // 10. Execute the OpenCL kernel on the list
    size_t global_item_size = size; // Process the entire lists
    size_t local_item_size = 1; // Divide work items into groups of 64
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

    // 11. Read the memory buffer vec3 on the device to the local variable vec3
    double *vec3 = (double*)malloc(sizeof(double)*size);
    ret = clEnqueueReadBuffer(command_queue, vec3_mem_obj, CL_TRUE, 0, size * sizeof(double), vec3, 0, NULL, NULL);

    // Print the input vectors and the result vector
    print_vector("Vector 1", vector1_in, size);
    print_vector("Vector 2", vector2_in, size);
    print_vector("Result", vec3, size);

    // 12. Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(vec1_mem_obj);
    ret = clReleaseMemObject(vec2_mem_obj);
    ret = clReleaseMemObject(vec3_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(vec3);
}
