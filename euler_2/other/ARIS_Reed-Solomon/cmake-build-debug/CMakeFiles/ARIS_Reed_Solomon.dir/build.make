# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.17

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

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2020.2.1\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2020.2.1\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/ARIS_Reed_Solomon.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ARIS_Reed_Solomon.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ARIS_Reed_Solomon.dir/flags.make

CMakeFiles/ARIS_Reed_Solomon.dir/main.c.obj: CMakeFiles/ARIS_Reed_Solomon.dir/flags.make
CMakeFiles/ARIS_Reed_Solomon.dir/main.c.obj: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/ARIS_Reed_Solomon.dir/main.c.obj"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\ARIS_Reed_Solomon.dir\main.c.obj   -c C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\main.c

CMakeFiles/ARIS_Reed_Solomon.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ARIS_Reed_Solomon.dir/main.c.i"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\main.c > CMakeFiles\ARIS_Reed_Solomon.dir\main.c.i

CMakeFiles/ARIS_Reed_Solomon.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ARIS_Reed_Solomon.dir/main.c.s"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\main.c -o CMakeFiles\ARIS_Reed_Solomon.dir\main.c.s

CMakeFiles/ARIS_Reed_Solomon.dir/RS/RS.c.obj: CMakeFiles/ARIS_Reed_Solomon.dir/flags.make
CMakeFiles/ARIS_Reed_Solomon.dir/RS/RS.c.obj: ../RS/RS.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/ARIS_Reed_Solomon.dir/RS/RS.c.obj"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\ARIS_Reed_Solomon.dir\RS\RS.c.obj   -c C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\RS\RS.c

CMakeFiles/ARIS_Reed_Solomon.dir/RS/RS.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ARIS_Reed_Solomon.dir/RS/RS.c.i"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\RS\RS.c > CMakeFiles\ARIS_Reed_Solomon.dir\RS\RS.c.i

CMakeFiles/ARIS_Reed_Solomon.dir/RS/RS.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ARIS_Reed_Solomon.dir/RS/RS.c.s"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\RS\RS.c -o CMakeFiles\ARIS_Reed_Solomon.dir\RS\RS.c.s

# Object files for target ARIS_Reed_Solomon
ARIS_Reed_Solomon_OBJECTS = \
"CMakeFiles/ARIS_Reed_Solomon.dir/main.c.obj" \
"CMakeFiles/ARIS_Reed_Solomon.dir/RS/RS.c.obj"

# External object files for target ARIS_Reed_Solomon
ARIS_Reed_Solomon_EXTERNAL_OBJECTS =

ARIS_Reed_Solomon.exe: CMakeFiles/ARIS_Reed_Solomon.dir/main.c.obj
ARIS_Reed_Solomon.exe: CMakeFiles/ARIS_Reed_Solomon.dir/RS/RS.c.obj
ARIS_Reed_Solomon.exe: CMakeFiles/ARIS_Reed_Solomon.dir/build.make
ARIS_Reed_Solomon.exe: CMakeFiles/ARIS_Reed_Solomon.dir/linklibs.rsp
ARIS_Reed_Solomon.exe: CMakeFiles/ARIS_Reed_Solomon.dir/objects1.rsp
ARIS_Reed_Solomon.exe: CMakeFiles/ARIS_Reed_Solomon.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable ARIS_Reed_Solomon.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\ARIS_Reed_Solomon.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ARIS_Reed_Solomon.dir/build: ARIS_Reed_Solomon.exe

.PHONY : CMakeFiles/ARIS_Reed_Solomon.dir/build

CMakeFiles/ARIS_Reed_Solomon.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\ARIS_Reed_Solomon.dir\cmake_clean.cmake
.PHONY : CMakeFiles/ARIS_Reed_Solomon.dir/clean

CMakeFiles/ARIS_Reed_Solomon.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug C:\Users\imrek\CLionProjects\ARIS_Reed-Solomon\cmake-build-debug\CMakeFiles\ARIS_Reed_Solomon.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ARIS_Reed_Solomon.dir/depend
