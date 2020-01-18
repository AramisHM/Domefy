FILE(GLOB_RECURSE Fpmed_Sources2 "./*.cpp" "./*.inl" "./*.h" "./*.rc")
# FILE(GLOB_RECURSE Fpmed_Includes "./*.h" "../include/*.h" "../include/Urho3D/*.h")


#Compile resource on Windows
# Reference: https://stackoverflow.com/questions/3526794/how-do-i-build-a-win32-app-with-a-resource-file-using-cmake-and-mingw
if(MINGW)
  set(CMAKE_RC_COMPILER_INIT windres)
  ENABLE_LANGUAGE(RC)
  SET(CMAKE_RC_COMPILE_OBJECT
    "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif(MINGW)


# include_directories(${CMAKE_SOURCE_DIR}/source/) ## same as above
# include_directories (../include/regular)

include_directories (./)
include_directories(${CMAKE_SOURCE_DIR}/source/ThirdParty/irrXML)
include_directories (../include)
include_directories (../include/Urho3D)
include_directories (../include/Urho3D/ThirdParty)

add_executable (fpmed ${Fpmed_Sources2})

set_target_properties(fpmed
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

if(WIN32)
	FILE(GLOB_RECURSE Dependency_Libs_A "${CMAKE_SOURCE_DIR}/lib/windows/*.a")
	FILE(GLOB_RECURSE Dependency_Libs_LIB "${CMAKE_SOURCE_DIR}/lib/windows/*.lib")
	set ( CMAKE_CXX_FLAGS "-msse3 -msse4.1")
  	target_link_libraries (
		  fpmed LINK_PUBLIC
		  ${Dependency_Libs_A}
		  ${Dependency_Libs_LIB}
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
		  uuid.lib
		)
	
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



