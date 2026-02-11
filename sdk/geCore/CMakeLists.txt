ge_setup_lz4()
ge_setup_nlohmann_json()
ge_setup_sol2()
ge_setup_lua()
ge_setup_sfml()
ge_setup_zlib()
ge_setup_minizip()

add_library(geCore SHARED
	include/geCodecManager.h                                                     
	include/geCoreBaseApp.h                                                      
	include/geCoreFeatures.h                                                     
	include/geDiskFileSystem.h                                                   
	include/geFileTracker.h                                                      
	include/geGameConfig.h                                                       
	include/geGraphicsInterfaces.h                                               
	include/geGraphicsTypes.h                                                    
	include/geICodec.h                                                           
	include/geInputLayout.h                                                      
	include/geMountManager.h                                                     
	include/gePrerequisitesCore.h                                                
	include/geRasterizationState.h                                               
	include/geRenderAPI.h                                                        
	include/geResource.h                                                         
	include/geSamplerState.h                                                     
	include/geShader.h                                                           
	include/geTexture.h                                                          
	include/geTextureManager.h                                                   
	include/geVertexDeclaration.h                                                
	include/geZipDataStream.h                                                    
	include/geZipFileSystem.h  
	
	src/geCodecManager.cpp
	src/geCoreBaseApp.cpp
	src/geDiskFileSystem.cpp
	src/geFileTracker.cpp
	src/geGameConfig.cpp
	src/geICodec.cpp
	src/geInputLayout.cpp
	src/geMountManager.cpp
	src/geRasterizationState.cpp
	src/geRenderAPI.cpp
	src/geSamplerState.cpp
	src/geShader.cpp
	src/geTexture.cpp
	src/geTextureManager.cpp
	src/geVertexDeclaration.cpp
	src/geZipDataStream.cpp
	src/geZipFileSystem.cpp
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

#if(WIN32)
#target_link_libraries(geCore
#	PRIVATE
#		$<$<PLATFORM_ID:Windows>:Shell32>
#		$<$<PLATFORM_ID:Windows>:DbgHelp>
#		$<$<PLATFORM_ID:Windows>:IPHLPAPI>
#		$<$<PLATFORM_ID:Windows>:Rpcrt4>
#)
#endif()
#
#if(UNIX AND NOT APPLE)
#find_package(ICU REQUIRED COMPONENTS uc i18n data)
#
#target_link_libraries(geCore
#	PRIVATE
#		ICU::uc
#		ICU::i18n
#		ICU::data
#)
#endif()

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