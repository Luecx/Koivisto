# --------------------------- COMPILERS ---------------------------------
CXX      = g++
EMCC	 = emcc
# ----------------------------- SOURCE  ---------------------------------
SRC_CXX  = $(wildcard *.cpp */*.cpp */*/*.cpp */*/*/*.cpp)
SRC_C	 = $(wildcard *.c */*.c */*/*.c */*/*/*.c)
# ----------------------------- PATHS ---------------------------------
_THIS    := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
_ROOT	 := $(_THIS)/..
_OUT     := $(_ROOT)/bin/
# ------------------------------ EXE ------------------------------------
NAME     = Koivisto
EVALFILE = $(_ROOT)/networks/default.net
EXE      = $(_OUT)$(NAME)_$(MAJOR).$(MINOR)
MINOR    = 0
MAJOR    = 8
ifeq ($(OS),Windows_NT)
    PREFIX := windows
    SUFFIX := .exe
else
    PREFIX := linux
    SUFFIX := 
endif

LIBS     = -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

########################################################################################################################
#--------------------------------------------- FLAGS    ---------------------------------------------------------------#
########################################################################################################################

WFLAGS = -std=c++17
CFLAGS = -O3 $(WFLAGS) -DNDEBUG -flto
RFLAGS = -O3 $(WFLAGS) -DNDEBUG -flto -static
PFLAGS = -O0 $(WFLAGS) -DNDEBUG -p -pg
DFLAGS = -O0 $(WFLAGS) -g


# ------------------------------- vector extensions ----------------------------------------------
POPCNTFLAGS = -DUSE_POPCNT -msse3 -mpopcnt
PEXTFLAGS   = $(POPCNTFLAGS) -DUSE_PEXT -mbmi2
SSEFLAGS    = $(POPCNTFLAGS) -msse -msse4.1 -mssse3 -msse2
AVX2FLAGS   = $(SSEFLAGS) -mavx2
AVX512FLAGS = $(AVX2FLAGS) -mavx512f -mavx512bw -mavx512dq
NATIVEFLAGS = -march=native

# ------------------------------- Emscripten ---------------------------------------------
EMS_MEMORY		 = -s TOTAL_MEMORY=4294967296 -s ALLOW_MEMORY_GROWTH=1 -s INITIAL_MEMORY=33554432
EMS_THREADS 	 = -s PROXY_TO_PTHREAD
EMS_KEEPALIVE	 = -s NO_EXIT_RUNTIME=1 -s "EXPORTED_RUNTIME_METHODS=['ccall']"
EMS_SIMD		 = -msimd128
EMS_ENVIRONMENT  = -s MODULARIZE=1 -s EXPORT_NAME=Koivisto -s ENVIRONMENT=web,worker
EMS_PREAMBLE     =  --pre-js web_preamble.js
EMS_MAKROS		 = $(EMS_MEMORY) $(EMS_THREADS) $(EMS_KEEPALIVE) $(EMS_SIMD) $(EMS_ENVIRONMENT) $(EMS_PREAMBLE)

# ------------------------------- Makros ----------------------------------------------
VERSION_MAKRO    = -DMINOR_VERSION=$(MINOR) -DMAJOR_VERSION=$(MAJOR)
ENABLE_TB_MAKRO  = -DSUPPORT_TABLEBASES
EVALFILE_MAKRO   = -DEVALFILE=\"$(EVALFILE)\"

FULL_MAKRO 		 = $(VERSION_MAKRO) $(ENABLE_TB_MAKRO) $(EVALFILE_MAKRO)

########################################################################################################################
#--------------------------------------------- TARGETS  ---------------------------------------------------------------#
########################################################################################################################

PYTHON_WRITE_WEIGHTS = python incbin/adjustweightsource.py $(EVALFILE)
PYTHON_RESET_WEIGHTS = python incbin/adjustweightsource.py incbin

openbench: updateNetwork
	$(CXX) $(CFLAGS) $(SRC_CXX) $(SRC_C) $(FULL_MAKRO) $(LIBS) $(NATIVEFLAGS) -o $(EXE)

emscripten: updateNetwork
	$(PYTHON_WRITE_WEIGHTS)
	$(EMCC) $(CFLAGS) $(SRC_CXX) $(EMS_MAKROS) $(VERSION_MAKRO) $(LIBS) $(AVX2FLAGS) -o $(EXE).js
	$(PYTHON_RESET_WEIGHTS)

native: updateNetwork
	mkdir -p $(ROOT)$(FOLDER)
	$(CXX) $(CFLAGS) $(SRC_CXX) $(SRC_C) $(FULL_MAKRO) $(LIBS) $(NATIVEFLAGS) -o $(EXE)-x64-$(PREFIX)-native$(SUFFIX)

release: updateNetwork
	mkdir -p $(ROOT)$(FOLDER)
	$(CC) $(RFLAGS) $(SRC_CXX) $(SRC_C) $(FULL_MAKRO) $(LIBS) $(AVX512FLAGS) -o $(EXE)-x64-$(PREFIX)-avx512$(SUFFIX)
	$(CC) $(RFLAGS) $(SRC_CXX) $(SRC_C) $(FULL_MAKRO) $(LIBS) $(AVX2FLAGS) -o $(EXE)-x64-$(PREFIX)-avx2$(SUFFIX)
	$(CC) $(RFLAGS) $(SRC_CXX) $(SRC_C) $(FULL_MAKRO) $(LIBS) $(SSEFLAGS) -o $(EXE)-x64-$(PREFIX)-sse2$(SUFFIX)

updateNetwork: resetweights
ifeq ($(EVALFILE),$(ROOT)networks/default.net)
	git -C .. submodule update --init
endif

resetweights:
	$(PYTHON_RESET_WEIGHTS)

