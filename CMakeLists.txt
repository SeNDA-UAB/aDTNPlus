cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME BundleAgent)
project(${PROJECT_NAME})

# Set CMake flags
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")

# Generate a static lib with the source
set(SRC_DIR "BundleAgent")
include_directories("${SRC_DIR}")
file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.cpp")
set (LIB_NAME "BundleAgent")
add_library ("${LIB_NAME}" STATIC ${SRC_FILES})

#Set the path of gtest
set(GTEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Test/gtest-1.7.0")
add_subdirectory(${GTEST_DIR})

#Enable gtest Testing
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

#Get the test files
set(TEST_EXEC "${LIB_NAME}_test")
set(TEST_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Test/BundleAgentTest")
file(GLOB_RECURSE TEST_SRC_FILES "${TEST_SRC_DIR}/*.cpp")

#Create an executable with the tests
add_executable(${TEST_EXEC} ${TEST_SRC_FILES})
target_link_libraries(${TEST_EXEC} ${LIB_NAME} gtest_main gtest)

#Add the executable as a test
add_test(BundleTest "${TEST_EXEC}")