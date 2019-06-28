DEBUG=0

OBJ=ezsift.o image_utility.o vgraph.o profile.o
EXECOBJA=test.o

INC_DIR=../include/
SRC_DIR=../src/
EXEC=test_vgraph

VPATH=./:$(INC_DIR):$(SRC_DIR):./profile

SLIB=libvgraph.so
ALIB=libvgraph.a
OBJDIR=./obj/

CC=clang
CXX=clang++

AR=ar
ARFLAGS=rcs
OPTS=-Ofast
LDFLAGS= -lm -pthread 
COMMON= -I. -I./profile -I$(INC_DIR) -I$(SRC_DIR)
CFLAGS=-Wall -Wno-unknown-pragmas -Wfatal-errors -fPIC -fopenmp

ifeq ($(DEBUG), 1) 
OPTS=-O0 -g
endif

CFLAGS+=$(OPTS)

EXECOBJ = $(addprefix $(OBJDIR), $(EXECOBJA))
EXECCPP = $(EXECOBJA:.o=.cpp) 
OBJS = $(addprefix $(OBJDIR), $(OBJ))
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


obj:
	mkdir -p obj

test:
	./$(EXEC)

.PHONY: clean

clean:
	rm -rf $(OBJS) $(SLIB) $(ALIB) $(EXEC) $(OBJDIR) *.ppm *.key
