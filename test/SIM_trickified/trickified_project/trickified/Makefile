PROJECT_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)
TRICK_HOME := $(abspath $(PROJECT_HOME)/../../..)

export TRICKIFY_BUILD_TYPE := PLO
export TRICKIFY_OBJECT_NAME := trickified_myproject.o
export TRICKIFY_CXX_FLAGS := -I$(PROJECT_HOME)/include -I$(TRICK_HOME)/include

all:
	@$(MAKE) -s -f $(TRICK_HOME)/share/trick/makefiles/trickify.mk

clean:
	@rm -rf build python trick $(TRICKIFY_OBJECT_NAME)
