
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

CC        = gcc

OBJ_DIR    = object_${TRICK_HOST_CPU}
LIBDIR    = ../lib_${TRICK_HOST_CPU}
LIBNAME   = libeqparse.a
DP_CFLAGS = -g

E_C_SRC   = $(filter-out eqparse_test.c, $(wildcard *.c))
E_C_OBJS  = $(addprefix $(OBJ_DIR)/,$(notdir $(subst .c,.o,$(E_C_SRC))))

ifeq ($(TRICK_HOST_TYPE), Linux)
 DP_CFLAGS += -Wall
endif
ifeq ($(TRICK_HOST_TYPE), Darwin)
 DP_CFLAGS += -Wall
endif

ifeq ($(TRICK_DP_FORCE_32BIT), 1)
       DP_CFLAGS += -m32
endif

#
# Make information
#
MAKEFILE    = makefile
AWKTEMPFILE = awk.temp
MAKEDEPEND  = makedepend

#############################################################################
##                        FILE NAME DEFINITIONS                            ##
#############################################################################

C_SRC = $(wildcard $(SRC_DIR)*.c)
C_OBJS = $($(addprefix $(OBJ_DIR)/,$(notdir $(subst .c,.o,$(C_SRC)))))

CPP_C_SRC = $(wildcard $(SRC_DIR)*.C) 
CPP_CC_SRC = $(wildcard $(SRC_DIR)*.cc) 
CPP_CPP_SRC = $(wildcard $(SRC_DIR)*.cpp) 
CPP_CXX_SRC = $(wildcard $(SRC_DIR)*.cxx) 
CPP_CPLUS_SRC = $(wildcard $(SRC_DIR)*.c++) 
CPP_SRC = $(CPP_C_SRC) $(CPP_CC_SRC) $(CPP_CPP_SRC) $(CPP_CXX_SRC) $(CPP_CPLUS_SRC) 
CPP_C_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(subst .C,.o,$(CPP_C_SRC))))
CPP_CC_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(subst .cc,.o,$(CPP_CC_SRC))))
CPP_CPP_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(subst .cpp,.o,$(CPP_CPP_SRC))))
CPP_CXX_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(subst .cxx,.o,$(CPP_CXX_SRC))))
CPP_CPLUS_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(subst .c++,.o,$(CPP_CPLUS_SRC))))
CPP_OBJS = $(CPP_C_OBJS) $(CPP_CC_OBJS) $(CPP_CPP_OBJS) $(CPP_CXX_OBJS) $(CPP_CPLUS_OBJS) 

F_SRC = $(wildcard $(SRC_DIR)*.f)
F_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(subst .f,.o,$(F_SRC))))

IO_C_SRC = $(wildcard $(IO_SRC_DIR)*.c)
IO_C_OBJS =$(subst io_src,$(OBJ_DIR),$(subst .c,.o,$(IO_C_SRC)))

IO_CPP_SRC = $(wildcard $(IO_SRC_DIR)*.cpp)
IO_CPP_OBJS =$(subst io_src,$(OBJ_DIR),$(subst .cpp,.o,$(IO_CPP_SRC)))

IO_SRC = $(IO_C_SRC) $(IO_CPP_SRC)
IO_OBJS = $(IO_C_OBJS) $(IO_CPP_OBJS)

OBJECT_FILES = $(IO_OBJS) $(C_OBJS) $(CPP_OBJS) $(F_OBJS)

#############################################################################
##                            MODEL TARGETS                                ##
#############################################################################

all: $(LIBDIR)/$(LIBNAME) $(OBJ_DIR)/eqparse_test 

$(OBJ_DIR)/eqparse_test: $(LIBDIR)/$(LIBNAME) 
	$(CC) $(DP_CFLAGS) -o $(OBJ_DIR)/eqparse_test eqparse_test.c $(LIBDIR)/$(LIBNAME) -lm


$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(LIBDIR):
	- mkdir -p $(LIBDIR)

OBJECTS: $(OBJECTS0) 

$(LIBDIR)/$(LIBNAME): $(E_C_OBJS) | $(LIBDIR)
	ar cr $(LIBDIR)/$(LIBNAME) $?

depend:
	@ $(MAKEDEPEND) -s"# DO NOT DELETE" -f $(MAKEFILE) 1>/dev/null 
	@ touch $(AWKTEMPFILE)
	@ $(CC) -MM $(TRICK_CFLAGS) $(MAKEDEP_CPP_INC) $(C_SRC) $(CPP_SRC) $(IO_SRC) >> $(AWKTEMPFILE)
	@ depend_cp $(AWKTEMPFILE) >> $(MAKEFILE)
	@ $(RM) -f $(AWKTEMPFILE) $(AWKTEMPFILE).bak
	@ echo "Appended dependencies to $(MAKEFILE)"

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(LIBDIR)/$(LIBNAME)

real_clean: clean

$(E_C_OBJS): $(OBJ_DIR)/%.o : %.c | $(OBJ_DIR)
	$(CC) $(DP_CFLAGS) -c $< -o $@


#----------------------------
# Dependencies

#######################################################################
# To update dependencies:
# - Save changes (if any)
# - run "make depend"
#######################################################################
# DO NOT DELETE
object_${TRICK_HOST_CPU}/eqparse.o: eqparse.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_chkvalid.o: eqparse_chkvalid.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_error.o: eqparse_error.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_evaluate.o: eqparse_evaluate.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_fillno.o: eqparse_fillno.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_funcsub.o: eqparse_funcsub.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_math.o: eqparse_math.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_negcheck.o: eqparse_negcheck.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_operatorcheck.o: eqparse_operatorcheck.c eqparse_protos.h \
 eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_postfix.o: eqparse_postfix.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_stack.o: eqparse_stack.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_takeinput.o: eqparse_takeinput.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_test.o: eqparse_test.c eqparse.h eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse.o: eqparse.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_chkvalid.o: eqparse_chkvalid.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_error.o: eqparse_error.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_evaluate.o: eqparse_evaluate.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_fillno.o: eqparse_fillno.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_funcsub.o: eqparse_funcsub.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_math.o: eqparse_math.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_negcheck.o: eqparse_negcheck.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_operatorcheck.o: eqparse_operatorcheck.c eqparse_protos.h \
 eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_postfix.o: eqparse_postfix.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_stack.o: eqparse_stack.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_takeinput.o: eqparse_takeinput.c eqparse_protos.h eqparse_stack.h 
object_${TRICK_HOST_CPU}/eqparse_test.o: eqparse_test.c eqparse.h eqparse_protos.h eqparse_stack.h 
