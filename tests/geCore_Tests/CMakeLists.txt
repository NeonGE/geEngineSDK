ge_setup_nlohmann_json()

add_executable(geCore_Tests
  src/core_VirtualFileSystem.cpp
)

# Mantener mismo layout de outputs (bin/lib) por platform/config
ge_set_output_dirs(geCore_Tests)
ge_enable_strict_warnings(geCore_Tests)

# Link a tu librería bajo prueba
target_link_libraries(geCore_Tests
	PRIVATE
		geUtilities
		geCore
		ge_json
		ge_build_settings
)

target_link_libraries(geCore_Tests PRIVATE Catch2::Catch2WithMain)

ge_link_rttr(geCore_Tests)
#ge_copy_rttr_runtime(geCore_Tests)

# (Opcional) Para que salga en ctest
catch_discover_tests(geCore_Tests)
