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
CMAKE_SOURCE_DIR = /home/kleftog/benchmarks/memcached-1.6.9/libevent

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kleftog/benchmarks/memcached-1.6.9/libevent/safestack

# Utility rule file for verify.

# Include the progress variables for this target.
include CMakeFiles/verify.dir/progress.make

CMakeFiles/verify: bin/test-changelist
CMakeFiles/verify: bin/test-eof
CMakeFiles/verify: bin/test-closed
CMakeFiles/verify: bin/test-fdleak
CMakeFiles/verify: bin/test-init
CMakeFiles/verify: bin/test-time
CMakeFiles/verify: bin/test-weof
CMakeFiles/verify: bin/test-dumpevents
CMakeFiles/verify: bin/test-ratelim
	./verify_tests.sh

verify: CMakeFiles/verify
verify: CMakeFiles/verify.dir/build.make

.PHONY : verify

# Rule to build all files generated by this target.
CMakeFiles/verify.dir/build: verify

.PHONY : CMakeFiles/verify.dir/build

CMakeFiles/verify.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/verify.dir/cmake_clean.cmake
.PHONY : CMakeFiles/verify.dir/clean

CMakeFiles/verify.dir/depend:
	cd /home/kleftog/benchmarks/memcached-1.6.9/libevent/safestack && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kleftog/benchmarks/memcached-1.6.9/libevent /home/kleftog/benchmarks/memcached-1.6.9/libevent /home/kleftog/benchmarks/memcached-1.6.9/libevent/safestack /home/kleftog/benchmarks/memcached-1.6.9/libevent/safestack /home/kleftog/benchmarks/memcached-1.6.9/libevent/safestack/CMakeFiles/verify.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/verify.dir/depend
