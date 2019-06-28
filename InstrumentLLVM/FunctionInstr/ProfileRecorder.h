#include "llvm/Pass.h"

#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/APFloat.h"

#include "llvm/Analysis/CaptureTracking.h"
#include "llvm/Analysis/ValueTracking.h"

#include "llvm/IR/CallSite.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/Type.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "llvm/Transforms/Instrumentation.h"


#include <fstream>
#include <sstream>


#ifndef PROFILERECORDER__H 
#define PROFILERECORDER__H 

using namespace llvm;
using namespace std;

typedef struct FuncProfileData {
  long long TotalBBs;
  long CallingTimes;
  double TotalCycles;
} FuncProfData;


typedef struct ProfileData {
  FuncProfData Funcs[10000]; //indexed by function ID
} ProfData;


//Variable for small function filtering  
extern ProfData LibProfData[20];


void LoadProf(){
   std::ifstream infile("function_time.prof");
   std::string line;
   int ii, jj;
   for(ii=0;ii<20;ii++){
      for(jj=0;jj<10000;jj++){
         LibProfData[ii].Funcs[jj].CallingTimes=0;
         LibProfData[ii].Funcs[jj].TotalCycles=0;
         LibProfData[ii].Funcs[jj].TotalBBs=0;
      }
   }

   while (std::getline(infile, line)){
      std::istringstream iss(line);
      int lib, func;
      long CallTimes;
      double Cycles;
      if (!(iss >> lib >> func >> CallTimes >> Cycles)) { break; }
      errs() << lib << " " << func << " " << CallTimes << " " << Cycles <<"\n";
      LibProfData[lib].Funcs[func].CallingTimes = CallTimes;
      LibProfData[lib].Funcs[func].TotalCycles = Cycles;
   }
}                     



#endif // PROFILERECORDER__H 

