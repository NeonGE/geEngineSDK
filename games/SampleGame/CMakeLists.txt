add_executable(SampleGame
	main.cpp
)

ge_set_output_dirs(SampleGame)
ge_enable_strict_warnings(SampleGame)

#Add the include directories
target_include_directories(SampleGame
    PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
		${GE_EXT_INC_DIR}
)

target_link_libraries(SampleGame
	PUBLIC
		ge_build_settings
		geUtilities
		geCore
		ge_sol2
		ge_lua
)

ge_link_rttr(SampleGame)

get_target_property(SAMPLEGAME_SOURCES SampleGame SOURCES)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SAMPLEGAME_SOURCES})

ge_set_folder(SampleGame, "Games")
