cmake_minimum_required(VERSION 2.6)

set(BINARY ${CMAKE_PROJECT_NAME}_test)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(../src)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)

# Link runTests with what we want to test and the GTest and pthread library
enable_testing()
add_executable(${BINARY} ${TEST_SOURCES})
# add_test(NAME ${BINARY} COMMAND ${BINARY})
target_link_libraries(${BINARY} ${GTEST_LIBRARIES} pthread)
gtest_discover_tests(${BINARY})