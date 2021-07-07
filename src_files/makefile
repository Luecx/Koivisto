CC       = g++
SRC      = *.cpp syzygy/tbprobe.c
LIBS     = -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
FOLDER   = bin/
ROOT     = ../
NAME     = Koivisto
EVALFILE = default.net
EXE      = $(ROOT)$(FOLDER)$(NAME)_$(MAJOR).$(MINOR)
MINOR    = 80
MAJOR    = 4
ifeq ($(OS),Windows_NT)
    PREFIX := windows
    SUFFIX := .exe
else
    PREFIX := linux
    SUFFIX := 
endif

WFLAGS = -std=c++17 -Wall -Wextra -Wshadow -DEVALFILE=\"$(EVALFILE)\"
CFLAGS = -O3 $(WFLAGS) -DNDEBUG -flto -march=native
RFLAGS = -O3 $(WFLAGS) -DNDEBUG -flto -static
PFLAGS = -O0 $(WFLAGS) -DNDEBUG -p -pg
DFLAGS = -O0 $(WFLAGS) -g

POPCNTFLAGS = -DUSE_POPCNT -msse3 -mpopcnt
PEXTFLAGS   = $(POPCNTFLAGS) -DUSE_PEXT -mbmi2
AVX2FLAGS   = -msse -msse3 -mpopcnt -mavx2 -msse4.1 -mssse3 -msse2
OPENMPFLAGS = -fopenmp

MAKROS      = -DMINOR_VERSION=$(MINOR) -DMAJOR_VERSION=$(MAJOR)

openbench:
	$(CC) $(CFLAGS) $(SRC) $(MAKROS) $(LIBS) $(POPCNTFLAGS) -o $(EXE)

native:
	mkdir -p $(ROOT)$(FOLDER)
	$(CC) $(CFLAGS) $(SRC) $(MAKROS) $(LIBS) $(POPCNTFLAGS) -o $(EXE)-x64-$(PREFIX)-native$(SUFFIX)
    
release:
	mkdir -p $(ROOT)$(FOLDER)
	$(CC) $(CFLAGS) $(SRC) $(MAKROS) $(LIBS) $(POPCNTFLAGS) -o $(EXE)-x64-$(PREFIX)-native$(SUFFIX)
	$(CC) $(RFLAGS) $(SRC) $(MAKROS) $(LIBS) -o $(EXE)-x64-$(PREFIX)$(SUFFIX)
	$(CC) $(RFLAGS) $(SRC) $(MAKROS) $(LIBS) $(POPCNTFLAGS) -o $(EXE)-x64-$(PREFIX)-popcnt$(SUFFIX)
	$(CC) $(RFLAGS) $(SRC) $(MAKROS) $(LIBS) $(POPCNTFLAGS) $(AVX) -o $(EXE)-x64-$(PREFIX)-popcnt-avx$(SUFFIX)

tuning:
	$(CC) $(CFLAGS) $(SRC) $(MAKROS) $(LIBS) $(POPCNTFLAGS) $(OPENMPFLAGS) -DTUNING -o $(EXE)-x64-$(PREFIX)-native$(SUFFIX)
