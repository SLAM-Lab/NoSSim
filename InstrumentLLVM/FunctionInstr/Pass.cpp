#include "ProfileRecorder.h"
#include "AnnotationRecorder.h"

using namespace llvm;
using namespace std;


ProfData LibProfData[20];//Indexed by libID
ProfData Set1[20]; 
ProfData Set2[20];
ProfData Set3[20]; 
ProfData Set4[20];
AnnotData LibAnnotData1[20];
AnnotData LibAnnotData2[20];

static cl::opt<string> libname("libname", cl::desc("Specify lib name under instrumentation"), cl::value_desc("libname"));
static cl::opt<string> Instru("InstruProf", cl::desc("Specify instrumentation Purpose, Profiling or annotation"), cl::value_desc("preprof/prof/profbb/annot"));
static cl::opt<string> min_granu("min_granu", cl::desc("Specify min granularity for profiling threshold"), cl::value_desc("min_granu"));

namespace {
struct FunctionInstr : public FunctionPass{
  FunctionInstr() : FunctionPass(ID) {}
  const char *getPassName() const override;
  bool runOnFunction(Function &F) override;
  bool doInitialization(Module &M) override;
  static char ID;   

       
 private:   
  void initializeCallbacks(Module &M);
  Type *IntptrTy;
  IntegerType *OrdTy;

  //PAPI instrumentation functions
  Function *PapiFuncEntry;
  Function *PapiFuncExit;
  Function *PapiInit;
  Function *PapiTerminate;  

  //Dynamic BB profiling function 
  Function *FuncCountBB;

  //Annotation Functions 
  Function *BAStart;
  Function *BATerminate;
  Function *BACountBB;
  Function* BAFuncDelay; 
  Function* BAFuncRecordBB;

  // Assign a unique ID number for each function 
  vector<StringRef> FunctionName;
  vector<int> FunID;
  int ticket;   

};
}
const char *FunctionInstr::getPassName() const {
  return "FunctionInstr";
}

bool FunctionInstr::doInitialization(Module &M) {
  if(Instru == "preprof"){
  	errs()<< "================== Pre-Profiling stage =================" << '\n';
  }

  if(Instru == "profbb"){
  	errs()<< "================== ProfilingBB stage =================" << '\n';
        LoadProf();
	for(int ii=0;ii<20;ii++){
          for(int jj=0;jj<10000;jj++){
            if(LibProfData[ii].Funcs[jj].CallingTimes!=0)
		errs()<<ii<<" "<<jj<<" "<< LibProfData[ii].Funcs[jj].CallingTimes<<" "
		<< LibProfData[ii].Funcs[jj].TotalCycles << '\n';
          }
    	}

  }


  if(Instru == "prof"){
  	errs()<< "================== Profiling stage =================" << '\n';
        LoadProf();
	for(int ii=0;ii<20;ii++){
          for(int jj=0;jj<10000;jj++){
            if(LibProfData[ii].Funcs[jj].CallingTimes!=0)
		errs()<<ii<<" "<<jj<<" "<< LibProfData[ii].Funcs[jj].CallingTimes<<" "
		<< LibProfData[ii].Funcs[jj].TotalCycles << '\n';
          }
    	}

  }

  if(Instru == "annot"){
  	errs()<< "================== Annotation stage =================" << '\n';
        CalParameter1();
        CalParameter2();

	errs()<<"================== Set1 data is  ==================\n";
	for(int ii=0;ii<20;ii++){
          for(int jj=0;jj<10000;jj++){
            if(Set1[ii].Funcs[jj].CallingTimes!=0)
		errs()<<ii<<" "<<jj<<" "<< Set1[ii].Funcs[jj].CallingTimes<<" "
		<< Set1[ii].Funcs[jj].TotalCycles <<" "<< Set1[ii].Funcs[jj].TotalBBs << '\n';
          }
    	}


	errs()<<"================== Set2 data is  ==================\n";
	for(int ii=0;ii<20;ii++){
          for(int jj=0;jj<10000;jj++){
            if(Set2[ii].Funcs[jj].CallingTimes!=0)
		errs()<<ii<<" "<<jj<<" "<< Set2[ii].Funcs[jj].CallingTimes<<" "
		<< Set2[ii].Funcs[jj].TotalCycles <<" "<<  Set2[ii].Funcs[jj].TotalBBs<< '\n';
          }
    	}


	errs()<<"================== Parameter table is  ==================\n";
	for(int ii=0;ii<20;ii++){
          for(int jj=0;jj<10000;jj++){
            if(LibAnnotData1[ii].Funcs[jj].StaticCycles !=0)
		errs()<<ii<<" "<<jj<<" "
			<< LibAnnotData1[ii].Funcs[jj].a<<" "
			<< LibAnnotData1[ii].Funcs[jj].StaticCycles << '\n';
          }
    	}

	errs()<<"================== Set3 data is  ==================\n";
	for(int ii=0;ii<20;ii++){
          for(int jj=0;jj<10000;jj++){
            if(Set3[ii].Funcs[jj].CallingTimes!=0)
		errs()<<ii<<" "<<jj<<" "<< Set3[ii].Funcs[jj].CallingTimes<<" "
		<< Set3[ii].Funcs[jj].TotalCycles <<" "<< Set3[ii].Funcs[jj].TotalBBs << '\n';
          }
    	}


	errs()<<"================== Set4 data is  ==================\n";
	for(int ii=0;ii<20;ii++){
          for(int jj=0;jj<10000;jj++){
            if(Set4[ii].Funcs[jj].CallingTimes!=0)
		errs()<<ii<<" "<<jj<<" "<< Set4[ii].Funcs[jj].CallingTimes<<" "
		<< Set4[ii].Funcs[jj].TotalCycles <<" "<<  Set4[ii].Funcs[jj].TotalBBs<< '\n';
          }
    	}


	errs()<<"================== Parameter table is  ==================\n";
	for(int ii=0;ii<20;ii++){
          for(int jj=0;jj<10000;jj++){
            if(LibAnnotData2[ii].Funcs[jj].StaticCycles !=0)
		errs()<<ii<<" "<<jj<<" "
			<< LibAnnotData2[ii].Funcs[jj].a<<" "
			<< LibAnnotData2[ii].Funcs[jj].StaticCycles << '\n';
          }
    	}



  }

  ticket = 0;
  FILE *f = fopen(("FunctionTable"+libname+".txt").c_str(), "w");
  if (f == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }
  fclose(f);
  return true;
}


