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
        )