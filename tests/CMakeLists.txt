cmake_minimum_required(VERSION 3.24)

enable_language(CXX)

include(FetchContent)
include(GoogleTest)

FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/main.zip FIND_PACKAGE_ARGS NAMES GTest)
FetchContent_MakeAvailable(googletest)

# Exclude googletest from all to make use of install target
if (IS_DIRECTORY "${googletest_SOURCE_DIR}")
  set_property(DIRECTORY ${googletest_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif ()


add_library(logger_test_common INTERFACE)
target_compile_features(logger_test_common INTERFACE cxx_std_20)
target_compile_options(logger_test_common INTERFACE -Wall -Wextra -Wconversion -pedantic )
target_link_libraries(logger_test_common INTERFACE GTest::gtest_main)
target_link_options(logger_test_common INTERFACE -pthread)


add_subdirectory(ut01_mvp)
add_subdirectory(ut02_timestamps)
add_subdirectory(ut03_affixes)
add_subdirectory(ut04_runtime_verbosity)
add_subdirectory(ut05_location)
add_subdirectory(ut06_thread_safety)
add_subdirectory(ut07_flush_hooks)
add_subdirectory(ut08_mixed)
