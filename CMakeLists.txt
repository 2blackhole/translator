cmake_minimum_required(VERSION 3.15)
project(calculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(src)

set(GTEST_TARGET_MAIN "gtest_main")
set(GTEST_TARGET "gtest")

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gtest/CMakeLists.txt")
    message(STATUS "Using local gtest")
    add_subdirectory(gtest)
else()
    message(STATUS "Downloading gtest with FetchContent")

    include(FetchContent)

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
    )

    if(WIN32)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()

    FetchContent_MakeAvailable(googletest)

    set(GTEST_TARGET_MAIN "GTest::gtest_main")
    set(GTEST_TARGET "GTest::gtest")
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

if(TARGET ${GTEST_TARGET_MAIN})
    message(STATUS "Linking with ${GTEST_TARGET_MAIN}")
    target_link_libraries(calculator_tests PRIVATE ${GTEST_TARGET_MAIN})
elseif(TARGET ${GTEST_TARGET})
    message(STATUS "Linking with ${GTEST_TARGET}")
    target_link_libraries(calculator_tests PRIVATE ${GTEST_TARGET})
else()
    message(FATAL_ERROR "gtest targets not found. Expected ${GTEST_TARGET_MAIN} or ${GTEST_TARGET}")
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