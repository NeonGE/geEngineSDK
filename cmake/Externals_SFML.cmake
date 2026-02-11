include(FetchContent)

option(GE_USE_SYSTEM_SFML  "Prefer system SFML if available" ON)
option(GE_FETCH_EXTERNALS  "Download externals (sfml/minizip/etc.)" ON)

function(ge_setup_sfml)
  if(TARGET ge_sfml)
    return()
  endif()

  # -----------------------------
  # Try system SFML first
  # -----------------------------
  if(GE_USE_SYSTEM_SFML)
    # Many distros export SFML targets as SFML::Graphics, etc.
    find_package(SFML 3 QUIET COMPONENTS System Window Graphics Audio Network)
    if(SFML_FOUND)
      add_library(ge_sfml INTERFACE)
      target_link_libraries(ge_sfml INTERFACE
        SFML::System SFML::Window SFML::Graphics SFML::Audio SFML::Network
        ge_build_settings
      )
      add_library(ge::sfml ALIAS ge_sfml)
      message(STATUS "[SFML] Using system SFML")
      return()
    endif()
  endif()

  # -----------------------------
  # FetchContent fallback
  # -----------------------------
  if(NOT GE_FETCH_EXTERNALS)
    message(FATAL_ERROR
      "[SFML] System SFML not found (or disabled) and GE_FETCH_EXTERNALS=OFF. "
      "Install SFML or enable GE_FETCH_EXTERNALS."
    )
  endif()

  set(GE_SFML_GIT_REPO "https://github.com/SFML/SFML.git")
  set(GE_SFML_GIT_TAG  "3.0.2")
  set(BUILD_SHARED_LIBS  ON CACHE BOOL "" FORCE)

  FetchContent_Declare(
    sfml_upstream
    GIT_REPOSITORY ${GE_SFML_GIT_REPO}
    GIT_TAG        ${GE_SFML_GIT_TAG}
    GIT_SHALLOW    TRUE
  )

  FetchContent_MakeAvailable(sfml_upstream)

  # SFML exports SFML:: targets after add_subdirectory
  add_library(ge_sfml INTERFACE)
  target_link_libraries(ge_sfml INTERFACE
    SFML::System SFML::Window SFML::Graphics SFML::Audio SFML::Network
    ge_build_settings
  )
  add_library(ge::sfml ALIAS ge_sfml)

  message(STATUS "[SFML] Fetched SFML ${GE_SFML_GIT_TAG}")
endfunction()
