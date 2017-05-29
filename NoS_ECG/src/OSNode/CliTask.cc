
#include "AppTasks.h"





void send_dat(void *arg, int stage, int numberof_rpeaks, int beat_count)
{

  char result[20];
  char detection[50];
  char segmentation[50];
  char extraction[50];
  char classification_dir[50];
  char dat_file[50];   			



  struct netconn *conn;
  err_t err;
  LwipCntxt *ctxt = (LwipCntxt *)arg;


  /* Create a new connection identifier. */

  conn = netconn_new(ctxt, NETCONN_TCP);

  /* Bind connection to well known port number 7. */

  //char this_str[16];
  //ipaddr_ntoa_r(&(((LwipCntxt* )ctxt)->ipaddr_dest), this_str, 16);

  sprintf(result, "../output/Node%d", (((LwipCntxt* )ctxt)->NodeID));

  sprintf(detection, "%s/detection%d", result, beat_count);
  sprintf(segmentation, "%s/segmentation%d", result, beat_count);
  sprintf(extraction, "%s/extraction%d", result, beat_count);
  sprintf(classification_dir, "%s/classification%d", result, beat_count);


//dat from detection stage:
  sprintf(dat_file, "%s/sig%d.dat", detection, 1);
  sprintf(dat_file, "%s/rpeak%d.dat", detection, 1);
  sprintf(dat_file, "%s/numberof_rpeaks.dat", detection);
//dat from segmentation stage:
  sprintf(dat_file, "%s/beat%d.dat", segmentation,  1);
//dat from extraction stage:
  sprintf(dat_file, "%s/coef%d.dat", extraction, 1);


  unsigned int buf_size = 0;
  char* buf;

  if(stage==3)
	  for(int iter=0; iter<numberof_rpeaks; iter++){
		 sprintf(dat_file, "%s/sig%d.dat", detection, iter);
	 	 buf_size += load_file_to_memory(dat_file, &buf);  
		 free(buf);
		 sprintf(dat_file, "%s/rpeak%d.dat", detection, iter);
	 	 buf_size += load_file_to_memory(dat_file, &buf);  
		 free(buf);
	  }
  else if(stage==2){

	  for(int iter=0; iter<numberof_rpeaks; iter++){
		 sprintf(dat_file, "%s/beat%d.dat", segmentation,  iter);
	 	 buf_size += load_file_to_memory(dat_file, &buf);  
		 free(buf);

	  }
  }
  else if(stage==1)
	  for(int iter=0; iter<numberof_rpeaks; iter++){
		 sprintf(dat_file, "%s/coef%d.dat", extraction, iter);
		 printf("%s\n", dat_file);
	 	 buf_size += load_file_to_memory(dat_file, &buf);  
		 free(buf);

	  }
  else {
	//No offload	
	
	}

  buf = (char*)malloc( buf_size+4 );
  //printf("Size is %d \n", buf_size);



  err = netconn_connect(ctxt, conn, &(((LwipCntxt* )ctxt)->ipaddr_dest), 7);

  //std::cout << "Connected in node " << (((LwipCntxt* )ctxt)->NodeID) << " connect to "<< this_str << std::endl;
  if (err != ERR_OK) {
    std::cout << "Connection refused" << std::endl;
    return;
  }


  size_t *bytes_written=NULL; 
/*
  char dat[30];   
  sprintf(dat, "send data %d", ((LwipCntxt* )ctxt)->NodeID);//large
*/
  //std::cout << "Before netconn_write_partly: "<< (((LwipCntxt* )ctxt)->NodeID)<<"  "<<buf_size<<" time: " << sc_core::sc_time_stamp().value() << std::endl;
  while (1) {


    err = netconn_write_partly(ctxt, conn, buf, (buf_size+4), NETCONN_COPY, bytes_written);
    //err = netconn_write_partly(ctxt, conn, dat, 30, NETCONN_COPY, bytes_written);
    if (err == ERR_OK) 
    {
      netconn_close(ctxt, conn);
      netconn_delete(ctxt, conn);
      break;
    }
  }
  //std::cout << "After netconn_write_partly: "<< (((LwipCntxt* )ctxt)->NodeID)<<"  "<<buf_size<<" time: " << sc_core::sc_time_stamp().value() << std::endl;
  free(buf);
}


void send_dats(void *arg)
{

  if( (((LwipCntxt*)(arg))->NodeID) == 0){
	return;
  }

  LwipCntxt *ctxt = (LwipCntxt *)arg;
  OSModelCtxt* OSmodel = taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() );

  int taskID = taskManager.getTaskID( sc_core::sc_get_current_process_handle());
  //int total_imgs = (((LwipCntxt*)(arg))->NodeID);  //sent images to lower indexed nodes

  ECG_init();




  char result[20];
  sprintf(result, "../output/Node%d", (((LwipCntxt*)(arg))->NodeID));

