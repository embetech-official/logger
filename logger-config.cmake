if (NOT TARGET embetech::logger)

  cmake_path(IS_PREFIX CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR} NORMALIZE LOGGER_CONFIGURED_FROM_PARENT_DIR)
  if (${LOGGER_CONFIGURED_FROM_PARENT_DIR})
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR})
  else ()
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR} ${CMAKE_BINARY_DIR}/embetech/logger)
  endif ()

endif ()
