LOCAL_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
TRICK_HOME := $(abspath $(LOCAL_DIR)/../../../..)
include $(LOCAL_DIR)/myproject_vars.mk

all:
	@echo MAKE LOCAL_DIR $(LOCAL_DIR)
	@$(TRICK_HOME)/bin/trick-ify -d "$(LOCAL_DIR)/../include_bar $(LOCAL_DIR)/../include_foo" -b o -n trickified_myproject -v

clean:
	@rm -rf build python trick $(TRICKIFY_OBJECT_NAME)
	@rm -rf $(MYPROJECT_TRICK)
	@rm -rf trickify_obj_list
	@rm -rf trickify_src_list
	@rm -rf S_source.hh
	@rm -rf ../include_foo/*.o
	@rm -rf ../include_bar/*.o
