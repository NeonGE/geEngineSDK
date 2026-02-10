cmake_minimum_required(VERSION 3.20)

project(geEngineSDK VERSION 0.1.2026 LANGUAGES C CXX)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

# -----------------------------
# Organized Outputs (per Platform + Config)
# -----------------------------
function(ge_set_output_dirs targetName)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_arch "x64")
  else()
    set(_arch "x86")
  endif()

  set(_bin_base "${CMAKE_BINARY_DIR}/bin/$<PLATFORM_ID>/${_arch}/$<CONFIG>")
  set(_lib_base "${CMAKE_BINARY_DIR}/lib/$<PLATFORM_ID>/${_arch}/$<CONFIG>")

  set_target_properties(${targetName} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${_bin_base}"  # .exe
    LIBRARY_OUTPUT_DIRECTORY "${_bin_base}"  # .dll/.so/.dylib
    ARCHIVE_OUTPUT_DIRECTORY "${_lib_base}"  # .lib/.a
  )
endfunction()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(${CMAKE_SOURCE_DIR}/cmake/Externals.cmake)
include(Externals_RTTR)

# -----------------------------
# Build settings shared by all targets
# -----------------------------
add_library(ge_build_settings INTERFACE)
target_compile_features(ge_build_settings INTERFACE cxx_std_20)
set_target_properties(ge_build_settings PROPERTIES CXX_EXTENSIONS OFF)

# Remove RTTI
if(MSVC)
  target_compile_options(ge_build_settings INTERFACE /GR-)
else()
  target_compile_options(ge_build_settings INTERFACE -fno-rtti)
endif()

# -----------------------------
# Subprojects
# -----------------------------
include(CTest)     # define BUILD_TESTING option + basic setup
include(Catch)

add_subdirectory(sdk/geUtilities)
#add_subdirectory(games/geGame)

enable_testing()
add_subdirectory(tests)