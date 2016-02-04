cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME BundleAgent)
project(${PROJECT_NAME} C CXX)

# Set CMake flags
set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=c++11 -lpthread --coverage")

# Generate a static lib with the source
set(SRC_DIR "BundleAgent")
set(TOOL_DIR "Tools")
set(LIB_DIR "Lib")
include_directories("${SRC_DIR}")
file(GLOB_RECURSE SRC_FILES_CPP "${SRC_DIR}/Bundle/*.cpp" "${SRC_DIR}/ExternTools/*.cpp" "${SRC_DIR}/Node/*.cpp" "${SRC_DIR}/Utils/*.cpp")
file(GLOB_RECURSE SRC_FILES_C "${SRC_DIR}/ExternTools/*.c")
file(GLOB MAIN_FILE "${SRC_DIR}/*.cpp")
set(EXEC_NAME "BundleAgent")
#Create program executable
set(EXEC_FILES "${SRC_FILES_CPP}" "${SRC_FILES_C}" "${MAIN_FILE}")
add_executable(${EXEC_NAME} ${EXEC_FILES})
find_package(Threads)
target_link_libraries(${EXEC_NAME} ${CMAKE_THREAD_LIBS_INIT})

# Exclude main for static library
#list(REMOVE_ITEM SRC_FILES_CPP "${SRC_DIR}/main.cpp")
set (LIB_NAME "${EXEC_NAME}_lib")
set (SRC_FILES "${SRC_FILES_CPP}" "${SRC_FILES_C}")
add_library ("${LIB_NAME}" STATIC ${SRC_FILES} )

#Set the path of gtest
set(GTEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Test/gtest-1.7.0")
add_subdirectory(${GTEST_DIR})

#Enable gtest Testing
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

#Get the test files
set(TEST_EXEC "${EXEC_NAME}_test")
set(TEST_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Test/BundleAgent")
file(GLOB_RECURSE TEST_SRC_FILES "${TEST_SRC_DIR}/*.cpp")

#Create an executable with the tests
add_executable(${TEST_EXEC} ${TEST_SRC_FILES})
target_link_libraries(${TEST_EXEC} ${LIB_NAME} gtest)

#Add the executable as a test
add_test(BundleTest "${TEST_EXEC}")

include_directories(${LIB_DIR})
file(GLOB_RECURSE SRC_FILES1_CPP "${SRC_DIR}/Node/BundleQueue/BundleContainer.cpp" "${SRC_DIR}/Bundle/*.cpp" "${SRC_DIR}/ExternTools/*.cpp" "${SRC_DIR}/Utils/*.cpp" "${LIB_DIR}/*.cpp")
file(GLOB MAIN1_FILE "${TOOL_DIR}/bundleContainerViewer.cpp")
set(BV_NAME "BCViewer")
set(BV_FILES "${SRC_FILES1_CPP}" "${SRC_FILES_C}" "${MAIN1_FILE}")
add_executable(${BV_NAME} ${BV_FILES})
set(BSD_NAME "basicSender")
file(GLOB MAIN_BSD_FILE "${TOOL_DIR}/basicSender.cpp")
set(BRV_NAME "basicRecv")
file(GLOB MAIN_BRV_FILE "${TOOL_DIR}/basicReceiver.cpp")
add_executable(${BSD_NAME} ${SRC_FILES1_CPP} ${SRC_FILES_C} ${MAIN_BSD_FILE})
add_executable(${BRV_NAME} ${SRC_FILES1_CPP} ${SRC_FILES_C} ${MAIN_BRV_FILE})