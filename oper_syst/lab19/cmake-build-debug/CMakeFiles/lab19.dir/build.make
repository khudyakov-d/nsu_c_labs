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
CMAKE_COMMAND = /home/daniel/optHome/CLion-2018.2.3/clion-2018.2.3/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/daniel/optHome/CLion-2018.2.3/clion-2018.2.3/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/daniel/Projects/os/lab19

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/daniel/Projects/os/lab19/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/lab19.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/lab19.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/lab19.dir/flags.make

CMakeFiles/lab19.dir/main.c.o: CMakeFiles/lab19.dir/flags.make
CMakeFiles/lab19.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/daniel/Projects/os/lab19/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/lab19.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/lab19.dir/main.c.o   -c /home/daniel/Projects/os/lab19/main.c

CMakeFiles/lab19.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/lab19.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/daniel/Projects/os/lab19/main.c > CMakeFiles/lab19.dir/main.c.i

CMakeFiles/lab19.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/lab19.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/daniel/Projects/os/lab19/main.c -o CMakeFiles/lab19.dir/main.c.s

# Object files for target lab19
lab19_OBJECTS = \
"CMakeFiles/lab19.dir/main.c.o"

# External object files for target lab19
lab19_EXTERNAL_OBJECTS =

lab19: CMakeFiles/lab19.dir/main.c.o
lab19: CMakeFiles/lab19.dir/build.make
lab19: CMakeFiles/lab19.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/daniel/Projects/os/lab19/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable lab19"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lab19.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/lab19.dir/build: lab19

.PHONY : CMakeFiles/lab19.dir/build

CMakeFiles/lab19.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/lab19.dir/cmake_clean.cmake
.PHONY : CMakeFiles/lab19.dir/clean

CMakeFiles/lab19.dir/depend:
	cd /home/daniel/Projects/os/lab19/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/daniel/Projects/os/lab19 /home/daniel/Projects/os/lab19 /home/daniel/Projects/os/lab19/cmake-build-debug /home/daniel/Projects/os/lab19/cmake-build-debug /home/daniel/Projects/os/lab19/cmake-build-debug/CMakeFiles/lab19.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/lab19.dir/depend

