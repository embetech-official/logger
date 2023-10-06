get_target_property(LOGGER_SOURCE_DIR logger SOURCE_DIR)

get_target_property(LOGGER_SOURCES logger SOURCES)
get_target_property(LOGGER_INCLUDES logger INCLUDE_DIRECTORIES)

list(TRANSFORM LOGGER_SOURCES PREPEND "${PROJECT_SOURCE_DIR}/src/")

add_library(logger_test_proxy8 ${LOGGER_SOURCES})
target_include_directories(logger_test_proxy8 PUBLIC ${LOGGER_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(logger_ut08 disabling_channel_at_ct.cpp disabling_logger_at_ct.cpp format_support.cpp)
target_link_libraries(logger_ut08 PRIVATE logger_test_proxy8 logger_test_common)

gtest_discover_tests(logger_ut08)
