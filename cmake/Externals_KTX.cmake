include(FetchContent)

option(GE_FETCH_EXTERNALS "Download externals" ON)

function(ge_setup_ktx)
  if(TARGET ktx)
    return()
  endif()

  if(NOT GE_FETCH_EXTERNALS)
    message(FATAL_ERROR "[KTX] GE_FETCH_EXTERNALS=OFF and KTX is not available.")
  endif()

  set(GE_KTX_GIT_REPO "https://github.com/KhronosGroup/KTX-Software.git")
  set(GE_KTX_GIT_TAG  "v4.4.2") # pin

  FetchContent_Declare(
    ktx_software
    GIT_REPOSITORY ${GE_KTX_GIT_REPO}
    GIT_TAG        ${GE_KTX_GIT_TAG}
    GIT_SHALLOW    TRUE
  )

  # =========================
  # Force STATIC build
  # =========================
  set(BUILD_SHARED_LIBS           OFF CACHE BOOL "" FORCE)
  set(KTX_FEATURE_STATIC_LIBRARY  ON  CACHE BOOL "" FORCE)

  # Disable everything we don't need
  set(KTX_FEATURE_TESTS           OFF CACHE BOOL "" FORCE)
  set(KTX_FEATURE_TOOLS           OFF CACHE BOOL "" FORCE)
  set(KTX_FEATURE_DOC             OFF CACHE BOOL "" FORCE)
  set(KTX_FEATURE_INSTALL         OFF CACHE BOOL "" FORCE)

  # No GL/Vulkan helpers needed for DX11 loader
  set(KTX_FEATURE_VULKAN          OFF CACHE BOOL "" FORCE)
  set(KTX_FEATURE_OPENGL          OFF CACHE BOOL "" FORCE)

  FetchContent_MakeAvailable(ktx_software)

  if(NOT TARGET ktx)
    message(FATAL_ERROR "[KTX] Target 'ktx' was not created.")
  endif()

  # Marcar includes como SYSTEM para no heredar warnings
  get_target_property(_ktx_inc_dirs ktx INTERFACE_INCLUDE_DIRECTORIES)
  if(_ktx_inc_dirs)
    set_target_properties(ktx PROPERTIES
      INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${_ktx_inc_dirs}"
    )
  endif()

  set_target_properties(astcenc-avx2-static PROPERTIES FOLDER "Dependencies/Khronos")
  set_target_properties(ktx PROPERTIES FOLDER "Dependencies/Khronos")
  set_target_properties(ktx_read PROPERTIES FOLDER "Dependencies/Khronos")
  set_target_properties(ktx_version PROPERTIES FOLDER "Dependencies/Khronos")
  set_target_properties(obj_basisu_cbind PROPERTIES FOLDER "Dependencies/Khronos")
  set_target_properties(objUtil PROPERTIES FOLDER "Dependencies/Khronos")

endfunction()