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
CMAKE_SOURCE_DIR = /home/kim/Documents/Koivisto

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kim/Documents/Koivisto

# Include any dependencies generated for this target.
include CMakeFiles/Koivisto.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Koivisto.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Koivisto.dir/flags.make

CMakeFiles/Koivisto.dir/src_files/main.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/main.cpp.o: src_files/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Koivisto.dir/src_files/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/main.cpp.o -c /home/kim/Documents/Koivisto/src_files/main.cpp

CMakeFiles/Koivisto.dir/src_files/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/main.cpp > CMakeFiles/Koivisto.dir/src_files/main.cpp.i

CMakeFiles/Koivisto.dir/src_files/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/main.cpp -o CMakeFiles/Koivisto.dir/src_files/main.cpp.s

CMakeFiles/Koivisto.dir/src_files/Bitboard.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/Bitboard.cpp.o: src_files/Bitboard.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/Koivisto.dir/src_files/Bitboard.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/Bitboard.cpp.o -c /home/kim/Documents/Koivisto/src_files/Bitboard.cpp

CMakeFiles/Koivisto.dir/src_files/Bitboard.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/Bitboard.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/Bitboard.cpp > CMakeFiles/Koivisto.dir/src_files/Bitboard.cpp.i

CMakeFiles/Koivisto.dir/src_files/Bitboard.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/Bitboard.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/Bitboard.cpp -o CMakeFiles/Koivisto.dir/src_files/Bitboard.cpp.s

CMakeFiles/Koivisto.dir/src_files/Board.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/Board.cpp.o: src_files/Board.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/Koivisto.dir/src_files/Board.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/Board.cpp.o -c /home/kim/Documents/Koivisto/src_files/Board.cpp

CMakeFiles/Koivisto.dir/src_files/Board.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/Board.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/Board.cpp > CMakeFiles/Koivisto.dir/src_files/Board.cpp.i

CMakeFiles/Koivisto.dir/src_files/Board.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/Board.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/Board.cpp -o CMakeFiles/Koivisto.dir/src_files/Board.cpp.s

CMakeFiles/Koivisto.dir/src_files/Move.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/Move.cpp.o: src_files/Move.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/Koivisto.dir/src_files/Move.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/Move.cpp.o -c /home/kim/Documents/Koivisto/src_files/Move.cpp

CMakeFiles/Koivisto.dir/src_files/Move.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/Move.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/Move.cpp > CMakeFiles/Koivisto.dir/src_files/Move.cpp.i

CMakeFiles/Koivisto.dir/src_files/Move.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/Move.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/Move.cpp -o CMakeFiles/Koivisto.dir/src_files/Move.cpp.s

CMakeFiles/Koivisto.dir/src_files/TranspositionTable.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/TranspositionTable.cpp.o: src_files/TranspositionTable.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/Koivisto.dir/src_files/TranspositionTable.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/TranspositionTable.cpp.o -c /home/kim/Documents/Koivisto/src_files/TranspositionTable.cpp

CMakeFiles/Koivisto.dir/src_files/TranspositionTable.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/TranspositionTable.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/TranspositionTable.cpp > CMakeFiles/Koivisto.dir/src_files/TranspositionTable.cpp.i

CMakeFiles/Koivisto.dir/src_files/TranspositionTable.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/TranspositionTable.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/TranspositionTable.cpp -o CMakeFiles/Koivisto.dir/src_files/TranspositionTable.cpp.s

CMakeFiles/Koivisto.dir/src_files/Perft.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/Perft.cpp.o: src_files/Perft.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/Koivisto.dir/src_files/Perft.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/Perft.cpp.o -c /home/kim/Documents/Koivisto/src_files/Perft.cpp

CMakeFiles/Koivisto.dir/src_files/Perft.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/Perft.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/Perft.cpp > CMakeFiles/Koivisto.dir/src_files/Perft.cpp.i

CMakeFiles/Koivisto.dir/src_files/Perft.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/Perft.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/Perft.cpp -o CMakeFiles/Koivisto.dir/src_files/Perft.cpp.s

CMakeFiles/Koivisto.dir/src_files/Util.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/Util.cpp.o: src_files/Util.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/Koivisto.dir/src_files/Util.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/Util.cpp.o -c /home/kim/Documents/Koivisto/src_files/Util.cpp

CMakeFiles/Koivisto.dir/src_files/Util.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/Util.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/Util.cpp > CMakeFiles/Koivisto.dir/src_files/Util.cpp.i

