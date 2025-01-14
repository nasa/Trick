
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

RM = rm -rf
CC = gcc
CPP = c++

DPX_DIR  = ..

INCLUDE_DIRS = -I${DPX_DIR} -I../.. -I${TRICK_HOME}/include -I/usr/include/libxml2

CFLAGS = -g -Wall $(UDUNITS_INCLUDES)

OBJDIR = object_${TRICK_HOST_CPU}
LIBDIR = ${DPX_DIR}/lib_${TRICK_HOST_CPU}
LIBNAME = libDPC.a
LIBOBJS = ${OBJDIR}/DPC_datastream_supplier.o \
          ${OBJDIR}/DPC_UnitConvDataStream.o \
          ${OBJDIR}/DPC_TimeCstrDataStream.o \
          ${OBJDIR}/DPC_std_curve.o \
          ${OBJDIR}/DPC_delta_curve.o \
          ${OBJDIR}/DPC_plot.o \
          ${OBJDIR}/DPC_standard_plot.o \
          ${OBJDIR}/DPC_delta_plot.o \
          ${OBJDIR}/DPC_page.o \
          ${OBJDIR}/DPC_table.o \
          ${OBJDIR}/DPC_product.o

ifeq ($(TRICK_HOST_TYPE), Linux)
 MAKEDEP_CPP_INC = -Iinclude ${INCLUDE_DIRS} 
endif

ifeq ($(TRICK_HOST_TYPE), Darwin)
 SDK_DIR = $(shell xcrun --show-sdk-path)
 INCLUDE_DIRS += -I${SDK_DIR}/usr/include/libxml2
endif

#############################################################################
##                            MODEL TARGETS                                ##
#############################################################################

all: ${LIBDIR}/${LIBNAME}

$(LIBOBJS): $(OBJDIR)/%.o : %.cpp | $(OBJDIR)
	$(CPP) $(CFLAGS) ${INCLUDE_DIRS} -c $< -o $@

${LIBDIR}/${LIBNAME}: ${LIBOBJS} | $(LIB_DIR)
	ar crs $@ ${LIBOBJS}

${OBJDIR}:
	mkdir -p ${OBJDIR}

${LIBDIR}:
	mkdir -p ${LIBDIR}

clean:
	${RM} *~
	${RM} ${OBJDIR}

spotless: clean
	${RM} ${LIBDIR}/${LIBNAME}

