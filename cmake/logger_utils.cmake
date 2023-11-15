# ----------------------------------------------------------------------
# This function sets logger channel level for given target. If the level is a defined variable, it is used as a value.
# ----------------------------------------------------------------------
function (logger_set_channel_level target channel level)
  if (DEFINED ${level})
    message(DEBUG "Setting log level for target ${target} to ${${level}} (from ${level}) ")
    set_property(
      CACHE ${level}
      PROPERTY STRINGS
               DEFAULT
               DISABLED
               EMERGENCY
               ALERT
               CRITICAL
               ERROR
               WARNING
               NOTICE
               INFO
               VERBOSE
               DEBUG
               TRACE
    )
    set(LEVEL_TO_SET ${${level}})
  else ()
    message(DEBUG "Setting log level for target ${target} to ${level} ")
    set(LEVEL_TO_SET ${level})
  endif ()

  if (LEVEL_TO_SET STREQUAL "DEFAULT")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
      set(LEVEL_TO_SET "DEBUG")
    else ()
      set(LEVEL_TO_SET "ERROR")
    endif ()
  endif ()

  set(LOGGER_LEVEL "LOGGER_LEVEL_${LEVEL_TO_SET}")

  get_target_property(CURRENT_LEVEL ${target} "${channel}_LOG_LEVEL")
  if (NOT CURRENT_LEVEL)
    set_target_properties(${target} PROPERTIES "${channel}_LOG_LEVEL" "${LOGGER_LEVEL}")
    target_compile_definitions(
      ${target} PRIVATE "-D${channel}_LOG_CHANNEL_LEVEL=$<TARGET_PROPERTY:${target},${channel}_LOG_LEVEL>"
    )
  else ()
    message(FATAL_ERROR "Overriding log level is not supported (target: ${target}, channel: ${channel})")
  endif ()
endfunction ()

# ----------------------------------------------------------------------
# This function defines macro LOGGER_FILE for each source file in all buildable targets. The macro is defined as string
# containing base filename (without path). When possible logger will use this macro, instead of __FILE__ This function
# shall be invoked AFTER defining last target with sources (e.g. add_executable(...)
# ----------------------------------------------------------------------
function (logger_normalize_printable_filenames)
  message(VERBOSE "Logger will use normalized filenames")
  set(TARGETS)
  logger_get_all_targets_recursive(TARGETS ${CMAKE_SOURCE_DIR})

  foreach (TARGET IN LISTS TARGETS)
    get_target_property(TARGET_SOURCE_DIR ${TARGET} SOURCE_DIR)
    get_target_property(TARGET_SOURCES ${TARGET} SOURCES)
    list(TRANSFORM TARGET_SOURCES PREPEND "${TARGET_SOURCE_DIR}/")

    message(DEBUG "target ${TARGET}, sources: ${TARGET_SOURCES}")

    foreach (SOURCE_FILE IN LISTS TARGET_SOURCES)
      get_filename_component(SOURCE_FILE_NAME ${SOURCE_FILE} NAME)
      get_source_file_property(SOURCE_COMPILE_DEFINITIONS ${SOURCE_FILE} TARGET_DIRECTORY ${TARGET} COMPILE_DEFINITIONS)

      if (NOT SOURCE_COMPILE_DEFINITIONS)
        set(SOURCE_COMPILE_DEFINITIONS)
      endif ()

      list(APPEND SOURCE_COMPILE_DEFINITIONS LOGGER_FILE=\"${SOURCE_FILE_NAME}\")

      message(DEBUG "file ${SOURCE_FILE} flags: ${SOURCE_COMPILE_DEFINITIONS}")

      set_source_files_properties(
        ${SOURCE_FILE} TARGET_DIRECTORY ${TARGET} PROPERTIES COMPILE_DEFINITIONS "${SOURCE_COMPILE_DEFINITIONS}"
      )
    endforeach ()
  endforeach ()
endfunction ()

# ----------------------------------------------------------------------
# Internal helper macro used in logger_normalize_printable_filenames function. Recursively looks for targets in build
# directories
# ----------------------------------------------------------------------
macro (logger_get_all_targets_recursive targets dir)
  get_property(
    subdirectories
    DIRECTORY ${dir}
    PROPERTY SUBDIRECTORIES
  )
  foreach (subdir ${subdirectories})
    logger_get_all_targets_recursive(${targets} ${subdir})
  endforeach ()

  get_property(
    current_targets
    DIRECTORY ${dir}
    PROPERTY BUILDSYSTEM_TARGETS
  )
  list(APPEND ${targets} ${current_targets})
endmacro ()
