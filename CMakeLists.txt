cmake_minimum_required(VERSION 3.16)
project(Koivisto)

set(CMAKE_CXX_STANDARD 14)

add_executable(Koivisto src_files/main.cpp
        src_files/Bitboard.h
        src_files/Bitboard.cpp
        src_files/Board.h
        src_files/Board.cpp
        src_files/Move.h
        src_files/Move.cpp
        src_files/TranspositionTable.h
        src_files/TranspositionTable.cpp
        src_files/Perft.h
        src_files/Perft.cpp
        src_files/Util.h
        src_files/Util.cpp
        src_files/search.cpp
        src_files/search.h
        src_files/uci.cpp
        src_files/uci.h
        src_files/eval.cpp
        src_files/eval.h
        src_files/MoveOrderer.cpp
        src_files/MoveOrderer.h
        src_files/History.cpp
        src_files/History.h
        src_files/Verification.h
        src_files/Tuning.cpp
        src_files/Tuning.h
        src_files/TimeManager.cpp
        src_files/TimeManager.h
        src_files/nn/DenseLayer.cpp
        src_files/nn/DenseLayer.h 
        src_files/nn/DenseInput.cpp 
        src_files/nn/DenseInput.h
        src_files/nn/DenseOutput.cpp
        src_files/nn/DenseOutput.h
        src_files/nn/DenseNetwork.cpp
        src_files/nn/DenseNetwork.h
        src_files/nn/data/CompactInput.cpp
        src_files/nn/data/CompactInput.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopt-info")
set(CMAKE_EXE_LINKER_FLAGS " -static")
target_link_libraries(Koivisto -static-libgcc -static-libstdc++)