# Architecture selection helpers, reused by any component's *-config.cmake in a bundle built for
# the architecture ids used by embetech-official/cmake-presets
# (https://github.com/embetech-official/cmake-presets): x86_64-linux-gnu, x86_64-windows-gnu,
# x86_64-windows-msvc, aarch64-linux-gnu, aarch32-linux-gnu, thumbv6m-none-eabi, thumbv7m-none-eabi,
# thumbv7em-none-eabi[hf], thumbv8m.base-none-eabi, thumbv8m.main-none-eabihf.
#
# detect_architecture_triplet(<output-variable>)
#   Sets <output-variable> in the caller's scope to one of the ids above, or to an empty string if none of
#   the detection tiers below could resolve it - a build using an unrecognized toolchain/macros must set its
#   own override (whatever cache variable name the calling package's config uses) before calling
#   find_package(). Doesn't touch any cache variable itself - purely a detector, caching is the caller's job.
include_guard(GLOBAL)
include(CheckSymbolExists)

function (detect_architecture_triplet out_var)
  set(known_triplets
      x86_64-linux-gnu
      x86_64-windows-gnu
      x86_64-windows-msvc
      aarch64-linux-gnu
      aarch32-linux-gnu
      thumbv6m-none-eabi
      thumbv7m-none-eabi
      thumbv7em-none-eabi
      thumbv7em-none-eabihf
      thumbv8m.base-none-eabi
      thumbv8m.main-none-eabihf
  )

  # 1) CMAKE_SYSTEM_PROCESSOR/CMAKE_SYSTEM_NAME - CMake's own standard target-identification variables,
  #    always set, so tried first. Covers the OS-hosted architectures.
  if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
      set(${out_var} x86_64-linux-gnu PARENT_SCOPE)
      return()

    elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
      set(${out_var} aarch64-linux-gnu PARENT_SCOPE)
      return()

    elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|armv7.*|armhf)$")
      set(${out_var} aarch32-linux-gnu PARENT_SCOPE)
      return()

    endif ()

  elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
    if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
      set(${out_var} x86_64-windows-msvc PARENT_SCOPE)

    else ()
      set(${out_var} x86_64-windows-gnu PARENT_SCOPE)

    endif ()

    return()
  endif ()

  # 2) Compiler preprocessor-macro probe, for the bare-metal Cortex-M targets CMAKE_SYSTEM_PROCESSOR alone
  #    can't disambiguate (a bare-metal toolchain file commonly just sets it to a generic "arm" regardless of
  #    core/FPU variant). These macros reflect the actual -march/-mfpu/-mfloat-abi flags in effect for THIS
  #    configure, so - unlike CMAKE_SYSTEM_PROCESSOR - they're exact. __ARM_PCS_VFP marks the hard-float
  #    calling convention (the "hf" suffix), distinct from mere FPU hardware presence. Each check only runs
  #    once the ones before it have failed to identify the core.
  check_symbol_exists(__ARM_ARCH_8M_MAIN__ "" EMBETECH_DETECT_ARCH_IS_8M_MAIN)
  if (EMBETECH_DETECT_ARCH_IS_8M_MAIN)
    set(${out_var} thumbv8m.main-none-eabihf PARENT_SCOPE) # only mainline variant this bundle ships; extend if a soft-float one appears
    return()
  endif ()

  check_symbol_exists(__ARM_ARCH_8M_BASE__ "" EMBETECH_DETECT_ARCH_IS_8M_BASE)
  if (EMBETECH_DETECT_ARCH_IS_8M_BASE)
    set(${out_var} thumbv8m.base-none-eabi PARENT_SCOPE)
    return()
  endif ()

  check_symbol_exists(__ARM_ARCH_7EM__ "" EMBETECH_DETECT_ARCH_IS_7EM)
  if (EMBETECH_DETECT_ARCH_IS_7EM)
    check_symbol_exists(__ARM_PCS_VFP "" EMBETECH_DETECT_ARCH_IS_HARDFP)
    if (EMBETECH_DETECT_ARCH_IS_HARDFP)
      set(${out_var} thumbv7em-none-eabihf PARENT_SCOPE)
    else ()
      set(${out_var} thumbv7em-none-eabi PARENT_SCOPE)
    endif ()
    return()
  endif ()

  check_symbol_exists(__ARM_ARCH_7M__ "" EMBETECH_DETECT_ARCH_IS_7M)
  if (EMBETECH_DETECT_ARCH_IS_7M)
    set(${out_var} thumbv7m-none-eabi PARENT_SCOPE)
    return()
  endif ()

  check_symbol_exists(__ARM_ARCH_6M__ "" EMBETECH_DETECT_ARCH_IS_6M)
  if (EMBETECH_DETECT_ARCH_IS_6M)
    set(${out_var} thumbv6m-none-eabi PARENT_SCOPE)
    return()
  endif ()

  # 3) Compiler-reported target triplet, for the OS-hosted architectures when tier 1 didn't resolve (e.g. an
  #    unrecognized CMAKE_SYSTEM_PROCESSOR value).
  execute_process(
    COMMAND "${CMAKE_C_COMPILER}" -dumpmachine OUTPUT_VARIABLE triplet OUTPUT_STRIP_TRAILING_WHITESPACE RESULT_VARIABLE dumpmachine_result
    ERROR_QUIET
  )
  if (dumpmachine_result EQUAL 0)
    if (triplet MATCHES "^x86_64-.*-mingw32")
      set(${out_var} x86_64-windows-gnu PARENT_SCOPE)
      return()
    elseif (triplet STREQUAL "x86_64-linux-gnu")
      set(${out_var} x86_64-linux-gnu PARENT_SCOPE)
      return()
    elseif (triplet MATCHES "^aarch64-.*-linux-gnu")
      set(${out_var} aarch64-linux-gnu PARENT_SCOPE)
      return()
    elseif (triplet MATCHES "^arm.*-linux-gnueabihf")
      set(${out_var} aarch32-linux-gnu PARENT_SCOPE)
      return()
    endif ()
  endif ()

  # 4) CMAKE_LIBRARY_ARCHITECTURE, last resort: it only reliably carries one of these exact ids when the
  #    caller happens to already configure with the same embetech-official/cmake-presets toolchain presets
  #    a bundle's components were built with - not something to rely on for a generic consumer, so it's
  #    tried only after every signal actually derived from invoking the compiler has failed to resolve.
  if (CMAKE_LIBRARY_ARCHITECTURE IN_LIST known_triplets)
    set(${out_var} "${CMAKE_LIBRARY_ARCHITECTURE}" PARENT_SCOPE)
    return()
  endif ()

  set(${out_var} "" PARENT_SCOPE)
endfunction ()

# find_architecture_targets_file(<project-name> <arch> <search-dir> <output-variable>)
#   Resolves <search-dir>/<project-name>-<arch>-targets.cmake, or errors out listing the
#   architectures actually found there.
function (find_architecture_targets_file project_name arch search_dir out_var)
  set(file "${search_dir}/${project_name}-${arch}-targets.cmake")
  if (NOT EXISTS "${file}")
    file(GLOB candidates "${search_dir}/${project_name}-*-targets.cmake")
    set(available "")
    foreach (candidate IN LISTS candidates)
      get_filename_component(name "${candidate}" NAME)
      string(REGEX REPLACE "^${project_name}-(.+)-targets\\.cmake$" "\\1" available_arch "${name}")
      list(APPEND available "${available_arch}")
    endforeach ()
    string(REPLACE ";" ", " available "${available}")
    message(FATAL_ERROR "${project_name} was not built for architecture '${arch}'. Available architectures: ${available}")
  endif ()
  set(${out_var} "${file}" PARENT_SCOPE)
endfunction ()
