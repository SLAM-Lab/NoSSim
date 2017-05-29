#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <climits>

#include "os_ctxt.h"
#include <vector>
#include <cstdio>
#include <cstdlib>


#ifndef __ANNOTATION__H
#define __ANNOTATION__H


#define MAX_ALLOWED 15
#define MIN_GRANU 200000
//#define CLI_TYPE_RPI3 0
//#define SRV_TYPE_RPI4 0


extern "C" void CountBB_BA(int LibID, int FunID);
extern "C" void FunctionDelay(double alpha1, double alpha2, long long delay1, long long delay2, int LibID, int FunID);
extern "C" void program_start_BA(int LibID, int FunID);
extern "C" void program_end_BA(int LibID, int FunID);
extern "C" void RecordFuncBBs( int LibID, int FunID);
extern unsigned char debug_flags;

#endif
