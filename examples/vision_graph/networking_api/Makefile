DEBUG=1
HCSIM=1
LWIP=/home/slam/OMNET/lwip-hcsim

VPATH=./src:./lwip:./hcsim
SLIB=libdistriot.so
ALIB=libdistriot.a
OBJDIR=./obj/

CC=gcc
CXX=clang++
AR=ar
ARFLAGS=rcs
LDFLAGS= -lm -pthread 
COMMON= -Iinclude/ -Isrc/ -Ilwip/ -Ihcsim/ -I$(LWIP)/src/include -I$(LWIP)/ports/hcsim -I$(LWIP)/ports/hcsim/context -I$(LWIP)/ports/port/include
CFLAGS=-Wall -fPIC

ifeq ($(HCSIM), 1) 
SYSTEMC_LIB=/home/slam/OMNET/systemc-2.3.1/lib-linux64
SYSTEMC=/home/slam/OMNET/systemc-2.3.1
HCSIM_DIR=/home/slam/OMNET/HCSim
LDFLAGS+=-L$(SYSTEMC_LIB) -lsystemc -L$(HCSIM_DIR)/lib -lhcsim -Wl,-rpath,$(SYSTEMC_LIB)
CFLAGS+=-DSC_INCLUDE_DYNAMIC_PROCESSES -I$(SYSTEMC)/include -I$(HCSIM_DIR)/include
endif

ifeq ($(DEBUG), 1) 
OPTS+=-O0 -g
else
OPTS+=-Ofast
endif

CFLAGS+=$(OPTS)
ifeq ($(HCSIM), 1) 
OBJ = thread_safe_queue.o data_blob.o exec_ctrl.o gateway.o client.o global_context.o network_util.o
else
OBJ = thread_safe_queue.o thread_util.o data_blob.o network_util.o exec_ctrl.o gateway.o client.o global_context.o
endif 

OBJS = $(addprefix $(OBJDIR), $(OBJ))
DEPS = $(wildcard */*.h) Makefile

all: obj $(SLIB) $(ALIB) 

$(ALIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(SLIB): $(OBJS)
	$(CXX) $(CFLAGS) -shared $^ -o $@

$(OBJDIR)%.o: %.c $(DEPS)
	$(CXX) $(COMMON) $(CFLAGS) -c $< -o $@

obj:
	mkdir -p obj

.PHONY: clean

clean:
	rm -rf $(OBJS) $(SLIB) $(ALIB) obj *.log
