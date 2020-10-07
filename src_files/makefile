CC      = g++
SRC     = *.cpp syzygy/*.c eun/*.cpp eun/data/*.cpp eun/generation/*.cpp
#SRC     = *.cpp syzygy/tbprobe.c
LIBS    = -lpthread -lm
EXE     = ../Koivisto

WFLAGS = -std=c++17 -Wall -Wextra -Wshadow
CFLAGS = -O3 $(WFLAGS) -DNDEBUG -flto -march=native
CCFLAGS = -O2 $(WFLAGS) -DNDEBUG -flto -march=native
#RFLAGS = -O3 $(WFLAGS) -DNDEBUG -flto -static
#TFLAGS = -O3 $(WFLAGS) -DNDEBUG -flto -march=native -fopenmp -DTUNE
#PFLAGS = -O0 $(WFLAGS) -DNDEBUG -p -pg
DFLAGS = -O0 $(WFLAGS) -g

POPCNTFLAGS = -DUSE_POPCNT -msse3 -mpopcnt
PEXTFLAGS   = $(POPCNTFLAGS) -DUSE_PEXT -mbmi2



release:
	$(CC) $(CCFLAGS) $(SRC) $(LIBS) $(POPCNTFLAGS) -o $(EXE)