CMakeFiles/Koivisto.dir/src_files/Util.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/Util.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/Util.cpp -o CMakeFiles/Koivisto.dir/src_files/Util.cpp.s

CMakeFiles/Koivisto.dir/src_files/search.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/search.cpp.o: src_files/search.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/Koivisto.dir/src_files/search.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/search.cpp.o -c /home/kim/Documents/Koivisto/src_files/search.cpp

CMakeFiles/Koivisto.dir/src_files/search.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/search.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/search.cpp > CMakeFiles/Koivisto.dir/src_files/search.cpp.i

CMakeFiles/Koivisto.dir/src_files/search.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/search.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/search.cpp -o CMakeFiles/Koivisto.dir/src_files/search.cpp.s

CMakeFiles/Koivisto.dir/src_files/uci.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/uci.cpp.o: src_files/uci.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/Koivisto.dir/src_files/uci.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/uci.cpp.o -c /home/kim/Documents/Koivisto/src_files/uci.cpp

CMakeFiles/Koivisto.dir/src_files/uci.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/uci.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/uci.cpp > CMakeFiles/Koivisto.dir/src_files/uci.cpp.i

CMakeFiles/Koivisto.dir/src_files/uci.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/uci.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/uci.cpp -o CMakeFiles/Koivisto.dir/src_files/uci.cpp.s

CMakeFiles/Koivisto.dir/src_files/eval.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/eval.cpp.o: src_files/eval.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/Koivisto.dir/src_files/eval.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/eval.cpp.o -c /home/kim/Documents/Koivisto/src_files/eval.cpp

CMakeFiles/Koivisto.dir/src_files/eval.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/eval.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/eval.cpp > CMakeFiles/Koivisto.dir/src_files/eval.cpp.i

CMakeFiles/Koivisto.dir/src_files/eval.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/eval.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/eval.cpp -o CMakeFiles/Koivisto.dir/src_files/eval.cpp.s

CMakeFiles/Koivisto.dir/src_files/MoveOrderer.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/MoveOrderer.cpp.o: src_files/MoveOrderer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/Koivisto.dir/src_files/MoveOrderer.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/MoveOrderer.cpp.o -c /home/kim/Documents/Koivisto/src_files/MoveOrderer.cpp

CMakeFiles/Koivisto.dir/src_files/MoveOrderer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/MoveOrderer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/MoveOrderer.cpp > CMakeFiles/Koivisto.dir/src_files/MoveOrderer.cpp.i

CMakeFiles/Koivisto.dir/src_files/MoveOrderer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/MoveOrderer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/MoveOrderer.cpp -o CMakeFiles/Koivisto.dir/src_files/MoveOrderer.cpp.s

CMakeFiles/Koivisto.dir/src_files/History.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/History.cpp.o: src_files/History.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object CMakeFiles/Koivisto.dir/src_files/History.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/History.cpp.o -c /home/kim/Documents/Koivisto/src_files/History.cpp

CMakeFiles/Koivisto.dir/src_files/History.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/History.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/History.cpp > CMakeFiles/Koivisto.dir/src_files/History.cpp.i

CMakeFiles/Koivisto.dir/src_files/History.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/History.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/History.cpp -o CMakeFiles/Koivisto.dir/src_files/History.cpp.s

CMakeFiles/Koivisto.dir/src_files/Tuning.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/Tuning.cpp.o: src_files/Tuning.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object CMakeFiles/Koivisto.dir/src_files/Tuning.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/Tuning.cpp.o -c /home/kim/Documents/Koivisto/src_files/Tuning.cpp

CMakeFiles/Koivisto.dir/src_files/Tuning.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/Tuning.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/Tuning.cpp > CMakeFiles/Koivisto.dir/src_files/Tuning.cpp.i

CMakeFiles/Koivisto.dir/src_files/Tuning.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/Tuning.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/Tuning.cpp -o CMakeFiles/Koivisto.dir/src_files/Tuning.cpp.s

CMakeFiles/Koivisto.dir/src_files/TimeManager.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/TimeManager.cpp.o: src_files/TimeManager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building CXX object CMakeFiles/Koivisto.dir/src_files/TimeManager.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/TimeManager.cpp.o -c /home/kim/Documents/Koivisto/src_files/TimeManager.cpp

CMakeFiles/Koivisto.dir/src_files/TimeManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/TimeManager.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/TimeManager.cpp > CMakeFiles/Koivisto.dir/src_files/TimeManager.cpp.i

