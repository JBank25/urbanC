# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /workspaces/urbanC/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /workspaces/urbanC/src/build

# Include any dependencies generated for this target.
include debug/CMakeFiles/debug_lib.dir/depend.make

# Include the progress variables for this target.
include debug/CMakeFiles/debug_lib.dir/progress.make

# Include the compile flags for this target's objects.
include debug/CMakeFiles/debug_lib.dir/flags.make

debug/CMakeFiles/debug_lib.dir/src/debug.c.o: debug/CMakeFiles/debug_lib.dir/flags.make
debug/CMakeFiles/debug_lib.dir/src/debug.c.o: ../debug/src/debug.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/workspaces/urbanC/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object debug/CMakeFiles/debug_lib.dir/src/debug.c.o"
	cd /workspaces/urbanC/src/build/debug && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/debug_lib.dir/src/debug.c.o -c /workspaces/urbanC/src/debug/src/debug.c

debug/CMakeFiles/debug_lib.dir/src/debug.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/debug_lib.dir/src/debug.c.i"
	cd /workspaces/urbanC/src/build/debug && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /workspaces/urbanC/src/debug/src/debug.c > CMakeFiles/debug_lib.dir/src/debug.c.i

debug/CMakeFiles/debug_lib.dir/src/debug.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/debug_lib.dir/src/debug.c.s"
	cd /workspaces/urbanC/src/build/debug && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /workspaces/urbanC/src/debug/src/debug.c -o CMakeFiles/debug_lib.dir/src/debug.c.s

# Object files for target debug_lib
debug_lib_OBJECTS = \
"CMakeFiles/debug_lib.dir/src/debug.c.o"

# External object files for target debug_lib
debug_lib_EXTERNAL_OBJECTS =

debug/libdebug_lib.a: debug/CMakeFiles/debug_lib.dir/src/debug.c.o
debug/libdebug_lib.a: debug/CMakeFiles/debug_lib.dir/build.make
debug/libdebug_lib.a: debug/CMakeFiles/debug_lib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/workspaces/urbanC/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libdebug_lib.a"
	cd /workspaces/urbanC/src/build/debug && $(CMAKE_COMMAND) -P CMakeFiles/debug_lib.dir/cmake_clean_target.cmake
	cd /workspaces/urbanC/src/build/debug && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/debug_lib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
debug/CMakeFiles/debug_lib.dir/build: debug/libdebug_lib.a

.PHONY : debug/CMakeFiles/debug_lib.dir/build

debug/CMakeFiles/debug_lib.dir/clean:
	cd /workspaces/urbanC/src/build/debug && $(CMAKE_COMMAND) -P CMakeFiles/debug_lib.dir/cmake_clean.cmake
.PHONY : debug/CMakeFiles/debug_lib.dir/clean

debug/CMakeFiles/debug_lib.dir/depend:
	cd /workspaces/urbanC/src/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /workspaces/urbanC/src /workspaces/urbanC/src/debug /workspaces/urbanC/src/build /workspaces/urbanC/src/build/debug /workspaces/urbanC/src/build/debug/CMakeFiles/debug_lib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : debug/CMakeFiles/debug_lib.dir/depend

