cmake_minimum_required(VERSION 3.15)
project(calculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(src)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gtest/CMakeLists.txt")
    message(STATUS "Using local gtest")
    add_subdirectory(gtest)
else()
    message(STATUS "Building gtest from source")
    add_library(gtest STATIC gtest/qtest-all.cc)
    target_include_directories(gtest PUBLIC gtest)

    add_library(gtest_main STATIC gtest/qtest-all.cc)
    target_link_libraries(gtest_main PUBLIC gtest)
endif()

add_executable(calculator_tests
        tests/main.cpp
        tests/test_pomidorki.cpp
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(calculator_tests PRIVATE
            -fsanitize=address
            -fsanitize=undefined
            -fsanitize=leak
    )
    target_link_libraries(calculator_tests PRIVATE
            -fsanitize=address
            -fsanitize=undefined
            -fsanitize=leak
    )
endif()

if(TARGET gtest_main)
    target_link_libraries(calculator_tests PRIVATE gtest_main)
else()
    target_link_libraries(calculator_tests PRIVATE gtest)
endif()

enable_testing()
add_test(NAME CalculatorTests COMMAND calculator_tests)

option(BUILD_CALCULATOR "Build standalone calculator" OFF)
if(BUILD_CALCULATOR)
    add_executable(calculator
            tests/main.cpp
    )
    target_include_directories(calculator PRIVATE src)
endif()