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
include CMakeFiles/event_extra_static.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/event_extra_static.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/event_extra_static.dir/flags.make

CMakeFiles/event_extra_static.dir/event_tagging.c.o: CMakeFiles/event_extra_static.dir/flags.make
CMakeFiles/event_extra_static.dir/event_tagging.c.o: ../event_tagging.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/event_extra_static.dir/event_tagging.c.o"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/event_extra_static.dir/event_tagging.c.o -c /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/event_tagging.c

CMakeFiles/event_extra_static.dir/event_tagging.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/event_extra_static.dir/event_tagging.c.i"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/event_tagging.c > CMakeFiles/event_extra_static.dir/event_tagging.c.i

CMakeFiles/event_extra_static.dir/event_tagging.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/event_extra_static.dir/event_tagging.c.s"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/event_tagging.c -o CMakeFiles/event_extra_static.dir/event_tagging.c.s

CMakeFiles/event_extra_static.dir/http.c.o: CMakeFiles/event_extra_static.dir/flags.make
CMakeFiles/event_extra_static.dir/http.c.o: ../http.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/event_extra_static.dir/http.c.o"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/event_extra_static.dir/http.c.o -c /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/http.c

CMakeFiles/event_extra_static.dir/http.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/event_extra_static.dir/http.c.i"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/http.c > CMakeFiles/event_extra_static.dir/http.c.i

CMakeFiles/event_extra_static.dir/http.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/event_extra_static.dir/http.c.s"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/http.c -o CMakeFiles/event_extra_static.dir/http.c.s

CMakeFiles/event_extra_static.dir/evdns.c.o: CMakeFiles/event_extra_static.dir/flags.make
CMakeFiles/event_extra_static.dir/evdns.c.o: ../evdns.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/event_extra_static.dir/evdns.c.o"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/event_extra_static.dir/evdns.c.o -c /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/evdns.c

CMakeFiles/event_extra_static.dir/evdns.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/event_extra_static.dir/evdns.c.i"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/evdns.c > CMakeFiles/event_extra_static.dir/evdns.c.i

CMakeFiles/event_extra_static.dir/evdns.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/event_extra_static.dir/evdns.c.s"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/evdns.c -o CMakeFiles/event_extra_static.dir/evdns.c.s

CMakeFiles/event_extra_static.dir/ws.c.o: CMakeFiles/event_extra_static.dir/flags.make
CMakeFiles/event_extra_static.dir/ws.c.o: ../ws.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/event_extra_static.dir/ws.c.o"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/event_extra_static.dir/ws.c.o -c /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/ws.c

CMakeFiles/event_extra_static.dir/ws.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/event_extra_static.dir/ws.c.i"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/ws.c > CMakeFiles/event_extra_static.dir/ws.c.i

CMakeFiles/event_extra_static.dir/ws.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/event_extra_static.dir/ws.c.s"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/ws.c -o CMakeFiles/event_extra_static.dir/ws.c.s

CMakeFiles/event_extra_static.dir/sha1.c.o: CMakeFiles/event_extra_static.dir/flags.make
CMakeFiles/event_extra_static.dir/sha1.c.o: ../sha1.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/event_extra_static.dir/sha1.c.o"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -DLITTLE_ENDIAN=1 -o CMakeFiles/event_extra_static.dir/sha1.c.o -c /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/sha1.c

CMakeFiles/event_extra_static.dir/sha1.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/event_extra_static.dir/sha1.c.i"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -DLITTLE_ENDIAN=1 -E /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/sha1.c > CMakeFiles/event_extra_static.dir/sha1.c.i

CMakeFiles/event_extra_static.dir/sha1.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/event_extra_static.dir/sha1.c.s"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -DLITTLE_ENDIAN=1 -S /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/sha1.c -o CMakeFiles/event_extra_static.dir/sha1.c.s

CMakeFiles/event_extra_static.dir/evrpc.c.o: CMakeFiles/event_extra_static.dir/flags.make
CMakeFiles/event_extra_static.dir/evrpc.c.o: ../evrpc.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/event_extra_static.dir/evrpc.c.o"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/event_extra_static.dir/evrpc.c.o -c /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/evrpc.c

CMakeFiles/event_extra_static.dir/evrpc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/event_extra_static.dir/evrpc.c.i"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/evrpc.c > CMakeFiles/event_extra_static.dir/evrpc.c.i

CMakeFiles/event_extra_static.dir/evrpc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/event_extra_static.dir/evrpc.c.s"
	/home/kleftog/software/sidecfi/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/evrpc.c -o CMakeFiles/event_extra_static.dir/evrpc.c.s

# Object files for target event_extra_static
event_extra_static_OBJECTS = \
"CMakeFiles/event_extra_static.dir/event_tagging.c.o" \
"CMakeFiles/event_extra_static.dir/http.c.o" \
"CMakeFiles/event_extra_static.dir/evdns.c.o" \
"CMakeFiles/event_extra_static.dir/ws.c.o" \
"CMakeFiles/event_extra_static.dir/sha1.c.o" \
"CMakeFiles/event_extra_static.dir/evrpc.c.o"

# External object files for target event_extra_static
event_extra_static_EXTERNAL_OBJECTS =

lib/libevent_extra.a: CMakeFiles/event_extra_static.dir/event_tagging.c.o
lib/libevent_extra.a: CMakeFiles/event_extra_static.dir/http.c.o
lib/libevent_extra.a: CMakeFiles/event_extra_static.dir/evdns.c.o
lib/libevent_extra.a: CMakeFiles/event_extra_static.dir/ws.c.o
lib/libevent_extra.a: CMakeFiles/event_extra_static.dir/sha1.c.o
lib/libevent_extra.a: CMakeFiles/event_extra_static.dir/evrpc.c.o
lib/libevent_extra.a: CMakeFiles/event_extra_static.dir/build.make
lib/libevent_extra.a: CMakeFiles/event_extra_static.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking C static library lib/libevent_extra.a"
	$(CMAKE_COMMAND) -P CMakeFiles/event_extra_static.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/event_extra_static.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/event_extra_static.dir/build: lib/libevent_extra.a

.PHONY : CMakeFiles/event_extra_static.dir/build

CMakeFiles/event_extra_static.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/event_extra_static.dir/cmake_clean.cmake
.PHONY : CMakeFiles/event_extra_static.dir/clean

CMakeFiles/event_extra_static.dir/depend:
	cd /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/CMakeFiles/event_extra_static.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/event_extra_static.dir/depend
