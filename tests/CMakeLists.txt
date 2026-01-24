if(NOT BUILD_TESTING)
  return()
endif()

ge_setup_catch2_amalgamated()

add_subdirectory(geUtilities_Tests)
