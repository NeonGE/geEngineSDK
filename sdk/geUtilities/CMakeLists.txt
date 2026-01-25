ge_setup_lz4()

# ---- Config: URLs del release de Catch2 (assets directos) ----
set(GE_JSON_HPP_URL "https://github.com/nlohmann/json/releases/download/v3.12.0/json.hpp")

# ---- Destinos dentro de tu repo ----
set(GE_EXT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/externals")
set(GE_EXT_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/externals")
file(MAKE_DIRECTORY "${GE_EXT_SRC_DIR}")
file(MAKE_DIRECTORY "${GE_EXT_INC_DIR}")

# ---- Descarga a un cache dentro del build ----
set(GE_DL_DIR "${CMAKE_BINARY_DIR}/_downloads/geUtilities_Externals")
file(MAKE_DIRECTORY "${GE_DL_DIR}")

set(GE_JSON_DL_HPP "${GE_DL_DIR}/json.hpp")

file(DOWNLOAD "${GE_JSON_HPP_URL}" "${GE_JSON_DL_HPP}" SHOW_PROGRESS STATUS js_hpp)
list(GET js_hpp 0 code_js_hpp)
if(NOT code_js_hpp EQUAL 0)
  message(FATAL_ERROR "No se pudo descargar HPP JSON: ${js_hpp}")
endif()

# ---- Copia al arbol de fuentes (repo) ----
set(GE_EXT_JSON_HPP "${GE_EXT_INC_DIR}/json.hpp")

file(COPY_FILE "${GE_JSON_DL_HPP}" "${GE_EXT_JSON_HPP}" ONLY_IF_DIFFERENT)

add_library(geUtilities SHARED
	include/geBitmapWriter.h
	include/geBox.h
	include/geBox2D.h
	include/geBox2DI.h
	include/geBoxSphereBounds.h
	include/geCapsuleShape.h
	include/geColor.h
	include/geCompression.h
	include/geCrashHandler.h
	include/geDataStream.h
	include/geDebug.h
	include/geDegree.h
	include/geDynArray.h
	include/geDynLib.h
	include/geDynLibManager.h
	include/geEnumClassHash.h
	include/geEvent.h
	include/geException.h
	include/geFileSystem.h
	include/geFlags.h
	include/geFloat10.h
	include/geFloat11.h
	include/geFloat16.h
	include/geFloat16Color.h
	include/geFloat32.h
	include/geFrameAlloc.h
	include/geFreeAlloc.h
	include/geFwdDeclUtil.h
	include/geGroupAlloc.h
	include/geLog.h
	include/geMacroUtil.h
	include/geMath.h
	include/geMatrix4.h
	include/geMemAllocProfiler.h
	include/geMemoryAllocator.h
	include/geMemorySerializer.h
	include/geMessageHandler.h
	include/geMessageHandlerFwd.h
	include/geMinHeap.h
	include/geModule.h
	include/geNonCopyable.h
	include/geNumericLimits.h
	include/geOrientedBox.h
	include/gePath.h
	include/gePlane.h
	include/gePlatformDefines.h
	include/gePlatformTypes.h
	include/gePlatformUsing.h
	include/gePlatformUtility.h
	include/gePoolAlloc.h
	include/gePrerequisitesUtilities.h
	include/geQuaternion.h
	include/geRadian.h
	include/geRandom.h
	include/geRect2.h
	include/geRotator.h
	include/geSmallVector.h
	include/geSphere.h
	include/geSpinLock.h
	include/geStackAlloc.h
	include/geStaticAlloc.h
	include/geStdHeaders.h
	include/geString.h
	include/geStringFormat.h
	include/geStringID.h
	include/geTaskScheduler.h
	include/geThreading.h
	include/geThreadPool.h
	include/geTime.h
	include/geTimer.h
	include/geTransform.h
	include/geTriangulation.h
	include/geUnicode.h
	include/geUtil.h
	include/geUUID.h
	include/geVector2.h
	include/geVector2Half.h
	include/geVector2I.h
	include/geVector3.h
	include/geVector4.h
	include/geVectorNI.h
	include/win32/geMinWindows.h
	include/externals/md5.h

	src/geBitmapWriter.cpp
	src/geBox.cpp
	src/geBox2D.cpp
	src/geBox2DI.cpp
	src/geBoxSphereBounds.cpp
	src/geColor.cpp
	src/geCompression.cpp
	src/geCrashHandler.cpp
	src/geDataStream.cpp
	src/geDebug.cpp
	src/geDegree.cpp
	src/geDynLib.cpp
	src/geDynLibManager.cpp
	src/geFileSystem.cpp
	src/geFrameAlloc.cpp
	src/geLog.cpp
	src/geMath.cpp
	src/geMatrix4.cpp
	src/geMemoryAllocator.cpp
	src/geMessageHandler.cpp
	src/gePath.cpp
	src/gePlatformUtility.cpp
	src/geQuaternion.cpp
	src/geRadian.cpp
	src/geRect2.cpp
	src/geRotator.cpp
	src/geSphere.cpp
	src/geStackAlloc.cpp
	src/geString.cpp
	src/geStringID.cpp
	src/geTaskScheduler.cpp
	src/geThreadPool.cpp
	src/geTime.cpp
	src/geTimer.cpp
	src/geTransform.cpp
	src/geUnicode.cpp
	src/geUtil.cpp
	src/geUUID.cpp
	src/geVector2.cpp
	src/geVector2I.cpp
	src/geVector3.cpp
	src/geVector4.cpp
	src/win32/geWin32CrashHandler.cpp
	src/win32/geWin32FileSystem.cpp
	src/externals/md5.cpp
)

ge_set_output_dirs(geUtilities)

#Add the include directories
target_include_directories(geUtilities
    PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${GE_EXT_INC_DIR}
)

target_link_libraries(geUtilities
	PUBLIC
		ge_build_settings
		ge_lz4
)

if(WIN32)
target_link_libraries(geUtilities
	PRIVATE
		$<$<PLATFORM_ID:Windows>:Shell32>
		$<$<PLATFORM_ID:Windows>:DbgHelp>
		$<$<PLATFORM_ID:Windows>:IPHLPAPI>
		$<$<PLATFORM_ID:Windows>:Rpcrt4>
)
endif()

get_target_property(GEUTIL_SOURCES geUtilities SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GEUTIL_SOURCES})

target_compile_definitions(geUtilities
    PRIVATE
		GE_UTILITIES_EXPORTS=1
    #PUBLIC
    #   GE_HAS_UTILITIES=1
)