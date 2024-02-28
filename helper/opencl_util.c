#include <stdio.h>
#include <CL/cl.h>

#include "opencl_check.h"

#include "opencl_util.h"

void printBuildLogs(const cl_device_id *devices, const int device_id, const cl_program program) {
    // If the build failed, get the build log
    cl_int local_status;
    size_t log_size;

    // Get the size of the build log
    local_status = clGetProgramBuildInfo(program, devices[device_id], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    CHECK(local_status);

    // Allocate memory for the build log
    char *build_log = malloc(log_size);
    if (!build_log) {
        printf("Failed to allocate memory for build log\n");
        exit(1);
    }

    // Get the build log
    local_status = clGetProgramBuildInfo(program, devices[device_id], CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
    if (local_status != CL_SUCCESS) {
        printf("Failed to get build log\n");
        free(build_log);
        exit(1);
    }

    // Print the build log
    printf("Build log:\n%s\n", build_log);

    // Free the allocated memory
    free(build_log);
}
