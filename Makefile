# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ms/Repos/json-query

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ms/Repos/json-query

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/bin/ccmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/ms/Repos/json-query/CMakeFiles /home/ms/Repos/json-query/CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/ms/Repos/json-query/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named jsonquery

# Build rule for target.
jsonquery: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 jsonquery
.PHONY : jsonquery

# fast build rule for target.
jsonquery/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/jsonquery.dir/build.make CMakeFiles/jsonquery.dir/build
.PHONY : jsonquery/fast

src/jsonquery.o: src/jsonquery.cpp.o

.PHONY : src/jsonquery.o

# target to build an object file
src/jsonquery.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/jsonquery.dir/build.make CMakeFiles/jsonquery.dir/src/jsonquery.cpp.o
.PHONY : src/jsonquery.cpp.o

src/jsonquery.i: src/jsonquery.cpp.i

.PHONY : src/jsonquery.i

# target to preprocess a source file
src/jsonquery.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/jsonquery.dir/build.make CMakeFiles/jsonquery.dir/src/jsonquery.cpp.i
.PHONY : src/jsonquery.cpp.i

src/jsonquery.s: src/jsonquery.cpp.s

.PHONY : src/jsonquery.s

# target to generate assembly for a file
src/jsonquery.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/jsonquery.dir/build.make CMakeFiles/jsonquery.dir/src/jsonquery.cpp.s
.PHONY : src/jsonquery.cpp.s

src/selector_parser.o: src/selector_parser.cpp.o

.PHONY : src/selector_parser.o

# target to build an object file
src/selector_parser.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/jsonquery.dir/build.make CMakeFiles/jsonquery.dir/src/selector_parser.cpp.o
.PHONY : src/selector_parser.cpp.o

src/selector_parser.i: src/selector_parser.cpp.i

.PHONY : src/selector_parser.i

# target to preprocess a source file
src/selector_parser.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/jsonquery.dir/build.make CMakeFiles/jsonquery.dir/src/selector_parser.cpp.i
.PHONY : src/selector_parser.cpp.i

src/selector_parser.s: src/selector_parser.cpp.s

.PHONY : src/selector_parser.s

# target to generate assembly for a file
src/selector_parser.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/jsonquery.dir/build.make CMakeFiles/jsonquery.dir/src/selector_parser.cpp.s
.PHONY : src/selector_parser.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... jsonquery"
	@echo "... src/jsonquery.o"
	@echo "... src/jsonquery.i"
	@echo "... src/jsonquery.s"
	@echo "... src/selector_parser.o"
	@echo "... src/selector_parser.i"
	@echo "... src/selector_parser.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