void FunctionInstr::initializeCallbacks(Module &M) {
  IRBuilder<> IRB(M.getContext());
  // Initialize the callbacks.
  PapiInit = cast<Function>( M.getOrInsertFunction(
      "program_start", IRB.getVoidTy(), IRB.getInt32Ty(),IRB.getInt32Ty(), nullptr));
  PapiTerminate = cast<Function>( M.getOrInsertFunction(
      "program_end", IRB.getVoidTy(), IRB.getInt32Ty(),IRB.getInt32Ty(), nullptr));
  FuncCountBB = cast<Function>( M.getOrInsertFunction(
      "CountBB", IRB.getVoidTy(), IRB.getInt32Ty(),IRB.getInt32Ty(), nullptr));
  PapiFuncEntry = cast<Function>( M.getOrInsertFunction(
      "papi_instrument_func_entry", IRB.getVoidTy(), IRB.getInt32Ty(),IRB.getInt32Ty(), nullptr));
  PapiFuncExit = cast<Function>( M.getOrInsertFunction(
      "papi_instrument_func_exit", IRB.getVoidTy(), IRB.getInt32Ty(),IRB.getInt32Ty(), nullptr));

  BAStart = cast<Function>( M.getOrInsertFunction(
      "program_start_BA", IRB.getVoidTy(), IRB.getInt32Ty(),IRB.getInt32Ty(), nullptr));
  BATerminate = cast<Function>( M.getOrInsertFunction(
      "program_end_BA", IRB.getVoidTy(), IRB.getInt32Ty(),IRB.getInt32Ty(), nullptr));
  BACountBB = cast<Function>( M.getOrInsertFunction(
      "CountBB_BA", IRB.getVoidTy(), IRB.getInt32Ty(),IRB.getInt32Ty(), nullptr));
  BAFuncRecordBB = cast<Function>( M.getOrInsertFunction(
      "RecordFuncBBs", IRB.getVoidTy(), IRB.getInt32Ty(),IRB.getInt32Ty(), nullptr));
  BAFuncDelay = cast<Function>( M.getOrInsertFunction(
      "FunctionDelay", IRB.getVoidTy(), IRB.getDoubleTy(), IRB.getDoubleTy(), IRB.getInt64Ty(), IRB.getInt64Ty(), 
	IRB.getInt32Ty(),IRB.getInt32Ty(),
	nullptr));




}

