# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

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
CMAKE_COMMAND = /opt/clion-2018.2.3/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /opt/clion-2018.2.3/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/daniel/CLionProjects/lr7_1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/daniel/CLionProjects/lr7_1/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/lr7_1.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/lr7_1.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/lr7_1.dir/flags.make

CMakeFiles/lr7_1.dir/main.c.o: CMakeFiles/lr7_1.dir/flags.make
CMakeFiles/lr7_1.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/daniel/CLionProjects/lr7_1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/lr7_1.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/lr7_1.dir/main.c.o   -c /home/daniel/CLionProjects/lr7_1/main.c

CMakeFiles/lr7_1.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/lr7_1.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/daniel/CLionProjects/lr7_1/main.c > CMakeFiles/lr7_1.dir/main.c.i

CMakeFiles/lr7_1.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/lr7_1.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/daniel/CLionProjects/lr7_1/main.c -o CMakeFiles/lr7_1.dir/main.c.s

# Object files for target lr7_1
lr7_1_OBJECTS = \
"CMakeFiles/lr7_1.dir/main.c.o"

# External object files for target lr7_1
lr7_1_EXTERNAL_OBJECTS =

lr7_1: CMakeFiles/lr7_1.dir/main.c.o
lr7_1: CMakeFiles/lr7_1.dir/build.make
lr7_1: CMakeFiles/lr7_1.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/daniel/CLionProjects/lr7_1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable lr7_1"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lr7_1.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/lr7_1.dir/build: lr7_1

.PHONY : CMakeFiles/lr7_1.dir/build

CMakeFiles/lr7_1.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/lr7_1.dir/cmake_clean.cmake
.PHONY : CMakeFiles/lr7_1.dir/clean

CMakeFiles/lr7_1.dir/depend:
	cd /home/daniel/CLionProjects/lr7_1/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/daniel/CLionProjects/lr7_1 /home/daniel/CLionProjects/lr7_1 /home/daniel/CLionProjects/lr7_1/cmake-build-debug /home/daniel/CLionProjects/lr7_1/cmake-build-debug /home/daniel/CLionProjects/lr7_1/cmake-build-debug/CMakeFiles/lr7_1.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/lr7_1.dir/depend
