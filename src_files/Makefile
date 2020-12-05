_THIS    := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
_SRC     := $(_THIS)/
_BUILD   := $(_THIS)/../_build
_BIN     := $(_THIS)/../bin
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
MINOR     = 9
MAJOR     = 4
_TARGET  := $(_BIN)/$(NAME)_$(MAJOR).$(MINOR)


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

DEBUG    ?= 1
POPCNT   ?= 1
PEXT     ?= 0
AVX      ?= 0
MODE     ?= native

ifeq ($(MODE),release)
DEBUG     = 0
endif


# MARK: Flags

_MACROS   = -DMINOR_VERSION=$(MINOR) -DMAJOR_VERSION=$(MAJOR)
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

_EXE     := $(_TARGET)-x64-$(PREFIX)

ifeq ($(MODE),native)
_EXE     := $(_EXE)-native
endif

ifeq ($(POPCNT),1)
_EXE     := $(_EXE)-popcnt
endif

ifeq ($(AVX),1)
_EXE     := $(_EXE)-avx
endif

_EXE     := $(_EXE)$(SUFFIX)


# MARK: Recipes

all: build


.SILENT: $(_BIN) $(_OBJS) $(_EXE)

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

EXE ?= $(_EXE)
$(_EXE): $(_OBJS) $(_BIN)
	$(CXX) $(_CFLAGS) $(_CXXFLAGS) -o $(EXE) $(_OBJS) $(_LDFLAGS)

build: $(_EXE)


# MARK: Convenience

.SILENT: native release
.PHONY: native release

native:
	-$(_MAKE) clean build DEBUG=0 MODE=native

release:
	-$(_MAKE) clean build DEBUG=0 POPCNT=0 MODE=release
	-$(_MAKE) clean build DEBUG=0 POPCNT=1 MODE=release
	-$(_MAKE) clean build DEBUG=0 POPCNT=1 AVX=1 MODE=release


# MARK: Cleanup

.SILENT: clean distclean
.PHONY: clean distclean

clean:
	-rm -rf $(_BUILD)

distclean:
	-rm -rf $(_BIN)
