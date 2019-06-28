#include "ProfileRecorder.h"

#ifndef ANNOTATIONRECORDER__H 
#define ANNOTATIONRECORDER__H 
#define PAPI_ERROR 5000
#define BB_ERROR 10


typedef struct FuncAnnotationData {
  double a;
  long long StaticCycles;
} FuncAnnotData;


typedef struct AnnotationData {
  FuncAnnotData Funcs[10000];
} AnnotData;



bool CyclesEqual(long long a,  long long b){

}


bool BBsEqual(long long a,  long long b){

}


void CalParameter1(){

}  



void CalParameter2(){

}  




#endif // ANNOTATIONRECORDER__H 

