CXX = g++
CFLAGS = -Wall -Wconversion -fPIC -w -fpermissive -g
EXE = ecg
SLIB = libecg.so


all: $(EXE)

obj: ecg_flow.c mysqrs.c mywqrs.c mydwt.c svmpredict.c noiseremoval.c offload.c profile.cpp
	$(CXX)  $(CFLAGS) -c ecg_flow.c mysqrs.c mywqrs.c mydwt.c svmpredict.c noiseremoval.c offload.c svm.cpp profile.cpp 

$(SLIB): obj
	$(CXX) $(CFLAGS) -shared ecg_flow.o mysqrs.o mywqrs.o mydwt.o svmpredict.o noiseremoval.o offload.o svm.o profile.o -o $@

$(EXE): $(SLIB)
	$(CXX) $(CFLAGS) main.c -o ecg -lm -L. -Wl,-rpath,. -lecg

test:
	mkdir -p detection
	mkdir -p segmentation
	mkdir -p extraction
	mkdir -p classification
	./$(EXE)

clean:
	rm -rf *~ *.o $(EXE) *.log *~ *.a *.bc *.so detection segmentation extraction classification


