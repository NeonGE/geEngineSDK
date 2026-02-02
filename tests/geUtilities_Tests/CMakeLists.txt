ge_setup_nlohmann_json()

add_executable(geUtilities_Tests
  src/core_types.cpp
  src/core_floats.cpp
  src/core_dependencies.cpp
  src/core_String.cpp

  src/math_main.cpp
  src/math_angle.cpp
  src/math_Vector2.cpp
  src/math_Vector2Half.cpp
  src/math_Vector3.cpp
  src/math_Vector4.cpp
  src/math_VectorNI.cpp
  src/math_Quaternion.cpp
  src/math_Matrix4.cpp
  src/math_Rotator.cpp
  src/math_Transform.cpp
  src/math_Bounds.cpp
  src/math_Color.cpp

  src/core_DataStream.cpp
  src/core_FileSystem.cpp
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

ge_link_rttr(geUtilities_Tests)
ge_copy_rttr_runtime(geUtilities_Tests)

# (Opcional) Para que salga en ctest
add_test(NAME geUtilities_Tests COMMAND geUtilities_Tests)
