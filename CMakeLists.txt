cmake_minimum_required(VERSION 3.20)

project(geEngineSDK VERSION 0.1.2026 LANGUAGES CXX)

# -----------------------------
# Organized Outputs
# -----------------------------
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # .lib / .a
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # .dll / .so
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # .exe

# Multi-config
if(CMAKE_CONFIGURATION_TYPES)
	foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
		string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin)
	endforeach()
endif()

#Sets the settings to compile all projects
add_library(ge_build_settings INTERFACE)
target_compile_features(ge_build_settings INTERFACE cxx_std_20)

#Remove RTTI
target_compile_options(ge_build_settings INTERFACE
    # MSVC / clang-cl
    $<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:/GR->

    # GCC / Clang (modo GNU)
    $<$<CXX_COMPILER_FRONTEND_VARIANT:GNU>:-fno-rtti>
)

set_target_properties(ge_build_settings PROPERTIES CXX_EXTENSIONS OFF)

# -----------------------------
# Subprojects
# -----------------------------
add_subdirectory(sdk/geUtilities)
#add_subdirectory(games/geGame)
#add_subdirectory(tests/geUtilities_Tests)