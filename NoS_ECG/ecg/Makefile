CXX ?= g++
CFLAGS = -Wall -Wconversion -fPIC -w -fpermissive -g


COPTS=-emit-llvm
LLVM_CC=clang++
LIB_ID=-libname=2
INS_LIB = /home/local/grimlock/zzhao/NoS/VisionGraphApps/InstrumentLLVM/build/FunctionInstr/libLLVMFunctionInstr.so
#INS_LIB = /home/zhuoran/Desktop/setup/InstrumentLLVM/build/FunctionInstr/libLLVMFunctionInstr.so
LLVM_INSTR_PASS = -functionInstr
INS_STAGE=annot#preprof#prof#profbb#annot 
MIN_GRANU=20000



HELPER_DIR=/home/zhuoran/Desktop/setup/raspberrypi3_setup/InstrumentHelper
HELPER_INC=$(HELPER_DIR)
HELPER_LIB=$(HELPER_DIR)/libhelper.a
CFLAGS:=$(CFLAGS) -I$(HELPER_DIR)


DATA_SET=SET1
CFLAGS:=$(CFLAGS) -D$(DATA_SET)

LIBNAME = libecg.a

all: $(LIBNAME) obj 

obj: ecg_flow.c mysqrs.c mywqrs.c mydwt.c svmpredict.c noiseremoval.c offload.c
	#$(CXX) $(CFLAGS) -c ecg_flow.c mysqrs.c mywqrs.c mydwt.c svmpredict.c noiseremoval.c offload.c svm.cpp -lm 
	$(LLVM_CC) $(COPTS) $(CFLAGS) -c ecg_flow.c mysqrs.c mywqrs.c mydwt.c svmpredict.c noiseremoval.c offload.c svm.cpp 
	llvm-link  ecg_flow.bc mysqrs.bc mywqrs.bc mydwt.bc svmpredict.bc noiseremoval.bc offload.bc svm.bc -o ecg.all.bc
	#opt -load $(INS_LIB) $(LLVM_INSTR_PASS) -libname=2 -InstruProf=$(INS_STAGE) -min_granu=$(MIN_GRANU) < ecg.all.bc > ecg.instru.bc
	#llc -filetype=obj ecg.instru.bc -o ecg.o
	llc -filetype=obj ecg.all.bc -o ecg.o

$(LIBNAME):  obj
	ar rs $(LIBNAME) ecg.o

clean:
	rm -f *~ *.o $(EXF) *.log *~ *.a *.bc

main:
	$(CXX) $(CFLAGS) main.c -o ecg -lm -L. -lecg -rdynamic $(HELPER_LIB) -lpapi
#/usr/bin/ar cr $(LIBNAME) $(EXF)
#/usr/bin/ranlib $(LIBNAME)
