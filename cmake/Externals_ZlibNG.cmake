include(FetchContent)

option(GE_USE_SYSTEM_ZLIB "Prefer system zlib if available" ON)
option(GE_FETCH_EXTERNALS "Download externals" ON)

function(ge_setup_zlib)
  if(TARGET ge_zlib)
    return()
  endif()

  # 1) system zlib
  if(GE_USE_SYSTEM_ZLIB)
    find_package(ZLIB QUIET)
    if(ZLIB_FOUND)
      add_library(ge_zlib INTERFACE)
      target_link_libraries(ge_zlib INTERFACE ZLIB::ZLIB ge_build_settings)
      add_library(ge::zlib ALIAS ge_zlib)
      message(STATUS "[zlib] Using system ZLIB")
      return()
    endif()
  endif()

  if(NOT GE_FETCH_EXTERNALS)
    message(FATAL_ERROR "[zlib] Not found and GE_FETCH_EXTERNALS=OFF")
  endif()

  # 2) fetch zlib-ng (drop-in replacement, exports zlib-like headers)
  set(ZLIBNG_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
  set(ZLIBNG_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

  FetchContent_Declare(
    zlibng_upstream
    GIT_REPOSITORY "https://github.com/zlib-ng/zlib-ng.git"
    GIT_TAG        "2.3.3"
    GIT_SHALLOW    TRUE
  )

  FetchContent_MakeAvailable(zlibng_upstream)

  # zlib-ng defines target: zlibstatic / zlib (varía por config). Detectamos.
  add_library(ge_zlib INTERFACE)
  if(TARGET zlibstatic)
    target_link_libraries(ge_zlib INTERFACE zlibstatic)
  elseif(TARGET zlib)
    target_link_libraries(ge_zlib INTERFACE zlib)
  else()
    message(FATAL_ERROR "[zlib-ng] Could not detect zlib target (zlibstatic/zlib).")
  endif()

  target_link_libraries(ge_zlib INTERFACE ge_build_settings)
  add_library(ge::zlib ALIAS ge_zlib)
endfunction()
