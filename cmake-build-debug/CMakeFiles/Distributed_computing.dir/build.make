# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_SOURCE_DIR = /mnt/c/Users/Traveler/Documents/Cole/Distribuida/P1/Distributed-computing

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/c/Users/Traveler/Documents/Cole/Distribuida/P1/Distributed-computing/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Distributed_computing.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Distributed_computing.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Distributed_computing.dir/flags.make

CMakeFiles/Distributed_computing.dir/main.c.o: CMakeFiles/Distributed_computing.dir/flags.make
CMakeFiles/Distributed_computing.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/Traveler/Documents/Cole/Distribuida/P1/Distributed-computing/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/Distributed_computing.dir/main.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Distributed_computing.dir/main.c.o   -c /mnt/c/Users/Traveler/Documents/Cole/Distribuida/P1/Distributed-computing/main.c

CMakeFiles/Distributed_computing.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Distributed_computing.dir/main.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/c/Users/Traveler/Documents/Cole/Distribuida/P1/Distributed-computing/main.c > CMakeFiles/Distributed_computing.dir/main.c.i

CMakeFiles/Distributed_computing.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Distributed_computing.dir/main.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/c/Users/Traveler/Documents/Cole/Distribuida/P1/Distributed-computing/main.c -o CMakeFiles/Distributed_computing.dir/main.c.s

CMakeFiles/Distributed_computing.dir/main.c.o.requires:

.PHONY : CMakeFiles/Distributed_computing.dir/main.c.o.requires

CMakeFiles/Distributed_computing.dir/main.c.o.provides: CMakeFiles/Distributed_computing.dir/main.c.o.requires
	$(MAKE) -f CMakeFiles/Distributed_computing.dir/build.make CMakeFiles/Distributed_computing.dir/main.c.o.provides.build
.PHONY : CMakeFiles/Distributed_computing.dir/main.c.o.provides

CMakeFiles/Distributed_computing.dir/main.c.o.provides.build: CMakeFiles/Distributed_computing.dir/main.c.o


# Object files for target Distributed_computing
Distributed_computing_OBJECTS = \
"CMakeFiles/Distributed_computing.dir/main.c.o"

# External object files for target Distributed_computing
Distributed_computing_EXTERNAL_OBJECTS =

Distributed_computing: CMakeFiles/Distributed_computing.dir/main.c.o
Distributed_computing: CMakeFiles/Distributed_computing.dir/build.make
Distributed_computing: CMakeFiles/Distributed_computing.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/c/Users/Traveler/Documents/Cole/Distribuida/P1/Distributed-computing/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable Distributed_computing"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Distributed_computing.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Distributed_computing.dir/build: Distributed_computing

.PHONY : CMakeFiles/Distributed_computing.dir/build

CMakeFiles/Distributed_computing.dir/requires: CMakeFiles/Distributed_computing.dir/main.c.o.requires

.PHONY : CMakeFiles/Distributed_computing.dir/requires

CMakeFiles/Distributed_computing.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Distributed_computing.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Distributed_computing.dir/clean

CMakeFiles/Distributed_computing.dir/depend:
	cd /mnt/c/Users/Traveler/Documents/Cole/Distribuida/P1/Distributed-computing/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/c/Users/Traveler/Documents/Cole/Distribuida/P1/Distributed-computing /mnt/c/Users/Traveler/Documents/Cole/Distribuida/P1/Distributed-computing /mnt/c/Users/Traveler/Documents/Cole/Distribuida/P1/Distributed-computing/cmake-build-debug /mnt/c/Users/Traveler/Documents/Cole/Distribuida/P1/Distributed-computing/cmake-build-debug /mnt/c/Users/Traveler/Documents/Cole/Distribuida/P1/Distributed-computing/cmake-build-debug/CMakeFiles/Distributed_computing.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Distributed_computing.dir/depend

