#include "ProfileRecorder.h"

#ifndef ANNOTATIONRECORDER__H 
#define ANNOTATIONRECORDER__H 
#define PAPI_ERROR 5000
#define BB_ERROR 10

//Variable for prediction parameter calculation 
extern ProfData Set1[20];
extern ProfData Set2[20];
extern ProfData Set3[20];
extern ProfData Set4[20];

typedef struct FuncAnnotationData {
  double a;
  long long StaticCycles;
} FuncAnnotData;


typedef struct AnnotationData {
  FuncAnnotData Funcs[10000];
} AnnotData;

extern AnnotData LibAnnotData1[20];
extern AnnotData LibAnnotData2[20];

bool CyclesEqual(long long a,  long long b){
	if( ((a-b)>=0) &&( (a-b)<PAPI_ERROR) )
		return true;
	else if ( ((b-a)>=0) && ((b-a)<PAPI_ERROR))
		return true;
	return false;

}


bool BBsEqual(long long a,  long long b){

	
	if( ((a-b)>=0) &&( (a-b)<BB_ERROR) )
		return true;
	else if ( ((b-a)>=0) && ((b-a)<BB_ERROR))
		return true;
	return false;

}


void CalParameter1(){
    std::ifstream infile1("Set1");
    std::ifstream infile2("Set2");
    std::ifstream infile3("Set3");
    std::ifstream infile4("Set4");

    std::ifstream BBfile1("BB1");
    std::ifstream BBfile2("BB2");


    std::string line;

    int ii;
    int jj;
    for(ii=0;ii<20;ii++){
        for(jj=0;jj<10000;jj++){
                Set1[ii].Funcs[jj].CallingTimes=0;
                Set1[ii].Funcs[jj].TotalCycles=0;
                Set1[ii].Funcs[jj].TotalBBs=0;

                Set2[ii].Funcs[jj].CallingTimes=0;
                Set2[ii].Funcs[jj].TotalCycles=0;
                Set2[ii].Funcs[jj].TotalBBs=0;
        }
    }

    int lib, func;
    long CallTimes;
    long long BBs;
    long long  Cycles;

    while (std::getline(infile1, line))
    {
        std::istringstream iss(line);
        if (!(iss >> lib >> func >> CallTimes >> Cycles >> BBs)) { break; } // error
        	Set1[lib].Funcs[func].CallingTimes=(CallTimes);
        	Set1[lib].Funcs[func].TotalCycles=(long long)(((double)(Cycles))/((double)CallTimes));
//	}
    }

    while (std::getline(infile2, line))
    {
        std::istringstream iss(line);
        if (!(iss >> lib >> func >> CallTimes >> Cycles >> BBs)) { break; } // error
        	Set2[lib].Funcs[func].CallingTimes=(CallTimes);
        	Set2[lib].Funcs[func].TotalCycles=(long long)(((double)(Cycles))/((double)CallTimes));
//	}
    }



    while (std::getline(BBfile1, line))
    {
        std::istringstream iss(line);
        if (!(iss >> lib >> func >> CallTimes >> Cycles >> BBs)) { break; } // error
        	Set1[lib].Funcs[func].TotalBBs=(long long)(((double)(BBs))/((double)CallTimes));
    }

    while (std::getline(BBfile2, line))
    {
        std::istringstream iss(line);
        if (!(iss >> lib >> func >> CallTimes >> Cycles >> BBs)) { break; } // error
        	Set2[lib].Funcs[func].TotalBBs=(long long)(((double)(BBs))/((double)CallTimes));
    }








    long long BB1;
    long long BB2;
    long long cycle1;
    long long cycle2; 


    for(int ii=0;ii<20;ii++){
          for(int jj=0;jj<10000;jj++){
            if((Set1[ii].Funcs[jj].CallingTimes!=0)||(Set2[ii].Funcs[jj].CallingTimes!=0)){
		cycle1 = Set1[ii].Funcs[jj].TotalCycles;
		cycle2 = Set2[ii].Funcs[jj].TotalCycles;
		BB1 = Set1[ii].Funcs[jj].TotalBBs;
		BB2 = Set2[ii].Funcs[jj].TotalBBs;
		if( BBsEqual(BB1, BB2) || (CyclesEqual(cycle1, cycle2)) ){
			LibAnnotData1[ii].Funcs[jj].a =  0;
			LibAnnotData1[ii].Funcs[jj].StaticCycles = (cycle1+cycle2)/2;
		}
		else{
			LibAnnotData1[ii].Funcs[jj].a =  ((double)(cycle1-cycle2)) / ((double)(BB1-BB2));
			if( LibAnnotData1[ii].Funcs[jj].a < 0 ){ LibAnnotData1[ii].Funcs[jj].a = 0;} 
			LibAnnotData1[ii].Funcs[jj].StaticCycles = (long)(((double)(cycle2*BB1-cycle1*BB2)) / ((double)(BB1-BB2)));
		}

	    }		
          }
    }


}  




