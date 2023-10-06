get_target_property(LOGGER_SOURCE_DIR logger SOURCE_DIR)

get_target_property(LOGGER_SOURCES logger SOURCES)
get_target_property(LOGGER_INCLUDES logger INCLUDE_DIRECTORIES)

list(TRANSFORM LOGGER_SOURCES PREPEND "${PROJECT_SOURCE_DIR}/src/")

add_library(logger_test_proxy6 ${LOGGER_SOURCES})
target_include_directories(logger_test_proxy6 PUBLIC ${LOGGER_INCLUDES})
target_compile_definitions(logger_test_proxy6 PUBLIC LOGGER_THREAD_SAFETY_HOOKS=1)

add_executable(logger_ut06 ut06_thread_safety.cpp)
target_link_libraries(logger_ut06 PRIVATE logger_test_proxy6 logger_test_common)

gtest_discover_tests(logger_ut06)
