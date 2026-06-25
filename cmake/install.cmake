include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(METADATA_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/logger)

install(TARGETS logger EXPORT logger-targets ARCHIVE FILE_SET HEADERS)
install(EXPORT logger-targets NAMESPACE embetech:: DESTINATION ${METADATA_DIR})

write_basic_package_version_file(logger-config-version.cmake COMPATIBILITY SameMajorVersion)

configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/logger-config_template.cmake ${CMAKE_CURRENT_BINARY_DIR}/logger-config.cmake INSTALL_DESTINATION ${METADATA_DIR}
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/logger-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/logger-config-version.cmake DESTINATION ${METADATA_DIR})

install(FILES ${PROJECT_SOURCE_DIR}/cmake/logger_utils.cmake DESTINATION ${METADATA_DIR})

install(FILES LICENSE.txt DESTINATION ${METADATA_DIR})

install(SCRIPT ${CMAKE_CURRENT_LIST_DIR}/install_header_licenses.cmake)
