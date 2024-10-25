LOCAL_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

include $(LOCAL_DIR)/myproject_vars.mk

# Append a prerequisite to the $(SWIG_SRC) target. This will build the
# Trickified library along with the sim if it does not already exist. Using
# $(SWIG_SRC) ensures that all Trickified .i files are created before SWIG is
# run on any simulation .i files, which may %import them. Note that this does
# NOT cause the Trickified library to be rebuilt if it already exists, even if
# the Trickified source code has changed.
$(SWIG_SRC): $(MYPROJECT_TRICK)

$(MYPROJECT_TRICK):
	@$(MAKE) -s -C $(MYPROJECT_HOME)/trickified
