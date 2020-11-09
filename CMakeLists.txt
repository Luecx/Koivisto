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
        src_files/Tuning.cpp
        src_files/Tuning.h
        src_files/TimeManager.cpp
        src_files/TimeManager.h
        src_files/syzygy/stdendian.h
        src_files/syzygy/tbprobe.h
        src_files/syzygy/tbconfig.h
        src_files/syzygy/tbprobe.c
        src_files/syzygy/tbchess.c
        src_files/eun/DenseInput.cpp
        src_files/eun/DenseLayer.cpp
        src_files/eun/DenseNetwork.cpp
        src_files/eun/DenseOutput.cpp
        src_files/eun/DenseInput.h
        src_files/eun/DenseLayer.h
        src_files/eun/DenseNetwork.h
        src_files/eun/DenseOutput.h
        src_files/eun/nntools.h
        src_files/eun/data/CompactInput.cpp
        src_files/eun/data/CompactInput.h
        src_files/eun/data/Weight.cpp
        src_files/eun/data/Weight.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMINOR_VERSION=0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMAJOR_VERSION=4")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopt-info")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--whole-archive -lpthread -Wl,--no-whole-archive")
set(CMAKE_EXE_LINKER_FLAGS " -static")

target_link_libraries(Koivisto -static-libgcc -static-libstdc++)
