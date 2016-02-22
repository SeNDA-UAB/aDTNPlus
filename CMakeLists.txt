cmake_minimum_required(VERSION 2.8.12)

enable_testing()

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS_PROFILE ${CMAKE_CXX_FLAGS} "-ggdb -Wall -std=c++11 -lpthread --coverage")
add_compile_options(-std=c++11)

add_subdirectory(BundleAgent)
add_subdirectory(Lib)
add_subdirectory(Tools)
add_subdirectory(Test)