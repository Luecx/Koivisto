cmake_minimum_required(VERSION 3.16)
project(Koivisto VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRCS src_files/*.c src_files/*.cpp src_files/*.h)

add_executable(Koivisto ${SRCS})
add_definitions(-DEVALFILE=\"../networks/default.net\")
add_definitions(-DMINOR_VERSION=15)
add_definitions(-DMAJOR_VERSION=7)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--whole-archive -lpthread -Wl,--no-whole-archive")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_EXE_LINKER_FLAGS " -static")

target_link_libraries(Koivisto -static-libgcc -static-libstdc++)
