include(FetchContent)

option(GE_USE_SYSTEM_LUA  "Prefer system Lua if available" ON)
option(GE_FETCH_EXTERNALS "Download externals (lua/etc.)" ON)

# Target created:
#   ge_lua   (STATIC)
# Alias:
#   lua::lua
#
# Consumers:
#   target_link_libraries(myTarget PRIVATE ge_lua)
#   target_include_directories comes transitively from ge_lua
function(ge_setup_lua)
  if(TARGET ge_lua)
    return()
  endif()

  # ------------------------------------------------------------
  # 1) Try system Lua first (when requested)
  # ------------------------------------------------------------
  if(GE_USE_SYSTEM_LUA)
    # CMake's FindLua module typically provides:
    #   LUA_INCLUDE_DIR, LUA_LIBRARIES, LUA_VERSION_STRING, etc.
    find_package(Lua QUIET)

    if(LUA_FOUND)
      add_library(ge_lua INTERFACE)
      target_include_directories(ge_lua INTERFACE "${LUA_INCLUDE_DIR}")
      target_link_libraries(ge_lua INTERFACE "${LUA_LIBRARIES}" ge_build_settings)
      add_library(lua::lua ALIAS ge_lua)
      message(STATUS "[Lua] Using system Lua: ${LUA_VERSION_STRING}")
      return()
    endif()
  endif()

  # ------------------------------------------------------------
  # 2) Fallback: build Lua from official sources (static)
  # ------------------------------------------------------------
  if(NOT GE_FETCH_EXTERNALS)
    message(FATAL_ERROR
      "[Lua] System Lua not found (or disabled) and GE_FETCH_EXTERNALS=OFF. "
      "Either install Lua (system) or enable GE_FETCH_EXTERNALS."
    )
  endif()

  # Choose Lua version (adjust as you like)
  set(GE_LUA_VERSION "5.5.0")
  set(GE_LUA_URL     "https://www.lua.org/ftp/lua-${GE_LUA_VERSION}.tar.gz")

  # SHA256 for lua-5.5.0.tar.gz from lua.org download page (recommended)
  set(GE_LUA_SHA256  "57ccc32bbbd005cab75bcc52444052535af691789dba2b9016d5c50640d68b3d")

  FetchContent_Declare(
    lua_upstream
    URL ${GE_LUA_URL}
    URL_HASH SHA256=${GE_LUA_SHA256}
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )

  FetchContent_MakeAvailable(lua_upstream)
  FetchContent_GetProperties(lua_upstream)

  if(NOT lua_upstream_POPULATED)
    message(FATAL_ERROR "[Lua] lua_upstream not populated")
  endif()

  set(_LUA_SRC_DIR "${lua_upstream_SOURCE_DIR}/src")

  if(NOT EXISTS "${_LUA_SRC_DIR}/lua.h")
    message(FATAL_ERROR "[Lua] Unexpected layout. Can't find ${_LUA_SRC_DIR}/lua.h")
  endif()

  # All Lua sources (library) are in src/*.c.
  # Exclude lua.c and luac.c (they are CLI tools main() entry points).
  file(GLOB _lua_c_files CONFIGURE_DEPENDS "${_LUA_SRC_DIR}/*.c")
  list(REMOVE_ITEM _lua_c_files
    "${_LUA_SRC_DIR}/lua.c"
    "${_LUA_SRC_DIR}/luac.c"
  )

  add_library(ge_lua STATIC ${_lua_c_files})

  # Force C language for this library
  set_target_properties(ge_lua PROPERTIES LINKER_LANGUAGE C)

  target_include_directories(ge_lua PUBLIC
    "${_LUA_SRC_DIR}"
  )

  target_link_libraries(ge_lua PUBLIC
    ge_build_settings
  )

  # Platform defs (conservative defaults)
  if(WIN32)
    # Many Lua builds on Windows define LUA_BUILD_AS_DLL only for DLL builds;
    # we are building static, so don't set that.
  elseif(UNIX AND NOT APPLE)
    target_compile_definitions(ge_lua PRIVATE LUA_USE_POSIX=1)
  elseif(APPLE)
    # macOS is POSIX too; this is generally fine
    target_compile_definitions(ge_lua PRIVATE LUA_USE_POSIX=1)
  endif()

  # Some platforms need libm for math; keep it safe
  if(UNIX)
    target_link_libraries(ge_lua PRIVATE m)
  endif()

  add_library(lua::lua ALIAS ge_lua)
  message(STATUS "[Lua] Built from source (lua.org) version ${GE_LUA_VERSION}")
endfunction()
