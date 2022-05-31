_THIS     := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
_ROOT     := $(_THIS)/..
_SRC	  := $(_ROOT)/src_files
_BIN      := $(_ROOT)/bin
_MAKE     := $(MAKE) --no-print-directory -C $(_THIS)

# compiler and sources
CXX        = g++
_LIBS     := -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
_CXXSRCS  := $(_SRC)/*.cpp
_CSRCS	  := $(_SRC)/syzygy/tbprobe.c
_SRCS     := $(_CSRCS) $(_CXXSRCS)

# engine name and version
NAME       = Koivisto
MINOR      = 9
MAJOR      = 8
MAKROS     = -DMINOR_VERSION=$(MINOR) -DMAJOR_VERSION=$(MAJOR)
EXE_NAME   = $(NAME)_$(MAJOR).$(MINOR)
EXE_DIR    = $(_ROOT)/bin
EXE        = $(EXE_DIR)/$(EXE_NAME)
EVALFILE   = $(_ROOT)/networks/default.net
EXE_INFO   =


# compilation flags
FLAGS          = -std=c++17 -Wall -Wextra -Wshadow -DEVALFILE=\"$(EVALFILE)\"
PGO_PRE_FLAGS  = -fprofile-generate -lgcov
PGO_POST_FLAGS = -fprofile-use -fno-peel-loops -fno-tracer

# ---------------------------------------------------------------------------------------------------------------------
# COMPILATION OPTIONS AND NAME ADJUSTMENT
# ---------------------------------------------------------------------------------------------------------------------

# options
NAMING   ?= 1
STATIC   ?= 0
NATIVE   ?= 0
DETECT   ?= 1
PGO      ?= 0
DEBUG    ?= 0
LTO      ?= 0
PEXT     ?= 0
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
	FLAGS    += -DUSE_PEXT -mbmi2
endif

ifeq ($(LTO),1)
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
	PROPS = $(shell echo | $(CXX) -march=native -E -dM -)
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
    PREFIX    := windows
    SUFFIX    := .exe
else
UNAME    := $(shell uname -s)
ifeq ($(UNAME),Linux)
    PREFIX    := linux
    SUFFIX    :=
else
    $(warning incompatible host, this might fail)
    PREFIX    := _
    SUFFIX    :=
endif
endif
ifeq ($(NAMING),1)
    EXE := $(EXE_DIR)/$(EXE_NAME)-$(PREFIX)$(EXE_INFO)$(SUFFIX)
endif
EXE_FULL   := $(abspath $(EXE))


# ---------------------------------------------------------------------------------------------------------------------
# RULES
# ---------------------------------------------------------------------------------------------------------------------

openbench:
	$(_MAKE) build DEBUG=0 NATIVE=1 PGO=1 LTO=1 DETECT=1 EVALFILE=$(EVALFILE) NAMING=0 EXE=$(EXE)

native:
	$(_MAKE) build DEBUG=0 NATIVE=1 PGO=0 LTO=1 DETECT=1 NAMING=$(NAMING) STATIC=$(STATIC) EXE_NAME=$(EXE_NAME)

pgo:
	$(_MAKE) build DEBUG=0 NATIVE=1 PGO=1 LTO=1 DETECT=1 NAMING=$(NAMING) STATIC=$(STATIC) EXE_NAME=$(EXE_NAME)


overview:
	$(info NAME      : $(NAME))
	$(info EXE       : $(EXE))
	$(info FULL NAME : $(EXE_FULL))
	$(info NAMING    : $(NAMING))
	$(info FLAGS     : $(FLAGS))
	$(info NATIVE    : $(NATIVE))
	$(info STATIC    : $(STATIC))
	$(info PEXT      : $(PEXT))
	$(info PGO       : $(PGO))
	$(info DEBUG     : $(DEBUG))
	$(info AVX512    : $(AVX512))
	$(info AVX2      : $(AVX2))
	$(info AVX       : $(AVX))
	$(info POPCNT    : $(POPCNT))
	$(info SSE42     : $(SSE42))
	$(info SSE41     : $(SSE41))
	$(info SSE3      : $(SSE3))
	$(info SSE2      : $(SSE2))
	$(info SSE       : $(SSE))

build: updateNetwork overview
	mkdir -p $(EXE_DIR)
    ifeq ($(PGO),1)
		$(CXX) $(PGO_PRE_FLAGS)  $(FLAGS) $(_SRCS) $(MAKROS) $(_LIBS) -o $(EXE)
		$(EXE_FULL) bench
		$(CXX) $(PGO_POST_FLAGS) $(FLAGS) $(_SRCS) $(MAKROS) $(_LIBS) -o $(EXE)
		@rm -f *.gcda
    else
		$(CXX) $(FLAGS) $(_SRCS) $(MAKROS) $(_LIBS) -o $(EXE)
    endif

release:
	#$(_MAKE) build DEBUG=0 PGO=1 LTO=1 DETECT=0 NAMING=1 STATIC=1 AVX512=1
	$(_MAKE) build DEBUG=0 PGO=1 LTO=1 DETECT=0 NAMING=1 STATIC=1 AVX2=1
	$(_MAKE) build DEBUG=0 PGO=1 LTO=1 DETECT=0 NAMING=1 STATIC=1 POPCNT=1
	$(_MAKE) build DEBUG=0 PGO=1 LTO=1 DETECT=0 NAMING=1 STATIC=1 SSE42=1
	$(_MAKE) build DEBUG=0 PGO=1 LTO=1 DETECT=0 NAMING=1 STATIC=1 SSE41=1
	$(_MAKE) build DEBUG=0 PGO=1 LTO=1 DETECT=0 NAMING=1 STATIC=1 SSE3=1
	$(_MAKE) build DEBUG=0 PGO=1 LTO=1 DETECT=0 NAMING=1 STATIC=1 SSE2=1
	$(_MAKE) build DEBUG=0 PGO=1 LTO=1 DETECT=0 NAMING=1 STATIC=1 SSE=1

	#$(_MAKE) build DEBUG=0 PEXT=1 PGO=1 LTO=1 DETECT=0 NAMING=1 STATIC=1 AVX512=1
	$(_MAKE) build DEBUG=0 PEXT=1 PGO=1 LTO=1 DETECT=0 NAMING=1 STATIC=1 AVX2=1
	$(_MAKE) build DEBUG=0 PEXT=1 PGO=1 LTO=1 DETECT=0 NAMING=1 STATIC=1 POPCNT=1
	$(_MAKE) build DEBUG=0 PEXT=1 PGO=1 LTO=1 DETECT=0 NAMING=1 STATIC=1 SSE42=1
	$(_MAKE) build DEBUG=0 PEXT=1 PGO=1 LTO=1 DETECT=0 NAMING=1 STATIC=1 SSE41=1
	$(_MAKE) build DEBUG=0 PEXT=1 PGO=1 LTO=1 DETECT=0 NAMING=1 STATIC=1 SSE3=1
	$(_MAKE) build DEBUG=0 PEXT=1 PGO=1 LTO=1 DETECT=0 NAMING=1 STATIC=1 SSE2=1
	$(_MAKE) build DEBUG=0 PEXT=1 PGO=1 LTO=1 DETECT=0 NAMING=1 STATIC=1 SSE=1

# update the network
updateNetwork:
    ifeq ($(EVALFILE),$(_ROOT)/networks/default.net)
		git -C .. submodule update --init
    endif
