cmake_minimum_required(VERSION 3.8)
project(magnetic_force LANGUAGES CXX CUDA)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CUDA_STANDARD 11)

find_package(OpenMP REQUIRED)
if(OPENMP_CXX_FOUND OR OPENMP_FOUND)
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Xcompiler ${OpenMP_CXX_FLAGS}")
endif(OPENMP_CXX_FOUND OR OPENMP_FOUND)


include_directories(fmmtl_minimum glm)
add_library(magnetic_solver SHARED
    magnetic_solver/ferro.cu 
    magnetic_solver/ferro.cuh
    magnetic_solver/magnetic_force.cu
    magnetic_solver/cuda_abstract_vector.cuh
)

target_link_libraries(magnetic_solver ${OpenMP_CXX_LIBRARIES})