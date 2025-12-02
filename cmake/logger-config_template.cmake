@PACKAGE_INIT@

include("${CMAKE_CURRENT_LIST_DIR}/@PROJECT_NAME@-targets.cmake")

# Make logger_utils module visible
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})