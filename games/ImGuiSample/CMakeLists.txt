ge_setup_imgui()
ge_setup_imgui_backend()

add_executable(ImGuiSample
	geImGuiPlatform.h
	geImGuiPlatform.cpp
	main.cpp
)

ge_set_output_dirs(ImGuiSample)
ge_enable_strict_warnings(ImGuiSample)

#Add the include directories
target_include_directories(ImGuiSample
    PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
		${GE_EXT_INC_DIR}
)

target_link_libraries(ImGuiSample
	PUBLIC
		ge_build_settings
		geUtilities
		geCore
		ge_sol2
		ge_lua
	PRIVATE
		ge::imgui_core
		ge::imgui_backend
)

ge_link_rttr(ImGuiSample)

get_target_property(IMGUISAMPLE_SOURCES ImGuiSample SOURCES)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${IMGUISAMPLE_SOURCES})

set_target_properties(ImGuiSample PROPERTIES
	FOLDER "Games"
)
