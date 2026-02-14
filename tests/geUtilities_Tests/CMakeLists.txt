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
  src/core_Compression.cpp
  src/core_Event.cpp
  src/core_Threading.cpp
  src/core_ThreadPool.cpp
  src/core_TaskScheduler.cpp
  src/core_Time.cpp
  src/core_UUID.cpp
  src/core_Random.cpp
  src/core_SmartEnum.cpp
)

# Mantener mismo layout de outputs (bin/lib) por platform/config
ge_set_output_dirs(geUtilities_Tests)
ge_enable_strict_warnings(geUtilities_Tests)

# Link a tu librería bajo prueba
target_link_libraries(geUtilities_Tests
	PRIVATE
		geUtilities
		ge_json
		ge_build_settings
)

target_link_libraries(geUtilities_Tests PRIVATE Catch2::Catch2WithMain)

ge_link_rttr(geUtilities_Tests)
#ge_copy_rttr_runtime(geUtilities_Tests)
ge_copy_runtime_dlls(geUtilities_Tests)

# (Opcional) Para que salga en ctest
#add_test(NAME geUtilities_Tests COMMAND geUtilities_Tests)
catch_discover_tests(geUtilities_Tests)

ge_set_folder(geUtilities_Tests "geEngine/Tests")