get_target_property(LOGGER_SOURCE_DIR logger SOURCE_DIR)

get_target_property(LOGGER_SOURCES logger SOURCES)
get_target_property(LOGGER_INCLUDES logger INCLUDE_DIRECTORIES)

list(TRANSFORM LOGGER_SOURCES PREPEND "${PROJECT_SOURCE_DIR}/src/")

add_library(logger_test_proxy2_1 ${LOGGER_SOURCES})
target_include_directories(logger_test_proxy2_1 PUBLIC ${LOGGER_INCLUDES})
target_compile_definitions(logger_test_proxy2_1 PUBLIC LOGGER_TIMESTAMPS=1)

add_executable(logger_ut02_1 ut02_1_timestamp.cpp)
target_link_libraries(logger_ut02_1 PRIVATE logger_test_proxy2_1 logger_test_common)

add_library(logger_test_proxy2_2 ${LOGGER_SOURCES})
target_include_directories(logger_test_proxy2_2 PUBLIC ${LOGGER_INCLUDES})
target_compile_definitions(logger_test_proxy2_2 PUBLIC LOGGER_TIMESTAMPS=1 LOGGER_HUMAN_READABLE_TIMESTAMP=1)

add_executable(logger_ut02_2 ut02_2_human_readable_timestamp.cpp)
target_link_libraries(logger_ut02_2 PRIVATE logger_test_proxy2_2 logger_test_common)

gtest_discover_tests(logger_ut02_1)
gtest_discover_tests(logger_ut02_2)
