# get_version_string
# Derives a semver-ish version string from a base version and the repository's tag/commit state.
# Owns every fallback/metadata variant itself, so callers never need to assemble a version string
# by hand.
#
# Signature:
#   get_version_string(<VERSION_OUT> VERSION <base-version> [LABEL <label>])
#
# Parameters:
# - VERSION_OUT: variable set to the final version string, e.g.:
#     "1.2.5"                        -- exact, clean release (HEAD is tag v1.2.5, no local changes)
#     "1.2.5-alpha"                  -- same, with LABEL "alpha"
#     "1.2.5+6-gf4083cf"             -- 6 commits past tag v1.2.5
#     "1.2.5+dirty"                  -- on tag v1.2.5, but with uncommitted local changes
#     "1.2.5-alpha+6-gf4083cf-dirty" -- combination of the above
#     "1.2.5+dirty"                  -- also used when git/tags are unavailable at all (no git
#                                        executable, or CMAKE_SOURCE_DIR isn't a git checkout,
#                                        e.g. a source tarball) -- can't prove a clean release, so
#                                        it's never reported as one
# - VERSION (required): base version, e.g. "1.2.5". Matched against tag "v<base-version>".
# - LABEL (optional): extra tag inserted as a semver prerelease component, e.g. "alpha", "rc1".
function (get_version_string VERSION_OUT)
  set(oneValueArgs VERSION LABEL)
  cmake_parse_arguments(arg "" "${oneValueArgs}" "" ${ARGN})

  if (NOT arg_VERSION)
    message(FATAL_ERROR "get_version_string: VERSION is required")
  endif ()

  set(version_string "${arg_VERSION}")
  if (arg_LABEL)
    string(APPEND version_string "-${arg_LABEL}")
  endif ()

  find_package(Git QUIET)
  if (GIT_FOUND)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} -C ${CMAKE_SOURCE_DIR} describe --tags --dirty --always --match "v${arg_VERSION}" OUTPUT_VARIABLE describe_output
      OUTPUT_STRIP_TRAILING_WHITESPACE RESULT_VARIABLE describe_result
    )
  endif ()

  if (NOT GIT_FOUND OR NOT describe_result EQUAL 0)
    message(WARNING "Could not determine git version info for ${CMAKE_SOURCE_DIR}: treating as an unreleased, dirty build")
    set(${VERSION_OUT} "${version_string}+dirty" PARENT_SCOPE)
    return()
  endif ()

  if (NOT describe_output STREQUAL "v${arg_VERSION}")
    string(REPLACE "v${arg_VERSION}-" "" describe_output "${describe_output}")
    string(APPEND version_string "+${describe_output}")
  endif ()

  set(${VERSION_OUT} "${version_string}" PARENT_SCOPE)
endfunction ()
