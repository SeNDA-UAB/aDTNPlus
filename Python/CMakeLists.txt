cmake_minimum_required(VERSION 2.8.12)

project(aDTNPlus)

add_subdirectory(pybind11)
include_directories(../Lib)
include_directories(../BundleAgent)
pybind11_add_module(aDTNPlus aDTNPlusPython.cpp)
target_link_libraries(aDTNPlus PRIVATE aDTNLib Bundle_lib)