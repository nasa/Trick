TRICK_CFLAGS += -Imodels -I/usr/local/cuda/include
TRICK_CXXFLAGS += -Imodels -I/usr/local/cuda/include
TRICK_USER_LINK_LIBS += models/fluid/src/gpuCode.o models/fluid/src/vectorAdd.o -L/usr/local/cuda/lib64 -lcudart -lcudadevrt

$(S_MAIN): models/fluid/src/gpuCode.o

models/fluid/src/vectorAdd.o: models/fluid/src/vectorAdd.cu
	nvcc -arch=sm_35 -I. -dc models/fluid/src/vectorAdd.cu -o models/fluid/src/vectorAdd.o

models/fluid/src/gpuCode.o: models/fluid/src/vectorAdd.o
	nvcc -arch=sm_35 -dlink models/fluid/src/vectorAdd.o $(S_OBJECTS) -o models/fluid/src/gpuCode.o
	#nvcc -arch=sm_35 -dlink $(TRICK_SYSTEM_LDFLAGS) $(S_OBJECTS) $(LINK_LISTS) $(TRICK_LDFLAGS) $(TRICK_USER_LINK_LIBS) $(READ_ONLY_LIBS) $(LD_WHOLE_ARCHIVE) $(TRICK_LIBS) $(LD_NO_WHOLE_ARCHIVE) $(TRICK_EXEC_LINK_LIBS)
	
clean: my_clean
	
	
my_clean:
	-rm -rf models/fluid/src/vectorAdd.o models/fluid/src/gpuCode.o
	