ge_setup_nlohmann_json()

add_executable(geUtilities_Tests
  src/core_types.cpp
  src/core_floats.cpp
  src/math_main.cpp
  src/math_Vector2.cpp
  src/math_Vector2Half.cpp
  src/math_Vector3.cpp
  src/math_Vector4.cpp
  src/math_VectorNI.cpp
)

# Mantener mismo layout de outputs (bin/lib) por platform/config
ge_set_output_dirs(geUtilities_Tests)

# Link a tu librería bajo prueba
target_link_libraries(geUtilities_Tests
	PRIVATE
		geUtilities
		ge_catch2
		ge_json
		ge_build_settings
)

# (Opcional) Para que salga en ctest
add_test(NAME geUtilities_Tests COMMAND geUtilities_Tests)
