set(INSTALL_DIR $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX})

file(STRINGS ${CMAKE_CURRENT_LIST_DIR}/../VERSION.txt PROJECT_VERSION)
set(PROJECT_LICENSE "MIT License")
set(PROJECT_COPYRIGHT "Embetech sp. z o.o.")

file(GLOB_RECURSE HEADERS_TO_CONFIGURE "${INSTALL_DIR}/*.h")
foreach (header ${HEADERS_TO_CONFIGURE})
  message(DEBUG "Configuring doxygen header: ${header}")
  configure_file(${header} ${header})
endforeach ()
