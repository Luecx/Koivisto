cmake_minimum_required(VERSION 3.16)
project(Koivisto VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

add_executable(Koivisto
        src_files/main.cpp
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
        src_files/TimeManager.cpp
        src_files/TimeManager.h
        src_files/polyglot.cpp
        src_files/polyglot.h
        src_files/syzygy/stdendian.h
        src_files/syzygy/tbprobe.h
        src_files/syzygy/tbconfig.h
        src_files/syzygy/tbprobe.c
        src_files/syzygy/tbchess.c
        src_files/gradient.h
        src_files/movegen.h
        src_files/movegen.cpp
        src_files/incbin/incbin.h)

add_definitions(-DEVALFILE=\"../networks/default.net\")
add_definitions(-DMINOR_VERSION=14)
add_definitions(-DMAJOR_VERSION=5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--whole-archive -lpthread -Wl,--no-whole-archive")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_EXE_LINKER_FLAGS " -static")

target_link_libraries(Koivisto -static-libgcc -static-libstdc++)
