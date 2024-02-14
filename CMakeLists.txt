cmake_minimum_required(VERSION 3.28)
project(ocl_example C)

set(CMAKE_C_STANDARD 17)

add_executable(ocl_example main.c)

# Set the include directories (-I "C:/OCLSDK/include/")
target_include_directories(ocl_example PRIVATE "C:/OCLSDK/include/")

# Specify the link directories (-L "C:/OCLSDK/lib/x86_64/")
link_directories("C:/OCLSDK/lib/x86_64/")

# Link the OpenCL library (-lOpenCL)
target_link_libraries(ocl_example "C:/OCLSDK/lib/x86_64/OpenCL.lib")
