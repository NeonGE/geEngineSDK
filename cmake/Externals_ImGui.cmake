include(FetchContent)

option(GE_FETCH_EXTERNALS "Download externals" ON)
option(GE_IMGUI_BUILD_DEMO "Build imgui_demo.cpp as a separate library" OFF)

function(ge_setup_imgui)
  # -----------------------------
  # Core target
  # -----------------------------
  if(NOT TARGET ge_imgui_core)

    if(NOT GE_FETCH_EXTERNALS)
      message(FATAL_ERROR "[ImGui] GE_FETCH_EXTERNALS=OFF and ImGui is not available.")
    endif()

    set(GE_IMGUI_GIT_REPO "https://github.com/ocornut/imgui.git")
    set(GE_IMGUI_GIT_TAG  "v1.92.5-docking") # pin; cámbialo cuando quieras

    FetchContent_Declare(
      imgui_upstream
      GIT_REPOSITORY ${GE_IMGUI_GIT_REPO}
      GIT_TAG        ${GE_IMGUI_GIT_TAG}
      GIT_SHALLOW    TRUE
    )

    # Download only (do not run upstream CMake)
    if(POLICY CMP0169)
      cmake_policy(PUSH)
      cmake_policy(SET CMP0169 OLD)
    endif()

    FetchContent_GetProperties(imgui_upstream)
    if(NOT imgui_upstream_POPULATED)
      FetchContent_Populate(imgui_upstream)
    endif()

    if(POLICY CMP0169)
      cmake_policy(POP)
    endif()

    set(_IMGUI_DIR "${imgui_upstream_SOURCE_DIR}")

    # Core compilation units (NO backends)
    set(_IMGUI_CORE_SOURCES
      "${_IMGUI_DIR}/imgui.cpp"
      "${_IMGUI_DIR}/imgui_draw.cpp"
      "${_IMGUI_DIR}/imgui_tables.cpp"
      "${_IMGUI_DIR}/imgui_widgets.cpp"
      "${_IMGUI_DIR}/imgui_demo.cpp"
    )

    add_library(ge_imgui_core STATIC ${_IMGUI_CORE_SOURCES})

    target_include_directories(ge_imgui_core PUBLIC
      "${_IMGUI_DIR}"
    )

    if(TARGET ge_build_settings)
      target_link_libraries(ge_imgui_core PUBLIC ge_build_settings)
    endif()

    add_library(ge::imgui_core ALIAS ge_imgui_core)

    set_target_properties(ge_imgui_core PROPERTIES
      FOLDER "Dependencies/ImGui"
    )
  endif()

  # -----------------------------
  # Optional demo (separate target)
  # -----------------------------
  if(GE_IMGUI_BUILD_DEMO AND NOT TARGET ge_imgui_demo)
    # ge_imgui_demo contains only imgui_demo.cpp and links the core
    # (useful to keep core lean).
    get_target_property(_IMGUI_DIR ge_imgui_core SOURCE_DIR)
    # SOURCE_DIR no siempre está; así que mejor reconstruimos la ruta desde include dir:
    # pero para simplicidad, tomamos el include dir público del core:
    get_target_property(_inc ge_imgui_core INTERFACE_INCLUDE_DIRECTORIES)
    list(GET _inc 0 _IMGUI_DIR_FROM_INC)

    add_library(ge_imgui_demo STATIC
      "${_IMGUI_DIR_FROM_INC}/imgui_demo.cpp"
    )

    target_link_libraries(ge_imgui_demo PUBLIC ge::imgui_core)

    add_library(ge::imgui_demo ALIAS ge_imgui_demo)

    set_target_properties(ge_imgui_demo PROPERTIES
      FOLDER "Dependencies/ImGui"
    )
  endif()
endfunction()

option(GE_IMGUI_ENABLE_BACKEND_DX11_WIN32 "Enable ImGui Win32+DX11 backends" OFF)

function(ge_setup_imgui_dx11_win32_backend)
  if(TARGET ge_imgui_backend_dx11_win32)
    return()
  endif()

  # Ensure core exists (and the repo is populated)
  ge_setup_imgui()

  if(NOT WIN32)
    message(FATAL_ERROR "[ImGui] DX11+Win32 backend is Windows-only.")
  endif()

  # Get ImGui include dir from core
  get_target_property(_inc ge_imgui_core INTERFACE_INCLUDE_DIRECTORIES)
  list(GET _inc 0 _IMGUI_DIR)

  add_library(ge_imgui_backend_dx11_win32 STATIC
    "${_IMGUI_DIR}/backends/imgui_impl_dx11.cpp"
    "${_IMGUI_DIR}/backends/imgui_impl_win32.cpp"
  )

  target_include_directories(ge_imgui_backend_dx11_win32 PUBLIC
    "${_IMGUI_DIR}"
    "${_IMGUI_DIR}/backends"
  )

  target_link_libraries(ge_imgui_backend_dx11_win32 PUBLIC
    ge::imgui_core
    ge_build_settings
  )

  # DX11 deps
  target_link_libraries(ge_imgui_backend_dx11_win32 PRIVATE
    d3d11
    dxgi
    dxguid
  )

  add_library(ge::imgui_backend_dx11_win32 ALIAS ge_imgui_backend_dx11_win32)

  set_target_properties(ge_imgui_backend_dx11_win32 PROPERTIES
    FOLDER "Dependencies/ImGui"
  )
endfunction()
