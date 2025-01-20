TRICK_CFLAGS += -I./models -I./build -g 
TRICK_CXXFLAGS += -I./models -I./build -std=c++11 -g

TRICK_ICGFLAGS += -use_tmm_alloc_args
TRICK_CONVERT_SWIG_FLAGS += --tmm
