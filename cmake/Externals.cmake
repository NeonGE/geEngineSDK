include(${CMAKE_SOURCE_DIR}/cmake/Externals_LZ4.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Externals_Json.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Externals_Catch2.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Link_RTTR.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Warnings.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Externals_Sol2.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Externals_Lua.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Externals_SFML.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Externals_ZlibNG.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Externals_MiniZip.cmake)

function(ge_setup_test_dependencies)
  ge_setup_nlohmann_json()
endfunction()

function(ge_copy_runtime_dlls targetName)
  if(WIN32 AND CMAKE_VERSION VERSION_GREATER_EQUAL "3.21")
    add_custom_command(TARGET ${targetName} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:${targetName}>
        $<TARGET_FILE_DIR:${targetName}>
      COMMAND_EXPAND_LISTS
      VERBATIM
    )
  endif()
endfunction()