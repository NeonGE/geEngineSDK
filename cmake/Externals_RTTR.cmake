include(FetchContent)

# CMake 4.2: FetchContent_Populate(rttr) con details declarados está deprecado.
# Si no quieres warning y aún necesitas "populate+patch", pon CMP0169 en OLD.
if(POLICY CMP0169)
  cmake_policy(SET CMP0169 OLD)
endif()

# =========================
# RTTR configuration
# =========================
set(BUILD_BENCHMARKS        OFF CACHE BOOL "" FORCE)
set(BUILD_DOCUMENTATION     OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES          OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS        OFF CACHE BOOL "" FORCE)
set(BUILD_WEBSITE_DOCU      OFF CACHE BOOL "" FORCE)
set(USE_PCH                 OFF CACHE BOOL "" FORCE)

set(BUILD_WITH_RTTI         OFF CACHE BOOL "" FORCE)

# Según tu preferencia: sin installer ni package dentro del engine
set(BUILD_INSTALLER         OFF CACHE BOOL "" FORCE)
set(BUILD_PACKAGE           OFF CACHE BOOL "" FORCE)

# (Opcional) si no quieres dll, apaga dynamic. Si sí quieres dll, prende.
# Tu screenshot tenía dynamic ON, pero aquí lo dejo ON por default.
set(BUILD_RTTR_DYNAMIC      ON  CACHE BOOL "" FORCE)
set(BUILD_STATIC            OFF CACHE BOOL "" FORCE)

FetchContent_Declare(rttr
  GIT_REPOSITORY https://github.com/rttrorg/rttr.git
  GIT_TAG        master
  GIT_SHALLOW    TRUE
)

FetchContent_GetProperties(rttr)
if(NOT rttr_POPULATED)
  FetchContent_Populate(rttr)

  # =========================
  # Patch: cmake_minimum_required(...) -> 3.5
  # =========================
  set(_rttr_cmake "${rttr_SOURCE_DIR}/CMakeLists.txt")
  if(NOT EXISTS "${_rttr_cmake}")
    message(FATAL_ERROR "RTTR CMakeLists.txt not found at: ${_rttr_cmake}")
  endif()

  file(READ "${_rttr_cmake}" _txt)
  set(_before "${_txt}")

  # Soporta:
  #   cmake_minimum_required(VERSION 2.8.12)
  #   cmake_minimum_required ( VERSION 2.8 )
  #   cmake_minimum_required(VERSION 3.2...3.20)
  string(REGEX REPLACE
    "cmake_minimum_required[ \t\r\n]*\\([ \t\r\n]*VERSION[ \t\r\n]+[0-9]+\\.[0-9]+(\\.[0-9]+)?([ \t\r\n]*\\.\\.\\.[ \t\r\n]*[0-9]+\\.[0-9]+(\\.[0-9]+)?)?[ \t\r\n]*\\)"
    "cmake_minimum_required(VERSION 3.5)"
    _txt
    "${_txt}"
  )

  if(_txt STREQUAL _before)
    # Ya estaba parcheado o cambió el formato: si ya es >= 3.5 no falles.
    string(REGEX MATCH
      "cmake_minimum_required[ \t\r\n]*\\([ \t\r\n]*VERSION[ \t\r\n]+([0-9]+)\\.([0-9]+)"
      _m
      "${_before}"
    )

    if(_m)
      set(_maj "${CMAKE_MATCH_1}")
      set(_min "${CMAKE_MATCH_2}")

      if(_maj GREATER 3 OR (_maj EQUAL 3 AND _min GREATER_EQUAL 5))
        message(STATUS "RTTR cmake_minimum_required already >= 3.5 (${_maj}.${_min}), patch not needed.")
      else()
        message(FATAL_ERROR
          "RTTR cmake_minimum_required is ${_maj}.${_min} (< 3.5) but patch didn't match.\n"
          "Open: ${_rttr_cmake}\n"
          "Paste the exact cmake_minimum_required(...) line here."
        )
      endif()
    else()
      message(FATAL_ERROR
        "RTTR patch failed: could not locate cmake_minimum_required(VERSION ...) in:\n"
        "${_rttr_cmake}\n"
        "Paste the exact cmake_minimum_required(...) line here."
      )
    endif()
  else()
    file(WRITE "${_rttr_cmake}" "${_txt}")
  endif()

  # =========================
  # Add RTTR
  # =========================
  add_subdirectory("${rttr_SOURCE_DIR}" "${rttr_BINARY_DIR}")
  ge_rttr_silence_gcc_warnings_for_thirdparty()
endif()
