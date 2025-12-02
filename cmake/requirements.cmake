file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.0/CPM.cmake
     ${CMAKE_BINARY_DIR}/cmake/CPM.cmake
     EXPECTED_HASH SHA256=2020b4fc42dba44817983e06342e682ecfc3d2f484a581f11cc5731fbe4dce8a
)

include(${CMAKE_BINARY_DIR}/cmake/CPM.cmake)

if (LOGGER_TESTS)
  cpmaddpackage("gh:google/googletest#main")
endif ()
