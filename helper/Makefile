#################################################################
#Helper files for instrumentation, profiling and back-annotation
#################################################################
PAPI=/home/local/grimlock/zzhao/papi

HELPER=helper
ANNOTATOR=annotation
HELPER_INC =  -Wno-c++11-compat-deprecated-writable-strings -Wno-write-strings  -I. -I$(PAPI)/include

####################################################################
# SystemC Options
####################################################################
# SystemC path
SYSTEMC_DIR=/usr/local/packages/systemc-2.2.0
TLM_INC=/usr/local/packages/systemc-tlm-2.0.1/include/tlm
HCSIM=../HCSim
DEFINES	= -DNDEBUG -DSC_INCLUDE_DYNAMIC_PROCESSES
OS_APP=/home/local/grimlock/zzhao/NoS/VisionGraphApps/OSNode
LFLAGS=-L$(SYSTEMC_DIR)/lib-linux64 -Wl,-rpath=$(SYSTEMC_DIR)/lib-linux64  -lsystemc -L$(HCSIM)/lib -lhcsim 
HELPER_INC:=$(HELPER_INC) $(DEFINES) -I$(OS_APP) -I$(SYSTEMC_DIR)/include -I$(HCSIM)/include  -I$(TLM_INC)

all: $(ANNOTATOR).cpp
	g++ -O3 $(HELPER_INC) -c  $(ANNOTATOR).cpp -o $(ANNOTATOR).o 
	/usr/bin/ar cr lib${HELPER}.a $(ANNOTATOR).o 
	/usr/bin/ranlib lib${HELPER}.a

clean:
	rm -f *.o *.a *.ll *.log *.bc *~




