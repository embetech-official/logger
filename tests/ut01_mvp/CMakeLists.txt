get_target_property(LOGGER_SOURCE_DIR logger SOURCE_DIR)

get_target_property(LOGGER_SOURCES logger SOURCES)
get_target_property(LOGGER_INCLUDES logger INCLUDE_DIRECTORIES)

list(TRANSFORM LOGGER_SOURCES PREPEND "${PROJECT_SOURCE_DIR}/src/")

add_library(logger_test_proxy1 ${LOGGER_SOURCES})
target_include_directories(logger_test_proxy1 PUBLIC ${LOGGER_INCLUDES})

add_executable(logger_ut01 ut01_mvp.c ut01_mvp.cpp)
target_link_libraries(logger_ut01 PRIVATE logger_test_proxy1 logger_test_common)

add_dependencies(logger_ut logger_ut01)

gtest_discover_tests(logger_ut01)
