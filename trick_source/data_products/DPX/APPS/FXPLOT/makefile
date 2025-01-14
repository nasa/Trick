
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

RM = rm -rf
CC = gcc
CPP = c++

DPX_DIR = ../..

CFLAGS = -g -Wall $(UDUNITS_INCLUDES)

INCDIRS = -I${DPX_DIR} -I../../.. -I${TRICK_HOME}/include -I/usr/include/libxml2 -I$(MOTIF_HOME)/include -I$(TRICK_HOME)/trick_source/data_products/fermi-ware

OBJDIR = object_${TRICK_HOST_CPU}
LIBDIR = ${DPX_DIR}/lib_${TRICK_HOST_CPU}

ifeq ($(TRICK_HOST_TYPE), Linux)
    LIBRTDEF = -lrt
else
    LIBRTDEF =
endif

ifeq ($(TRICK_HOST_TYPE), Darwin)
     INCDIRS += -I/usr/X11/include
     XLIBS = -L${MOTIF_HOME}/lib -lXm -L/usr/X11R6/lib ${X_LIB_DIR} -lXt -lX11 -lc++abi
     SDK_DIR = $(shell xcrun --show-sdk-path)
     INCDIRS += -I${SDK_DIR}/usr/include/libxml2
else
     ifneq (${MOTIF_HOME},/usr)
         XLIBS = -L${MOTIF_HOME}/lib64 -L${MOTIF_HOME}/lib
     endif
     XLIBS += -lXm -lXt -lX11
endif

DP_LIBS       = -L../../../lib_${TRICK_HOST_CPU} -llog -lvar -leqparse -L${TRICK_LIB_DIR} -ltrick_units
DPX_LIBS      = -L${DPX_DIR}/lib_${TRICK_HOST_CPU} -lDPC -lDPM
FERMI_WARE_LIB = $(TRICK_HOME)/trick_source/data_products/fermi-ware/object_${TRICK_HOST_CPU}/libfermi.a

#HDF5_LIB is assigned in Makefile.common
ALL_LIBS = $(DPX_LIBS) $(FERMI_WARE_LIB) ${DP_LIBS} ${TRICK_UNIT_LIBS} $(LIBXML) ${HDF5_LIB} -ldl $(FERMI_WARE_DIR) $(UDUNITS_LDFLAGS)

#############################################################################
##                            MODEL TARGETS                                ##
#############################################################################

C_OBJECT_FILES = $(OBJDIR)/parse_format.o \
    $(OBJDIR)/post_dialog.o

CXX_OBJECT_FILES = $(OBJDIR)/fermi_view.o \
    $(OBJDIR)/fxplot.o \
    $(OBJDIR)/curve_view_node.o \
    $(OBJDIR)/plot_view_node.o \
    $(OBJDIR)/table_view_node.o \
    $(OBJDIR)/page_view_node.o \
    $(OBJDIR)/product_view_node.o

OBJECT_FILES = $(C_OBJECT_FILES) $(CXX_OBJECT_FILES)


all: ${TRICK_HOME}/bin/trick-fxplot

${TRICK_HOME}/bin/trick-fxplot: ${LIBDIR}/libDPM.a ${LIBDIR}/libDPC.a $(OBJECT_FILES)
	@echo "===== Making trick-fxplot ====="
	${CPP} ${CFLAGS} ${INCDIRS} -o $@ $(OBJECT_FILES) ${ALL_LIBS} ${XLIBS} ${LIBRTDEF}

$(C_OBJECT_FILES) : $(OBJDIR)/%.o : %.c
	${CC} ${CFLAGS} ${INCDIRS} -c $< -o $@

$(CXX_OBJECT_FILES) : $(OBJDIR)/%.o : %.cpp
	${CPP} ${CFLAGS} ${INCDIRS} -c $< -o $@

$(OBJECT_FILES) : | $(OBJDIR)

$(OBJDIR) :
	@ mkdir $@

${LIBDIR}/libDPM.a:
	@echo "===== Making libDPM.a ====="
	$(MAKE) -C ${DPX_DIR}/DPM

${LIBDIR}/libDPC.a:
	@echo "===== Making libDPC.a ====="
	$(MAKE) -C ${DPX_DIR}/DPC

clean:
	${RM} $(OBJDIR)

spotless: clean

fxplot: ../../../lib_${TRICK_HOST_CPU}/liblog.a \
  ../../../lib_${TRICK_HOST_CPU}/libvar.a \
  ../../../lib_${TRICK_HOST_CPU}/libeqparse.a


# Dependencies
