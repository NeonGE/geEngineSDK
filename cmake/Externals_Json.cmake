# nlohmann/json - header-only
# Target: ge_json (INTERFACE)
# Include: <nlohmann/json.hpp>

option(GE_FETCH_EXTERNALS "Download externals (Catch2/json/etc.)" ON)

function(ge_setup_nlohmann_json)
  if(TARGET ge_json)
    return()
  endif()

  # URL del release asset (1 archivo)
  set(_JSON_HPP_URL "https://github.com/nlohmann/json/releases/download/v3.12.0/json.hpp")

  # Donde lo guardamos en el build tree (no ensucia el repo)
  set(_DST_DIR "${CMAKE_BINARY_DIR}/_externals/nlohmann_json/include/nlohmann")
  file(MAKE_DIRECTORY "${_DST_DIR}")

  set(_HPP_OUT "${_DST_DIR}/json.hpp")

  if(GE_FETCH_EXTERNALS)
    # Descarga directa al destino
    file(DOWNLOAD "${_JSON_HPP_URL}" "${_HPP_OUT}" SHOW_PROGRESS STATUS st_json)
    list(GET st_json 0 code_json)
    if(NOT code_json EQUAL 0)
      message(FATAL_ERROR "No se pudo descargar nlohmann/json.hpp: ${st_json}")
    endif()
  else()
    # Modo offline: exige que ya exista (por ejemplo si lo copiaste manualmente al build o lo cacheas)
    if(NOT EXISTS "${_HPP_OUT}")
      message(FATAL_ERROR
        "GE_FETCH_EXTERNALS=OFF pero no existe:\n"
        "  ${_HPP_OUT}\n"
        "Activa GE_FETCH_EXTERNALS o provee ese archivo en el build cache."
      )
    endif()
  endif()

  add_library(ge_json INTERFACE)

  # Para incluir: #include <nlohmann/json.hpp>
  target_include_directories(ge_json INTERFACE
    "${CMAKE_BINARY_DIR}/_externals/nlohmann_json/include"
  )

  # Que herede tus settings comunes (C++20, RTTI off, etc.)
  target_link_libraries(ge_json INTERFACE ge_build_settings)

  # (Opcional) Para que IDEs lo muestren bajo un target
  target_sources(ge_json INTERFACE "${_HPP_OUT}")
  source_group(TREE "${CMAKE_BINARY_DIR}/_externals" FILES "${_HPP_OUT}")
endfunction()
