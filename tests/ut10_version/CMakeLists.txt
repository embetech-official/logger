add_executable(logger_version10 ut10_version.cpp)
target_compile_definitions(logger_version10 PRIVATE PROJECT_DIR="${PROJECT_SOURCE_DIR}")
target_link_libraries(logger_version10 PRIVATE logger_test_common logger)

add_dependencies(logger_ut logger_version10)

gtest_discover_tests(logger_version10)
