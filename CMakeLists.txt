
cmake_minimum_required(VERSION 3.20)

project(CampusCare)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

include(GoogleTest)

# ==========================================
# UNIT TESTING
# ==========================================

add_executable(
    unit_tests
    tests/unitTest.cpp
)

target_link_libraries(
    unit_tests
    PRIVATE
    GTest::gtest_main
)

gtest_discover_tests(
    unit_tests
    DISCOVERY_MODE PRE_TEST
)

# ==========================================
# MOCK TESTING
# ==========================================

add_executable(
    CampusCareTests
    tests/CampusCareTests.cpp
)

target_link_libraries(
    CampusCareTests
    PRIVATE
    GTest::gtest_main
    GTest::gmock
)

gtest_discover_tests(
    CampusCareTests
    DISCOVERY_MODE PRE_TEST
)
