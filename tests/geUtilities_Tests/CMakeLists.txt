add_executable(geUtilities_Tests
  src/main.cpp
)

# Mantener mismo layout de outputs (bin/lib) por platform/config
ge_set_output_dirs(geUtilities_Tests)

# Link a tu librería bajo prueba
target_link_libraries(geUtilities_Tests PRIVATE
  geUtilities
  ge_catch2
  ge_build_settings
)

# (Opcional) Para que salga en ctest
add_test(NAME geUtilities_Tests COMMAND geUtilities_Tests)
