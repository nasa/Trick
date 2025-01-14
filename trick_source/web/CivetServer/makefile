include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

RM = rm -rf
CC = gcc
CPP = c++
CURL = curl
MV = mv
CP = cp
MKDIR = mkdir

CFLAGS = -g -Wall
CPPFLAGS = -g -Wall -std=c++11

INCLUDE_DIRS = -I${PWD}/include -I${CIVETWEB_HOME}/include -I${TRICK_HOME}/include

OBJDIR = obj
LIBDIR = lib
INCDIR = include

#TRICK_LIB_DIR comes from Makefile.common

TRICK_HTTP_OBJS = \
       ${OBJDIR}/VariableServerSession.o \
       ${OBJDIR}/VariableServerVariable.o \
       ${OBJDIR}/http_GET_handlers.o \
       ${OBJDIR}/MyCivetServer.o \
       ${OBJDIR}/simpleJSON.o

#############################################################################
##                            MODEL TARGETS                                ##
#############################################################################

all: ${TRICK_LIB_DIR}/libtrickCivet.a

$(TRICK_HTTP_OBJS): $(OBJDIR)/%.o : src/%.cpp | $(OBJDIR)
	$(CPP) $(CPPFLAGS) ${TRICK_SYSTEM_CXXFLAGS} ${INCLUDE_DIRS} -c $< -o $@

${TRICK_LIB_DIR}/libtrickCivet.a: ${TRICK_HTTP_OBJS}
	ar crs $@ ${TRICK_HTTP_OBJS}

# ---------------------------------------------------------------------------

${OBJDIR}:
	mkdir -p ${OBJDIR}

clean:
	${RM} *~
	${RM} ${OBJDIR}
