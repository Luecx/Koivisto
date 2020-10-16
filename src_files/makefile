#thank you @Andrew Grant for your help with this file! :)

CC      = g++
SRC     = *.cpp syzygy/tbprobe.c
LIBS    = -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
FOLDER  = bin/
ROOT    = ../
NAME    = Koivisto
MINOR   = 2
MAJOR   = 2
EXE     = $(ROOT)$(FOLDER)$(NAME)_$(MAJOR).$(MINOR)

WFLAGS = -std=c++17 -Wall -Wextra -Wshadow
CFLAGS = -O3 $(WFLAGS) -DNDEBUG -flto -march=native
RFLAGS = -O3 $(WFLAGS) -DNDEBUG -flto -static
PFLAGS = -O0 $(WFLAGS) -DNDEBUG -p -pg
DFLAGS = -O0 $(WFLAGS) -g

POPCNTFLAGS = -DUSE_POPCNT -msse3 -mpopcnt
PEXTFLAGS   = $(POPCNTFLAGS) -DUSE_PEXT -mbmi2
AVX2FLAGS   = -msse -msse3 -mpopcnt -mavx2 -msse4.1 -mssse3 -msse2

MAKROS      = -DMINOR_VERSION=$(MINOR) -DMAJOR_VERSION=$(MAJOR)

native:
	$(CC) $(CFLAGS) $(SRC) $(MAKROS) $(LIBS) $(POPCNTFLAGS) -o $(EXE)-x64-native.exe
    

release:
	$(CC) $(CFLAGS) $(SRC) $(MAKROS) $(LIBS) $(POPCNTFLAGS) -o $(EXE)-x64-native.exe
	$(CC) $(RFLAGS) $(SRC) $(MAKROS) $(LIBS) -o $(EXE)-x64.exe
	$(CC) $(RFLAGS) $(SRC) $(MAKROS) $(LIBS) $(POPCNTFLAGS) -o $(EXE)-popcnt.exe
	$(CC) $(RFLAGS) $(SRC) $(MAKROS) $(LIBS) $(POPCNTFLAGS) $(AVX) -o $(EXE)-popcnt-avx.exe

	
