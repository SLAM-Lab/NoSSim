DEBUG=0

OBJ=ezsift.o image_utility.o
EXECOBJA=image_match.o

INC_DIR=./ezSIFT/include/
SRC_DIR=./ezSIFT/src/
APP_DIR=./ezSIFT/examples/image_match/
HELPER_DIR=../ProfHelper
EXEC=vgraph

VPATH=$(INC_DIR):$(SRC_DIR):$(APP_DIR)

SLIB=lib${EXEC}.so
ALIB=lib${EXEC}.a
OBJDIR=./obj/

LLVM_DIR=~/Desktop/llvm/install/bin
CC=$(LLVM_DIR)/clang
CXX=$(LLVM_DIR)/clang++
LLVM_LINK=$(LLVM_DIR)/llvm-link
LLVM_LLC=$(LLVM_DIR)/llc
INS_LIB = ../FunctionInstr/libLLVMFunctionInstr.so
LLVM_INSTR_PASS = -functionInstr
INS_STAGE=annot#prof#annot

AR=ar
ARFLAGS=rcs
OPTS=-Ofast
LDFLAGS= -lm -pthread 
COMMON= -I$(INC_DIR) -I$(SRC_DIR)
CFLAGS=-Wall -Wno-unknown-pragmas -Wfatal-errors -fPIC

ifeq ($(DEBUG), 1) 
OPTS=-O0 -g
endif

CFLAGS+=$(OPTS)

EXECOBJ = $(addprefix $(OBJDIR), $(EXECOBJA))
EXECBC = $(EXECOBJ:.o=.bc) 
EXECCPP = $(EXECOBJA:.o=.cpp) 
OBJS = $(addprefix $(OBJDIR), $(OBJ))
BYTECODES = $(OBJS:.o=.bc) 
DEPS = $(wildcard $(SRC_DIR)/*.h) Makefile 

all: obj $(SLIB) $(ALIB) $(EXEC)

$(EXEC): $(EXECOBJ) $(ALIB)
	$(CXX) $(COMMON) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(ALIB)

$(ALIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(SLIB): $(OBJS)
	$(CXX) $(CFLAGS) -shared $^ -o $@ $(LDFLAGS)

$(OBJDIR)%.o: %.cpp $(DEPS)
	$(CXX) $(COMMON) $(CFLAGS) -c $< -o $@

$(OBJDIR)%.bc: %.cpp $(DEPS) 
	$(CXX) -emit-llvm $(COMMON) $(CFLAGS) -c $< -o $@

profile: obj $(BYTECODES)  
	$(LLVM_LINK) $(BYTECODES) -o obj/all.bc	
	$(LLVM_DIR)/opt -load $(INS_LIB) $(LLVM_INSTR_PASS) -libname=1 -InstruProf=$(INS_STAGE) < obj/all.bc > obj/instru.bc
	$(LLVM_LLC) -filetype=obj obj/instru.bc -o obj/all.o
	$(AR) $(ARFLAGS) $(ALIB) obj/all.o
	$(CXX) -emit-llvm $(COMMON) $(CFLAGS) -c $(APP_DIR)$(EXECCPP) -o $(EXECBC)
	$(LLVM_DIR)/opt -load $(INS_LIB) $(LLVM_INSTR_PASS) -libname=0 -InstruProf=$(INS_STAGE) < $(EXECBC) > obj/instru.bc
	$(LLVM_LLC) -filetype=obj obj/instru.bc -o obj/all.o
	$(CXX) $(COMMON) $(CFLAGS) obj/all.o -o $(EXEC) $(LDFLAGS) $(ALIB) -L$(HELPER_DIR) -lhelper


obj:
	mkdir -p obj

test:
	./$(EXEC) ./data/img1.pgm ./data/img2.pgm

.PHONY: clean

clean:
	rm -rf $(OBJS) $(SLIB) $(ALIB) $(EXEC) $(OBJDIR) *.ppm *.key
