_THIS     := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
_ROOT     := $(_THIS)/..
_SRC	  := $(_ROOT)/src_files
_BIN      := $(_ROOT)/bin
_MAKE     := $(MAKE) --no-print-directory -C $(_THIS)

# compiler and sources
CXX        = g++
_LIBS     := -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
_CXXSRCS  := $(wildcard $(_SRC)/*.cpp $(_SRC)/*/*.cpp $(_SRC)/*/*/*.cpp $(_SRC)/*/*/*/*.cpp)
_CSRCS	  := $(wildcard $(_SRC)/*.c $(_SRC)/*/*.c $(_SRC)/*/*/*.c $(_SRC)/*/*/*/*.c)
_SRCS     := $(_CSRCS) $(_CXXSRCS)

# engine name and version
NAME       = Koivisto
MINOR      = 9
MAJOR      = 8
MAKROS     = -DMINOR_VERSION=$(MINOR) -DMAJOR_VERSION=$(MAJOR)
EXE       := $(_BIN)/$(NAME)_$(MAJOR).$(MINOR)
EXE_NAME  := $(EXE)
EXE_DIR    = $(dir $(abspath $(firstword $(EXE))))
EVALFILE  := $(_ROOT)/networks/default.net
EXE_INFO   =
EXE_SUFFIX =


# compilation flags
FLAGS         ?= -std=c++17 -Wall -Wextra -Wshadow -DEVALFILE=\"$(EVALFILE)\"
PGO_PRE_FLAGS  = -fprofile-generate -lgcov
PGO_POST_FLAGS = -fprofile-use -fno-peel-loops -fno-tracer

# ---------------------------------------------------------------------------------------------------------------------
# COMPILATION OPTIONS AND NAME ADJUSTMENT
# ---------------------------------------------------------------------------------------------------------------------

# options
NAMING   ?= 1         # adjusts the name based on the used flags
STATIC   ?= 0         # compiles a static executable
NATIVE   ?= 0         # uses native flags
DETECT   ?= 1         # enables detection and automatically toggle flags like: PEXT
PGO      ?= 0         # enables profile guided optimizations
DEBUG    ?= 0         # enables debug flags
FLTO     ?= 0         # enables link time optimization
PEXT     ?= 0         # enables pext
# vector instructions
AVX512   ?= 0
AVX2     ?= $(AVX512)
AVX      ?= $(AVX2)
POPCNT   ?= $(AVX)
SSE42    ?= $(POPCNT)
SSE41    ?= $(SSE42)
SSE3     ?= $(SSE41)
SSE2     ?= $(SSE3)
SSE      ?= $(SSE2)

# adjust the exe name as well as the FLAGS
ifeq ($(PGO),1)
    EXE_INFO := $(EXE_INFO)-pgo
endif

ifeq ($(DEBUG),1)
	EXE_INFO := $(EXE_INFO)-debug
else
    FLAGS    += -DNDEBUG -O3
endif

ifeq ($(NATIVE),1)
	EXE_INFO := $(EXE_INFO)-native
	FLAGS    += -march=native
endif

ifeq ($(PEXT),1)
	EXE_INFO := $(EXE_INFO)-pext
	FLAGS    += -DUSE_PEXT -bmi2
endif

ifeq ($(FLTO),1)
	FLAGS    += -flto
endif

ifeq ($(STATIC),1)
	FLAGS    += -static -static-libgcc -static-libstdc++
endif

# toggle avx options based on the highest one
ifeq ($(AVX512),1)
	FLAGS += -mavx512f -mavx512bw -mavx512dq
endif
ifeq ($(AVX2),1)
	FLAGS += -mavx2
endif
ifeq ($(AVX),1)
	FLAGS += -mavx
endif
ifeq ($(POPCNT),1)
	FLAGS += -DUSE_POPCNT -mpopcnt
endif
ifeq ($(SSE42),1)
	FLAGS += -msse4.2
endif
ifeq ($(SSE41),1)
	FLAGS += -msse4.1
endif
ifeq ($(SSE3),1)
	FLAGS += -msse3
endif
ifeq ($(SSE2),1)
	FLAGS += -msse2
endif
ifeq ($(SSE),1)
	FLAGS += -msse
endif

# set name based on highest vector extension
ifeq ($(AVX512),1)
	EXE_INFO := $(EXE_INFO)-avx512
else ifeq ($(AVX2),1)
	EXE_INFO := $(EXE_INFO)-avx2
else ifeq ($(AVX),1)
	EXE_INFO := $(EXE_INFO)-avx
else ifeq ($(POPCNT),1)
	EXE_INFO := $(EXE_INFO)-popcnt
else ifeq ($(SSE42),1)
	EXE_INFO := $(EXE_INFO)-sse42
else ifeq ($(SSE41),1)
	EXE_INFO := $(EXE_INFO)-sse41
else ifeq ($(SSE3),1)
	EXE_INFO := $(EXE_INFO)-msse3
else ifeq ($(SSE2),1)
	EXE_INFO := $(EXE_INFO)-sse2
else ifeq ($(SSE),1)
	EXE_INFO := $(EXE_INFO)-sse
endif

# auto detect certain flags
ifeq ($(DETECT),1)
	PROPS = $(shell echo | $(CC) -march=native -E -dM -)
	ifneq ($(findstring __BMI2__, $(PROPS)),)
		ifeq ($(findstring __znver1, $(PROPS)),)
			ifeq ($(findstring __znver2, $(PROPS)),)
				FLAGS += $(PEXTFLAGS)
			endif
		endif
	endif
endif

# ---------------------------------------------------------------------------------------------------------------------
# HOST DETECTION AND NAME ADJUSTMENT IF NAMING IS ACTIVE
# ---------------------------------------------------------------------------------------------------------------------
ifeq ($(OS),Windows_NT)
    PREFIX    = windows
    SUFFIX    = .exe
else
UNAME    := $(shell uname -s)
ifeq ($(UNAME),Linux)
    PREFIX    = linux
    SUFFIX    =
else
    $(warning incompatible host, this might fail)
    PREFIX    = _
    SUFFIX    =
endif
endif
ifeq ($(NAMING),1)
	EXE_NAME := $(EXE)-$(PREFIX)$(EXE_INFO)$(SUFFIX)
endif

# ---------------------------------------------------------------------------------------------------------------------
# RULES
# ---------------------------------------------------------------------------------------------------------------------

openbench:
	-$(_MAKE) build DEBUG=0 NATIVE=1 PGO=1 FLTO=1 DETECT=1 NAMING=0 EXE=$(EXE)

native:
	-$(_MAKE) build DEBUG=0 NATIVE=1 PGO=0 FLTO=1 DETECT=1 NAMING=1 STATIC=$(STATIC) EXE=$(EXE)

pgo:
	-$(_MAKE) build DEBUG=0 NATIVE=1 PGO=1 FLTO=1 DETECT=1 NAMING=1 STATIC=$(STATIC) EXE=$(EXE)


overview:
	$(info TARGET_FOLER is $(EXE_DIR))
	$(info EXECUTABLE   is $(EXE_NAME))
	$(info FLAGS        is $(FLAGS))
	$(info NATIVE       is $(NATIVE))
	$(info STATIC       is $(STATIC))
	$(info PEXT         is $(PEXT))
	$(info PGO          is $(PGO))
	$(info DEBUG        is $(DEBUG))
	$(info AVX512       is $(AVX512))
	$(info AVX2         is $(AVX2))
	$(info AVX          is $(AVX))
	$(info POPCNT       is $(POPCNT))
	$(info SSE42        is $(SSE42))
	$(info SSE41        is $(SSE41))
	$(info SSE3         is $(SSE3))
	$(info SSE2         is $(SSE2))
	$(info SSE          is $(SSE))

build: updateNetwork overview
	mkdir -p $(EXE_DIR)
    ifeq ($(PGO),1)
		$(CXX) $(PGO_PRE_FLAGS)  $(FLAGS) $(_SRCS) $(MAKROS) $(_LIBS) -o $(EXE_NAME)
		$(EXE_NAME) bench
		$(CXX) $(PGO_POST_FLAGS) $(FLAGS) $(_SRCS) $(MAKROS) $(_LIBS) -o $(EXE_NAME)
		@rm -f *.gcda
    else
		$(CXX) $(FLAGS) $(_SRCS) $(MAKROS) $(_LIBS) -o $(EXE_NAME)
    endif

# update the network
updateNetwork:
    ifeq ($(EVALFILE),$(ROOT)networks/default.net)
		git -C .. submodule update --init
    endif
