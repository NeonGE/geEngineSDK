# Add here common plugins
# add_subdirectory(PluginCommon)
add_subdirectory(geRenderAPINull)


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
