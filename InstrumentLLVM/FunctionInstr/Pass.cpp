#include "ProfileRecorder.h"

using namespace llvm;
using namespace std;


ProfData LibProfData[20];//Indexed by libID


static cl::opt<string> libname("libname", cl::desc("Specify lib name under instrumentation"), cl::value_desc("libname"));
static cl::opt<string> Instru("InstruProf", cl::desc("Specify instrumentation Purpose, Profiling or annotation"), cl::value_desc("preprof/prof/profbb/annot"));
static cl::opt<string> min_granu("min_granu", cl::desc("Specify min granularity for profiling threshold"), cl::value_desc("min_granu"));

namespace {
struct FunctionInstr : public FunctionPass{
  FunctionInstr() : FunctionPass(ID) {}
  StringRef getPassName() const override;
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
  Function* BAFuncDelay; 

  // Assign a unique ID number for each function 
  vector<StringRef> FunctionName;
  vector<int> FunID;
  int ticket;   

};
}
StringRef FunctionInstr::getPassName() const {
  return "FunctionInstr";
}

bool FunctionInstr::doInitialization(Module &M) {


  if(Instru == "profbb"){
  	errs()<< "================== ProfilingBB stage =================" << '\n';
  }


  if(Instru == "prof"){
  	errs()<< "================== Profiling stage =================" << '\n';

  }

  if(Instru == "annot"){
  	errs()<< "================== Annotation stage =================" << '\n';
        LoadProf();
	for(int ii=0;ii<20;ii++){
          for(int jj=0;jj<10000;jj++){
            if(LibProfData[ii].Funcs[jj].CallingTimes!=0)
		errs()<<ii<<" "<<jj<<" "<< LibProfData[ii].Funcs[jj].CallingTimes<<" "
		<< LibProfData[ii].Funcs[jj].TotalCycles << '\n';
          }
    	}
  }

  ticket = 0;

  std::ofstream outfile ("FunctionTable"+libname+".txt");
  outfile.close();


  return true;
}


void FunctionInstr::initializeCallbacks(Module &M) {
  IRBuilder<> IRB(M.getContext());
  // Initialize the callbacks.
  PapiInit = cast<Function>( M.getOrInsertFunction(
      "program_start", IRB.getVoidTy(), IRB.getInt32Ty(),IRB.getInt32Ty()));
  PapiTerminate = cast<Function>( M.getOrInsertFunction(
      "program_end", IRB.getVoidTy(), IRB.getInt32Ty(),IRB.getInt32Ty()));
  FuncCountBB = cast<Function>( M.getOrInsertFunction(
      "count_bb", IRB.getVoidTy(), IRB.getInt32Ty(),IRB.getInt32Ty()));
  PapiFuncEntry = cast<Function>( M.getOrInsertFunction(
      "function_start", IRB.getVoidTy(), IRB.getInt32Ty(),IRB.getInt32Ty()));
  PapiFuncExit = cast<Function>( M.getOrInsertFunction(
      "function_exit", IRB.getVoidTy(), IRB.getInt32Ty(),IRB.getInt32Ty()));

  BAStart = cast<Function>( M.getOrInsertFunction(
      "simulation_start", IRB.getVoidTy(), IRB.getInt32Ty(),IRB.getInt32Ty()));
  BATerminate = cast<Function>( M.getOrInsertFunction(
      "simulation_end", IRB.getVoidTy(), IRB.getInt32Ty(),IRB.getInt32Ty()));
  BAFuncDelay = cast<Function>( M.getOrInsertFunction(
      "func_waitfor", IRB.getVoidTy(), IRB.getDoubleTy(), IRB.getDoubleTy(), IRB.getInt32Ty(),IRB.getInt32Ty()));




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

  if((Instru == "annot") || (Instru == "profbb")){
   Res = true; //A true function should be returned if the function is modified
   for (auto &BB : F) {
     if(isa<LandingPadInst>(BB.getFirstNonPHI())) continue;//Skip exception handling code
     IRBuilder<> IRB(BB.getFirstNonPHI());
     if(Instru == "profbb")
	IRB.CreateCall(FuncCountBB, {IRB.getInt32(libID), IRB.getInt32(CurFunID)});
   }

  }
	

//Record function lists for each library
   std::ofstream outfile;
   outfile.open("FunctionTable"+libname+".txt", std::ios_base::app);
   outfile << "Ext Visible:" << (F.getLinkage() == llvm::GlobalValue::ExternalLinkage) << ":" << libID << ":" << CurFunID << ":" << F.getName().data() << "\n";
   outfile.close();

//We want to only annotated funtions available in source code
   if(F.getLinkage() == llvm::GlobalValue::ExternalLinkage){
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
			ConstantFP::get(IRBRet.getContext(), APFloat( 1.0 )), 
			ConstantFP::get(IRBRet.getContext(), APFloat( LibProfData[libID].Funcs[CurFunID].TotalCycles)), 
			IRBRet.getInt32(libID), IRBRet.getInt32(CurFunID)});
    
		IRBRet.CreateCall(BATerminate, {IRBRet.getInt32(libID), IRBRet.getInt32(CurFunID)});  
	 	//The calling order in result code will be the same as the order of CreateCall
	  }
      }
	
   }else{
      IRBuilder<> IRB(F.getEntryBlock().getFirstNonPHI());
      if((Instru == "profbb"))
         IRB.CreateCall(PapiFuncEntry, {IRB.getInt32(libID), IRB.getInt32(CurFunID)});
      if((Instru == "prof") || (Instru == "preprof"))
         IRB.CreateCall(PapiFuncEntry, {IRB.getInt32(libID), IRB.getInt32(CurFunID)});
      for (auto RetInst : RetVec) {
         IRBuilder<> IRBRet(RetInst);
         if((Instru == "profbb"))
            IRBRet.CreateCall(PapiFuncExit, {IRBRet.getInt32(libID), IRBRet.getInt32(CurFunID)});  
      	  if((Instru == "prof") || (Instru == "preprof"))
      	      IRBRet.CreateCall(PapiFuncExit, {IRBRet.getInt32(libID), IRBRet.getInt32(CurFunID)});  
	  if(Instru == "annot" )
      	  	IRBRet.CreateCall(BAFuncDelay, {
			ConstantFP::get(IRBRet.getContext(), APFloat( 1.0 )), 
			ConstantFP::get(IRBRet.getContext(), APFloat( LibProfData[libID].Funcs[CurFunID].TotalCycles)), 
			IRBRet.getInt32(libID), IRBRet.getInt32(CurFunID)});  
   	}
   }
  } 




  return Res;
}



char FunctionInstr::ID = 0;
static RegisterPass<FunctionInstr> X("functionInstr", "functionInstr Pass", false, false);