CMakeFiles/Koivisto.dir/src_files/TimeManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/TimeManager.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/TimeManager.cpp -o CMakeFiles/Koivisto.dir/src_files/TimeManager.cpp.s

CMakeFiles/Koivisto.dir/src_files/nn/DenseLayer.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/nn/DenseLayer.cpp.o: src_files/nn/DenseLayer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building CXX object CMakeFiles/Koivisto.dir/src_files/nn/DenseLayer.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/nn/DenseLayer.cpp.o -c /home/kim/Documents/Koivisto/src_files/nn/DenseLayer.cpp

CMakeFiles/Koivisto.dir/src_files/nn/DenseLayer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/nn/DenseLayer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/nn/DenseLayer.cpp > CMakeFiles/Koivisto.dir/src_files/nn/DenseLayer.cpp.i

CMakeFiles/Koivisto.dir/src_files/nn/DenseLayer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/nn/DenseLayer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/nn/DenseLayer.cpp -o CMakeFiles/Koivisto.dir/src_files/nn/DenseLayer.cpp.s

CMakeFiles/Koivisto.dir/src_files/nn/DenseInput.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/nn/DenseInput.cpp.o: src_files/nn/DenseInput.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Building CXX object CMakeFiles/Koivisto.dir/src_files/nn/DenseInput.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/nn/DenseInput.cpp.o -c /home/kim/Documents/Koivisto/src_files/nn/DenseInput.cpp

CMakeFiles/Koivisto.dir/src_files/nn/DenseInput.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/nn/DenseInput.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/nn/DenseInput.cpp > CMakeFiles/Koivisto.dir/src_files/nn/DenseInput.cpp.i

CMakeFiles/Koivisto.dir/src_files/nn/DenseInput.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/nn/DenseInput.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/nn/DenseInput.cpp -o CMakeFiles/Koivisto.dir/src_files/nn/DenseInput.cpp.s

CMakeFiles/Koivisto.dir/src_files/nn/DenseOutput.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/nn/DenseOutput.cpp.o: src_files/nn/DenseOutput.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_17) "Building CXX object CMakeFiles/Koivisto.dir/src_files/nn/DenseOutput.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/nn/DenseOutput.cpp.o -c /home/kim/Documents/Koivisto/src_files/nn/DenseOutput.cpp

CMakeFiles/Koivisto.dir/src_files/nn/DenseOutput.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/nn/DenseOutput.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/nn/DenseOutput.cpp > CMakeFiles/Koivisto.dir/src_files/nn/DenseOutput.cpp.i

CMakeFiles/Koivisto.dir/src_files/nn/DenseOutput.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/nn/DenseOutput.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/nn/DenseOutput.cpp -o CMakeFiles/Koivisto.dir/src_files/nn/DenseOutput.cpp.s

CMakeFiles/Koivisto.dir/src_files/nn/DenseNetwork.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/nn/DenseNetwork.cpp.o: src_files/nn/DenseNetwork.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_18) "Building CXX object CMakeFiles/Koivisto.dir/src_files/nn/DenseNetwork.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/nn/DenseNetwork.cpp.o -c /home/kim/Documents/Koivisto/src_files/nn/DenseNetwork.cpp

CMakeFiles/Koivisto.dir/src_files/nn/DenseNetwork.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/nn/DenseNetwork.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/nn/DenseNetwork.cpp > CMakeFiles/Koivisto.dir/src_files/nn/DenseNetwork.cpp.i

CMakeFiles/Koivisto.dir/src_files/nn/DenseNetwork.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/nn/DenseNetwork.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/nn/DenseNetwork.cpp -o CMakeFiles/Koivisto.dir/src_files/nn/DenseNetwork.cpp.s

CMakeFiles/Koivisto.dir/src_files/nn/data/CompactInput.cpp.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/nn/data/CompactInput.cpp.o: src_files/nn/data/CompactInput.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_19) "Building CXX object CMakeFiles/Koivisto.dir/src_files/nn/data/CompactInput.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/nn/data/CompactInput.cpp.o -c /home/kim/Documents/Koivisto/src_files/nn/data/CompactInput.cpp

CMakeFiles/Koivisto.dir/src_files/nn/data/CompactInput.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Koivisto.dir/src_files/nn/data/CompactInput.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kim/Documents/Koivisto/src_files/nn/data/CompactInput.cpp > CMakeFiles/Koivisto.dir/src_files/nn/data/CompactInput.cpp.i

