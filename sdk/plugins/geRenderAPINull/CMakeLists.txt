set(PLUGIN_NAME geRenderAPINull)

# -----------------------------
# Gather sources
# -----------------------------
file(GLOB_RECURSE PLUGIN_HEADERS
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

file(GLOB_RECURSE PLUGIN_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(${PLUGIN_NAME} SHARED
	${PLUGIN_HEADERS}
	${PLUGIN_SOURCES}
)

ge_set_output_dirs(${PLUGIN_NAME})
ge_enable_strict_warnings(${PLUGIN_NAME})

#Add the include directories
target_include_directories(${PLUGIN_NAME}
    PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${GE_EXT_INC_DIR}
)

target_link_libraries(${PLUGIN_NAME}
	PUBLIC
		ge_build_settings
		geUtilities
		geCore
		ge_sol2
		ge_lua
)

#if(WIN32)
#target_link_libraries(${PLUGIN_NAME}
#	PRIVATE
#		dxguid
#		dxgi
#		d3d11
#		d3dcompiler)
#endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
             FILES ${PLUGIN_HEADERS} ${PLUGIN_SOURCES})

set_target_properties(${PLUGIN_NAME}
    PROPERTIES
        FOLDER "geEngine/Plugins"
)
