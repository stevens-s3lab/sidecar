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
CMAKE_SOURCE_DIR = /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build

# Include any dependencies generated for this target.
include CMakeFiles/test-closed.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test-closed.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test-closed.dir/flags.make

CMakeFiles/test-closed.dir/test/test-closed.c.o: CMakeFiles/test-closed.dir/flags.make
CMakeFiles/test-closed.dir/test/test-closed.c.o: ../test/test-closed.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/test-closed.dir/test/test-closed.c.o"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/test-closed.dir/test/test-closed.c.o -c /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/test/test-closed.c

CMakeFiles/test-closed.dir/test/test-closed.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test-closed.dir/test/test-closed.c.i"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/test/test-closed.c > CMakeFiles/test-closed.dir/test/test-closed.c.i

CMakeFiles/test-closed.dir/test/test-closed.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test-closed.dir/test/test-closed.c.s"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/test/test-closed.c -o CMakeFiles/test-closed.dir/test/test-closed.c.s

# Object files for target test-closed
test__closed_OBJECTS = \
"CMakeFiles/test-closed.dir/test/test-closed.c.o"

# External object files for target test-closed
test__closed_EXTERNAL_OBJECTS =

bin/test-closed: CMakeFiles/test-closed.dir/test/test-closed.c.o
bin/test-closed: CMakeFiles/test-closed.dir/build.make
bin/test-closed: /usr/lib/x86_64-linux-gnu/libssl.so
bin/test-closed: /usr/lib/x86_64-linux-gnu/libz.so
bin/test-closed: /usr/lib/x86_64-linux-gnu/libcrypto.so
bin/test-closed: lib/libevent_extra-2.2.so.1.0.0
bin/test-closed: lib/libevent_core-2.2.so.1.0.0
bin/test-closed: CMakeFiles/test-closed.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable bin/test-closed"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test-closed.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test-closed.dir/build: bin/test-closed

.PHONY : CMakeFiles/test-closed.dir/build

CMakeFiles/test-closed.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test-closed.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test-closed.dir/clean

CMakeFiles/test-closed.dir/depend:
	cd /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/CMakeFiles/test-closed.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test-closed.dir/depend
