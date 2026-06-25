include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

if (LOGGER_TESTS)
  CPMAddPackage("gh:google/googletest@1.17.0")
endif ()
