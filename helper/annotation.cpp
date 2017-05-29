#include "annotation.h"
//#include "profile.h"
#define OS_MODEL_ANNOTATION
/*
long long cycles = 0;
long TotalBBs;
long FuncBBs[10][10000];
int CurFunID = 0;
int CurLibID = 0;
CallingTracker AnnotationTracker;
*/
extern "C"{

	void CountBB_BA(int LibID, int FunID){
		
		//if(LibID==12)
		//printf("CountBB_BA Lib %d Function %d \n", LibID, FunID );


		AnnotationCtxt* annotCtxt = 
			taskManager.getAnnotCtxt(sc_core::sc_get_current_process_handle());	

		if(annotCtxt==NULL){
			return ;
		} 

		   (annotCtxt->TotalBBs)++;
	}

	void RecordFuncBBs( int LibID, int FunID){
		//printf("RecordFuncBBs Lib %d Function %d \n", LibID, FunID );
		AnnotationCtxt* annotCtxt = 
			taskManager.getAnnotCtxt(sc_core::sc_get_current_process_handle());	


		if(annotCtxt==NULL){
			return ;
		} 


		if((annotCtxt->AnnotationTracker).NumFuncInExec!=0){
	//CallingStack.back();
			(annotCtxt->CurLibID) = (annotCtxt->AnnotationTracker).LibID[(annotCtxt->AnnotationTracker).NumFuncInExec-1];
			(annotCtxt->CurFunID) = (annotCtxt->AnnotationTracker).FunID[(annotCtxt->AnnotationTracker).NumFuncInExec-1];
			(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID] = 
				(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID] + (annotCtxt->TotalBBs); 	
		}
	//CallingStack.push_back(FunID);
		(annotCtxt->AnnotationTracker).LibID[(annotCtxt->AnnotationTracker).NumFuncInExec] = LibID;
		(annotCtxt->AnnotationTracker).FunID[(annotCtxt->AnnotationTracker).NumFuncInExec] = FunID;
		(annotCtxt->AnnotationTracker).NumFuncInExec++;
//		CurLibID = LibID;
//		CurFunID = FunID;
		(annotCtxt->TotalBBs) = 0;
		//printf("RecordFuncBBs Lib %d Function %d \n", LibID, FunID );		
	}

	void FunctionDelay(double alpha1, double alpha2, long long delay1, long long delay2, int LibID, int FunID){
		//printf("FunctionDelay Lib %d Function %d \n", LibID, FunID );
//		printf("%lld\n", delay);
		AnnotationCtxt* annotCtxt = 
			taskManager.getAnnotCtxt(sc_core::sc_get_current_process_handle());

		if(annotCtxt==NULL){
			return ;
		} 


//		printf("(annotCtxt->AnnotationTracker).NumFuncInExec: %d\n", (annotCtxt->AnnotationTracker).NumFuncInExec);
	//CallingStack.back();
		(annotCtxt->CurLibID) = (annotCtxt->AnnotationTracker).LibID[(annotCtxt->AnnotationTracker).NumFuncInExec-1];
		(annotCtxt->CurFunID) = (annotCtxt->AnnotationTracker).FunID[(annotCtxt->AnnotationTracker).NumFuncInExec-1];
//		printf("(annotCtxt->AnnotationTracker).NumFuncInExec: %d\n", (annotCtxt->AnnotationTracker).NumFuncInExec);
	//CallingStack.pop_back();
		(annotCtxt->AnnotationTracker).NumFuncInExec--;
//		printf("annotCtxt->CurLibID: %d\n", (annotCtxt->CurLibID));
//		printf("annotCtxt->CurFunID: %d\n", (annotCtxt->CurFunID));
		(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID] = 
			(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID] + (annotCtxt->TotalBBs); 	
		(annotCtxt->TotalBBs) = 0;
//		printf("Function alpha is %f\n", alpha);
	#ifdef OS_MODEL_ANNOTATION
		OSModelCtxt* OSmodel = taskManager.getTaskCtxt(sc_core::sc_get_current_process_handle());
		int taskID = taskManager.getTaskID(sc_core::sc_get_current_process_handle());






		if(OSmodel->NodeID==0){//Server Node
		   if(OSmodel->SRV_TYPE == 2){
			//RPi4
			if( ( (long)(alpha1*((double)(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID])) + delay1) < 0){
				OSmodel -> os_port->timeWait(   0,
						taskID);
			}else{
				OSmodel -> os_port->timeWait( //  0,
 					     1000000000000 * ( (double)((long)(alpha1*((double)(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID])) + delay1))/(2400000000), 
						taskID);
				OSmodel -> os_port->statRecordlwIP(1000000000000 * ( (double)((long)(alpha1*((double)(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID])) + delay1))/(2400000000));
			}
		   }else if(OSmodel->SRV_TYPE == 1){
			//RPi3
			if( ( (long)(alpha1*((double)(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID])) + delay1) < 0){
				OSmodel -> os_port->timeWait(   0,
						taskID);
			}else{
				OSmodel -> os_port->timeWait( //  0,
 					     1000000000000 * ( (double)((long)(alpha1*((double)(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID])) + delay1))/(1200000000), 
						taskID);
				OSmodel -> os_port->statRecordlwIP(1000000000000 * ( (double)((long)(alpha1*((double)(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID])) + delay1))/(1200000000));
			}
		   }else{
			//RPi0
			if( ( (long)(alpha2*((double)(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID])) + delay2) < 0){
				OSmodel -> os_port->timeWait(   0,
						taskID);
			}else{
				OSmodel -> os_port->timeWait( //  0,
 					     1000000000000 * ( (double)((long)(alpha2*((double)(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID])) + delay2))/(1000000000), 
						taskID);
				OSmodel -> os_port->statRecordlwIP(1000000000000 * ( (double)((long)(alpha2*((double)(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID])) + delay2))/(1000000000));
			}
		   }
		}
		else{//Client Node
		   if(OSmodel->CLI_TYPE == 2){
			//RPi4
			if( ( (long)(alpha1*((double)(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID])) + delay1) < 0){
				OSmodel -> os_port->timeWait(   0,
						taskID);
			}else{
				OSmodel -> os_port->timeWait( //  0,
 					     1000000000000 * ( (double)((long)(alpha1*((double)(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID])) + delay1))/(2400000000), 
						taskID);
				OSmodel -> os_port->statRecordlwIP(1000000000000 * ( (double)((long)(alpha1*((double)(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID])) + delay1))/(2400000000));
			}
		   }else if(OSmodel->CLI_TYPE == 1){
			//RPi3
			if( ( (long)(alpha1*((double)(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID])) + delay1) < 0){
				OSmodel -> os_port->timeWait(   0,
						taskID);
			}else{
				OSmodel -> os_port->timeWait( //  0,
 					     1000000000000 * ( (double)((long)(alpha1*((double)(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID])) + delay1))/(1200000000), 
						taskID);
				OSmodel -> os_port->statRecordlwIP(1000000000000 * ( (double)((long)(alpha1*((double)(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID])) + delay1))/(1200000000));
			}
		   }else{
			//RPi0
			if( ( (long)(alpha2*((double)(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID])) + delay2) < 0){
				OSmodel -> os_port->timeWait(   0,
						taskID);
			}else{
				OSmodel -> os_port->timeWait( //  0,
 					     1000000000000 * ( (double)((long)(alpha2*((double)(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID])) + delay2))/(1000000000), 
						taskID);
				OSmodel -> os_port->statRecordlwIP(1000000000000 * ( (double)((long)(alpha2*((double)(annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID])) + delay2))/(1000000000));
			}
		   }
		}


//	        if ( OSmodel->NodeID == 2 )
//			debug_flags = 0x00;
//		else
//			debug_flags = 0x00;

		( (annotCtxt->FuncBBs)[annotCtxt->CurLibID][annotCtxt->CurFunID] ) = 0;

	#endif	


	}

	void program_start_BA(int LibID, int FunID){
/*
		int ii;
		int jj;
		for(ii=0;ii<10;ii++)
		  for(jj=0;jj<10000;jj++){
			FuncBBs[ii][jj] = 0;	
		  }
		TotalBBs = 0;
		AnnotationTracker.NumFuncInExec=0;
*/
		RecordFuncBBs(LibID, FunID);
	}

	void program_end_BA(int LibID, int FunID){
//		printf("Total cycles is: %lld\n", cycles);
	}

}

