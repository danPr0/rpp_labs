cmake_minimum_required(VERSION 3.28)
project(rpp)

set(CMAKE_CXX_STANDARD 20)

find_package(MPI REQUIRED)

add_executable(merge_mpi merge_mpi.cpp)
add_executable(merge_seq merge_seq.cpp)

target_link_libraries(merge_mpi PRIVATE MPI::MPI_CXX)
