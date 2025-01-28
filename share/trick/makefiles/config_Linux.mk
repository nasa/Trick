
INSTALL = /bin/install

# options for partial linking
LD_PARTIAL = -Ur
LD_FILELIST = @

# if we are on a Rehat system, the lib directory is lib64 on 64 bit machines
ifeq ($(TRICK_FORCE_32BIT), 1)
TRICK_LIB_DIR  := ${TRICK_HOME}/lib
else
ifneq ("$(wildcard /etc/redhat-release)","")
UNAME_M := $(shell uname -m)
ifeq ($(UNAME_M),x86_64)
TRICK_LIB_DIR  := ${TRICK_HOME}/lib64
else
TRICK_LIB_DIR  := ${TRICK_HOME}/lib
endif
else
TRICK_LIB_DIR  := ${TRICK_HOME}/lib
endif
endif

SHARED_LIB_OPT := -shared
RPATH = -Wl,-rpath=${TRICK_LIB_DIR}

TRICK_SYSTEM_LDFLAGS += -Wl,--export-dynamic
PLATFORM_LIBS = -lrt

LD_WHOLE_ARCHIVE := -Wl,-whole-archive
LD_NO_WHOLE_ARCHIVE := -Wl,-no-whole-archive

ifneq ($(TPROCTE),)
TRICK_ADDITIONAL_CXXFLAGS += -D_TPRO_CTE -I$(TPROCTE)/linux/h -I$(TPROCTE)/linux/tsync/h -I$(TPROCTE)/common/h -I$(TPROCTE)/common/tsync/h
PLATFORM_LIBS += $(TPROCTE)/linux/tsync/lib/libtsync.a $(TPROCTE)/linux/lib/libtpro.a
TRICK_EXCLUDE += :$(TPROCTE)
endif

ifneq ($(BC635),)
# BC635 driver ver 118 and earlier do not have proper include and lib dirs.
TRICK_ADDITIONAL_CXXFLAGS += -D_BC635 -DLINUX -I$(BC635)/sample
PLATFORM_LIBS += $(BC635)/sample/bcsdklib.a
endif

ifeq ($(HAVE_ZEROCONF),1)
PLATFORM_LIBS += -lavahi-client
endif

# Check BFD library version
ifneq ($(BFD_HOME),)
HAVE_BFD := 1
PLATFORM_LIBS += -lbfd
endif

# determine if -std=c++0x or -std=c++11 compile flags are desired
ifeq "" "c++0x"
TRICK_ADDITIONAL_CXXFLAGS += -std=c++0x -D_HAVE_TR1_RANDOM
endif

ifeq "" "c++11"
TRICK_ADDITIONAL_CXXFLAGS += -std=c++11 -D_HAVE_STL_RANDOM
endif

ifeq "" "c++14"
TRICK_ADDITIONAL_CXXFLAGS += -std=c++14 -D_HAVE_STL_RANDOM
endif

TRICK_ADDITIONAL_TEST_FLAGS += -std=c++14

