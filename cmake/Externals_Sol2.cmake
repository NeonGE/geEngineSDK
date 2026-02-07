include(FetchContent)

function(ge_setup_sol2)
  if(TARGET ge_sol2)
    return()
  endif()

  # CMake 4.2+: silencia deprecación de Populate (CMP0169)
  if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
  endif()

  set(GE_SOL2_GIT_REPO "https://github.com/ThePhD/sol2.git")
  set(GE_SOL2_GIT_TAG  "v3.5.0")

  FetchContent_Declare(
    sol2_upstream
    GIT_REPOSITORY ${GE_SOL2_GIT_REPO}
    GIT_TAG        ${GE_SOL2_GIT_TAG}
    GIT_SHALLOW    TRUE
  )

  FetchContent_GetProperties(sol2_upstream)
  if(NOT sol2_upstream_POPULATED)
    FetchContent_Populate(sol2_upstream)
  endif()

  if(NOT EXISTS "${sol2_upstream_SOURCE_DIR}/include/sol/sol.hpp")
    message(FATAL_ERROR "[sol2] Can't find include/sol/sol.hpp in: ${sol2_upstream_SOURCE_DIR}")
  endif()

  add_library(ge_sol2 INTERFACE)
  target_include_directories(ge_sol2 INTERFACE "${sol2_upstream_SOURCE_DIR}/include")
  target_link_libraries(ge_sol2 INTERFACE ge_build_settings)

  add_library(sol2::sol2 ALIAS ge_sol2)
endfunction()
