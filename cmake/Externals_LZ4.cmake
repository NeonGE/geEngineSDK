include(FetchContent)

function(ge_setup_lz4)
  if(TARGET ge_lz4)
    return()
  endif()

  set(GE_LZ4_GIT_REPO "https://github.com/lz4/lz4.git")
  set(GE_LZ4_GIT_TAG  "v1.9.4")

  FetchContent_Declare(
    lz4_upstream
    GIT_REPOSITORY ${GE_LZ4_GIT_REPO}
    GIT_TAG        ${GE_LZ4_GIT_TAG}
    GIT_SHALLOW    TRUE
  )

  FetchContent_MakeAvailable(lz4_upstream)
  FetchContent_GetProperties(lz4_upstream)

  if(NOT lz4_upstream_POPULATED)
    message(FATAL_ERROR "[LZ4] lz4_upstream not populated")
  endif()

  set(_SRC "${lz4_upstream_SOURCE_DIR}/lib/lz4.c")
  set(_HDR "${lz4_upstream_SOURCE_DIR}/lib/lz4.h")

  if(NOT EXISTS "${_SRC}" OR NOT EXISTS "${_HDR}")
    message(FATAL_ERROR "[LZ4] No encontré lib/lz4.c o lib/lz4.h en: ${lz4_upstream_SOURCE_DIR}")
  endif()

  add_library(ge_lz4 STATIC "${_SRC}" "${_HDR}")
  set_source_files_properties("${_SRC}" PROPERTIES LANGUAGE C)

  target_include_directories(ge_lz4 PUBLIC
    "${lz4_upstream_SOURCE_DIR}/lib"
  )

  target_link_libraries(ge_lz4 PUBLIC ge_build_settings)

  target_compile_definitions(ge_lz4 PUBLIC
    LZ4_HEAPMODE=1
  )

  # Opcional: para IDE
  source_group(TREE "${lz4_upstream_SOURCE_DIR}" FILES "${_SRC}" "${_HDR}")
endfunction()
