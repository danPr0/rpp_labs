cmake_minimum_required(VERSION 3.28)
project(rpp)

set(CMAKE_CXX_STANDARD 20)

# added -fopenmp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fopenmp")


find_package(MPI REQUIRED)
#find_package(OpenMP REQUIRED)

add_executable(merge_seq merge_seq.cpp)
add_executable(merge_mpi merge_mpi.cpp)
add_executable(merge_openmp merge_openmp.cpp)

target_link_libraries(merge_mpi PRIVATE MPI::MPI_CXX)
#target_link_libraries(merge_openmp PRIVATE OpenMP::OpenMP_C)
