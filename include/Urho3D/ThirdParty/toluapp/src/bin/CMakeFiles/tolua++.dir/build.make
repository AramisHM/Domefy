# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/aramis/Desktop/Urho3D-1.7

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/aramis/Desktop/Urho3D-1.7

# Include any dependencies generated for this target.
include Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/depend.make

# Include the progress variables for this target.
include Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/progress.make

# Include the compile flags for this target's objects.
include Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/flags.make

Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/all.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/array.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/basic.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/class.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/clean.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/code.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/compat-5.1.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/compat.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/container.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/custom.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/declaration.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/define.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/doit.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/enumerate.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/feature.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/function.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/module.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/namespace.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/operator.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/package.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/template_class.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/typedef.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/variable.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/lua/verbatim.lua
Source/ThirdParty/toluapp/src/bin/generated/toluabind.c: Source/ThirdParty/toluapp/src/bin/tolua_scons.pkg
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/aramis/Desktop/Urho3D-1.7/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating toluabind.c and toluabind.h"
	cd /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin && /home/aramis/Desktop/Urho3D-1.7/bin/tool/tolua++ -o /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin/generated/toluabind.c -H /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin/generated/toluabind.h -n tolua tolua_scons.pkg 2>/dev/null || /usr/bin/cmake -E copy_if_different /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin/toluabind.c /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin/generated/toluabind.c

Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/tolua.c.o: Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/flags.make
Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/tolua.c.o: Source/ThirdParty/toluapp/src/bin/tolua.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aramis/Desktop/Urho3D-1.7/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/tolua.c.o"
	cd /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/tolua++.dir/tolua.c.o   -c /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin/tolua.c

Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/tolua.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/tolua++.dir/tolua.c.i"
	cd /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin/tolua.c > CMakeFiles/tolua++.dir/tolua.c.i

Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/tolua.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/tolua++.dir/tolua.c.s"
	cd /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin/tolua.c -o CMakeFiles/tolua++.dir/tolua.c.s

Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/tolua.c.o.requires:

.PHONY : Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/tolua.c.o.requires

Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/tolua.c.o.provides: Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/tolua.c.o.requires
	$(MAKE) -f Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/build.make Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/tolua.c.o.provides.build
.PHONY : Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/tolua.c.o.provides

Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/tolua.c.o.provides.build: Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/tolua.c.o


Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/generated/toluabind.c.o: Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/flags.make
Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/generated/toluabind.c.o: Source/ThirdParty/toluapp/src/bin/generated/toluabind.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aramis/Desktop/Urho3D-1.7/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/generated/toluabind.c.o"
	cd /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/tolua++.dir/generated/toluabind.c.o   -c /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin/generated/toluabind.c

Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/generated/toluabind.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/tolua++.dir/generated/toluabind.c.i"
	cd /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin/generated/toluabind.c > CMakeFiles/tolua++.dir/generated/toluabind.c.i

Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/generated/toluabind.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/tolua++.dir/generated/toluabind.c.s"
	cd /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin/generated/toluabind.c -o CMakeFiles/tolua++.dir/generated/toluabind.c.s

Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/generated/toluabind.c.o.requires:

.PHONY : Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/generated/toluabind.c.o.requires

Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/generated/toluabind.c.o.provides: Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/generated/toluabind.c.o.requires
	$(MAKE) -f Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/build.make Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/generated/toluabind.c.o.provides.build
.PHONY : Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/generated/toluabind.c.o.provides

Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/generated/toluabind.c.o.provides.build: Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/generated/toluabind.c.o


# Object files for target tolua++
tolua_______OBJECTS = \
"CMakeFiles/tolua++.dir/tolua.c.o" \
"CMakeFiles/tolua++.dir/generated/toluabind.c.o"

# External object files for target tolua++
tolua_______EXTERNAL_OBJECTS =

bin/tool/tolua++: Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/tolua.c.o
bin/tool/tolua++: Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/generated/toluabind.c.o
bin/tool/tolua++: Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/build.make
bin/tool/tolua++: Source/ThirdParty/toluapp/src/lib/libtoluapp.a
bin/tool/tolua++: Source/ThirdParty/Lua/libLua.a
bin/tool/tolua++: Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/aramis/Desktop/Urho3D-1.7/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C executable ../../../../../bin/tool/tolua++"
	cd /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tolua++.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/build: bin/tool/tolua++

.PHONY : Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/build

Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/requires: Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/tolua.c.o.requires
Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/requires: Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/generated/toluabind.c.o.requires

.PHONY : Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/requires

Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/clean:
	cd /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin && $(CMAKE_COMMAND) -P CMakeFiles/tolua++.dir/cmake_clean.cmake
.PHONY : Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/clean

Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/depend: Source/ThirdParty/toluapp/src/bin/generated/toluabind.c
	cd /home/aramis/Desktop/Urho3D-1.7 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/aramis/Desktop/Urho3D-1.7 /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin /home/aramis/Desktop/Urho3D-1.7 /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Source/ThirdParty/toluapp/src/bin/CMakeFiles/tolua++.dir/depend

