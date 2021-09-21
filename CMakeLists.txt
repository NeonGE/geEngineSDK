cmake_minimum_required(VERSION 3.12.0)

# Options
set(GE_SHARED_LIBS ON CACHE BOOL "If true, The libraries will be compiled as shared libs, static otherwise")
set(GE_ENABLE_EXCEPTIONS OFF CACHE BOOL "If true C++ exceptions will be enabled when compiling.")
set(GE_ENABLE_RTTI OFF CACHE BOOL "If true C++ RTTI will be enabled when compiling.")

#Set the main project name
project(geEngineSDK)

#Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#Create some variables to the folders of the 
set (GESDK_MAIN_DIR ${PROJECT_SOURCE_DIR})
set (GE_BINARY_OUTPUT_DIR ${GESDK_MAIN_DIR}/bin)
set (GE_LIBRARY_OUTPUT_DIR ${GESDK_MAIN_DIR}/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG 			${GE_BINARY_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO 	${GE_BINARY_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL 		${GE_BINARY_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE 			${GE_BINARY_OUTPUT_DIR})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG 			${GE_LIBRARY_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO 	${GE_LIBRARY_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL 		${GE_LIBRARY_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE 			${GE_LIBRARY_OUTPUT_DIR})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG 			${GE_LIBRARY_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO 	${GE_LIBRARY_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL 		${GE_LIBRARY_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE 			${GE_LIBRARY_OUTPUT_DIR})

if(WIN32)
	add_compile_options(/W4)
endif()

#Remove /EHsc from CMAKE_CXX_FLAGS for MSVC to disable exceptions
if (MSVC AND NOT GE_ENABLE_EXCEPTIONS)
	if(CMAKE_CXX_FLAGS MATCHES "/EHsc")
		string(REPLACE "/EHsc" "/EHs-c-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	endif()
endif()

#Enable colored output
if (CMAKE_GENERATOR STREQUAL "Ninja")
	check_cxx_compiler_flag("-fdiagnostics-color=always" F_DIAGNOSTIC_COLOR_ALWAYS)
	if (F_DIAGNOSTIC_COLOR_ALWAYS)
		add_compile_options("-fdiagnostics-color=always")
	endif()
endif()

find_path(SUBMODULE_SOURCES "geUtilities/include/gePrerequisitesUtil.h")
if(NOT SUBMODULE_SOURCES)
	execute_process(COMMAND git submodule update --init -- geUtilities WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
else()
	execute_process(COMMAND git submodule update -- geUtilities WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
mark_as_advanced(SUBMODULE_SOURCES)

add_subdirectory(geUtilities)