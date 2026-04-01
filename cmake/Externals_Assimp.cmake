include(ExternalProject)

option(GE_FETCH_EXTERNALS "Download externals" ON)
option(GE_ASSIMP_SHARED "Build Assimp as shared library" ON)
set(GE_ASSIMP_GIT_REPO "https://github.com/assimp/assimp.git" CACHE STRING "")
set(GE_ASSIMP_GIT_TAG  "v6.0.4" CACHE STRING "")

function(_ge_assimp_get_msvc_toolset_suffix outVar)
  if(MSVC_VERSION GREATER_EQUAL 1950)
    set(${outVar} "vc145" PARENT_SCOPE)
  elseif(MSVC_VERSION GREATER_EQUAL 1940)
    set(${outVar} "vc144" PARENT_SCOPE)
  elseif(MSVC_VERSION GREATER_EQUAL 1930)
    set(${outVar} "vc143" PARENT_SCOPE)
  elseif(MSVC_VERSION GREATER_EQUAL 1920)
    set(${outVar} "vc142" PARENT_SCOPE)
  elseif(MSVC_VERSION GREATER_EQUAL 1910)
    set(${outVar} "vc141" PARENT_SCOPE)
  elseif(MSVC_VERSION GREATER_EQUAL 1900)
    set(${outVar} "vc140" PARENT_SCOPE)
  else()
    set(${outVar} "vc" PARENT_SCOPE)
  endif()
endfunction()

function(_ge_assimp_configure_imported_locations target libDir binDir)
  if(WIN32)
    _ge_assimp_get_msvc_toolset_suffix(_ge_assimp_toolset)

    if(GE_ASSIMP_SHARED)
      set_target_properties(${target} PROPERTIES
        IMPORTED_IMPLIB_DEBUG          "${libDir}/assimp-${_ge_assimp_toolset}-mt.lib"
        IMPORTED_IMPLIB_RELEASE        "${libDir}/assimp-${_ge_assimp_toolset}-mt.lib"
        IMPORTED_IMPLIB_RELWITHDEBINFO "${libDir}/assimp-${_ge_assimp_toolset}-mt.lib"
        IMPORTED_IMPLIB_MINSIZEREL     "${libDir}/assimp-${_ge_assimp_toolset}-mt.lib"

        IMPORTED_LOCATION_DEBUG          "${binDir}/assimp-${_ge_assimp_toolset}-mt.dll"
        IMPORTED_LOCATION_RELEASE        "${binDir}/assimp-${_ge_assimp_toolset}-mt.dll"
        IMPORTED_LOCATION_RELWITHDEBINFO "${binDir}/assimp-${_ge_assimp_toolset}-mt.dll"
        IMPORTED_LOCATION_MINSIZEREL     "${binDir}/assimp-${_ge_assimp_toolset}-mt.dll"
      )
    else()
      set_target_properties(${target} PROPERTIES
        IMPORTED_LOCATION_DEBUG          "${libDir}/assimp-${_ge_assimp_toolset}-mt.lib"
        IMPORTED_LOCATION_RELEASE        "${libDir}/assimp-${_ge_assimp_toolset}-mt.lib"
        IMPORTED_LOCATION_RELWITHDEBINFO "${libDir}/assimp-${_ge_assimp_toolset}-mt.lib"
        IMPORTED_LOCATION_MINSIZEREL     "${libDir}/assimp-${_ge_assimp_toolset}-mt.lib"
      )
    endif()
  elseif(APPLE)
    if(GE_ASSIMP_SHARED)
      set_target_properties(${target} PROPERTIES
        IMPORTED_LOCATION_DEBUG          "${binDir}/libassimp.dylib"
        IMPORTED_LOCATION_RELEASE        "${binDir}/libassimp.dylib"
        IMPORTED_LOCATION_RELWITHDEBINFO "${binDir}/libassimp.dylib"
        IMPORTED_LOCATION_MINSIZEREL     "${binDir}/libassimp.dylib"
      )
    else()
      set_target_properties(${target} PROPERTIES
        IMPORTED_LOCATION_DEBUG          "${libDir}/libassimp.a"
        IMPORTED_LOCATION_RELEASE        "${libDir}/libassimp.a"
        IMPORTED_LOCATION_RELWITHDEBINFO "${libDir}/libassimp.a"
        IMPORTED_LOCATION_MINSIZEREL     "${libDir}/libassimp.a"
      )
    endif()
  else()
    if(GE_ASSIMP_SHARED)
      set_target_properties(${target} PROPERTIES
        IMPORTED_LOCATION_DEBUG          "${binDir}/libassimp.so"
        IMPORTED_LOCATION_RELEASE        "${binDir}/libassimp.so"
        IMPORTED_LOCATION_RELWITHDEBINFO "${binDir}/libassimp.so"
        IMPORTED_LOCATION_MINSIZEREL     "${binDir}/libassimp.so"
      )
    else()
      set_target_properties(${target} PROPERTIES
        IMPORTED_LOCATION_DEBUG          "${libDir}/libassimp.a"
        IMPORTED_LOCATION_RELEASE        "${libDir}/libassimp.a"
        IMPORTED_LOCATION_RELWITHDEBINFO "${libDir}/libassimp.a"
        IMPORTED_LOCATION_MINSIZEREL     "${libDir}/libassimp.a"
      )
    endif()
  endif()
