
RM = rm -rf
CC = gcc
CPP = c++


CFLAGS = -g -Wall
INCLUDE_DIRS = -Iinclude
OBJDIR = obj
LIBDIR = lib
LIBNAME = libDecl.a
LIBOBJS = \
	${OBJDIR}/CompositeValue.o \
	${OBJDIR}/IntegerValue.o \
	${OBJDIR}/FloatingPointValue.o \
	${OBJDIR}/PointerValue.o \
	${OBJDIR}/StringValue.o \
	${OBJDIR}/StructMember.o \
	${OBJDIR}/NormalStructMember.o \
	${OBJDIR}/StaticStructMember.o \
	${OBJDIR}/BitfieldStructMember.o \
	${OBJDIR}/Enumerator.o \
	${OBJDIR}/TypeDictionary.o \
	${OBJDIR}/EnumDictionary.o \
	${OBJDIR}/DataType.o \
	${OBJDIR}/CompositeDataType.o \
	${OBJDIR}/EnumDataType.o \
	${OBJDIR}/ArrayDataType.o \
	${OBJDIR}/PointerDataType.o \
	${OBJDIR}/PrimitiveDataType.o \
	${OBJDIR}/LexicalAnalyzer.o \
	${OBJDIR}/ParsedDeclaration.o \
	${OBJDIR}/AllocInfo.o \
	${OBJDIR}/MemMgr.o \
	${OBJDIR}/ClassicChkPtAgent.o

#############################################################################
##                            MODEL TARGETS                                ##
#############################################################################

all: ${LIBDIR}/${LIBNAME}

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

spotless: clean
	${RM} ${LIBDIR}/${LIBNAME}


