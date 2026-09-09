set(@PROJECT_NAME@_VERSION @PROJECT_VERSION@)

@PACKAGE_INIT@

include("${CMAKE_CURRENT_LIST_DIR}/architecture_selection.cmake")

if (NOT DEFINED @PROJECT_NAME@_ARCHITECTURE)
  detect_architecture_triplet(@PROJECT_NAME@_ARCHITECTURE)
  if (NOT @PROJECT_NAME@_ARCHITECTURE)
    message(FATAL_ERROR "Unable to auto-detect the target architecture for @PROJECT_NAME@. "
                        "Set @PROJECT_NAME@_ARCHITECTURE manually before calling find_package(@PROJECT_NAME@)."
    )
  endif ()
endif ()

message(DEBUG "@PROJECT_NAME@_ARCHITECTURE=${@PROJECT_NAME@_ARCHITECTURE}")

find_architecture_targets_file(@PROJECT_NAME@ "${@PROJECT_NAME@_ARCHITECTURE}" "${CMAKE_CURRENT_LIST_DIR}" targets_file)
include("${targets_file}")
unset(targets_file)

check_required_components("@PROJECT_NAME@")
