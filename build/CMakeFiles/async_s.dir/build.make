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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jbj/Desktop/myjbj/proj/connect-pool/grpc-demo/demo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jbj/Desktop/myjbj/proj/connect-pool/grpc-demo/demo/build

# Include any dependencies generated for this target.
include CMakeFiles/async_s.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/async_s.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/async_s.dir/flags.make

CMakeFiles/async_s.dir/async_s.cc.o: CMakeFiles/async_s.dir/flags.make
CMakeFiles/async_s.dir/async_s.cc.o: ../async_s.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jbj/Desktop/myjbj/proj/connect-pool/grpc-demo/demo/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/async_s.dir/async_s.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/async_s.dir/async_s.cc.o -c /home/jbj/Desktop/myjbj/proj/connect-pool/grpc-demo/demo/async_s.cc

CMakeFiles/async_s.dir/async_s.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/async_s.dir/async_s.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jbj/Desktop/myjbj/proj/connect-pool/grpc-demo/demo/async_s.cc > CMakeFiles/async_s.dir/async_s.cc.i

CMakeFiles/async_s.dir/async_s.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/async_s.dir/async_s.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jbj/Desktop/myjbj/proj/connect-pool/grpc-demo/demo/async_s.cc -o CMakeFiles/async_s.dir/async_s.cc.s

# Object files for target async_s
async_s_OBJECTS = \
"CMakeFiles/async_s.dir/async_s.cc.o"

# External object files for target async_s
async_s_EXTERNAL_OBJECTS =

async_s: CMakeFiles/async_s.dir/async_s.cc.o
async_s: CMakeFiles/async_s.dir/build.make
async_s: libhw_grpc_proto.a
async_s: /usr/local/lib/libgrpc++_reflection.a
async_s: /usr/local/lib/libgrpc++.a
async_s: /usr/local/lib/libprotobuf.a
async_s: /usr/local/lib/libgrpc.a
async_s: /usr/local/lib/libz.a
async_s: /usr/local/lib/libcares.a
async_s: /usr/local/lib/libaddress_sorting.a
async_s: /usr/local/lib/libre2.a
async_s: /usr/local/lib/libabsl_raw_hash_set.a
async_s: /usr/local/lib/libabsl_hashtablez_sampler.a
async_s: /usr/local/lib/libabsl_hash.a
async_s: /usr/local/lib/libabsl_city.a
async_s: /usr/local/lib/libabsl_low_level_hash.a
async_s: /usr/local/lib/libabsl_statusor.a
async_s: /usr/local/lib/libabsl_bad_variant_access.a
async_s: /usr/local/lib/libgpr.a
async_s: /usr/local/lib/libupb.a
async_s: /usr/local/lib/libabsl_status.a
async_s: /usr/local/lib/libabsl_random_distributions.a
async_s: /usr/local/lib/libabsl_random_seed_sequences.a
async_s: /usr/local/lib/libabsl_random_internal_pool_urbg.a
async_s: /usr/local/lib/libabsl_random_internal_randen.a
async_s: /usr/local/lib/libabsl_random_internal_randen_hwaes.a
async_s: /usr/local/lib/libabsl_random_internal_randen_hwaes_impl.a
async_s: /usr/local/lib/libabsl_random_internal_randen_slow.a
async_s: /usr/local/lib/libabsl_random_internal_platform.a
async_s: /usr/local/lib/libabsl_random_internal_seed_material.a
async_s: /usr/local/lib/libabsl_random_seed_gen_exception.a
async_s: /usr/local/lib/libabsl_cord.a
async_s: /usr/local/lib/libabsl_bad_optional_access.a
async_s: /usr/local/lib/libabsl_cordz_info.a
async_s: /usr/local/lib/libabsl_cord_internal.a
async_s: /usr/local/lib/libabsl_cordz_functions.a
async_s: /usr/local/lib/libabsl_exponential_biased.a
async_s: /usr/local/lib/libabsl_cordz_handle.a
async_s: /usr/local/lib/libabsl_str_format_internal.a
async_s: /usr/local/lib/libabsl_synchronization.a
async_s: /usr/local/lib/libabsl_stacktrace.a
async_s: /usr/local/lib/libabsl_symbolize.a
async_s: /usr/local/lib/libabsl_debugging_internal.a
async_s: /usr/local/lib/libabsl_demangle_internal.a
async_s: /usr/local/lib/libabsl_graphcycles_internal.a
async_s: /usr/local/lib/libabsl_malloc_internal.a
async_s: /usr/local/lib/libabsl_time.a
async_s: /usr/local/lib/libabsl_strings.a
async_s: /usr/local/lib/libabsl_throw_delegate.a
async_s: /usr/local/lib/libabsl_int128.a
async_s: /usr/local/lib/libabsl_strings_internal.a
async_s: /usr/local/lib/libabsl_base.a
async_s: /usr/local/lib/libabsl_spinlock_wait.a
async_s: /usr/local/lib/libabsl_raw_logging_internal.a
async_s: /usr/local/lib/libabsl_log_severity.a
async_s: /usr/local/lib/libabsl_civil_time.a
async_s: /usr/local/lib/libabsl_time_zone.a
async_s: /usr/local/lib/libssl.a
async_s: /usr/local/lib/libcrypto.a
async_s: CMakeFiles/async_s.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jbj/Desktop/myjbj/proj/connect-pool/grpc-demo/demo/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable async_s"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/async_s.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/async_s.dir/build: async_s

.PHONY : CMakeFiles/async_s.dir/build

CMakeFiles/async_s.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/async_s.dir/cmake_clean.cmake
.PHONY : CMakeFiles/async_s.dir/clean

CMakeFiles/async_s.dir/depend:
	cd /home/jbj/Desktop/myjbj/proj/connect-pool/grpc-demo/demo/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jbj/Desktop/myjbj/proj/connect-pool/grpc-demo/demo /home/jbj/Desktop/myjbj/proj/connect-pool/grpc-demo/demo /home/jbj/Desktop/myjbj/proj/connect-pool/grpc-demo/demo/build /home/jbj/Desktop/myjbj/proj/connect-pool/grpc-demo/demo/build /home/jbj/Desktop/myjbj/proj/connect-pool/grpc-demo/demo/build/CMakeFiles/async_s.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/async_s.dir/depend