void CalParameter2(){
    std::ifstream infile1("Set3");
    std::ifstream infile2("Set4");

    std::ifstream BBfile1("BB1");
    std::ifstream BBfile2("BB2");


    std::string line;

    int ii;
    int jj;
    for(ii=0;ii<20;ii++){
        for(jj=0;jj<10000;jj++){
                Set3[ii].Funcs[jj].CallingTimes=0;
                Set3[ii].Funcs[jj].TotalCycles=0;
                Set3[ii].Funcs[jj].TotalBBs=0;

                Set4[ii].Funcs[jj].CallingTimes=0;
                Set4[ii].Funcs[jj].TotalCycles=0;
                Set4[ii].Funcs[jj].TotalBBs=0;
        }
    }

    int lib, func;
    long CallTimes;
    long long BBs;
    long long  Cycles;

    while (std::getline(infile1, line))
    {
        std::istringstream iss(line);
        if (!(iss >> lib >> func >> CallTimes >> Cycles >> BBs)) { break; } // error
        	Set3[lib].Funcs[func].CallingTimes=(CallTimes);
        	Set3[lib].Funcs[func].TotalCycles=(long long)(((double)(Cycles))/((double)CallTimes));
//	}
    }

    while (std::getline(infile2, line))
    {
        std::istringstream iss(line);
        if (!(iss >> lib >> func >> CallTimes >> Cycles >> BBs)) { break; } // error
        	Set4[lib].Funcs[func].CallingTimes=(CallTimes);
        	Set4[lib].Funcs[func].TotalCycles=(long long)(((double)(Cycles))/((double)CallTimes));
//	}
    }



    while (std::getline(BBfile1, line))
    {
        std::istringstream iss(line);
        if (!(iss >> lib >> func >> CallTimes >> Cycles >> BBs)) { break; } // error
        	Set3[lib].Funcs[func].TotalBBs=(long long)(((double)(BBs))/((double)CallTimes));
    }

    while (std::getline(BBfile2, line))
    {
        std::istringstream iss(line);
        if (!(iss >> lib >> func >> CallTimes >> Cycles >> BBs)) { break; } // error
        	Set4[lib].Funcs[func].TotalBBs=(long long)(((double)(BBs))/((double)CallTimes));
    }








    long long BB1;
    long long BB2;
    long long cycle1;
    long long cycle2; 


    for(int ii=0;ii<20;ii++){
          for(int jj=0;jj<10000;jj++){
            if((Set3[ii].Funcs[jj].CallingTimes!=0)||(Set4[ii].Funcs[jj].CallingTimes!=0)){
		cycle1 = Set3[ii].Funcs[jj].TotalCycles;
		cycle2 = Set4[ii].Funcs[jj].TotalCycles;
		BB1 = Set3[ii].Funcs[jj].TotalBBs;
		BB2 = Set4[ii].Funcs[jj].TotalBBs;
		if( BBsEqual(BB1, BB2) || (CyclesEqual(cycle1, cycle2)) ){
			LibAnnotData2[ii].Funcs[jj].a =  0;
			LibAnnotData2[ii].Funcs[jj].StaticCycles = (cycle1+cycle2)/2;
		}
		else{
			LibAnnotData2[ii].Funcs[jj].a =  ((double)(cycle1-cycle2)) / ((double)(BB1-BB2));
			if( LibAnnotData2[ii].Funcs[jj].a < 0 ){ LibAnnotData2[ii].Funcs[jj].a = 0;} 
			LibAnnotData2[ii].Funcs[jj].StaticCycles = (long)(((double)(cycle2*BB1-cycle1*BB2)) / ((double)(BB1-BB2)));
		}

	    }		
          }
    }


}  




#endif // ANNOTATIONRECORDER__H 