CMakeFiles/Koivisto.dir/src_files/nn/data/CompactInput.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Koivisto.dir/src_files/nn/data/CompactInput.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kim/Documents/Koivisto/src_files/nn/data/CompactInput.cpp -o CMakeFiles/Koivisto.dir/src_files/nn/data/CompactInput.cpp.s

CMakeFiles/Koivisto.dir/src_files/syzygy/tbprobe.c.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/syzygy/tbprobe.c.o: src_files/syzygy/tbprobe.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_20) "Building C object CMakeFiles/Koivisto.dir/src_files/syzygy/tbprobe.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/syzygy/tbprobe.c.o   -c /home/kim/Documents/Koivisto/src_files/syzygy/tbprobe.c

CMakeFiles/Koivisto.dir/src_files/syzygy/tbprobe.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Koivisto.dir/src_files/syzygy/tbprobe.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kim/Documents/Koivisto/src_files/syzygy/tbprobe.c > CMakeFiles/Koivisto.dir/src_files/syzygy/tbprobe.c.i

CMakeFiles/Koivisto.dir/src_files/syzygy/tbprobe.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Koivisto.dir/src_files/syzygy/tbprobe.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kim/Documents/Koivisto/src_files/syzygy/tbprobe.c -o CMakeFiles/Koivisto.dir/src_files/syzygy/tbprobe.c.s

CMakeFiles/Koivisto.dir/src_files/syzygy/tbchess.c.o: CMakeFiles/Koivisto.dir/flags.make
CMakeFiles/Koivisto.dir/src_files/syzygy/tbchess.c.o: src_files/syzygy/tbchess.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_21) "Building C object CMakeFiles/Koivisto.dir/src_files/syzygy/tbchess.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Koivisto.dir/src_files/syzygy/tbchess.c.o   -c /home/kim/Documents/Koivisto/src_files/syzygy/tbchess.c

CMakeFiles/Koivisto.dir/src_files/syzygy/tbchess.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Koivisto.dir/src_files/syzygy/tbchess.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kim/Documents/Koivisto/src_files/syzygy/tbchess.c > CMakeFiles/Koivisto.dir/src_files/syzygy/tbchess.c.i

CMakeFiles/Koivisto.dir/src_files/syzygy/tbchess.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Koivisto.dir/src_files/syzygy/tbchess.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kim/Documents/Koivisto/src_files/syzygy/tbchess.c -o CMakeFiles/Koivisto.dir/src_files/syzygy/tbchess.c.s

# Object files for target Koivisto
Koivisto_OBJECTS = \
"CMakeFiles/Koivisto.dir/src_files/main.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/Bitboard.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/Board.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/Move.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/TranspositionTable.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/Perft.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/Util.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/search.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/uci.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/eval.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/MoveOrderer.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/History.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/Tuning.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/TimeManager.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/nn/DenseLayer.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/nn/DenseInput.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/nn/DenseOutput.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/nn/DenseNetwork.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/nn/data/CompactInput.cpp.o" \
"CMakeFiles/Koivisto.dir/src_files/syzygy/tbprobe.c.o" \
"CMakeFiles/Koivisto.dir/src_files/syzygy/tbchess.c.o"

# External object files for target Koivisto
Koivisto_EXTERNAL_OBJECTS =

Koivisto: CMakeFiles/Koivisto.dir/src_files/main.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/Bitboard.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/Board.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/Move.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/TranspositionTable.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/Perft.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/Util.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/search.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/uci.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/eval.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/MoveOrderer.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/History.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/Tuning.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/TimeManager.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/nn/DenseLayer.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/nn/DenseInput.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/nn/DenseOutput.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/nn/DenseNetwork.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/nn/data/CompactInput.cpp.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/syzygy/tbprobe.c.o
Koivisto: CMakeFiles/Koivisto.dir/src_files/syzygy/tbchess.c.o
Koivisto: CMakeFiles/Koivisto.dir/build.make
Koivisto: CMakeFiles/Koivisto.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/kim/Documents/Koivisto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_22) "Linking CXX executable Koivisto"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Koivisto.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Koivisto.dir/build: Koivisto

.PHONY : CMakeFiles/Koivisto.dir/build

CMakeFiles/Koivisto.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Koivisto.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Koivisto.dir/clean

CMakeFiles/Koivisto.dir/depend:
	cd /home/kim/Documents/Koivisto && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kim/Documents/Koivisto /home/kim/Documents/Koivisto /home/kim/Documents/Koivisto /home/kim/Documents/Koivisto /home/kim/Documents/Koivisto/CMakeFiles/Koivisto.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Koivisto.dir/depend

