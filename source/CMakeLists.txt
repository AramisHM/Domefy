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



# Add macro definition to help some code adapt to old Urho3D version
if (FRACTALS)
else ()
add_definitions(-DFPMED_LATEST_URHO3D)
endif ()


include_directories (./)
include_directories(${CMAKE_SOURCE_DIR}/source/ThirdParty/irrXML)
include_directories (../include/common)
if (FRACTALS)
include_directories (../include/fractals)
include_directories (../include/fractals/Urho3D)
include_directories (../include/fractals/Urho3D/ThirdParty)
else ()
include_directories (../include/regular)
include_directories (../include/regular/Urho3D)
include_directories (../include/regular/Urho3D/ThirdParty)
endif ()


add_executable (fpmed ${Fpmed_Sources2})

set_target_properties(fpmed
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

if(MINGW)
FILE(GLOB_RECURSE Dependency_Libs "${CMAKE_SOURCE_DIR}/lib/GCC/windows/common*.a")
if (FRACTALS)
FILE(GLOB_RECURSE Dependency_Libs "${CMAKE_SOURCE_DIR}/lib/GCC/windows/urho-frac/*.a")
else ()
FILE(GLOB_RECURSE Dependency_Libs "${CMAKE_SOURCE_DIR}/lib/GCC/windows/urho3d-latest/*.a")
endif ()
	
	
	set ( CMAKE_CXX_FLAGS "-msse3 -msse4.1")
  	target_link_libraries (
		  fpmed LINK_PUBLIC
		  ${Dependency_Libs}
		  Comdlg32.lib
		  Comctl32.lib
		  ole32.lib
		  oleaut32.lib
		  winmm.lib
		  ws2_32.lib
		  opengl32.lib
		  imm32.lib
		  version.lib
		  advapi32.lib
		  kernel32.lib
		  user32.lib
		  gdi32.lib
		  Setupapi.lib
		  shell32.lib
		  shlwapi.lib)
	
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
		
	FILE(GLOB_RECURSE Dependency_Libs "${CMAKE_SOURCE_DIR}/lib/GCC/linux/*.a")

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

	target_link_libraries (fpmed LINK_PUBLIC ${Dependency_Libs} "${CMAKE_SOURCE_DIR}/lib/GCC/windows/libAHMNet.a" pthread GLU GL dl X11 Xext Xinerama Xft fontconfig Xxf86vm Xfixes Xrender Xcursor)
endif()



