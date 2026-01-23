include(FetchContent)

# -----------------------------
# Config
# -----------------------------
set(GE_LZ4_GIT_REPO https://github.com/lz4/lz4.git)
set(GE_LZ4_GIT_TAG  v1.9.4)

# -----------------------------
# Declare
# -----------------------------
FetchContent_Declare(
  lz4_upstream
  GIT_REPOSITORY ${GE_LZ4_GIT_REPO}
  GIT_TAG        ${GE_LZ4_GIT_TAG}
  GIT_SHALLOW    TRUE
)

# -----------------------------
# Make available (modern)
# -----------------------------
FetchContent_MakeAvailable(lz4_upstream)

# -----------------------------
# Get source dir
# -----------------------------
FetchContent_GetProperties(lz4_upstream)
if(NOT lz4_upstream_POPULATED)
  message(FATAL_ERROR "lz4_upstream not populated")
endif()

# -----------------------------
# Paths in upstream repo
# -----------------------------
set(GE_LZ4_SRC_IN "${lz4_upstream_SOURCE_DIR}/lib/lz4.c")
set(GE_LZ4_HDR_IN "${lz4_upstream_SOURCE_DIR}/lib/lz4.h")

# -----------------------------
# Destinations in YOUR project
# -----------------------------
set(GE_LZ4_DST_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/externals")
set(GE_LZ4_DST_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/externals")

file(MAKE_DIRECTORY "${GE_LZ4_DST_SRC_DIR}" "${GE_LZ4_DST_INC_DIR}")

set(GE_LZ4_SRC_OUT "${GE_LZ4_DST_SRC_DIR}/lz4.c")
set(GE_LZ4_HDR_OUT "${GE_LZ4_DST_INC_DIR}/lz4.h")

# -----------------------------
# Copy (only if changed)
# -----------------------------
file(COPY_FILE "${GE_LZ4_SRC_IN}" "${GE_LZ4_SRC_OUT}" ONLY_IF_DIFFERENT)
file(COPY_FILE "${GE_LZ4_HDR_IN}" "${GE_LZ4_HDR_OUT}" ONLY_IF_DIFFERENT)

# -----------------------------
# Export to parent scope
# -----------------------------
set(GE_LZ4_SOURCES
    "${GE_LZ4_SRC_OUT}"
    "${GE_LZ4_HDR_OUT}"
    PARENT_SCOPE
)
