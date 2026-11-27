cmake_minimum_required(VERSION 3.16)
project(gps_log_checker CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The NMEA code lives in a small library so both the program and the tests can use it
add_library(nmea src/nmea.cpp)
target_include_directories(nmea PUBLIC src)

add_executable(gps_log_checker src/main.cpp)
target_link_libraries(gps_log_checker PRIVATE nmea)

# Tests: download Catch2 once, then build the test program
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.4
)
FetchContent_MakeAvailable(Catch2)

add_executable(nmea_tests tests/test_nmea.cpp)
target_link_libraries(nmea_tests PRIVATE nmea Catch2::Catch2WithMain)

enable_testing()
add_test(NAME nmea_tests COMMAND nmea_tests)
