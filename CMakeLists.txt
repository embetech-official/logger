cmake_minimum_required(VERSION 3.25)

project(logger VERSION 1.1.4)

include(CMakeDependentOption)

option(LOGGER_FP_SUPPORT "Enables formatted printing of floating point types" ON)

cmake_dependent_option(
  LOGGER_EXPONENT_SUPPORT "Enables printing floating point types using scientific notation" OFF LOGGER_FP_SUPPORT OFF
)

option(LOGGER_PTRDIFF_SUPPORT "Enables formatted printing of ptrdiff_t" OFF)

option(LOGGER_TIMESTAMPS "Enables support for timestamping logger messages" ON)

cmake_dependent_option(
  LOGGER_HUMAN_READABLE_TIMESTAMP "Prints timestamps in human readable format (hh:mm::ss.ms)" OFF LOGGER_TIMESTAMPS OFF
)

option(LOGGER_CUSTOM_AFFIXES "Enables function set to add custom message affixes" OFF)

option(LOGGER_RUNTIME_VERBOSITY "Enables changing logger verbosity at runtime via LOGGER_SetRuntimeLevel function" OFF)

option(
  LOGGER_HEADER_WITH_LOCATION
  "Appends logger header with source file name. By default, uses __FILE__ but the displayed name may be truncated using logger_normalize_printable_filenames CMake function"
  OFF
)

option(LOGGER_THREAD_SAFETY_HOOKS
       "Enables LOGGER_SetLockingMechanism funtion that allows setting custom lock mechanism" OFF
)

option(LOGGER_FLUSH_HOOKS "Enables LOGGER_SetFlushHook funtion that will be called after each logger message" OFF)

option(LOGGER_VERBOSE_ERRORS
       "Uses custom compiler pragmas to display additional debug information on compilation errors" OFF
)

option(LOGGER_TESTS "Build test cases" OFF)
option(LOGGER_INSTALL "Configure install target" ON)

set(EXTENDED_MODULE_PATH ${CMAKE_MODULE_PATH};${CMAKE_CURRENT_LIST_DIR}/cmake)
list(REMOVE_DUPLICATES EXTENDED_MODULE_PATH)
set(CMAKE_MODULE_PATH
    ${EXTENDED_MODULE_PATH}
    CACHE INTERNAL "" FORCE
)

add_subdirectory(src)

if (LOGGER_TESTS OR PROJECT_IS_TOP_LEVEL)
  include(CTest)
  enable_testing()
  add_subdirectory(tests EXCLUDE_FROM_ALL)
endif ()
