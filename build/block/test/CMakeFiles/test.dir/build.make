# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.16.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.16.2/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/zhangxinlong/数据之海/sea

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/zhangxinlong/数据之海/sea/build

# Include any dependencies generated for this target.
include block/test/CMakeFiles/test.dir/depend.make

# Include the progress variables for this target.
include block/test/CMakeFiles/test.dir/progress.make

# Include the compile flags for this target's objects.
include block/test/CMakeFiles/test.dir/flags.make

block/test/CMakeFiles/test.dir/test.c.o: block/test/CMakeFiles/test.dir/flags.make
block/test/CMakeFiles/test.dir/test.c.o: ../block/test/test.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/zhangxinlong/数据之海/sea/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object block/test/CMakeFiles/test.dir/test.c.o"
	cd /Users/zhangxinlong/数据之海/sea/build/block/test && /usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/test.dir/test.c.o   -c /Users/zhangxinlong/数据之海/sea/block/test/test.c

block/test/CMakeFiles/test.dir/test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test.dir/test.c.i"
	cd /Users/zhangxinlong/数据之海/sea/build/block/test && /usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/zhangxinlong/数据之海/sea/block/test/test.c > CMakeFiles/test.dir/test.c.i

block/test/CMakeFiles/test.dir/test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test.dir/test.c.s"
	cd /Users/zhangxinlong/数据之海/sea/build/block/test && /usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/zhangxinlong/数据之海/sea/block/test/test.c -o CMakeFiles/test.dir/test.c.s

# Object files for target test
test_OBJECTS = \
"CMakeFiles/test.dir/test.c.o"

# External object files for target test
test_EXTERNAL_OBJECTS =

block/test/test: block/test/CMakeFiles/test.dir/test.c.o
block/test/test: block/test/CMakeFiles/test.dir/build.make
block/test/test: block/libsea_block.dylib
block/test/test: block/test/CMakeFiles/test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/zhangxinlong/数据之海/sea/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable test"
	cd /Users/zhangxinlong/数据之海/sea/build/block/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
block/test/CMakeFiles/test.dir/build: block/test/test

.PHONY : block/test/CMakeFiles/test.dir/build

block/test/CMakeFiles/test.dir/clean:
	cd /Users/zhangxinlong/数据之海/sea/build/block/test && $(CMAKE_COMMAND) -P CMakeFiles/test.dir/cmake_clean.cmake
.PHONY : block/test/CMakeFiles/test.dir/clean

block/test/CMakeFiles/test.dir/depend:
	cd /Users/zhangxinlong/数据之海/sea/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/zhangxinlong/数据之海/sea /Users/zhangxinlong/数据之海/sea/block/test /Users/zhangxinlong/数据之海/sea/build /Users/zhangxinlong/数据之海/sea/build/block/test /Users/zhangxinlong/数据之海/sea/build/block/test/CMakeFiles/test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : block/test/CMakeFiles/test.dir/depend
