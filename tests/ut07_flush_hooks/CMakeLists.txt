get_target_property(LOGGER_SOURCE_DIR logger SOURCE_DIR)

get_target_property(LOGGER_SOURCES logger SOURCES)
get_target_property(LOGGER_INCLUDES logger INCLUDE_DIRECTORIES)

list(TRANSFORM LOGGER_SOURCES PREPEND "${PROJECT_SOURCE_DIR}/src/")

add_library(logger_test_proxy7 ${LOGGER_SOURCES})
target_include_directories(logger_test_proxy7 PUBLIC ${LOGGER_INCLUDES})
target_compile_definitions(logger_test_proxy7 PUBLIC CONFIG_LOGGER_FLUSH_HOOKS=1)

add_executable(logger_ut07 ut07_flush_hooks.cpp)
target_link_libraries(logger_ut07 PRIVATE logger_test_proxy7 logger_test_common)

gtest_discover_tests(logger_ut07)
