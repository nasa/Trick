
RM = rm -rf
CC = gcc
CPP = c++

CFLAGS = -g -Wall -std=c++11 ${TRICK_CXXFLAGS}
INCLUDE_DIRS = -Iinclude
OBJDIR = obj
LIBDIR = lib
LIBNAME = libSAInteg.a
LIBOBJS = ${OBJDIR}/SAIntegrator.o \
	  ${OBJDIR}/RootFinder.o

all: test examples

test: ${LIBDIR}/${LIBNAME}
	${MAKE} -C unittest

examples: ${LIBDIR}/${LIBNAME}
	${MAKE} -C examples

$(LIBOBJS): $(OBJDIR)/%.o : src/%.cpp | $(OBJDIR)
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
	${MAKE} -C unittest clean
	${MAKE} -C examples clean

spotless: 
	${RM} *~
	${RM} ${OBJDIR}
	${RM} ${LIBDIR}
	${MAKE} -C unittest spotless
	${MAKE} -C examples spotless