endfunction()

function(ge_setup_assimp)
  if(TARGET ge_assimp)
    return()
  endif()

  if(NOT GE_FETCH_EXTERNALS)
    message(FATAL_ERROR "[Assimp] GE_FETCH_EXTERNALS=OFF and Assimp is not available.")
  endif()

  set(_ge_assimp_prefix "${CMAKE_BINARY_DIR}/_deps/assimp_ep")
  set(_ge_assimp_install "${_ge_assimp_prefix}/install")

  set(_ge_assimp_platform "${CMAKE_SYSTEM_NAME}")
  set(_ge_assimp_bin_dir "${CMAKE_BINARY_DIR}/bin/${_ge_assimp_platform}/${_arch}/Release")
  set(_ge_assimp_lib_dir "${CMAKE_BINARY_DIR}/lib/${_ge_assimp_platform}/${_arch}/Release")

  file(MAKE_DIRECTORY "${_ge_assimp_install}/include")
  file(MAKE_DIRECTORY "${_ge_assimp_install}/lib")
  file(MAKE_DIRECTORY "${_ge_assimp_bin_dir}")
  file(MAKE_DIRECTORY "${_ge_assimp_lib_dir}")

  ExternalProject_Add(assimp_ep
    PREFIX              "${_ge_assimp_prefix}"
    GIT_REPOSITORY      "${GE_ASSIMP_GIT_REPO}"
    GIT_TAG             "${GE_ASSIMP_GIT_TAG}"
    GIT_SHALLOW         TRUE
    UPDATE_DISCONNECTED TRUE

    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
      -DBUILD_SHARED_LIBS=$<IF:$<BOOL:${GE_ASSIMP_SHARED}>,ON,OFF>
      -DASSIMP_INSTALL=ON
      -DASSIMP_BUILD_ZLIB=ON
      -DASSIMP_BUILD_ASSIMP_TOOLS=OFF
      -DASSIMP_BUILD_SAMPLES=OFF
      -DASSIMP_BUILD_TESTS=OFF
      -DASSIMP_BUILD_DOCS=OFF
      -DASSIMP_WARNINGS_AS_ERRORS=OFF
      -DASSIMP_NO_EXPORT=ON
      -DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=ON
      -DASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT=OFF
      -DASSIMP_INJECT_DEBUG_POSTFIX=OFF
      -DCMAKE_POSITION_INDEPENDENT_CODE=ON
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE=${_ge_assimp_bin_dir}
      -DCMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE=${_ge_assimp_bin_dir}
      -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE=${_ge_assimp_lib_dir}

    BUILD_COMMAND
      ${CMAKE_COMMAND} --build <BINARY_DIR> --config Release

    INSTALL_COMMAND
      ${CMAKE_COMMAND} --build <BINARY_DIR> --target install --config Release

    INSTALL_DIR "${_ge_assimp_install}"
  )

  if(GE_ASSIMP_SHARED)
    add_library(ge_assimp SHARED IMPORTED GLOBAL)
  else()
    add_library(ge_assimp STATIC IMPORTED GLOBAL)
  endif()

  add_dependencies(ge_assimp assimp_ep)

  _ge_assimp_configure_imported_locations(ge_assimp "${_ge_assimp_lib_dir}" "${_ge_assimp_bin_dir}")

  set_target_properties(ge_assimp PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${_ge_assimp_install}/include"
  )

  if(UNIX AND NOT APPLE)
    target_link_libraries(ge_assimp INTERFACE ${CMAKE_DL_LIBS})
  endif()

  target_link_libraries(ge_assimp INTERFACE ge_build_settings)

  if(NOT TARGET ge::assimp)
    add_library(ge::assimp ALIAS ge_assimp)
  endif()

  set_property(TARGET assimp_ep PROPERTY FOLDER "Dependencies/Assimp")
  set_property(TARGET ge_assimp PROPERTY FOLDER "Dependencies/Assimp")
endfunction()

function(ge_deploy_assimp_runtime targetName)
  if(NOT TARGET ge_assimp)
    message(FATAL_ERROR "[Assimp] ge_assimp target does not exist. Call ge_setup_assimp() first.")
  endif()

  if(NOT TARGET ${targetName})
    message(FATAL_ERROR "[Assimp] Target '${targetName}' does not exist.")
  endif()

  # Asegura que Assimp se construya antes que este target
  add_dependencies(${targetName} ge_assimp)

  if(GE_ASSIMP_SHARED)
    add_custom_command(TARGET ${targetName} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:ge_assimp>
        $<TARGET_FILE_DIR:${targetName}>
      VERBATIM
    )
  endif()
endfunction()