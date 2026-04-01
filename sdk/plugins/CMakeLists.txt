# Add here common plugins
add_subdirectory(geRenderAPINull)
add_subdirectory(geCodec_StbImage)
add_subdirectory(geCodec_DDSLoader)
add_subdirectory(geCodec_KTXLoader)
add_subdirectory(geCodec_EXRLoader)
add_subdirectory(geCodec_Assimp)

# Windows only
if(WIN32)
  add_subdirectory(geRenderAPIDX11)
endif()

# macOS only
if(APPLE)
  #add_subdirectory(PluginMacStuff)
endif()

# Linux (no Apple)
if(UNIX AND NOT APPLE)
  #add_subdirectory(PluginX11)
endif()
