_THIS     := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
_PARENT   := $(abspath $(dir $(_THIS)))
_MAKE     := $(MAKE) --no-print-directory -C $(_PARENT)

all: native

native:
	# NOTE: This assumes that EXE is given as an absolute path. Use other args as needed.
	$(_MAKE) clean build DEBUG=0 NATIVE=1 EXE=$(EXE)