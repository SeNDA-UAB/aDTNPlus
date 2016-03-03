set(GTEST_DIR gtest-1.7.0)
add_subdirectory(${GTEST_DIR})
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(../BundleAgent)
set(TEST_EXEC BundleAgent_test)

file(GLOB_RECURSE TEST_SRC_FILES "BundleAgent/*.cpp")

add_executable(${TEST_EXEC} ${TEST_SRC_FILES})
target_link_libraries(${TEST_EXEC} BundleAgent_lib ${CMAKE_DL_LIBS} gtest)

add_test(BundleTtest ${TEST_EXEC})
