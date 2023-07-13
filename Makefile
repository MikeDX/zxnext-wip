# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.26

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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.26.3/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.26.3/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/mike/src/specnext-sprites-example

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/mike/src/specnext-sprites-example

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/opt/homebrew/Cellar/cmake/3.26.3/bin/ccmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/opt/homebrew/Cellar/cmake/3.26.3/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /Users/mike/src/specnext-sprites-example/CMakeFiles /Users/mike/src/specnext-sprites-example//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /Users/mike/src/specnext-sprites-example/CMakeFiles 0
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
# Target rules for targets named zxn_sprite_1

# Build rule for target.
zxn_sprite_1: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 zxn_sprite_1
.PHONY : zxn_sprite_1

# fast build rule for target.
zxn_sprite_1/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/zxn_sprite_1.dir/build.make CMakeFiles/zxn_sprite_1.dir/build
.PHONY : zxn_sprite_1/fast

src/layer2.o: src/layer2.c.o
.PHONY : src/layer2.o

# target to build an object file
src/layer2.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/zxn_sprite_1.dir/build.make CMakeFiles/zxn_sprite_1.dir/src/layer2.c.o
.PHONY : src/layer2.c.o

src/layer2.i: src/layer2.c.i
.PHONY : src/layer2.i

# target to preprocess a source file
src/layer2.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/zxn_sprite_1.dir/build.make CMakeFiles/zxn_sprite_1.dir/src/layer2.c.i
.PHONY : src/layer2.c.i

src/layer2.s: src/layer2.c.s
.PHONY : src/layer2.s

# target to generate assembly for a file
src/layer2.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/zxn_sprite_1.dir/build.make CMakeFiles/zxn_sprite_1.dir/src/layer2.c.s
.PHONY : src/layer2.c.s

src/main.o: src/main.c.o
.PHONY : src/main.o

# target to build an object file
src/main.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/zxn_sprite_1.dir/build.make CMakeFiles/zxn_sprite_1.dir/src/main.c.o
.PHONY : src/main.c.o

src/main.i: src/main.c.i
.PHONY : src/main.i

# target to preprocess a source file
src/main.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/zxn_sprite_1.dir/build.make CMakeFiles/zxn_sprite_1.dir/src/main.c.i
.PHONY : src/main.c.i

src/main.s: src/main.c.s
.PHONY : src/main.s

# target to generate assembly for a file
src/main.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/zxn_sprite_1.dir/build.make CMakeFiles/zxn_sprite_1.dir/src/main.c.s
.PHONY : src/main.c.s

src/sprites.o: src/sprites.c.o
.PHONY : src/sprites.o

# target to build an object file
src/sprites.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/zxn_sprite_1.dir/build.make CMakeFiles/zxn_sprite_1.dir/src/sprites.c.o
.PHONY : src/sprites.c.o

src/sprites.i: src/sprites.c.i
.PHONY : src/sprites.i

# target to preprocess a source file
src/sprites.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/zxn_sprite_1.dir/build.make CMakeFiles/zxn_sprite_1.dir/src/sprites.c.i
.PHONY : src/sprites.c.i

src/sprites.s: src/sprites.c.s
.PHONY : src/sprites.s

# target to generate assembly for a file
src/sprites.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/zxn_sprite_1.dir/build.make CMakeFiles/zxn_sprite_1.dir/src/sprites.c.s
.PHONY : src/sprites.c.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... zxn_sprite_1"
	@echo "... src/layer2.o"
	@echo "... src/layer2.i"
	@echo "... src/layer2.s"
	@echo "... src/main.o"
	@echo "... src/main.i"
	@echo "... src/main.s"
	@echo "... src/sprites.o"
	@echo "... src/sprites.i"
	@echo "... src/sprites.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

