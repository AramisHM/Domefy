#  tells to use or not cef
add_definitions(-DCEF_INTEGRATION)
set(CEF_INTEGRATION 1)

if(CEF_INTEGRATION)
	set(CEF_DISTRIBUTION ../third_party/cef_binary_3.2785.1466.g80e473e_windows32)
	file (GLOB SIMPLE_CC_FILES ${CEF_DISTRIBUTION}/cefsimple/*.cc )
	file (GLOB SIMPLE_H_FILES ${CEF_DISTRIBUTION}/cefsimple/*.h )
	set (CEF_SOURCE_FILES ${SIMPLE_CC_FILES} ${SIMPLE_H_FILES})
endif(CEF_INTEGRATION)

FILE(GLOB_RECURSE Fpmed_Sources2 "./*.cpp" "./*.inl" "./*.h" "./*.rc")
set (Fpmed_Sources2 ${Fpmed_Sources2} ${CEF_SOURCE_FILES})
# FILE(GLOB_RECURSE Fpmed_Includes "./*.h" "../include/*.h" "../include/Urho3D/*.h")


#Compile resource on Windows
# Reference: https://stackoverflow.com/questions/3526794/how-do-i-build-a-win32-app-with-a-resource-file-using-cmake-and-mingw
if(MINGW)
  set(CMAKE_RC_COMPILER_INIT windres)
  ENABLE_LANGUAGE(RC)
  SET(CMAKE_RC_COMPILE_OBJECT
    "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif(MINGW)

include_directories (./)
include_directories(${CMAKE_SOURCE_DIR}/source/ThirdParty/irrXML)
include_directories (../include)
include_directories (../include/Urho3D)
include_directories (../include/Urho3D/ThirdParty)

if(CEF_INTEGRATION)
	# CEF specific
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${CEF_DISTRIBUTION})
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${CEF_DISTRIBUTION}/include)
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/${CEF_DISTRIBUTION}/cefsimple)
	set(CEF_BLD_TYPE Release)
endif(CEF_INTEGRATION)

add_executable (fpmed ${Fpmed_Sources2})

set_target_properties(fpmed
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

if(WIN32)
	add_definitions(-DURHO3D_SSE)
	add_definitions(-DURHO3D_MINIDUMPS)
	add_definitions(-DURHO3D_FILEWATCHER)
	add_definitions(-DURHO3D_PROFILING)
	add_definitions(-DURHO3D_LOGGING)
	add_definitions(-DURHO3D_OPENGL)
	add_definitions(-DGLEW_STATIC)
	add_definitions(-DGLEW_NO_GLU)
	add_definitions(-DURHO3D_ANGELSCRIPT)
	add_definitions(-DURHO3D_NAVIGATION)
	add_definitions(-DURHO3D_NETWORK)
	add_definitions(-DURHO3D_PHYSICS)
	add_definitions(-DURHO3D_URHO2D)
	add_definitions(-DURHO3D_STATIC_DEFINE)
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	set ( CMAKE_CXX_FLAGS "-msse3 -msse4.1")
if(MINGW)
	LINK_DIRECTORIES("${CMAKE_SOURCE_DIR}/bin" "${CMAKE_SOURCE_DIR}/lib/windows")
	FILE(GLOB_RECURSE Dependency_Libs_A "${CMAKE_SOURCE_DIR}/lib/mingw/*.a")
	set (CompilerSpecificTargetLibs
	Comdlg32.lib
	Comctl32.lib
	winmm.lib
	ws2_32.lib
	opengl32.lib
	imm32.lib
	version.lib
	advapi32.lib
	user32.lib
	gdi32.lib
	Setupapi.lib
	shlwapi.lib
	d3d9.lib
	dxguid.lib
	winmm.lib
	imm32.lib
	version.lib
	ws2_32.lib
	winmm.lib
	kernel32.lib
	winspool.lib
	shell32.lib
	ole32.lib
	oleaut32.lib
	uuid.lib)
else()
	LINK_DIRECTORIES("${CMAKE_SOURCE_DIR}/bin" "${CMAKE_SOURCE_DIR}/lib/windows")
	FILE(GLOB_RECURSE Dependency_Libs_A "${CMAKE_SOURCE_DIR}/lib/windows/*.a")
	FILE(GLOB_RECURSE Dependency_Libs_LIB "${CMAKE_SOURCE_DIR}/lib/windows/*.lib")
	set (CompilerSpecificTargetLibs
	Comdlg32.lib
	Comctl32.lib
	winmm.lib
	ws2_32.lib
	opengl32.lib
	imm32.lib
	version.lib
	advapi32.lib
	user32.lib
	gdi32.lib
	Setupapi.lib
	shlwapi.lib
	d3d9.lib
	d3d11.lib
	d3dcompiler.lib
	dxguid.lib
	winmm.lib
	imm32.lib
	version.lib
	ws2_32.lib
	winmm.lib
	dbghelp.lib
	kernel32.lib
	winspool.lib
	shell32.lib
	ole32.lib
	oleaut32.lib
	uuid.lib)
endif()
	target_link_libraries (
		  fpmed LINK_PUBLIC
		  ${Dependency_Libs_A}
		  ${Dependency_Libs_LIB}
		  ${CompilerSpecificTargetLibs}
		)
else()
		
	FILE(GLOB_RECURSE Dependency_Libs "${CMAKE_SOURCE_DIR}/lib/linux/*.a")

	add_definitions(-DURHO3D_SSE)
	add_definitions(-DURHO3D_MINIDUMPS)
	add_definitions(-DURHO3D_FILEWATCHER)
	add_definitions(-DURHO3D_PROFILING)
	add_definitions(-DURHO3D_LOGGING)
	add_definitions(-DURHO3D_OPENGL)
	add_definitions(-DGLEW_STATIC)
	add_definitions(-DGLEW_NO_GLU)
	add_definitions(-DURHO3D_ANGELSCRIPT)
	add_definitions(-DURHO3D_NAVIGATION)
	add_definitions(-DURHO3D_NETWORK)
	add_definitions(-DURHO3D_PHYSICS)
	add_definitions(-DURHO3D_URHO2D)
	add_definitions(-DURHO3D_STATIC_DEFINE)

	target_link_libraries (fpmed LINK_PUBLIC ${Dependency_Libs} pthread GLU GL dl X11 Xext Xinerama Xft fontconfig Xxf86vm Xfixes Xrender Xcursor ogg vorbis theora)
endif()

if(CEF_INTEGRATION)
	if(WIN32)
	# CEF copy resource files
	set(CEF_BINARY_FILES
		natives_blob.bin
		snapshot_blob.bin
	)

	set(CEF_RESOURCE_FILES
		cef.pak
		cef_100_percent.pak
		cef_200_percent.pak
		cef_extensions.pak
		devtools_resources.pak
		icudtl.dat
		locales
	)
	foreach(fileResource ${CEF_BINARY_FILES} )
		file(COPY ${CEF_DISTRIBUTION}/${CEF_BLD_TYPE}/${fileResource} DESTINATION ${CMAKE_SOURCE_DIR}/bin)
	endforeach()

	foreach(fileResource ${CEF_RESOURCE_FILES} )
		file(COPY ${CEF_DISTRIBUTION}/Resources/${fileResource} DESTINATION ${CMAKE_SOURCE_DIR}/bin)
	endforeach()
	endif()
endif(CEF_INTEGRATION)