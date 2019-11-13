set(CEF_DISTRIBUTION "../cef_binary_3.2785.1465.g87ce149_linux64")


file (GLOB SIMPLE_CC_FILES ${CEF_DISTRIBUTION}/cefsimple/*.cc )
file (GLOB SIMPLE_H_FILES ${CEF_DISTRIBUTION}/cefsimple/*.h )
set (CEF_SOURCE_FILES ${SIMPLE_CC_FILES} ${SIMPLE_H_FILES})

FILE(GLOB_RECURSE Fpmed_Sources2 "./*.cpp" "./*.inl" "./*.h" "./*.rc")
set (Fpmed_Sources2 ${Fpmed_Sources2} ${CEF_SOURCE_FILES})
# FILE(GLOB_RECURSE Fpmed_Includes "./*.h")


#Compile resource on Windows
# Reference: https://stackoverflow.com/questions/3526794/how-do-i-build-a-win32-app-with-a-resource-file-using-cmake-and-mingw
if(MINGW)
  set(CMAKE_RC_COMPILER_INIT windres)
  ENABLE_LANGUAGE(RC)
  SET(CMAKE_RC_COMPILE_OBJECT
    "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif(MINGW)


include_directories (./)
# include_directories(${CMAKE_SOURCE_DIR}/source/) ## same as above
include_directories (../include)
include_directories (../include/Urho3D)
include_directories (../include/Urho3D/ThirdParty)

########################
# specify what we want here
set(CEF_BLD_TYPE Release)

	
message ("source dir" ${CMAKE_SOURCE_DIR})
FILE(GLOB_RECURSE Urho3DLibs "${CMAKE_SOURCE_DIR}/lib/linux/*.a" )
message("urho libs: " ${Urho3DLibs})
FILE(GLOB_RECURSE CEFStaticLibs "${CMAKE_SOURCE_DIR}/source/${CEF_DISTRIBUTION}/libcef_dll_wrapper/*.a")
message("cef static libs: " ${CEFStaticLibs})
FILE(GLOB_RECURSE CEFWrapperLibs "${CMAKE_SOURCE_DIR}/source/${CEF_DISTRIBUTION}/${CEF_BLD_TYPE}/libcef.so")
message("cef dynamic wrapper libs: " ${CEFWrapperLibs})

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
add_definitions(-DNDEBUG)

#################################################
# libcef_dll_wrapper
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${CEF_DISTRIBUTION})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${CEF_DISTRIBUTION}/include)
set(Dependency_Libs ${Dependency_Libs} ${Urho3DLibs} ${CEFWrapperLibs} ${CEFStaticLibs} ) 




########################
# copy resource files
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

#################################################
# cefsimple
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/${CEF_DISTRIBUTION}/cefsimple)





add_executable (fpmed ${Fpmed_Sources2})

set_target_properties(fpmed
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)


LINK_DIRECTORIES("${CMAKE_SOURCE_DIR}/bin")

message( ${Dependency_Libs} )
target_link_libraries (fpmed LINK_PUBLIC ${Dependency_Libs} pthread GLU GL dl X11 Xext Xinerama Xft fontconfig Xxf86vm Xfixes Xrender Xcursor)


