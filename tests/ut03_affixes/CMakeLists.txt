get_target_property(LOGGER_SOURCE_DIR logger SOURCE_DIR)

get_target_property(LOGGER_SOURCES logger SOURCES)
get_target_property(LOGGER_INCLUDES logger INCLUDE_DIRECTORIES)

list(TRANSFORM LOGGER_SOURCES PREPEND "${PROJECT_SOURCE_DIR}/src/")

add_library(logger_test_proxy3_1 ${LOGGER_SOURCES})
target_include_directories(logger_test_proxy3_1 PUBLIC ${LOGGER_INCLUDES})
target_compile_definitions(logger_test_proxy3_1 PUBLIC CONFIG_LOGGER_CUSTOM_AFFIXES=1)

add_executable(logger_ut03_1 ut03_1_affixes_no_timestamp.cpp)
target_link_libraries(logger_ut03_1 PRIVATE logger_test_proxy3_1 logger_test_common)

add_library(logger_test_proxy3_2 ${LOGGER_SOURCES})
target_include_directories(logger_test_proxy3_2 PUBLIC ${LOGGER_INCLUDES})
target_compile_definitions(logger_test_proxy3_2 PUBLIC CONFIG_LOGGER_CUSTOM_AFFIXES=1 CONFIG_LOGGER_TIMESTAMPS=1)

add_executable(logger_ut03_2 ut03_2_affixes_timestamp.cpp)
target_link_libraries(logger_ut03_2 PRIVATE logger_test_proxy3_2 logger_test_common)

add_dependencies(logger_ut logger_ut03_1)
add_dependencies(logger_ut logger_ut03_2)

gtest_discover_tests(logger_ut03_1)
gtest_discover_tests(logger_ut03_2)
