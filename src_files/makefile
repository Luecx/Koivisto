CC       = g++
SRC      = *.cpp syzygy/tbprobe.c
LIBS     = -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
FOLDER   = bin/
ROOT     = ../
NAME     = Koivisto
EVALFILE = $(ROOT)networks/default.net
EXE      = $(ROOT)$(FOLDER)$(NAME)_$(MAJOR).$(MINOR)
MINOR    = 7
MAJOR    = 6
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
AVX2FLAGS   = $(POPCNTFLAGS) -msse -mpopcnt -mavx2 -msse4.1 -mssse3 -msse2
OPENMPFLAGS = -fopenmp

MAKROS      = -DMINOR_VERSION=$(MINOR) -DMAJOR_VERSION=$(MAJOR)


openbench: updateNetwork
	$(CC) $(CFLAGS) $(SRC) $(MAKROS) $(LIBS) $(AVX2FLAGS) -o $(EXE)

native: updateNetwork
	mkdir -p $(ROOT)$(FOLDER)
	$(CC) $(CFLAGS) $(SRC) $(MAKROS) $(LIBS) $(AVX2FLAGS) -o $(EXE)-x64-$(PREFIX)-native$(SUFFIX)
    
release: updateNetwork
	mkdir -p $(ROOT)$(FOLDER)
	$(CC) $(RFLAGS) $(SRC) $(MAKROS) $(LIBS) $(AVX2FLAGS) -o $(EXE)-x64-$(PREFIX)-avx2$(SUFFIX)

tuning: updateNetwork
	$(CC) $(CFLAGS) $(SRC) $(MAKROS) $(LIBS) $(AVX2FLAGS) $(OPENMPFLAGS) -DTUNING -o $(EXE)-x64-$(PREFIX)-native$(SUFFIX)
	

updateNetwork:
ifeq ($(EVALFILE),$(ROOT)networks/default.net)
	git -C .. submodule update --init
endif


