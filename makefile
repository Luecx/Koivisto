_THIS    := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
_SRC     := $(_THIS)/src_files
_BUILD   := $(_THIS)/_build
_BIN     := $(_THIS)/bin
_MAKE    := $(MAKE) --no-print-directory -C $(_THIS)

CC        = gcc
CXX       = g++
_CSRCS   := $(_SRC)/syzygy/tbprobe.c
_CXXSRCS := $(wildcard $(_SRC)/*.cpp)
_SRCS    := $(_CSRCS) $(_CXXSRCS)
_COBJS   := $(patsubst $(_SRC)/%.c,$(_BUILD)/%.o,$(_CSRCS))
_CXXOBJS := $(patsubst $(_SRC)/%.cpp,$(_BUILD)/%.o,$(_CXXSRCS))
_OBJS    := $(_COBJS) $(_CXXOBJS)

NAME      = Koivisto
MINOR     = 13
MAJOR     = 5
_TARGET  := $(_BIN)/$(NAME)_$(MAJOR).$(MINOR)
EVALFILE  = $(_THIS)/networks/default.net


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


# MARK: Args

DEBUG    ?= 0
POPCNT   ?= 1
PEXT     ?= 0
AVX      ?= 1
MODE     ?= native

ifeq ($(MODE),release)
DEBUG     = 0
endif


# MARK: Flags

_MACROS   = -DMINOR_VERSION=$(MINOR) -DMAJOR_VERSION=$(MAJOR) -DEVALFILE=\"$(EVALFILE)\"
_WARN     = -Wall -Wextra -Wshadow
_CFLAGS   = $(_WARN) $(_MACROS)
_CXXFLAGS = -std=c++17
_LDFLAGS  = -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

ifeq ($(DEBUG),1)
_CFLAGS  += -O0 -g
else
_CFLAGS  += -O3 -DNDEBUG -flto
ifeq ($(MODE),release)
_CFLAGS  += -static
else
ifeq ($(MODE),native)
_CFLAGS  += -march=native
endif
endif
endif

ifeq ($(POPCNT),1)
_CFLAGS  += -DUSE_POPCNT -msse3 -mpopcnt
endif

ifeq ($(PEXT),1)
_CFLAGS  += -DUSE_PEXT -mbmi2
endif

ifeq ($(AVX),1)
_CFLAGS  += -msse -msse3 -mpopcnt -mavx2 -msse4.1 -mssse3 -msse2
endif


# MARK: Executable

EXE     := $(_TARGET)-x64-$(PREFIX)

ifeq ($(MODE),native)
EXE     := $(EXE)-native
endif

#ifeq ($(POPCNT),1)
#EXE     := $(EXE)-popcnt
#endif

#ifeq ($(AVX),1)
#EXE     := $(EXE)-avx
#endif

EXE     := $(EXE)$(SUFFIX)


# MARK: Recipes

all: build


.SILENT: $(_BIN) $(_OBJS) $(EXE)

$(_BIN):
	-mkdir -p $(_BIN)

$(_COBJS): $(_BUILD)/%.o: $(_SRC)/%.c
	@echo "Compiling $(patsubst $(_SRC)/%,%,$<)"
	-mkdir -p $(dir $@)
	$(CC) -c $(_CFLAGS) $< -o $@

$(_CXXOBJS): $(_BUILD)/%.o: $(_SRC)/%.cpp
	@echo "Compiling $(patsubst $(_SRC)/%,%,$<)"
	-mkdir -p $(dir $@)
	$(CXX) -c $(_CFLAGS) $(_CXXFLAGS) $< -o $@

$(EXE): updateNetwork $(_OBJS) $(_BIN)
	$(CXX) $(_CFLAGS) $(_CXXFLAGS) -o $(EXE) $(_OBJS) $(_LDFLAGS)

build: $(EXE)


# MARK: Convenience

.SILENT: native release
.PHONY: native release

native:
	-$(_MAKE) clean build DEBUG=0 MODE=native EVALFILE=$(EVALFILE)

release:
	-$(_MAKE) clean build DEBUG=0 POPCNT=1 MODE=release EVALFILE=$(EVALFILE)
	-$(_MAKE) clean build DEBUG=0 POPCNT=1 AVX=1 MODE=release EVALFILE=$(EVALFILE)


# MARK: Cleanup

.SILENT: clean distclean
.PHONY: clean distclean

updateNetwork:
ifeq ($(EVALFILE),$(_THIS)/networks/default.net)
	git -C . submodule update --init
endif

clean:
	-rm -rf $(_BUILD)

distclean:
	-rm -rf $(_BIN)
