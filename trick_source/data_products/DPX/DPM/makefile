
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

RM = rm -rf
CC = gcc
CPP = c++

DPX_DIR = ..

INCLUDE_DIRS = -I${DPX_DIR} -I../.. -I${TRICK_HOME}/include -I/usr/include/libxml2

CFLAGS = -g -Wall $(UDUNITS_INCLUDES)

OBJDIR = object_${TRICK_HOST_CPU}
LIBDIR = ${DPX_DIR}/lib_${TRICK_HOST_CPU}
LIBNAME = libDPM.a
LIBOBJS = ${OBJDIR}/DPM_attribute.o \
          ${OBJDIR}/DPM_component.o \
          ${OBJDIR}/DPM_var.o \
          ${OBJDIR}/DPM_run.o \
          ${OBJDIR}/DPM_axis.o \
          ${OBJDIR}/DPM_column.o \
          ${OBJDIR}/DPM_measurement.o \
          ${OBJDIR}/DPM_page.o \
          ${OBJDIR}/DPM_table.o \
          ${OBJDIR}/DPM_curve.o \
          ${OBJDIR}/DPM_relation.o \
          ${OBJDIR}/DPM_inputs.o \
          ${OBJDIR}/DPM_outputs.o \
          ${OBJDIR}/DPM_extfn.o \
          ${OBJDIR}/DPM_product.o \
          ${OBJDIR}/DPM_session.o \
          ${OBJDIR}/DPM_parse_tree.o \
          ${OBJDIR}/DPM_time_constraints.o

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

${LIBDIR}/${LIBNAME}: ${LIBOBJS} | ${LIBDIR}
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


# Dependencies

#######################################################################
# To update dependencies:
# - Save changes (if any)
# - run "make depend"
#######################################################################
# DO NOT DELETE
