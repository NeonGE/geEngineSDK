include(FetchContent)

# Toggle para poder apagar descargas (CI offline / ya vendorizado)
option(GE_FETCH_EXTERNALS "Download externals (Catch2, etc.)" ON)

# Crea un target: ge_catch2 (STATIC) + include dirs
# Recomendación: llamar esto solo si BUILD_TESTING está ON.
function(ge_setup_catch2_amalgamated)
  if(TARGET ge_catch2)
    return()
  endif()

  # URLs directas (assets del release)
  set(_CATCH2_CPP_URL "https://github.com/catchorg/Catch2/releases/download/v3.12.0/catch_amalgamated.cpp")
  set(_CATCH2_HPP_URL "https://github.com/catchorg/Catch2/releases/download/v3.12.0/catch_amalgamated.hpp")

  # Dónde lo guardas DENTRO del repo (para que lo veas en el IDE / puedas commitear)
  set(_DST_INC_DIR "${CMAKE_SOURCE_DIR}/tests/externals/include")
  set(_DST_SRC_DIR "${CMAKE_SOURCE_DIR}/tests/externals/src")
  file(MAKE_DIRECTORY "${_DST_INC_DIR}" "${_DST_SRC_DIR}")

  set(_CATCH2_CPP_OUT "${_DST_SRC_DIR}/catch_amalgamated.cpp")
  set(_CATCH2_HPP_OUT "${_DST_INC_DIR}/catch_amalgamated.hpp")

  if(GE_FETCH_EXTERNALS)
    # Cache dentro del build
    set(_DL_DIR "${CMAKE_BINARY_DIR}/_downloads/externals_catch2")
    file(MAKE_DIRECTORY "${_DL_DIR}")

    set(_CATCH2_CPP_DL "${_DL_DIR}/catch_amalgamated.cpp")
    set(_CATCH2_HPP_DL "${_DL_DIR}/catch_amalgamated.hpp")

    file(DOWNLOAD "${_CATCH2_CPP_URL}" "${_CATCH2_CPP_DL}" SHOW_PROGRESS STATUS st_cpp)
    file(DOWNLOAD "${_CATCH2_HPP_URL}" "${_CATCH2_HPP_DL}" SHOW_PROGRESS STATUS st_hpp)

    list(GET st_cpp 0 code_cpp)
    list(GET st_hpp 0 code_hpp)
    if(NOT code_cpp EQUAL 0)
      message(FATAL_ERROR "No se pudo descargar Catch2 CPP: ${st_cpp}")
    endif()
    if(NOT code_hpp EQUAL 0)
      message(FATAL_ERROR "No se pudo descargar Catch2 HPP: ${st_hpp}")
    endif()

    # Copia al repo (solo si cambió)
    file(COPY_FILE "${_CATCH2_CPP_DL}" "${_CATCH2_CPP_OUT}" ONLY_IF_DIFFERENT)
    file(COPY_FILE "${_CATCH2_HPP_DL}" "${_CATCH2_HPP_OUT}" ONLY_IF_DIFFERENT)
  else()
    # Modo offline: exige que ya exista en el repo
    if(NOT EXISTS "${_CATCH2_CPP_OUT}" OR NOT EXISTS "${_CATCH2_HPP_OUT}")
      message(FATAL_ERROR
        "GE_FETCH_EXTERNALS=OFF pero no encuentro Catch2 en:\n"
        "  ${_CATCH2_CPP_OUT}\n"
        "  ${_CATCH2_HPP_OUT}\n"
        "Activa GE_FETCH_EXTERNALS o agrega esos archivos al repo."
      )
    endif()
  endif()

  # Target reutilizable para tests (compila el .cpp una vez)
  add_library(ge_catch2 STATIC
    "${_CATCH2_CPP_OUT}"
    "${_CATCH2_HPP_OUT}"
  )

  target_include_directories(ge_catch2 PUBLIC
    "${_DST_INC_DIR}"
  )

  # Para que todos tus tests hereden C++20, RTTI off, etc.
  target_link_libraries(ge_catch2 PUBLIC ge_build_settings)

  # Opcional: organización en IDE
  get_target_property(_S ge_catch2 SOURCES)
  source_group(TREE "${CMAKE_SOURCE_DIR}/tests" FILES ${_S})
endfunction()
