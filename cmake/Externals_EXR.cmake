include(FetchContent)

option(GE_FETCH_EXTERNALS "Download externals" ON)
option(GE_EXR_USE_SYSTEM_ZLIB "Use system zlib instead of bundled miniz" OFF)

function(ge_setup_exr)
  if(TARGET ge_tinyexr)
    return()
  endif()

  if(NOT GE_FETCH_EXTERNALS)
    message(FATAL_ERROR "[EXR] GE_FETCH_EXTERNALS=OFF and TinyEXR is not available.")
  endif()

  set(GE_TINYEXR_GIT_REPO "https://github.com/syoyo/tinyexr.git")
  set(GE_TINYEXR_GIT_TAG  "release") # fija tag/commit si quieres reproducibilidad

  FetchContent_Declare(
    tinyexr_upstream
    GIT_REPOSITORY ${GE_TINYEXR_GIT_REPO}
    GIT_TAG        ${GE_TINYEXR_GIT_TAG}
    GIT_SHALLOW    TRUE
  )

  # Esto descarga, pero NO ejecuta el CMake del upstream (evita el warning)
  FetchContent_GetProperties(tinyexr_upstream)
  if(NOT tinyexr_upstream_POPULATED)
    FetchContent_Populate(tinyexr_upstream)
  endif()

  add_library(ge_tinyexr STATIC)
  add_library(ge::tinyexr ALIAS ge_tinyexr)

  # TU que genera la implementación de tinyexr (1 sola vez)
  set(_tinyexr_impl "${CMAKE_CURRENT_BINARY_DIR}/ge_tinyexr_impl.cpp")
  file(WRITE "${_tinyexr_impl}"
"#define TINYEXR_IMPLEMENTATION\n"
"#include \"tinyexr.h\"\n"
  )

  target_sources(ge_tinyexr PRIVATE
    "${_tinyexr_impl}"
  )

  target_include_directories(ge_tinyexr PUBLIC
    "${tinyexr_upstream_SOURCE_DIR}"
  )

  if(GE_EXR_USE_SYSTEM_ZLIB)
    find_package(ZLIB REQUIRED)
    target_compile_definitions(ge_tinyexr PUBLIC
      TINYEXR_USE_MINIZ=0
      TINYEXR_USE_STB_ZLIB=0
    )
    target_link_libraries(ge_tinyexr PUBLIC ZLIB::ZLIB)
  else()
    # ruta correcta según el CMake del repo tinyexr: deps/miniz/miniz.c
    target_sources(ge_tinyexr PRIVATE
      "${tinyexr_upstream_SOURCE_DIR}/deps/miniz/miniz.c"
    )
    target_include_directories(ge_tinyexr PUBLIC
      "${tinyexr_upstream_SOURCE_DIR}/deps/miniz"
    )
    target_compile_definitions(ge_tinyexr PUBLIC
      TINYEXR_USE_MINIZ=1
      TINYEXR_USE_STB_ZLIB=0
    )
  endif()

  set_target_properties(ge_tinyexr PROPERTIES FOLDER "Dependencies/TinyEXR")
endfunction()