cmake_minimum_required(VERSION 2.8.12)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # require at least gcc 4.9
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
    message(FATAL_ERROR "GCC version must be at least 4.9")
  endif()
else()
  message(WARNING "You are using an unsupported compiler. Compilation has only been tested with CGG.")
endif()
  
enable_testing()

set(CMAKE_CXX_STANDARD 14)
option(TOOLS "Build the given tools." ON)
option(TESTS "Build test program." OFF)
option(PYTHON "Build the python library." ON) 

set(CMAKE_CXX_FLAGS_PROFILE ${CMAKE_CXX_FLAGS} "-ggdb -Wall -std=c++14 -lpthread --coverage")
add_compile_options(-std=c++14)

add_subdirectory(BundleAgent)
add_subdirectory(Lib)
if(TOOLS)
  add_subdirectory(Tools)
endif(TOOLS)
if(TESTS)
  add_subdirectory(Test)
endif(TESTS)
if(PYTHON)
  add_subdirectory(Python)
endif(PYTHON)
