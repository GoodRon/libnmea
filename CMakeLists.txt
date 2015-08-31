cmake_minimum_required(VERSION 3.1)
project(libnmea)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# GTest
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
set(GTEST_RESULT_ROOT ${CMAKE_CURRENT_BINARY_DIR}/Testing)
include(CMake/gtest_helper.cmake)
add_custom_target(libnmea_tests_all COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

include_directories(.)
add_subdirectory(libtokens)
include_directories(libtokens)

add_subdirectory(gtests)

set(SOURCE_FILES
    gps.h
    nmea.cxx
    nmea.h)

add_library(nmea ${SOURCE_FILES})
target_link_libraries(nmea tokens)