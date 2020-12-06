_THIS     := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
_PARENT   := $(abspath $(dir $(_THIS)))
_MAKE     := $(MAKE) --no-print-directory -f $(_PARENT)/Makefile

all: native

native:
	$(_MAKE) clean build DEBUG=0 MODE=NATIVE EXE=$(EXE)