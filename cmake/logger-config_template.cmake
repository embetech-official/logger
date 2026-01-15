@PACKAGE_INIT@

include("${CMAKE_CURRENT_LIST_DIR}/@PROJECT_NAME@-targets.cmake")

set(LOGGER_UTILS "${CMAKE_CURRENT_LIST_DIR}/logger_utils.cmake" CACHE INTERNAL "Path to logger_utils module")
# Make logger_utils module visible
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
