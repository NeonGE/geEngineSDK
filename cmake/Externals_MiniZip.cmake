include(FetchContent)

option(GE_USE_SYSTEM_MINIZIP "Prefer system minizip-ng if available" ON)
option(GE_FETCH_EXTERNALS    "Download externals (sfml/minizip/etc.)" ON)

function(ge_setup_minizip)
  if(TARGET ge_minizip)
    return()
  endif()

  # ------------------------------------------------------------
  # 0) Ensure zlib wrapper exists (you said you already added it)
  # ------------------------------------------------------------
  if(NOT TARGET ge::zlib AND NOT TARGET ge_zlib)
    message(FATAL_ERROR
      "[minizip-ng] zlib dependency not found. "
      "Include your Externals_Zlib*.cmake and call ge_setup_zlib() before ge_setup_minizip()."
    )
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

      # ensure zlib + engine settings
      if(TARGET ge::zlib)
        target_link_libraries(ge_minizip INTERFACE ge::zlib ge_build_settings)
      else()
        target_link_libraries(ge_minizip INTERFACE ge_zlib ge_build_settings)
      endif()

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
  # 2a) Configure minizip-ng options (MUST be CACHE+FORCE)
  # ------------------------------------------------------------
  set(MZ_FETCH_LIBS         OFF CACHE BOOL "" FORCE)
  set(MZ_BUILD_DEPENDENCIES OFF CACHE BOOL "" FORCE)

  set(MZ_BZIP2    OFF CACHE BOOL "" FORCE)
  set(MZ_COMPAT   OFF CACHE BOOL "" FORCE)
  set(MZ_LZMA     OFF CACHE BOOL "" FORCE)
  set(MZ_PKCRYPT  OFF CACHE BOOL "" FORCE)

  set(MZ_ZSTD     OFF CACHE BOOL "" FORCE)
  set(WITH_GTEST  OFF CACHE BOOL "" FORCE)
  set(INSTALL_GTEST  OFF CACHE BOOL "" FORCE)
  set(ZLIBNG_ENABLE_TESTS  OFF CACHE BOOL "" FORCE)

  set(MZ_BUILD_TESTS      OFF CACHE BOOL "" FORCE)
  set(MZ_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
  set(MZ_BUILD_EXAMPLES   OFF CACHE BOOL "" FORCE)
  set(BUILD_TESTING       OFF CACHE BOOL "" FORCE)


  # If you want static by default for externals:
  # set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

  # Some setups use ZLIB from system; we are providing ours:
  # This variable name can differ across versions; leaving it ON usually doesn't hurt.
  set(MZ_ZLIB ON CACHE BOOL "" FORCE)

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

  # Ensure zlib external is linked after minizip (order helps some linkers)
  if(TARGET ge::zlib)
    target_link_libraries(ge_minizip INTERFACE ge::zlib)
  else()
    target_link_libraries(ge_minizip INTERFACE ge_zlib)
  endif()

  target_link_libraries(ge_minizip INTERFACE ge_build_settings)

  add_library(ge::minizip ALIAS ge_minizip)
  message(STATUS "[minizip-ng] Fetched ${GE_MINIZIP_GIT_TAG}")
endfunction()
