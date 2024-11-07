LOCAL_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
include $(LOCAL_DIR)/myproject_vars.mk

all:
	#@$(MAKE) -s -f $(TRICK_HOME)/share/trick/makefiles/trickify.mk
	#@trick-ify -d "/users/plherrin/trick/test/SIM_trickified/trickified_project/include" -b a -n trickified_myproject
	@trick-ify -d "/users/plherrin/trick/test/SIM_trickified/trickified_project/testing" -b a -n trickified_myproject

clean:
	@rm -rf build python trick $(TRICKIFY_OBJECT_NAME)
	@rm -rf $(MYPROJECT_TRICK)
	@rm -rf trickify_obj_list
	@rm -rf trickify_src_list
	@rm -rf /users/plherrin/trick/test/SIM_trickified/trickified_project/testing/*.o
