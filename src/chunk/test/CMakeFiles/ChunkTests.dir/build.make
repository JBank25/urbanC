# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.25.1/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.25.1/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/joshuaurbank/Documents/Dev/Projects/urbanC

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/joshuaurbank/Documents/Dev/Projects/urbanC

# Include any dependencies generated for this target.
include src/chunk/test/CMakeFiles/ChunkTests.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/chunk/test/CMakeFiles/ChunkTests.dir/compiler_depend.make

# Include the progress variables for this target.
include src/chunk/test/CMakeFiles/ChunkTests.dir/progress.make

# Include the compile flags for this target's objects.
include src/chunk/test/CMakeFiles/ChunkTests.dir/flags.make

src/chunk/test/CMakeFiles/ChunkTests.dir/chunk_tests.c.o: src/chunk/test/CMakeFiles/ChunkTests.dir/flags.make
src/chunk/test/CMakeFiles/ChunkTests.dir/chunk_tests.c.o: src/chunk/test/chunk_tests.c
src/chunk/test/CMakeFiles/ChunkTests.dir/chunk_tests.c.o: src/chunk/test/CMakeFiles/ChunkTests.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/joshuaurbank/Documents/Dev/Projects/urbanC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/chunk/test/CMakeFiles/ChunkTests.dir/chunk_tests.c.o"
	cd /Users/joshuaurbank/Documents/Dev/Projects/urbanC/src/chunk/test && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/chunk/test/CMakeFiles/ChunkTests.dir/chunk_tests.c.o -MF CMakeFiles/ChunkTests.dir/chunk_tests.c.o.d -o CMakeFiles/ChunkTests.dir/chunk_tests.c.o -c /Users/joshuaurbank/Documents/Dev/Projects/urbanC/src/chunk/test/chunk_tests.c

src/chunk/test/CMakeFiles/ChunkTests.dir/chunk_tests.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ChunkTests.dir/chunk_tests.c.i"
	cd /Users/joshuaurbank/Documents/Dev/Projects/urbanC/src/chunk/test && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/joshuaurbank/Documents/Dev/Projects/urbanC/src/chunk/test/chunk_tests.c > CMakeFiles/ChunkTests.dir/chunk_tests.c.i

src/chunk/test/CMakeFiles/ChunkTests.dir/chunk_tests.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ChunkTests.dir/chunk_tests.c.s"
	cd /Users/joshuaurbank/Documents/Dev/Projects/urbanC/src/chunk/test && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/joshuaurbank/Documents/Dev/Projects/urbanC/src/chunk/test/chunk_tests.c -o CMakeFiles/ChunkTests.dir/chunk_tests.c.s

# Object files for target ChunkTests
ChunkTests_OBJECTS = \
"CMakeFiles/ChunkTests.dir/chunk_tests.c.o"

# External object files for target ChunkTests
ChunkTests_EXTERNAL_OBJECTS =

src/chunk/test/ChunkTests: src/chunk/test/CMakeFiles/ChunkTests.dir/chunk_tests.c.o
src/chunk/test/ChunkTests: src/chunk/test/CMakeFiles/ChunkTests.dir/build.make
src/chunk/test/ChunkTests: src/chunk/libChunk.a
src/chunk/test/ChunkTests: src/memory/libMemory.a
src/chunk/test/ChunkTests: src/value/libValue.a
src/chunk/test/ChunkTests: src/object/libObject.a
src/chunk/test/ChunkTests: src/vm/libVm.a
src/chunk/test/ChunkTests: src/debug/libDebug.a
src/chunk/test/ChunkTests: src/table/libTable.a
src/chunk/test/ChunkTests: src/compiler/libCompiler.a
src/chunk/test/ChunkTests: src/chunk/libChunk.a
src/chunk/test/ChunkTests: src/memory/libMemory.a
src/chunk/test/ChunkTests: src/value/libValue.a
src/chunk/test/ChunkTests: src/object/libObject.a
src/chunk/test/ChunkTests: src/vm/libVm.a
src/chunk/test/ChunkTests: src/debug/libDebug.a
src/chunk/test/ChunkTests: src/table/libTable.a
src/chunk/test/ChunkTests: src/compiler/libCompiler.a
src/chunk/test/ChunkTests: src/scanner/libScanner.a
src/chunk/test/ChunkTests: src/chunk/test/CMakeFiles/ChunkTests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/joshuaurbank/Documents/Dev/Projects/urbanC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ChunkTests"
	cd /Users/joshuaurbank/Documents/Dev/Projects/urbanC/src/chunk/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ChunkTests.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/chunk/test/CMakeFiles/ChunkTests.dir/build: src/chunk/test/ChunkTests
.PHONY : src/chunk/test/CMakeFiles/ChunkTests.dir/build

src/chunk/test/CMakeFiles/ChunkTests.dir/clean:
	cd /Users/joshuaurbank/Documents/Dev/Projects/urbanC/src/chunk/test && $(CMAKE_COMMAND) -P CMakeFiles/ChunkTests.dir/cmake_clean.cmake
.PHONY : src/chunk/test/CMakeFiles/ChunkTests.dir/clean

src/chunk/test/CMakeFiles/ChunkTests.dir/depend:
	cd /Users/joshuaurbank/Documents/Dev/Projects/urbanC && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/joshuaurbank/Documents/Dev/Projects/urbanC /Users/joshuaurbank/Documents/Dev/Projects/urbanC/src/chunk/test /Users/joshuaurbank/Documents/Dev/Projects/urbanC /Users/joshuaurbank/Documents/Dev/Projects/urbanC/src/chunk/test /Users/joshuaurbank/Documents/Dev/Projects/urbanC/src/chunk/test/CMakeFiles/ChunkTests.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/chunk/test/CMakeFiles/ChunkTests.dir/depend

