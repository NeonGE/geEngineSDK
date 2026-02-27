include(FetchContent)

if(POLICY CMP0169)
  cmake_policy(SET CMP0169 OLD)
endif()
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON CACHE BOOL "" FORCE)

option(GE_USE_SYSTEM_MINIZIP "Prefer system minizip-ng if available" ON)
option(GE_FETCH_EXTERNALS    "Download externals (sfml/minizip/etc.)" ON)

# Opcional: exponer toggles de compresión
option(GE_MINIZIP_ENABLE_BZIP2 "Enable BZIP2 support in minizip-ng" ON)
option(GE_MINIZIP_ENABLE_LZMA  "Enable LZMA/XZ support in minizip-ng"  ON)
option(GE_MINIZIP_ENABLE_ZSTD  "Enable ZSTD support in minizip-ng" ON)

function(ge_setup_minizip)
  if(TARGET ge_minizip)
    return()
  endif()

  # ------------------------------------------------------------
  # 1) Try system package (best-effort)
  # ------------------------------------------------------------
  if(GE_USE_SYSTEM_MINIZIP)
    find_package(minizip-ng QUIET)
    if(minizip-ng_FOUND)
      add_library(ge_minizip INTERFACE)

      if(TARGET minizip-ng::minizip-ng)
        target_link_libraries(ge_minizip INTERFACE minizip-ng::minizip-ng)
      elseif(TARGET minizip::minizip)
        target_link_libraries(ge_minizip INTERFACE minizip::minizip)
      elseif(TARGET minizip-ng)
        target_link_libraries(ge_minizip INTERFACE minizip-ng)
      elseif(TARGET minizip)
        target_link_libraries(ge_minizip INTERFACE minizip)
      else()
        message(FATAL_ERROR "[minizip-ng] System package found, but no known CMake target was exported.")
      endif()

      target_link_libraries(ge_minizip INTERFACE ge_build_settings)
      add_library(ge::minizip ALIAS ge_minizip)

      message(STATUS "[minizip-ng] Using system package")
      return()
    endif()
  endif()

  # ------------------------------------------------------------
  # 2) FetchContent fallback
  # ------------------------------------------------------------
  if(NOT GE_FETCH_EXTERNALS)
    message(FATAL_ERROR
      "[minizip-ng] System package not found (or disabled) and GE_FETCH_EXTERNALS=OFF. "
      "Install minizip-ng or enable GE_FETCH_EXTERNALS."
    )
  endif()

  set(GE_MINIZIP_GIT_REPO "https://github.com/zlib-ng/minizip-ng.git")
  set(GE_MINIZIP_GIT_TAG  "4.1.0")

  # ------------------------------------------------------------
  # 2a) Configure minizip-ng options (CACHE+FORCE)
  #
  # Queremos que minizip-ng resuelva/traiga sus dependencias
  # (zlib, bzip2, lzma, zstd) según configuración.
  # ------------------------------------------------------------
  set(MZ_ZLIB ON CACHE BOOL "" FORCE)

  set(MZ_BZIP2 ${GE_MINIZIP_ENABLE_BZIP2} CACHE BOOL "" FORCE)
  set(MZ_LZMA  ${GE_MINIZIP_ENABLE_LZMA}  CACHE BOOL "" FORCE)
  set(MZ_ZSTD  ${GE_MINIZIP_ENABLE_ZSTD}  CACHE BOOL "" FORCE)

  # Que minizip-ng consiga dependencias
  set(MZ_FETCH_LIBS ON CACHE BOOL "" FORCE)

  # Si quieres evitar que “medio use” libs del sistema y medio las fetchee,
  # puedes forzar fetch (útil para builds reproducibles):
  set(MZ_FORCE_FETCH_LIBS ON CACHE BOOL "" FORCE)

  # (Este flag existe en varias versiones; si no existe, no pasa nada grave)
  set(MZ_BUILD_DEPENDENCIES ON CACHE BOOL "" FORCE)

  # Opcionales/seguridad:
  set(MZ_COMPAT  OFF CACHE BOOL "" FORCE)
  set(MZ_PKCRYPT OFF CACHE BOOL "" FORCE)

  # Evitar tests/ejemplos
  set(MZ_BUILD_TESTS      OFF CACHE BOOL "" FORCE)
  set(MZ_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
  set(MZ_BUILD_EXAMPLES   OFF CACHE BOOL "" FORCE)
  set(BUILD_TESTING       OFF CACHE BOOL "" FORCE)

  # No instalar nada de minizip-ng (ni sus deps como zlib) desde tu build de externals
  set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)

  set(BUILD_SHARED_LIBS OFF)
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

  FetchContent_Declare(
    minizip_upstream
    GIT_REPOSITORY ${GE_MINIZIP_GIT_REPO}
    GIT_TAG        ${GE_MINIZIP_GIT_TAG}
    GIT_SHALLOW    TRUE
  )

  FetchContent_MakeAvailable(minizip_upstream)

  # ------------------------------------------------------------
  # 3) Wrap minizip-ng target
  # ------------------------------------------------------------
  add_library(ge_minizip INTERFACE)

  if(TARGET minizip-ng::minizip-ng)
    target_link_libraries(ge_minizip INTERFACE minizip-ng::minizip-ng)
  elseif(TARGET minizip::minizip)
    target_link_libraries(ge_minizip INTERFACE minizip::minizip)
  elseif(TARGET minizip-ng)
    target_link_libraries(ge_minizip INTERFACE minizip-ng)
  elseif(TARGET minizip)
    target_link_libraries(ge_minizip INTERFACE minizip)
  else()
    message(FATAL_ERROR
      "[minizip-ng] Could not detect upstream target. "
      "Expected one of: minizip-ng::minizip-ng, minizip::minizip, minizip-ng, minizip."
    )
  endif()

  # IMPORTANTE: ya NO linkeamos zlib/bzip2/lzma/zstd aquí manualmente.
  # El target upstream ya debe venir linkeado correctamente según MZ_*.

  target_link_libraries(ge_minizip INTERFACE ge_build_settings)

  add_library(ge::minizip ALIAS ge_minizip)
  message(STATUS "[minizip-ng] Fetched ${GE_MINIZIP_GIT_TAG} (fetch deps ON)")

  set_target_properties(bzip2 PROPERTIES FOLDER "Dependencies/MiniZip")
  set_target_properties(liblzma PROPERTIES FOLDER "Dependencies/MiniZip")
  set_target_properties(libzstd_static PROPERTIES FOLDER "Dependencies/MiniZip")
endfunction()