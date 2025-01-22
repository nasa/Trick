TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))..)
export TRICK_HOST_CPU := $(shell $(TRICK_HOME)/bin/trick-gte TRICK_HOST_CPU)
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common


SIM_DIRECTORIES = $(wildcard SIM_*)
UNIT_TEST_RESULTS = $(addprefix $(TRICK_HOME)/trick_test/, $(addsuffix .xml, $(SIM_DIRECTORIES)))

# The auto-generated makefile for each test does not know to call other makefiles the developer may have included.
# Developer generated clean calls must be directly called here,
# Otherwise build artifacts may not be cleaned and give misleading test results.
clean_trickify:
	for i in $(SIM_DIRECTORIES) ; do \
		if [ -f "$$i/trickified_project/trickified/"[Mm]"akefile" ] ; then \
			$(MAKE) -C $$i/trickified_project/trickified/ clean ; \
		elif [ -f "$$i/models/trickified/"[Mm]"akefile" ] ; then \
			$(MAKE) -C $$i/models/trickified/ clean ; \
		fi \
	done

clean: clean_trickify
	rm -f $(UNIT_TEST_RESULTS)
	- for i in $(SIM_DIRECTORIES) ; do \
		if [ -f "$$i/"[Mm]"akefile" ] ; then \
			$(MAKE) -C $$i spotless ; \
		fi \
	done
