get_target_property(LOGGER_SOURCE_DIR logger SOURCE_DIR)

get_target_property(LOGGER_SOURCES logger SOURCES)
get_target_property(LOGGER_INCLUDES logger INCLUDE_DIRECTORIES)

list(TRANSFORM LOGGER_SOURCES PREPEND "${PROJECT_SOURCE_DIR}/src/")

add_library(logger_test_proxy4 ${LOGGER_SOURCES})
target_include_directories(logger_test_proxy4 PUBLIC ${LOGGER_INCLUDES})
target_compile_definitions(logger_test_proxy4 PUBLIC CONFIG_LOGGER_RUNTIME_VERBOSITY=1)

add_executable(logger_ut04 ut04_runtime_verbosity.cpp)
target_link_libraries(logger_ut04 PRIVATE logger_test_proxy4 logger_test_common)

add_dependencies(logger_ut logger_ut04)

gtest_discover_tests(logger_ut04)
