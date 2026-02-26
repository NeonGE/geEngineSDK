include(FetchContent)

if(POLICY CMP0169)
  cmake_policy(SET CMP0169 OLD)
endif()

option(GE_USE_COMPRESSONATOR "Enable Compressonator SDK for DDS loading" ON)

if (GE_USE_COMPRESSONATOR AND NOT TARGET CMP_Framework)

  FetchContent_Declare(
    compressonator_upstream
    GIT_REPOSITORY https://github.com/GPUOpen-Tools/compressonator.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
  )

  FetchContent_GetProperties(compressonator_upstream)
  if (NOT compressonator_upstream_POPULATED)
    FetchContent_Populate(compressonator_upstream)
    FetchContent_GetProperties(compressonator_upstream)
  endif()

  if (NOT EXISTS "${compressonator_upstream_SOURCE_DIR}/cmp_framework/CMakeLists.txt")
    message(FATAL_ERROR
      "[Compressonator] Populate failed. SOURCE_DIR='${compressonator_upstream_SOURCE_DIR}'")
  endif()

  # Upstream asume estas vars para FOLDER
  set(PROJECT_FOLDER_SDK "Dependencies")
  set(PROJECT_FOLDER_SDK_LIBS "${PROJECT_FOLDER_SDK}/Compressonator")
  set(PROJECT_FOLDER_SDK_EXTERNAL "${PROJECT_FOLDER_SDK}/External Libraries")

  # Root real de compressonator para rutas
  set(COMPRESSONATOR_ROOT_PATH "${compressonator_upstream_SOURCE_DIR}" CACHE PATH "" FORCE)

  # ---- Patch cmp_framework: PROJECT_SOURCE_DIR -> COMPRESSONATOR_ROOT_PATH
  set(_cmp_fw_cmake "${compressonator_upstream_SOURCE_DIR}/cmp_framework/CMakeLists.txt")
  file(READ "${_cmp_fw_cmake}" _cmp_fw_txt)

  # Evita repatchear
  if (NOT _cmp_fw_txt MATCHES "COMPRESSONATOR_ROOT_PATH")
    string(REPLACE "PROJECT_SOURCE_DIR" "COMPRESSONATOR_ROOT_PATH" _cmp_fw_txt "${_cmp_fw_txt}")
    file(WRITE "${_cmp_fw_cmake}" "${_cmp_fw_txt}")
  endif()

  # (Opcional pero recomendable) Patch cmp_core también si usa PROJECT_SOURCE_DIR
  set(_cmp_core_cmake "${compressonator_upstream_SOURCE_DIR}/cmp_core/CMakeLists.txt")
  if (EXISTS "${_cmp_core_cmake}")
    file(READ "${_cmp_core_cmake}" _cmp_core_txt)
    if (_cmp_core_txt MATCHES "PROJECT_SOURCE_DIR" AND NOT _cmp_core_txt MATCHES "COMPRESSONATOR_ROOT_PATH")
      string(REPLACE "PROJECT_SOURCE_DIR" "COMPRESSONATOR_ROOT_PATH" _cmp_core_txt "${_cmp_core_txt}")
      file(WRITE "${_cmp_core_cmake}" "${_cmp_core_txt}")
    endif()
  endif()

  add_subdirectory("${compressonator_upstream_SOURCE_DIR}/cmp_core"
                   "${compressonator_upstream_BINARY_DIR}/cmp_core"
                   EXCLUDE_FROM_ALL)

  add_subdirectory("${compressonator_upstream_SOURCE_DIR}/cmp_framework"
                   "${compressonator_upstream_BINARY_DIR}/cmp_framework"
                   EXCLUDE_FROM_ALL)

  add_subdirectory("${compressonator_upstream_SOURCE_DIR}/applications/_plugins/cimage/dds"
                   "${compressonator_upstream_BINARY_DIR}/cmp_plugin_dds"
                   EXCLUDE_FROM_ALL)

  foreach(t CMP_Core CMP_Core_SSE CMP_Core_AVX CMP_Core_AVX512 CMP_Framework)
    if (TARGET ${t})
      set_target_properties(${t} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
  endforeach()

endif()