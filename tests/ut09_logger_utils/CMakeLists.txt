
add_executable(logger_utils9 ut09_logger_utils.cpp)
logger_set_max_level(logger_utils9 CHANNEL UT09 CONFIG "Debug:DEBUG" "Release:INFO")
target_link_libraries(logger_utils9 PRIVATE logger_test_common logger)

add_dependencies(logger_ut logger_utils9)

gtest_discover_tests(logger_utils9)