//Prepare for the ECG processing 
  int gain, baseline, i, j;
  char line[80];
  double rp;
  FILE *fp;
  double *full_sig;
  gain = 200;
  baseline = 1024;
  ECG_ctxt ecg_ctxt;
  ecg_ctxt.learning = 1;
  full_sig = (double *)malloc((input_signal_window)*sizeof(double));
  FILE *fp_result;
  char result_file[50];
  sprintf(result_file, "%s/out.log", result);
  fp_result = fopen(result_file,"w"); 
  fclose(fp_result);
  fp = fopen(data_record,"r"); 
  if (fp == 0) printf("Error: could not open data file.\n");





  int total_heartbeat=HEARTBEAT_NUM;
  int beat_count=0;
  char detection[50];
  char segmentation[50];
  char extraction[50];
  char classification_dir[50];

  int numberof_rpeaks=1;


  i=0;
  for (j=0; j<no_of_input_windows*input_signal_window; j++) {
		if(j==(total_heartbeat*input_signal_window)) break;
		fgets(line,80,fp);
		sscanf(line,"%lf", &rp);
		full_sig[i] = (rp - baseline)/gain;
		i = i + 1;
		if (i==input_signal_window) {
 

			sprintf(detection, "%s/detection%d", result, beat_count);
			sprintf(segmentation, "%s/segmentation%d", result, beat_count);
			sprintf(extraction, "%s/extraction%d", result, beat_count);
			sprintf(classification_dir, "%s/classification%d", result, beat_count);
			beat_count++;
			cli_wait(arg);//wait for server to be available
			IP4_ADDR(&(((LwipCntxt*)(ctxt))->ipaddr_dest), 192, 168, 0, (2));
			send_dat(arg, (OSmodel->OFFLOAD_LEVEL), numberof_rpeaks, beat_count);

			if((OSmodel->OFFLOAD_LEVEL) == 0){
			  std::cout<< " O-0 in cli"<<((LwipCntxt*)(arg))->NodeID<< "\n";
			  if((OSmodel->CLI_CORE_NUM)==2){
		  	 	 if((stage0_cli[beat_count-1]+stage1_cli[beat_count-1]+stage2_cli[beat_count-1]+stage3_cli[beat_count-1]) > cli_commu[(((LwipCntxt*)(arg))->NodeID)] ){
					OSmodel -> os_port -> timeWait(
					(stage0_cli[beat_count-1]+stage1_cli[beat_count-1]+stage2_cli[beat_count-1]+stage3_cli[beat_count-1]-cli_commu[(((LwipCntxt*)(arg))->NodeID)]), taskID);
					OSmodel -> os_port->incBusyTime(1, cli_commu[(((LwipCntxt*)(arg))->NodeID)] );			
				 }
			  	 else{
					OSmodel -> os_port -> timeWait(0, taskID);
			  	        OSmodel -> os_port->incBusyTime(1, stage0_cli[beat_count-1]+stage1_cli[beat_count-1]+stage2_cli[beat_count-1]+stage3_cli[beat_count-1]);		
				 }
			  }
			  else{
			  	OSmodel -> os_port -> timeWait(stage0_cli[beat_count-1], taskID);
			  	OSmodel -> os_port -> timeWait(stage1_cli[beat_count-1], taskID);
			  	OSmodel -> os_port -> timeWait(stage2_cli[beat_count-1], taskID);
			  	OSmodel -> os_port -> timeWait(stage3_cli[beat_count-1], taskID);

			  }
			  OSmodel -> os_port->statRecordApp(stage0_cli[beat_count-1]+stage1_cli[beat_count-1]+stage2_cli[beat_count-1]+stage3_cli[beat_count-1]);		
			  heartbeat_detection(full_sig, &ecg_ctxt, detection,detection,detection, &numberof_rpeaks);
			  heartbeat_segmentation(detection, segmentation,  detection);
			  feature_extraction(segmentation, extraction, detection);
			  classification(extraction, classification_dir, detection, result);

			}
			if((OSmodel->OFFLOAD_LEVEL) == 1){
			  std::cout<< " O-1 in cli"<<((LwipCntxt*)(arg))->NodeID<< "\n";

			  //std::cout<<"Last cli commu time is: " << cli_commu[(((LwipCntxt*)(arg))->NodeID)]<<"\n";
			  if((OSmodel->CLI_CORE_NUM)==2){
				  //std::cout<<"Cli "<< (((LwipCntxt*)(arg))->NodeID) << " commu time is: " << cli_commu[(((LwipCntxt*)(arg))->NodeID)] <<"\n";	
				  //std::cout<<"Cli "<< (((LwipCntxt*)(arg))->NodeID) << " comp time is: " << (stage0_cli[beat_count-1]+stage1_cli[beat_count-1]+stage2_cli[beat_count-1]) <<"\n";	
				  if((stage0_cli[beat_count-1]+stage1_cli[beat_count-1]+stage2_cli[beat_count-1]) > cli_commu[(((LwipCntxt*)(arg))->NodeID)] ){
					OSmodel -> os_port -> timeWait((stage0_cli[beat_count-1]+stage1_cli[beat_count-1]+stage2_cli[beat_count-1]-cli_commu[(((LwipCntxt*)(arg))->NodeID)]), taskID);
			  	        OSmodel -> os_port->incBusyTime(1, cli_commu[(((LwipCntxt*)(arg))->NodeID)] );		
				  }
				  else{
					OSmodel -> os_port -> timeWait(0, taskID);
			  	        OSmodel -> os_port->incBusyTime(1, stage0_cli[beat_count-1]+stage1_cli[beat_count-1]+stage2_cli[beat_count-1]);		
				  }
			  }
			  else{
				  OSmodel -> os_port -> timeWait(stage0_cli[beat_count-1], taskID);
				  OSmodel -> os_port -> timeWait(stage1_cli[beat_count-1], taskID);
				  OSmodel -> os_port -> timeWait(stage2_cli[beat_count-1], taskID);

			  }
			  OSmodel -> os_port->statRecordApp(stage0_cli[beat_count-1]+stage1_cli[beat_count-1]+stage2_cli[beat_count-1]);		
			  heartbeat_detection(full_sig, &ecg_ctxt, detection,detection,detection, &numberof_rpeaks);
			  heartbeat_segmentation(detection, segmentation,  detection);
			  feature_extraction(segmentation, extraction, detection);

			}
			if((OSmodel->OFFLOAD_LEVEL) == 2){
			  std::cout<< " O-2 in cli"<<((LwipCntxt*)(arg))->NodeID<< "\n";	
			  if((OSmodel->CLI_CORE_NUM)==2){
				  //std::cout<<"Cli "<< (((LwipCntxt*)(arg))->NodeID) << " commu time is: " << cli_commu[(((LwipCntxt*)(arg))->NodeID)] <<"\n";	
				  //std::cout<<"Cli "<< (((LwipCntxt*)(arg))->NodeID) << " comp time is: " << (stage0_cli[beat_count-1]+stage1_cli[beat_count-1]) <<"\n";	
				  //std::cout<<"Last cli commu time is: " << cli_commu[(((LwipCntxt*)(arg))->NodeID)]<<"\n";
				  if((stage0_cli[beat_count-1]+stage1_cli[beat_count-1]) > cli_commu[(((LwipCntxt*)(arg))->NodeID)] ){
					OSmodel -> os_port -> timeWait((stage0_cli[beat_count-1]+stage1_cli[beat_count-1]-cli_commu[(((LwipCntxt*)(arg))->NodeID)]), taskID);
			  	        OSmodel -> os_port->incBusyTime(1, cli_commu[(((LwipCntxt*)(arg))->NodeID)] );		
				  }
				  else{
					OSmodel -> os_port -> timeWait(0, taskID);
			  	  	OSmodel -> os_port->incBusyTime(1, stage0_cli[beat_count-1]+stage1_cli[beat_count-1]);		
				  }
			  }
			  else{
				  OSmodel -> os_port -> timeWait(stage0_cli[beat_count-1], taskID);
				  OSmodel -> os_port -> timeWait(stage1_cli[beat_count-1], taskID);

			  }
			  OSmodel -> os_port->statRecordApp(stage0_cli[beat_count-1]+stage1_cli[beat_count-1]);		
			  heartbeat_detection(full_sig, &ecg_ctxt, detection,detection,detection, &numberof_rpeaks);
			  heartbeat_segmentation(detection, segmentation,  detection);

			}
			if((OSmodel->OFFLOAD_LEVEL) == 3){	
		  	  //std::cout<<"Last cli commu time is: " << cli_commu[(((LwipCntxt*)(arg))->NodeID)]<<"\n";
			  std::cout<< " O-3 in cli"<<((LwipCntxt*)(arg))->NodeID<< "\n";
			  if((OSmodel->CLI_CORE_NUM)==2){
				  //std::cout<<"Cli "<< (((LwipCntxt*)(arg))->NodeID) << " commu time is: " << cli_commu[(((LwipCntxt*)(arg))->NodeID)] <<"\n";	
				  //std::cout<<"Cli "<< (((LwipCntxt*)(arg))->NodeID) << " comp time is: " << (stage0_cli[beat_count-1]+stage1_cli[beat_count-1]) <<"\n";
				  if((stage0_cli[beat_count-1]) > cli_commu[(((LwipCntxt*)(arg))->NodeID)] ){
					OSmodel -> os_port -> timeWait((stage0_cli[beat_count-1]-cli_commu[(((LwipCntxt*)(arg))->NodeID)]), taskID);
			  	        OSmodel -> os_port->incBusyTime(1, cli_commu[(((LwipCntxt*)(arg))->NodeID)] );		
				  }
				  else{
					OSmodel -> os_port -> timeWait(0, taskID);
			  	        OSmodel -> os_port->incBusyTime(1, stage0_cli[beat_count-1]);		
				  }
			  }
			  else{
				  OSmodel -> os_port -> timeWait(stage0_cli[beat_count-1], taskID);

			  }
			  OSmodel -> os_port->statRecordApp(stage0_cli[beat_count-1]);		
			  heartbeat_detection(full_sig, &ecg_ctxt, detection,detection,detection, &numberof_rpeaks);

			}


			i=0;
		}
  }


  fclose(fp);
  free(full_sig);    

}





