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
include Source/ThirdParty/Lua/CMakeFiles/luac.dir/depend.make

# Include the progress variables for this target.
include Source/ThirdParty/Lua/CMakeFiles/luac.dir/progress.make

# Include the compile flags for this target's objects.
include Source/ThirdParty/Lua/CMakeFiles/luac.dir/flags.make

Source/ThirdParty/Lua/CMakeFiles/luac.dir/src/luac.c.o: Source/ThirdParty/Lua/CMakeFiles/luac.dir/flags.make
Source/ThirdParty/Lua/CMakeFiles/luac.dir/src/luac.c.o: Source/ThirdParty/Lua/src/luac.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aramis/Desktop/Urho3D-1.7/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object Source/ThirdParty/Lua/CMakeFiles/luac.dir/src/luac.c.o"
	cd /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/Lua && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/luac.dir/src/luac.c.o   -c /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/Lua/src/luac.c

Source/ThirdParty/Lua/CMakeFiles/luac.dir/src/luac.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/luac.dir/src/luac.c.i"
	cd /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/Lua && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/Lua/src/luac.c > CMakeFiles/luac.dir/src/luac.c.i

Source/ThirdParty/Lua/CMakeFiles/luac.dir/src/luac.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/luac.dir/src/luac.c.s"
	cd /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/Lua && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/Lua/src/luac.c -o CMakeFiles/luac.dir/src/luac.c.s

Source/ThirdParty/Lua/CMakeFiles/luac.dir/src/luac.c.o.requires:

.PHONY : Source/ThirdParty/Lua/CMakeFiles/luac.dir/src/luac.c.o.requires

Source/ThirdParty/Lua/CMakeFiles/luac.dir/src/luac.c.o.provides: Source/ThirdParty/Lua/CMakeFiles/luac.dir/src/luac.c.o.requires
	$(MAKE) -f Source/ThirdParty/Lua/CMakeFiles/luac.dir/build.make Source/ThirdParty/Lua/CMakeFiles/luac.dir/src/luac.c.o.provides.build
.PHONY : Source/ThirdParty/Lua/CMakeFiles/luac.dir/src/luac.c.o.provides

Source/ThirdParty/Lua/CMakeFiles/luac.dir/src/luac.c.o.provides.build: Source/ThirdParty/Lua/CMakeFiles/luac.dir/src/luac.c.o


# Object files for target luac
luac_OBJECTS = \
"CMakeFiles/luac.dir/src/luac.c.o"

# External object files for target luac
luac_EXTERNAL_OBJECTS =

bin/luac: Source/ThirdParty/Lua/CMakeFiles/luac.dir/src/luac.c.o
bin/luac: Source/ThirdParty/Lua/CMakeFiles/luac.dir/build.make
bin/luac: Source/ThirdParty/Lua/libLua.a
bin/luac: Source/ThirdParty/Lua/CMakeFiles/luac.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/aramis/Desktop/Urho3D-1.7/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ../../../bin/luac"
	cd /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/Lua && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/luac.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Source/ThirdParty/Lua/CMakeFiles/luac.dir/build: bin/luac

.PHONY : Source/ThirdParty/Lua/CMakeFiles/luac.dir/build

Source/ThirdParty/Lua/CMakeFiles/luac.dir/requires: Source/ThirdParty/Lua/CMakeFiles/luac.dir/src/luac.c.o.requires

.PHONY : Source/ThirdParty/Lua/CMakeFiles/luac.dir/requires

Source/ThirdParty/Lua/CMakeFiles/luac.dir/clean:
	cd /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/Lua && $(CMAKE_COMMAND) -P CMakeFiles/luac.dir/cmake_clean.cmake
.PHONY : Source/ThirdParty/Lua/CMakeFiles/luac.dir/clean

Source/ThirdParty/Lua/CMakeFiles/luac.dir/depend:
	cd /home/aramis/Desktop/Urho3D-1.7 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/aramis/Desktop/Urho3D-1.7 /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/Lua /home/aramis/Desktop/Urho3D-1.7 /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/Lua /home/aramis/Desktop/Urho3D-1.7/Source/ThirdParty/Lua/CMakeFiles/luac.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Source/ThirdParty/Lua/CMakeFiles/luac.dir/depend

