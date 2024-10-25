LOCAL_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

TRICK_CXXFLAGS += -I$(LOCAL_DIR)/models
include $(CURDIR)/trickified_project/trickified/myproject.mk
