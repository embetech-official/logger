include(FindDoxygen)

if (NOT DOXYGEN_FOUND)
  message(STATUS "Doxygen not found, documentation will not be generated")
  return()
endif ()

set(DOXYGEN_PROJECT_NAME "LOGGER")
set(USE_MDFILE_AS_MAINPAGE ${CMAKE_CURRENT_SOURCE_DIR}/logger.md)
set(DOXYGEN_LAYOUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/DoxygenLayout.xml)
set(DOXYGEN_IMAGE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# Nested under doc/logger/<version>/ so a release's install tree carries versioned documentation
# alongside include/ and lib/<arch>/ in the same combined archive.
set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/doc/logger/${PROJECT_VERSION})
set(DOXYGEN_GENERATE_TAGFILE ${DOXYGEN_OUTPUT_DIRECTORY}/html/logger.tag)

set(DOXYGEN_WARN_AS_ERROR YES)

set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)
set(DOXYGEN_TIMESTAMP YES)
set(DOXYGEN_CASE_SENSE_NAMES NO)
set(DOXYGEN_SHOW_INCLUDE_FILES NO)
set(DOXYGEN_SORT_MEMBER_DOCS NO)
set(DOXYGEN_ENABLE_PREPROCESSING YES)
set(DOXYGEN_MACRO_EXPANSION YES)
set(DOXYGEN_EXPAND_ONLY_PREDEF YES)
set(DOXYGEN_PREDEFINED "_DOXYGEN_" "__attribute__((x))=")
set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
set(DOXYGEN_REFERENCES_LINK_SOURCE NO)
set(DOXYGEN_VERBATIM_HEADERS NO)
set(DOXYGEN_HTML_COLORSTYLE DARK)
set(DOXYGEN_GENERATE_LATEX NO)
set(DOXYGEN_ALIASES [[purpose=@par Purpose:]] [[license=@par License:]] [[compiler=@par Compiler:]] [[ hardware=@par Hardware:]]
                    [[comments=@par Comments:]] [[doc=@par Documented using:]]
)
set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_HTML_COLORSTYLE AUTO_LIGHT)
set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_SEARCHENGINE YES)
set(DOXYGEN_TYPEDEF_HIDES_STRUCT YES)
set(DOXYGEN_ENUM_VALUES_PER_LINE 1)
set(WARN_IF_UNDOCUMENTED YES)
set(WARN_IF_DOC_ERROR YES)
set(WARN_IF_INCOMPLETE_DOC YES)
set(WARN_NO_PARAMDOC YES)

# Create local copy of public headers and configure VERSION and LICENSE information
include(${PROJECT_SOURCE_DIR}/cmake/install_header_licenses.cmake)
get_target_property(LOGGER_PUBLIC_HEADER_DIR logger HEADER_DIRS)
get_target_property(LOGGER_PUBLIC_HEADERS logger HEADER_SET)
set(LOGGER_DOC_STAGED_HEADERS_DIR ${CMAKE_CURRENT_BINARY_DIR}/staged_headers)
foreach (header ${LOGGER_PUBLIC_HEADERS})
  file(RELATIVE_PATH relative_header ${LOGGER_PUBLIC_HEADER_DIR} ${header})
  logger_configure_license_header(${header} ${LOGGER_DOC_STAGED_HEADERS_DIR}/${relative_header})
endforeach ()

# Strip the staged/build paths so generated docs show clean header names instead of build/gcc*/doc/staged_headers/...
set(DOXYGEN_STRIP_FROM_PATH ${LOGGER_DOC_STAGED_HEADERS_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# Deliberately not an ALL target: see the install build preset in CMakePresets.json - with
# Ninja Multi-Config, ALL would run doxygen once per configuration, both writing the same
# install-tree directory.
doxygen_add_docs(logger_doc ${CMAKE_CURRENT_SOURCE_DIR} ${LOGGER_DOC_STAGED_HEADERS_DIR} COMMENT "Generate documentation")
