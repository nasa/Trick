LOCAL_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
PROJECT_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)
TRICK_HOME := $(abspath $(PROJECT_HOME)/../../..)

include $(LOCAL_DIR)/myproject_vars.mk

export TRICKIFY_SOURCE := $(PROJECT_HOME)/include
export TRICKIFY_CXX_FLAGS := -I $(PROJECT_HOME)/include -I $(TRICK_HOME)/include

all:
	#@$(MAKE) -s -f $(TRICK_HOME)/share/trick/makefiles/trickify.mk
	@trick-ify -d "/users/plherrin/trick/test/SIM_trickified/trickified_project/include" -b a -n trickified_myproject

clean:
	@rm -rf build python trick $(TRICKIFY_OBJECT_NAME)
	@rm -rf $(MYPROJECT_TRICK)
