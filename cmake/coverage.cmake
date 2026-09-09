if (NOT PROJECT_IS_TOP_LEVEL)
  return()
endif ()

if (NOT CMAKE_CXX_FLAGS MATCHES "--coverage" AND NOT CMAKE_C_FLAGS MATCHES "--coverage")
  message(STATUS "coverage: skipped ('coverage' target requires --coverage in CMAKE_CXX_FLAGS/CMAKE_C_FLAGS)")
  return()
endif ()

find_program(GCOVR gcovr)

if (NOT GCOVR)
  message(STATUS "coverage: skipped (gcovr not found, install with 'pip install gcovr')")
  return()
endif ()

message(STATUS "coverage: 'coverage' target enabled (${GCOVR})")

add_custom_target(
  coverage
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
  COMMAND
    ${GCOVR} --root ${PROJECT_SOURCE_DIR} --filter "${PROJECT_SOURCE_DIR}/src/.*" --object-directory ${CMAKE_BINARY_DIR}
    --merge-mode-functions=separate --html-details -o ${CMAKE_BINARY_DIR}/coverage/index.html --lcov ${CMAKE_BINARY_DIR}/coverage/lcov.info
    --cobertura ${CMAKE_BINARY_DIR}/coverage/coverage.cobertura.xml --print-summary
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Generating gcov coverage report for logger (run tests first)"
)