bool FunctionInstr::runOnFunction(Function &F) {

// Make a Harshing number for each function 
  bool Res = false;
  bool Covered = 0;  
  int CurFunID = 0;
  int libID= atoi(libname.c_str());
  long long cyc_threshold = atoi(min_granu.c_str());
  vector<int>::iterator itID = FunID.begin();

  //errs()<<"Threshold is: "<< cyc_threshold <<'\n';
  if(F.getLinkage() == llvm::GlobalValue::ExternalLinkage)
  {
  
    for (vector<StringRef>::iterator it = FunctionName.begin() ;
	it != FunctionName.end(); ++it, ++itID) 
	if( it->equals(F.getName()) ){
	  Covered = 1;
	  CurFunID = *itID;  
	  break;	
	}
    if (Covered == 0){
	ticket ++ ;
	FunctionName.push_back( F.getName());
	FunID.push_back( ticket );
	Covered = 1;
	CurFunID = ticket;  
    }
  }
//  errs()<<"Is external linkable: "<<(F.getLinkage() == llvm::GlobalValue::ExternalLinkage) <<'\n';
//  errs()<<"Lib is: "<<(libname.c_str())<<'\n';
//  errs()<<"Func ID is: " << ticket << '\n';
//  errs().write_escaped(F.getName()) << '\n';
//  errs()<<(F.getName().equals(StringRef("main")))<< '\n';  
//  StringRef OutputFilename( "Prof.txt" );
//  std::error_code EC;
//  raw_fd_ostream* out = new raw_fd_ostream(OutputFilename, EC, llvm::sys::fs::O_RDWR);
//  out->close();

//Record function lists for each library
  initializeCallbacks(*F.getParent());
  SmallVector<Instruction*, 8> RetVec;

//Only
  if((Instru == "annot") || (Instru == "profbb")){

   Res = true; //A true function should be returned if the function is modified
   for (auto &BB : F) {
     if(isa<LandingPadInst>(BB.getFirstNonPHI())) continue;//Skip exception handling code
     IRBuilder<> IRB(BB.getFirstNonPHI());
     if(Instru == "profbb")
	IRB.CreateCall(FuncCountBB, {IRB.getInt32(libID), IRB.getInt32(CurFunID)});
     if(Instru == "annot" )IRB.CreateCall(BACountBB, {IRB.getInt32(libID), IRB.getInt32(CurFunID)});
   }

  }




//Filterting out small functions in profiling stage
  if((Instru == "profbb") && !(F.getName().equals(StringRef("main"))))
    if( LibProfData[libID].Funcs[CurFunID].TotalCycles < cyc_threshold )
	return Res;		

//errs()<<libID << ":" << CurFunID << ":"<< LibProfData[libID].Funcs[CurFunID].TotalCycles <<'\n';
//Filterting out small functions in profiling stage
  if((Instru == "prof") && !(F.getName().equals(StringRef("main"))))
    if( LibProfData[libID].Funcs[CurFunID].TotalCycles < cyc_threshold )
	{
		return Res;		
	}
//Filterting out small functions in annotation stage
  if((Instru == "annot") && !(F.getName().equals(StringRef("main"))))
    if( (Set1[libID].Funcs[CurFunID].CallingTimes == 0) &&  (Set2[libID].Funcs[CurFunID].CallingTimes == 0))
	return Res;
//Record function lists for each library
  FILE *f = fopen(("FunctionTable"+libname+".txt").c_str(), "a");
  if (f == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }

  if(F.getLinkage() == llvm::GlobalValue::ExternalLinkage)
  	fprintf(f, "Ext Visible:%d:%d:%d:%s\n", (F.getLinkage() == llvm::GlobalValue::ExternalLinkage), libID, CurFunID, F.getName().data());
  fclose(f);










  if(F.getLinkage() == llvm::GlobalValue::ExternalLinkage)//We want to only annotated funtions available in source code  
  {


   Res = true; //A true function should be returned if the function is modified
   for (auto &BB : F) {
     if(isa<LandingPadInst>(BB.getFirstNonPHI())) continue;//Skip exception handling code
     IRBuilder<> IRB(BB.getFirstNonPHI());
     for (auto &Inst : BB) {//Extract function exit points
       if (isa<ReturnInst>(Inst))
         RetVec.push_back(&Inst);
     } 
   }



   if((F.getName().equals(StringRef("main")))){
	IRBuilder<> IRB(F.getEntryBlock().getFirstNonPHI());
        if((Instru == "profbb"))
		IRB.CreateCall(PapiInit, {IRB.getInt32(libID), IRB.getInt32(CurFunID)});
        if((Instru == "prof") || (Instru == "preprof"))
		IRB.CreateCall(PapiInit, {IRB.getInt32(libID), IRB.getInt32(CurFunID)});
        if(Instru == "annot" )IRB.CreateCall(BAStart, {IRB.getInt32(libID), IRB.getInt32(CurFunID)});
   	for (auto RetInst : RetVec) {
      	  IRBuilder<> IRBRet(RetInst);
      	  if((Instru == "profbb"))
		IRBRet.CreateCall(PapiTerminate, {IRBRet.getInt32(libID), IRBRet.getInt32(CurFunID)});  
      	  if((Instru == "prof") || (Instru == "preprof"))
		IRBRet.CreateCall(PapiTerminate, {IRBRet.getInt32(libID), IRBRet.getInt32(CurFunID)});  
	  if(Instru == "annot" ){
      	  	IRBRet.CreateCall(BAFuncDelay, {
			ConstantFP::get(IRBRet.getContext(), APFloat( LibAnnotData1[libID].Funcs[CurFunID].a )), 
			ConstantFP::get(IRBRet.getContext(), APFloat( LibAnnotData1[libID].Funcs[CurFunID].a )), 
			IRBRet.getInt64(LibAnnotData1[libID].Funcs[CurFunID].StaticCycles), 
			IRBRet.getInt64(LibAnnotData2[libID].Funcs[CurFunID].StaticCycles), 
			IRBRet.getInt32(libID), IRBRet.getInt32(CurFunID)});  
		IRBRet.CreateCall(BATerminate, {IRBRet.getInt32(libID), IRBRet.getInt32(CurFunID)});  
	 	//The calling order in result code will be the same as the order of CreateCall
	  }
   	}
	
   }
   else{
   	IRBuilder<> IRB(F.getEntryBlock().getFirstNonPHI());
      	if((Instru == "profbb"))
   		IRB.CreateCall(PapiFuncEntry, {IRB.getInt32(libID), IRB.getInt32(CurFunID)});
      	if((Instru == "prof") || (Instru == "preprof"))
   		IRB.CreateCall(PapiFuncEntry, {IRB.getInt32(libID), IRB.getInt32(CurFunID)});
	if(Instru == "annot" )
   		IRB.CreateCall(BAFuncRecordBB, {IRB.getInt32(libID), IRB.getInt32(CurFunID)});
   	for (auto RetInst : RetVec) {
      	  IRBuilder<> IRBRet(RetInst);
      	  if((Instru == "profbb"))
      	      IRBRet.CreateCall(PapiFuncExit, {IRBRet.getInt32(libID), IRBRet.getInt32(CurFunID)});  
      	  if((Instru == "prof") || (Instru == "preprof"))
      	      IRBRet.CreateCall(PapiFuncExit, {IRBRet.getInt32(libID), IRBRet.getInt32(CurFunID)});  
	  if(Instru == "annot" )
      	  	IRBRet.CreateCall(BAFuncDelay, {
			ConstantFP::get(IRBRet.getContext(), APFloat( LibAnnotData1[libID].Funcs[CurFunID].a )), 
			ConstantFP::get(IRBRet.getContext(), APFloat( LibAnnotData1[libID].Funcs[CurFunID].a )), 
			IRBRet.getInt64(LibAnnotData1[libID].Funcs[CurFunID].StaticCycles), 
			IRBRet.getInt64(LibAnnotData2[libID].Funcs[CurFunID].StaticCycles), 
			IRBRet.getInt32(libID), IRBRet.getInt32(CurFunID)}); 
   	}
   }
  } 




  return Res;
}



char FunctionInstr::ID = 0;
static RegisterPass<FunctionInstr> Y("functionInstr", "functionInstr Pass", false, false);
