#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

int main() {
    // Get the number of platforms
    cl_uint numPlatforms;
    clGetPlatformIDs(0, NULL, &numPlatforms);

    if (numPlatforms ==  0) {
        printf("No OpenCL platforms found.\n");
        return  1;
    }

    // Get the platform IDs
    cl_platform_id* platforms = malloc(sizeof(cl_platform_id) * numPlatforms);
    clGetPlatformIDs(numPlatforms, platforms, NULL);

    // Print the platform names
    for (cl_uint i =  0; i < numPlatforms; ++i) {
        char buffer[1024];
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(buffer), buffer, NULL);
        printf("Platform %d: %s\n", i +  1, buffer);
    }

    free(platforms);
    return  0;
}
