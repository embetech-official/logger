get_target_property(LOGGER_SOURCE_DIR logger SOURCE_DIR)

get_target_property(LOGGER_SOURCES logger SOURCES)
get_target_property(LOGGER_INCLUDES logger INCLUDE_DIRECTORIES)

list(TRANSFORM LOGGER_SOURCES PREPEND "${PROJECT_SOURCE_DIR}/src/")

add_library(logger_test_proxy5_1 ${LOGGER_SOURCES})
target_include_directories(logger_test_proxy5_1 PUBLIC ${LOGGER_INCLUDES})
target_compile_definitions(logger_test_proxy5_1 PUBLIC CONFIG_LOGGER_HEADER_WITH_LOCATION=1 LOGGER_FILE="OVERRIDEN")

add_executable(logger_ut05_1 ut05_1_location.cpp)
target_link_libraries(logger_ut05_1 PRIVATE logger_test_proxy5_1 logger_test_common)

add_library(logger_test_proxy5_2 ${LOGGER_SOURCES})
target_include_directories(logger_test_proxy5_2 PUBLIC ${LOGGER_INCLUDES})
target_compile_definitions(
  logger_test_proxy5_2 PUBLIC CONFIG_LOGGER_HEADER_WITH_LOCATION=1 CONFIG_LOGGER_TIMESTAMPS=1 LOGGER_FILE="OVERRIDEN"
)

add_executable(logger_ut05_2 ut05_2_location_timestamp.cpp)
target_link_libraries(logger_ut05_2 PRIVATE logger_test_proxy5_2 logger_test_common)

add_library(logger_test_proxy5_3 ${LOGGER_SOURCES})
target_include_directories(logger_test_proxy5_3 PUBLIC ${LOGGER_INCLUDES})
target_compile_definitions(
  logger_test_proxy5_3 PUBLIC CONFIG_LOGGER_HEADER_WITH_LOCATION=1 CONFIG_LOGGER_TIMESTAMPS=1
                              CONFIG_LOGGER_CUSTOM_AFFIXES=1 LOGGER_FILE="OVERRIDEN"
)

add_executable(logger_ut05_3 ut05_3_location_timestamp_affix.cpp)
target_link_libraries(logger_ut05_3 PRIVATE logger_test_proxy5_3 logger_test_common)

add_dependencies(logger_ut logger_ut05_1)
add_dependencies(logger_ut logger_ut05_2)
add_dependencies(logger_ut logger_ut05_3)

gtest_discover_tests(logger_ut05_1)
gtest_discover_tests(logger_ut05_2)
gtest_discover_tests(logger_ut05_3)
