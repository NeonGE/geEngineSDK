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

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

function(ge_set_folder targetName folderName)
  if(TARGET ${targetName})
    set_target_properties(${targetName} PROPERTIES FOLDER "${folderName}")
  endif()
endfunction()

function(ge_folder_all_targets_under prefix folderName)
  get_property(_allTargets GLOBAL PROPERTY TARGETS)
  foreach(t IN LISTS _allTargets)
    if(t MATCHES "^${prefix}")
      set_target_properties(${t} PROPERTIES FOLDER "${folderName}")
    endif()
  endforeach()
endfunction()

set(BUILD_TESTING off CACHE BOOL "" FORCE)
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

# Set Projects to a folder

# -----------------------------
# Subprojects
# -----------------------------
include(CTest)     # define BUILD_TESTING option + basic setup
include(Catch)

add_subdirectory(sdk)

ge_set_folder(Catch2          "Dependencies/Catch2")
ge_set_folder(Catch2WithMain  "Dependencies/Catch2")
ge_set_folder(minizip-ng      "Dependencies/MiniZip")
ge_set_folder(zlib-ng         "Dependencies/MiniZip")
ge_set_folder(ge_lz4          "Dependencies/LZ4")
ge_set_folder(ge_lua          "Dependencies/LUA")
ge_set_folder(rttr_core       "Dependencies/RTTR")
ge_set_folder(sfml-audio      "Dependencies/SFML")
ge_set_folder(sfml-graphics   "Dependencies/SFML")
ge_set_folder(sfml-main       "Dependencies/SFML")
ge_set_folder(sfml-network    "Dependencies/SFML")
ge_set_folder(sfml-system     "Dependencies/SFML")
ge_set_folder(sfml-window     "Dependencies/SFML")

set(BUILD_TESTING on CACHE BOOL "" FORCE)

enable_testing()
add_subdirectory(tests)

# Add games at the end
add_subdirectory(games)
