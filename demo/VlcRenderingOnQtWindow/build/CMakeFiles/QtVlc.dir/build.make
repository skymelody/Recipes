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
CMAKE_SOURCE_DIR = /home/skymelody/XSpace/TestArea/TestQtVlc

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/skymelody/XSpace/TestArea/TestQtVlc/build

# Include any dependencies generated for this target.
include CMakeFiles/QtVlc.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/QtVlc.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/QtVlc.dir/flags.make

CMakeFiles/QtVlc.dir/QtVlc_autogen/mocs_compilation.cpp.o: CMakeFiles/QtVlc.dir/flags.make
CMakeFiles/QtVlc.dir/QtVlc_autogen/mocs_compilation.cpp.o: QtVlc_autogen/mocs_compilation.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/skymelody/XSpace/TestArea/TestQtVlc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/QtVlc.dir/QtVlc_autogen/mocs_compilation.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/QtVlc.dir/QtVlc_autogen/mocs_compilation.cpp.o -c /home/skymelody/XSpace/TestArea/TestQtVlc/build/QtVlc_autogen/mocs_compilation.cpp

CMakeFiles/QtVlc.dir/QtVlc_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QtVlc.dir/QtVlc_autogen/mocs_compilation.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/skymelody/XSpace/TestArea/TestQtVlc/build/QtVlc_autogen/mocs_compilation.cpp > CMakeFiles/QtVlc.dir/QtVlc_autogen/mocs_compilation.cpp.i

CMakeFiles/QtVlc.dir/QtVlc_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QtVlc.dir/QtVlc_autogen/mocs_compilation.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/skymelody/XSpace/TestArea/TestQtVlc/build/QtVlc_autogen/mocs_compilation.cpp -o CMakeFiles/QtVlc.dir/QtVlc_autogen/mocs_compilation.cpp.s

CMakeFiles/QtVlc.dir/main.cc.o: CMakeFiles/QtVlc.dir/flags.make
CMakeFiles/QtVlc.dir/main.cc.o: ../main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/skymelody/XSpace/TestArea/TestQtVlc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/QtVlc.dir/main.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/QtVlc.dir/main.cc.o -c /home/skymelody/XSpace/TestArea/TestQtVlc/main.cc

CMakeFiles/QtVlc.dir/main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QtVlc.dir/main.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/skymelody/XSpace/TestArea/TestQtVlc/main.cc > CMakeFiles/QtVlc.dir/main.cc.i

CMakeFiles/QtVlc.dir/main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QtVlc.dir/main.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/skymelody/XSpace/TestArea/TestQtVlc/main.cc -o CMakeFiles/QtVlc.dir/main.cc.s

# Object files for target QtVlc
QtVlc_OBJECTS = \
"CMakeFiles/QtVlc.dir/QtVlc_autogen/mocs_compilation.cpp.o" \
"CMakeFiles/QtVlc.dir/main.cc.o"

# External object files for target QtVlc
QtVlc_EXTERNAL_OBJECTS =

QtVlc: CMakeFiles/QtVlc.dir/QtVlc_autogen/mocs_compilation.cpp.o
QtVlc: CMakeFiles/QtVlc.dir/main.cc.o
QtVlc: CMakeFiles/QtVlc.dir/build.make
QtVlc: /usr/lib/x86_64-linux-gnu/libQt5Widgets.so.5.14.2
QtVlc: /usr/lib/x86_64-linux-gnu/libQt5Gui.so.5.14.2
QtVlc: /usr/lib/x86_64-linux-gnu/libQt5Core.so.5.14.2
QtVlc: CMakeFiles/QtVlc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/skymelody/XSpace/TestArea/TestQtVlc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable QtVlc"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/QtVlc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/QtVlc.dir/build: QtVlc

.PHONY : CMakeFiles/QtVlc.dir/build

CMakeFiles/QtVlc.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/QtVlc.dir/cmake_clean.cmake
.PHONY : CMakeFiles/QtVlc.dir/clean

CMakeFiles/QtVlc.dir/depend:
	cd /home/skymelody/XSpace/TestArea/TestQtVlc/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/skymelody/XSpace/TestArea/TestQtVlc /home/skymelody/XSpace/TestArea/TestQtVlc /home/skymelody/XSpace/TestArea/TestQtVlc/build /home/skymelody/XSpace/TestArea/TestQtVlc/build /home/skymelody/XSpace/TestArea/TestQtVlc/build/CMakeFiles/QtVlc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/QtVlc.dir/depend

