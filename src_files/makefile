CC       = g++
SRC      = *.cpp syzygy/tbprobe.c
LIBS     = -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
FOLDER   = bin/
ROOT     = ../
NAME     = Koivisto
EVALFILE = $(ROOT)networks/default.net
EXE      = $(ROOT)$(FOLDER)$(NAME)_$(MAJOR).$(MINOR)
MINOR    = 9
MAJOR    = 8
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
SSEFLAGS    = $(POPCNTFLAGS) -msse -msse4.1 -mssse3 -msse2
AVX2FLAGS   = $(SSEFLAGS) -mavx2
AVX512FLAGS = $(AVX2FLAGS) -mavx512f -mavx512bw -mavx512dq
PGOMAKEFLAGS = -fprofile-generate -lgcov
PGOUSEFLAGS  = -fprofile-use -fno-peel-loops -fno-tracer
OPENMPFLAGS = -fopenmp
NATIVEFLAGS = -march=native
PEXTFLAGS   = -DUSE_PEXT

MAKROS      = -DMINOR_VERSION=$(MINOR) -DMAJOR_VERSION=$(MAJOR)

# autodetect features for pext and append it to native flags
PROPS = $(shell echo | $(CC) -march=native -E -dM -)
ifneq ($(findstring __BMI2__, $(PROPS)),)
    ifeq ($(findstring __znver1, $(PROPS)),)
        ifeq ($(findstring __znver2, $(PROPS)),)
            NATIVEFLAGS += $(PEXTFLAGS)
        endif
    endif
endif

# targets
openbench: updateNetwork
	$(CC) $(CFLAGS) $(SRC) $(MAKROS) $(LIBS) $(NATIVEFLAGS) $(PGOMAKEFLAGS) -o $(EXE)
	$(EXE) bench
	$(CC) $(CFLAGS) $(SRC) $(MAKROS) $(LIBS) $(NATIVEFLAGS) $(PGOUSEFLAGS) -o $(EXE)
	@rm -f *.gcda

native: updateNetwork
	mkdir -p $(ROOT)$(FOLDER)
	$(CC) $(CFLAGS) $(SRC) $(MAKROS) $(LIBS) $(NATIVEFLAGS) -o $(EXE)-x64-$(PREFIX)-native$(SUFFIX)
    
release: updateNetwork
	mkdir -p $(ROOT)$(FOLDER)
	$(CC) $(RFLAGS) $(SRC) $(MAKROS) $(LIBS) $(AVX512FLAGS) -o $(EXE)-x64-$(PREFIX)-avx512$(SUFFIX)
	$(CC) $(RFLAGS) $(SRC) $(MAKROS) $(LIBS) $(AVX2FLAGS) -o $(EXE)-x64-$(PREFIX)-avx2$(SUFFIX)
	$(CC) $(RFLAGS) $(SRC) $(MAKROS) $(LIBS) $(SSEFLAGS) -o $(EXE)-x64-$(PREFIX)-sse2$(SUFFIX)

updateNetwork:
ifeq ($(EVALFILE),$(ROOT)networks/default.net)
	git -C .. submodule update --init
endif


