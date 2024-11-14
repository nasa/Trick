# This file can be used to create an object file or library containing the io_*
# and *_py code that Trick would normally generate during the simulation build
# process. Sims can then link against that, reducing compilation time.
#
# To use it, create a directory that includes a file named S_source.hh that
# includes all header files for which you want io_* and _py* code generated.
# This is not the S_source.hh that Trick generates during compilation, but
# Trick's tools assume this name for now, so you have to use it. Then, in that
# directory, run the following command:
#
# make -f $(TRICK_HOME)/share/trick/makefiles/trickify.mk
#
# You can, of course, call this from your own makefile. But remember to switch
# to the directory containing your S_source.hh first using make's -C option if
# necessary. For instance:
#
# all:
# 	$(MAKE) -C <directory> -f $(TRICK_HOME)/share/trick/makefiles/trickify.mk
#
# There are a number of variables you can export from your own makefile or
# define on the command line when calling make:
#
# TRICKIFY_CXX_FLAGS (required)
# Define this variable to specify compiler flags. Most importantly, include the
# paths to the header files included by your S_source.hh. For instance:
# -I$(HOME)/myproject/foo/include -I$(HOME)/myproject/bar/include
#
# TRICKIFY_BUILD_TYPE (optional)
# Valid options are:
# 1. STATIC (.a)
#    Create a static library. This will require the use of --whole-archive (on
#    Linux) or -all_load/-force_load (on Mac) when linking the sim executeable.
#    Trick uses dlsym to dynamically load symbols at run time, but the linker,
#    by default, will not include symbols from static libraries that are not
#    known to be needed at compile time.
# 2. SHARED (.so)
#    Create a shared object (dynamically linked library). This may require the
#    use of -rpath to ensure the linker can find the shared object at runtime
#    unless you explicitly link against it (as opposed to using -L and -l)
#    during compilation.
# 3. PLO (.o) [default]
#    Create a partially-linked object. No special linker options are required.
#
# TRICKIFY_OBJECT_NAME (optional)
# The name of the generated object file or library. The default value is
# trickified.o. You should choose something more meaningful, especially if
# you're using another build type.
#
# TRICKIFY_PYTHON_DIR (optional)
# The file into which generated Python modules are zipped. The default
# value is python (in the current directory).
#
# TRICKIFY_SOURCE
#
# -----------------------------------------------------------------------------
#
# EXAMPLE:
#
# For a project with the following structure:
#
# $(HOME)/potato/
#     include/
#     trickified/
#         S_source.hh
#         Makefile
#
# The following Makefile would be appropriate:
#
# ifndef TRICK_HOME
#     $(error TRICK_HOME must be set)
# endif
#
# TRICKIFY := $(TRICK_HOME)/share/trick/makefiles/trickify.mk
#
# ifeq ($(wildcard $(TRICKIFY)),)
#     $(error This makefile requires at least Trick 17.1)
# endif
#
# export TRICKIFY_CXX_FLAGS := -I$(HOME)/potato/include
# export TRICKIFY_OBJECT_NAME := trickified_potato.o
#
# all:
# 	$(MAKE) -f $(TRICKIFY)
#
# clean:
# 	rm -rf $(TRICKIFY_OBJECT_NAME) build python .trick
#
# -----------------------------------------------------------------------------
#
# For more information, see:
# https://nasa.github.io/trick/documentation/building_a_simulation/Trickified-Project-Libraries

ifndef TRICKIFY_CXX_FLAGS
    $(error TRICKIFY_CXX_FLAGS must be set)
endif

TRICKIFY_BUILD_TYPE ?= PLO
TRICKIFY_OBJECT_NAME ?= trickified.o

# We started zipping the Python modules, and this variable is now misnamed :(
# python.zip would be a better default value, but leave it as python for backward compatibility.
TRICKIFY_PYTHON_DIR ?= python
TRICKIFY_PYTHON_DIR := $(abspath $(TRICKIFY_PYTHON_DIR))

include $(dir $(lastword $(MAKEFILE_LIST)))Makefile.common

BUILD_DIR := $(dir $(MAKE_OUT))
PY_LINK_LIST := $(BUILD_DIR)trickify_py_link_list
IO_LINK_LIST := $(BUILD_DIR)trickify_io_link_list
OBJ_LINK_LIST := trickify_obj_list
ifdef FULL_TRICKIFY_BUILD
	LINK_LISTS := @$(IO_LINK_LIST) @$(PY_LINK_LIST) @$(OBJ_LINK_LIST)
else
	LINK_LISTS := @$(IO_LINK_LIST) @$(PY_LINK_LIST)
endif
ifneq ($(wildcard $(BUILD_DIR)),)
	SWIG_OBJECTS := $(shell cat $(PY_LINK_LIST))
	IO_OBJECTS   := $(shell cat $(IO_LINK_LIST))
endif

TRICK_CFLAGS   += $(TRICKIFY_CXX_FLAGS)
TRICK_CXXFLAGS += $(TRICKIFY_CXX_FLAGS)

# Ensure we can process all headers
TRICK_EXT_LIB_DIRS :=

.PHONY: all
all: $(TRICKIFY_OBJECT_NAME) $(TRICKIFY_PYTHON_DIR)

