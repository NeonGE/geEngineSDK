ge_setup_lz4()
ge_setup_nlohmann_json()
ge_setup_sol2()
ge_setup_lua()
ge_setup_sfml()
ge_setup_minizip()

file(GLOB_RECURSE GE_CORE_HEADERS
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

file(GLOB_RECURSE GE_CORE_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(geCore SHARED
	${GE_CORE_HEADERS}
	${GE_CORE_SOURCES}
)

ge_set_output_dirs(geCore)
ge_enable_strict_warnings(geCore)

#Add the include directories
target_include_directories(geCore
    PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${GE_EXT_INC_DIR}
)

target_link_libraries(geCore
	PUBLIC
		ge_build_settings
)

ge_link_rttr(geCore)

target_link_libraries(geCore
	PUBLIC
		ge::sfml
		ge::minizip
	PRIVATE
		geUtilities
		ge_sol2
		ge_lua
)

get_target_property(GECORE_SOURCES geCore SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GECORE_SOURCES})

target_compile_definitions(geCore
    PRIVATE
		GE_CORE_EXPORTS=1
    #PUBLIC
    #   GE_HAS_UTILITIES=1
)

ge_set_folder(geCore      "geEngine")