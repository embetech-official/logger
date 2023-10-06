include(CMakePackageConfigHelpers)

write_basic_package_version_file(${PROJECT_NAME}-config-version.cmake COMPATIBILITY SameMajorVersion)

configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/component-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}_targets)

install(DIRECTORY include/embetech DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
install(FILES ${PROJECT_SOURCE_DIR}/cmake/logger_utils.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

install(EXPORT ${PROJECT_NAME}_targets NAMESPACE embetech:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})


function (create_internal_version_file)
  set(CONFIG_FILE ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}-config-version.cmake)
  write_basic_package_version_file(${CONFIG_FILE} COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)
  file(STRINGS ${CONFIG_FILE} LINES)
  # Remove architecture-dependent part
  foreach (LINE RANGE 53 69)
    list(POP_BACK LINES)
  endforeach ()

  file(WRITE ${CONFIG_FILE} "")
  foreach (LINE IN LISTS LINES)
    file(APPEND ${CONFIG_FILE} "${LINE}\n")
  endforeach ()
endfunction ()
