# Rewrites the @license/@copyright/@version placeholders in a public header with the real
# project values. Used both as the install(SCRIPT) below (rewriting installed headers in place)
# and by doc/CMakeLists.txt (staging substituted copies for the Doxygen build), so the installed
# headers and the generated docs always show the same values.
#
# PROJECT_LICENSE/PROJECT_COPYRIGHT/PROJECT_VERSION are scoped to this function so configure_file
# sees them without overwriting the caller's own PROJECT_VERSION (set by project() at the top level).
function (logger_configure_license_header source destination)
  file(STRINGS ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../VERSION.txt PROJECT_VERSION)
  set(PROJECT_LICENSE "MIT License")
  set(PROJECT_COPYRIGHT "Embetech sp. z o.o.")
  configure_file(${source} ${destination})
endfunction ()

if (CMAKE_SCRIPT_MODE_FILE)
  # Running as the install(SCRIPT) itself (not merely included by another -P script):
  # rewrite the headers already copied into the install tree.
  set(INSTALL_DIR $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX})
  file(GLOB_RECURSE HEADERS_TO_CONFIGURE "${INSTALL_DIR}/*.h")
  foreach (header ${HEADERS_TO_CONFIGURE})
    message(DEBUG "Configuring doxygen header: ${header}")
    logger_configure_license_header(${header} ${header})
  endforeach ()
endif ()
