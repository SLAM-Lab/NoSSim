#################################################################
#Helper files for instrumentation, profiling and back-annotation
#################################################################
HELPER_INC = -I. 
#DEFINES = -DOS_MODEL
HELPER_INC:=$(HELPER_INC) $(DEFINES) 

all: profile.c profile.h annotation.c annotation.h
	g++ -O3 $(HELPER_INC) -c  annotation.c -o annotation.o
	g++ -O3 $(HELPER_INC) -c  profile.c -o profile.o 
	/usr/bin/ar cr libhelper.a profile.o annotation.o
	/usr/bin/ranlib libhelper.a

clean:
	rm -f *.o *.a *.log




