cmake_policy(SET CMP0135 NEW)

include(FetchContent)

# Optional - needed only when building tests is enabled
FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/main.zip FIND_PACKAGE_ARGS NAMES GTest)
