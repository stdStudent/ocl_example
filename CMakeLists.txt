cmake_minimum_required(VERSION 3.28)
project(ocl_example C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")

add_executable(ocl_example main.c
        helper/read_file.c
        helper/read_file.h
        helper/timestamp.c
        helper/timestamp.h
        helper/opencl_check.h
        matrix/matrix_comparison.c
        matrix/matrix_comparison.h
        matrix/matrix_sequential.c
        matrix/matrix_sequential.h
        helper/opencl_util.c
        helper/opencl_util.h)


# The OpenCL SDK is downloaded from here:
# https://github.com/GPUOpen-LibrariesAndSDKs/OCL-SDK/releases/latest

# Set the include directories (-I "C:/OCLSDK/include/")
target_include_directories(ocl_example PRIVATE "C:/OCLSDK/include/")

# Specify the link directories (-L "C:/OCLSDK/lib/x86_64/")
link_directories("C:/OCLSDK/lib/x86_64/")

# Link the OpenCL library (-lOpenCL)
target_link_libraries(ocl_example "C:/OCLSDK/lib/x86_64/OpenCL.lib")

# Link libm for math.h (-lm)
target_link_libraries(ocl_example m)
