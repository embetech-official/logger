# ----------------------------------------------------------------------
# This function sets maximum compiled logger channel levels for given target.
# You MUST define a valid level for each build configuration used.
#
# logger_set_max_level(
#   target_name
#   CHANNEL <logger_channel>
#   CONFIG <build_config>:<logger_level> [<build_config>:<logger_level>...]
# )
#
# The logger_level can be one of the following: DISABLED, EMERGENCY, ALERT, CRITICAL, ERROR, WARNING, NOTICE, INFO, VERBOSE, DEBUG, TRACE
# This function may be invoked once per target and channel.
# ----------------------------------------------------------------------
function (logger_set_max_level target)
  if (NOT TARGET "${target}")
    message(FATAL_ERROR "Target ${target} does not exist")
  endif ()

  set(flags)
  set(single CHANNEL)
  set(multi CONFIG)
  cmake_parse_arguments(PARSE_ARGV 1 arg "${flags}" "${single}" "${multi}")

  if (NOT arg_CHANNEL)
    message(FATAL_ERROR "No log channel specified")
  endif ()
  set(channel ${arg_CHANNEL})

  get_target_property(current ${target} "${channel}_LOG_LEVEL")
  if (current)
    message(FATAL_ERROR "Log level for channel ${channel} already set for target ${target}")
  endif ()

  if (NOT arg_CONFIG)
    message(FATAL_ERROR "No log configuration specified for channel ${channel}")
  endif ()

  set(used_build_types)

  set(allowed_levels
      "DISABLED"
      "EMERGENCY"
      "ALERT"
      "CRITICAL"
      "ERROR"
      "WARNING"
      "NOTICE"
      "INFO"
      "VERBOSE"
      "DEBUG"
      "TRACE"
  )

  foreach (entry_str IN LISTS arg_CONFIG)
    string(REPLACE ":" ";" entry "${entry_str}")
    string(TOUPPER "${entry}" entry)

    list(LENGTH entry entry_len)
    if (NOT entry_len EQUAL 2)
      message(FATAL_ERROR "Invalid configuration string: ${entry_str}")
    endif ()

    list(GET entry 0 build_type)
    list(GET entry 1 level)

    if (build_type IN_LIST used_build_types)
      message(FATAL_ERROR "Duplicate build type: ${build_type} in expression '${entry_str}'")
    endif ()
    list(APPEND used_build_types ${build_type})

    if (NOT level IN_LIST allowed_levels)
      message(FATAL_ERROR "Invalid log level: ${level} in expression '${entry_str}'")
    endif ()

    set(config_entry
        "$<$<CONFIG:${build_type}>:${channel}_LOG_CHANNEL_LEVEL=LOGGER_LEVEL_${level}>"
    )
    list(APPEND compile_definitions "${config_entry}")
  endforeach ()

  message(DEBUG "compile_definitions for target ${target}: ${compile_definitions}")

  set_target_properties(${target} PROPERTIES "${channel}_LOG_LEVEL" "${compile_definitions}")

  get_target_property(type ${target} TYPE)
  if ("${type}" STREQUAL "INTERFACE_LIBRARY")
    set(visibility "INTERFACE")
  else ()
    set(visibility "PRIVATE")
  endif ()

  target_compile_definitions(${target} ${visibility} ${compile_definitions})

endfunction ()

# ----------------------------------------------------------------------
# This function sets logger channel level for given target. If the level is a defined variable, it
# is used as a value.
# ----------------------------------------------------------------------
function (logger_set_channel_level target channel level)
  message(
    DEPRECATION
      "logger_set_channel_level is deprecated (incompatible with multi-config builds). Use logger_set_max_level instead"
  )
  if (NOT TARGET "${target}")
    message(FATAL_ERROR "Target ${target} does not exist")
  endif ()

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
      ${target}
      PRIVATE "-D${channel}_LOG_CHANNEL_LEVEL=$<TARGET_PROPERTY:${target},${channel}_LOG_LEVEL>"
    )
  else ()
    message(
      FATAL_ERROR "Overriding log level is not supported (target: ${target}, channel: ${channel})"
    )
  endif ()
endfunction ()

# ----------------------------------------------------------------------
# This function defines macro LOGGER_FILE for each source file in all buildable targets. The macro
# is defined as string containing base filename (without path). When possible logger will use this
# macro, instead of __FILE__ This function shall be invoked AFTER defining last target with sources
# (e.g. add_executable(...)
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
      get_source_file_property(
        SOURCE_COMPILE_DEFINITIONS ${SOURCE_FILE} TARGET_DIRECTORY ${TARGET} COMPILE_DEFINITIONS
      )

      if (NOT SOURCE_COMPILE_DEFINITIONS)
        set(SOURCE_COMPILE_DEFINITIONS)
      endif ()

      list(APPEND SOURCE_COMPILE_DEFINITIONS LOGGER_FILE=\"${SOURCE_FILE_NAME}\")

      message(DEBUG "file ${SOURCE_FILE} flags: ${SOURCE_COMPILE_DEFINITIONS}")

      set_source_files_properties(
        ${SOURCE_FILE} TARGET_DIRECTORY ${TARGET} PROPERTIES COMPILE_DEFINITIONS
                                                             "${SOURCE_COMPILE_DEFINITIONS}"
      )
    endforeach ()
  endforeach ()
endfunction ()

# ----------------------------------------------------------------------
# Internal helper macro used in logger_normalize_printable_filenames function. Recursively looks for
# targets in build directories
# ----------------------------------------------------------------------
macro (logger_get_all_targets_recursive targets dir)
  get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
  foreach (subdir ${subdirectories})
    logger_get_all_targets_recursive(${targets} ${subdir})
  endforeach ()

  get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
  list(APPEND ${targets} ${current_targets})
endmacro ()
