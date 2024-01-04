include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(CMAKES_EXPORT_DIR cmake/logger)

write_basic_package_version_file(logger-config-version.cmake COMPATIBILITY SameMajorVersion)

configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/logger-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/logger-config.cmake
  INSTALL_DESTINATION ${CMAKES_EXPORT_DIR}
)

install(TARGETS logger EXPORT logger_targets)

install(DIRECTORY include/embetech DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/logger-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/logger-config-version.cmake
        DESTINATION ${CMAKES_EXPORT_DIR}
)

install(FILES ${PROJECT_SOURCE_DIR}/cmake/logger_utils.cmake DESTINATION cmake/logger)

install(
  EXPORT logger_targets
  NAMESPACE embetech::
  DESTINATION ${CMAKES_EXPORT_DIR}
)