$(TRICKIFY_OBJECT_NAME): $(SWIG_OBJECTS) $(IO_OBJECTS) | $(dir $(TRICKIFY_OBJECT_NAME))
	$(info $(call COLOR,Linking)    $@)
ifeq ($(TRICKIFY_BUILD_TYPE),PLO)
	$(call ECHO_AND_LOG,$(LD) $(LD_PARTIAL) -o $@ $(LINK_LISTS))
else ifeq ($(TRICKIFY_BUILD_TYPE),SHARED)
	$(call ECHO_AND_LOG,$(TRICK_CXX) -shared -o $@ $(LINK_LISTS))
else ifeq ($(TRICKIFY_BUILD_TYPE),STATIC)
	$(call ECHO_AND_LOG,ar rcs $@ $(LINK_LISTS))
endif

$(dir $(TRICKIFY_OBJECT_NAME)) $(BUILD_DIR) $(dir $(TRICKIFY_PYTHON_DIR)) .trick:
	@mkdir -p $@

$(IO_OBJECTS): %.o: %.cpp
	$(info $(call COLOR,Compiling)  $<)
	$(call ECHO_AND_LOG,$(TRICK_CXX) $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) -std=c++11 -Wno-invalid-offsetof -MMD -MP -c -o $@ $<)

$(IO_OBJECTS:.o=.d): %.d: ;

-include $(IO_OBJECTS:.o=.d)

$(SWIG_OBJECTS): %.o: %.cpp
	$(info $(call COLOR,Compiling)  $<)
	$(call ECHO_AND_LOG,$(TRICK_CXX) $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) $(PYTHON_INCLUDES) -Wno-unused-parameter -Wno-shadow -c -o $@ $<)

$(SWIG_OBJECTS:.o=.cpp): %.cpp: %.i | %.d .trick $(SWIG_OBJECTS:.o=.i)
	$(info $(call COLOR,SWIGing)    $<)
	$(call ECHO_AND_LOG,$(SWIG) $(TRICK_INCLUDE) $(TRICK_DEFINES) $(TRICK_VERSIONS) $(TRICK_SWIG_FLAGS) -c++ -python -includeall -ignoremissing -w201 -w303 -w315 -w325 -w362 -w389 -w401 -w451 -MMD -MP -outdir .trick -o $@ $<)

$(SWIG_OBJECTS:.o=.d): ;

-include $(SWIG_OBJECTS:.o=.d)

define create_convert_swig_rule
$(shell basename $(BUILD_DIR))/%_py.i: /%.$1
	$$(call ECHO_AND_LOG,${TRICK_HOME}/$(LIBEXEC)/trick/convert_swig $${TRICK_CONVERT_SWIG_FLAGS} $$<)
endef

$(foreach EXTENSION,H h hh hxx h++ hpp,$(eval $(call create_convert_swig_rule,$(EXTENSION))))

$(TRICKIFY_PYTHON_DIR): $(SWIG_OBJECTS:.o=.cpp) | $(dir $(TRICKIFY_PYTHON_DIR))
	$(info $(call COLOR,Compiling)  Python modules)
	$(call ECHO_AND_LOG,$(PYTHON) -m compileall -q .trick)
	$(info $(call COLOR,Zipping)    Python modules into $@)
	$(call ECHO_AND_LOG,cd .trick && zip -Arq $@ .)

# SWIG_OBJECTS and IO_OBJECTS are meant to contain all of the *_py and io_*
# object file names, respectively, by looking at products of ICG and
# make_makefile_swig. However, we can't run a rule for ICG before those
# variables are expanded because they are prerequisites of
# TRICKIFY_OBJECT_NAME, and prerequisites are immediately expanded.
# Therefore, when make processes this file (for the first time after a clean),
# the variables are empty because the files from which they are populated have
# yet to be created by ICG. What we really want is to run ICG before OBJECTS is
# expanded, but only if we need to.
#
# We can do this by taking advantage of (abusing) make's behavior in the
# presence of included files. When you include another file, make attempts to
# update that file, and then re-executes the original makefile with a clean
# slate if any of the included files changed. So we need a rule that:
#
# 1. Executes only if S_source.hh or anything in its include tree changes.
# 2. Runs ICG and make_makefile_swig to generate io_* files and S_library_swig.
# 3. Updates its dependency file.
#
# We can then include the dependency file to trigger make to re-execute this
# main makefile if changes were detected, thus updating SWIG_OBJECTS and
# IO_OBJECTS to the latest results of ICG and make_makefile_swig.
#
# gcc's option to automatically generate dependency information is exactly
# what we need. It will produce a file containing a make rule whose
# dependencies are everything in S_source.hh's include tree plus S_source.hh
# itself. By telling gcc to make the target of the rule the same target we use
# to execute the gcc call, we end up with a target that maintains its own
# dependency list. The method is laid out in more detail here:
# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

$(BUILD_DIR)S_source.d: | $(BUILD_DIR)
	$(call ECHO_AND_LOG,$(TRICK_HOME)/bin/trick-ICG $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) $(TRICK_ICGFLAGS) S_source.hh)
	$(call ECHO_AND_LOG,$(TRICK_HOME)/$(LIBEXEC)/trick/make_makefile_swig)
	$(call ECHO_AND_LOG,$(TRICK_CC) -MM -MP -MT $@ -MF $@ $(TRICKIFY_CXX_FLAGS) S_source.hh)

-include $(BUILD_DIR)S_source.d
