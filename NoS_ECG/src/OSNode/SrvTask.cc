
#include "AppTasks.h"



void tcpip_init_done(void *arg)
{
  LwipCntxt* ctxt = (LwipCntxt*)arg;

  netif_set_default(ctxt, netif_add(((LwipCntxt*)arg), &(ctxt->netif),&(ctxt->ipaddr), &(ctxt->netmask), &(ctxt->gw), NULL, tapif_init,
                  tcpip_input));
  netif_set_up(ctxt, &(ctxt->netif));

}





void computeTask(void *arg)
{
  OSModelCtxt* OSmodel = taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() );
  if(OSmodel->NodeID != 0){return;}




  char result[20];
  char detection[50];
  char segmentation[50];
  char extraction[50];
  char classification_dir[50];
	
  int taskID = taskManager.getTaskID( sc_core::sc_get_current_process_handle());

  OSmodel -> os_port -> timeWait(0, taskID);
  OSmodel -> os_port->syncGlobalTime(taskID);		
  //os_port->timeWait(0, taskID);
  //os_port->syncGlobalTime(taskID);
  while(1){
		if((OSmodel -> flag_compute)==0) {
			OSmodel-> os_port -> preWait(taskID,OSmodel-> Blocking_taskID);
			sc_core::wait(OSmodel->comp_sig);
			OSmodel->os_port->postWait(taskID);

		}
		if((OSmodel -> flag_compute)>0)  (OSmodel -> flag_compute)--;
		std::cout << "2-s compute recv_round: "   <<  recv_round  <<" time is: " << sc_core::sc_time_stamp().value() << std::endl;
		for(int i = 0; i < CLI_NUM; i++){
			//printf("../output/Node%d\n", (i+1));
			sprintf(result, "../output/Node%d", (i+1));
			sprintf(detection, "%s/detection%d", result, recv_round);
			sprintf(segmentation, "%s/segmentation%d", result, recv_round);
			sprintf(extraction, "%s/extraction%d", result, recv_round);
			sprintf(classification_dir, "%s/classification%d", result, recv_round);

			if((CLI_OFFLOAD_LEVEL[schd[i]]) == 0){
			  std::cout<< " O-0 from cli"<<schd[i]<<std::endl;
			  //No offloading
			  //OSmodel -> os_port -> timeWait(10000000000, taskID);
			  //OSmodel -> os_port->syncGlobalTime(taskID);	
			}
			if((CLI_OFFLOAD_LEVEL[schd[i]]) == 1){
			  std::cout<< " O-1 from cli"<<schd[i]<<std::endl;
			  OSmodel -> os_port -> timeWait(stage3_srv[recv_round-1], taskID);
			  OSmodel -> os_port->statRecordApp(stage3_srv[recv_round-1]);	
			  classification(extraction, classification_dir, detection, result);
			 // OSmodel -> os_port->syncGlobalTime(taskID);	
			}
			if((CLI_OFFLOAD_LEVEL[schd[i]]) == 2){
			  std::cout<< " O-2 from cli"<<schd[i]<<std::endl;
			  OSmodel -> os_port -> timeWait(stage2_srv[recv_round-1], taskID);
			  OSmodel -> os_port -> timeWait(stage3_srv[recv_round-1], taskID);	
			  OSmodel -> os_port->statRecordApp(stage2_srv[recv_round-1]+stage3_srv[recv_round-1]);
			  feature_extraction(segmentation, extraction, detection);
			  //OSmodel -> os_port->syncGlobalTime(taskID);	
			  classification(extraction, classification_dir, detection, result);
			  //OSmodel -> os_port->syncGlobalTime(taskID);	
			}

			if((CLI_OFFLOAD_LEVEL[schd[i]]) == 3){
			  std::cout<< " O-3 from cli"<<schd[i]<<std::endl;
			  OSmodel -> os_port -> timeWait(stage1_srv[recv_round-1], taskID);
			  OSmodel -> os_port -> timeWait(stage2_srv[recv_round-1], taskID);
			  OSmodel -> os_port -> timeWait(stage3_srv[recv_round-1], taskID);
			  OSmodel -> os_port->statRecordApp(stage1_srv[recv_round-1]+stage2_srv[recv_round-1]+stage3_srv[recv_round-1]);	
			  heartbeat_segmentation(detection, segmentation,  detection);
			  //OSmodel -> os_port->syncGlobalTime(taskID);	
			  feature_extraction(segmentation, extraction, detection);
			  //OSmodel -> os_port->syncGlobalTime(taskID);	
			  classification(extraction, classification_dir, detection, result);

			}

		}
	        OSmodel -> os_port->syncGlobalTime(taskID);	
		//std::cout << "finish recv_round: "   <<  recv_round  <<" time is: " << sc_core::sc_time_stamp().value() << std::endl;
		recv_round++;


  }


}



/*
    clock_gettime(CLOCK_REALTIME, &tmstart);

    clock_gettime(CLOCK_REALTIME, &now);
    double seconds = (double)((now.tv_sec+now.tv_nsec*1e-9) - (double)(tmstart.tv_sec+tmstart.tv_nsec*1e-9));
    printf("wall time %fs\n", seconds);
*/



void recv_dats(void *arg)
{


  clock_gettime(CLOCK_REALTIME, &tmstart);
  OSModelCtxt* OSmodel = taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() );
  if(OSmodel->NodeID != 0){return;}

  //srv_avail(arg, 1);

  char this_str[16];
  ipaddr_ntoa_r(&(((LwipCntxt* )arg)->ipaddr_dest), this_str, 16);
  printf("server ip is %s\n", this_str);

  struct netconn *conn, *newconn;
  err_t err;
  LwipCntxt *ctxt = (LwipCntxt *)arg;
  conn = netconn_new_with_proto_and_callback(ctxt, NETCONN_TCP, 0, NULL);
  /* Bind connection to well known port number 7. */
  netconn_bind(ctxt, conn, NULL, 7);
  /* Tell connection to go into listening mode. */
  netconn_listen(ctxt, conn);






  char result[20];
  char detection[50];
  char segmentation[50];
  char extraction[50];
  char classification_dir[50];
			



  int taskID = taskManager.getTaskID( sc_core::sc_get_current_process_handle());
  int push = 0;
  OSmodel->Blocking_taskID = taskID;


  while(1){
    if( (push!=0 )&& ((push%CLI_NUM) ==0)) 
	{
		//std::cout << "notify recv_round: "   <<  recv_round  <<" time is: " << sc_core::sc_time_stamp().value() << std::endl;

	    if((OSmodel->SRV_CORE_NUM)==2){
		(OSmodel -> comp_sig).notify(sc_core::SC_ZERO_TIME);
		(OSmodel -> flag_compute)++;
	    }
	    else{
		std::cout << "1-s notify recv_round: "   <<  recv_round  <<" time is: " << sc_core::sc_time_stamp().value() << std::endl;
		for(int i = 0; i < CLI_NUM; i++){
			//printf("../output/Node%d\n", (i+1));

			sprintf(result, "../output/Node%d", (i+1));
			sprintf(detection, "%s/detection%d", result, recv_round);
			sprintf(segmentation, "%s/segmentation%d", result, recv_round);
			sprintf(extraction, "%s/extraction%d", result, recv_round);
			sprintf(classification_dir, "%s/classification%d", result, recv_round);

			if((CLI_OFFLOAD_LEVEL[schd[i]]) == 0){
			  //No offloading		
		          std::cout << "Offloading level is 0 ... ... ... ... ... ..." << std::endl;	
			}
			if((CLI_OFFLOAD_LEVEL[schd[i]]) == 1){
		          std::cout << "Offloading level is 1 ... ... ... ... ... ..." << std::endl;
			  OSmodel -> os_port -> timeWait(stage3_srv[recv_round-1], taskID);			
			  OSmodel -> os_port -> statRecordApp(stage3_srv[recv_round-1]);	
			  classification(extraction, classification_dir, detection, result);
			}
			if((CLI_OFFLOAD_LEVEL[schd[i]]) == 2){
		          std::cout << "Offloading level is 2 ... ... ... ... ... ..." << std::endl;
			  OSmodel -> os_port -> timeWait(stage2_srv[recv_round-1], taskID);
			  OSmodel -> os_port -> timeWait(stage3_srv[recv_round-1], taskID);
			  OSmodel -> os_port->statRecordApp(stage2_srv[recv_round-1]+stage3_srv[recv_round-1]);		
			  feature_extraction(segmentation, extraction, detection);
			  classification(extraction, classification_dir, detection, result);
			}

			if((CLI_OFFLOAD_LEVEL[schd[i]]) == 3){
		          std::cout << "Offloading level is 3 ... ... ... ... ... ..." << std::endl;
			  OSmodel -> os_port -> timeWait(stage1_srv[recv_round-1], taskID);
			  OSmodel -> os_port -> timeWait(stage2_srv[recv_round-1], taskID);
			  OSmodel -> os_port -> timeWait(stage3_srv[recv_round-1], taskID);
			  OSmodel -> os_port->statRecordApp(stage1_srv[recv_round-1]+stage2_srv[recv_round-1]+stage3_srv[recv_round-1]);		
			  heartbeat_segmentation(detection, segmentation,  detection);
			  feature_extraction(segmentation, extraction, detection);
			  classification(extraction, classification_dir, detection, result);
			}

		}
		   //OSmodel -> os_port -> timeWait(task[recv_round], taskID);	
		recv_round++;
	    }


	    clock_gettime(CLOCK_REALTIME, &now);
	    double seconds = (double)((now.tv_sec+now.tv_nsec*1e-9) - (double)(tmstart.tv_sec+tmstart.tv_nsec*1e-9));
	    printf("wall time %fs\n", seconds/(20*CLI_NUM));
		//printf("recv_round %d\n", recv_round);
		//sc_core::wait(10, SIM_RESOLUTION);
 	}

    srv_send(arg, schd[push%CLI_NUM]);


    err = netconn_accept(ctxt, conn, &newconn);

    if (err == ERR_OK) {
      struct netbuf *buf;
      void *data;
      u16_t len;
      int ii;
      char *img = (char*)malloc( 4000000 );
      int img_ii = 0;
      while ((err = netconn_recv(ctxt, newconn, &buf)) == ERR_OK) {
        do {
             netbuf_data(ctxt, buf, &data, &len);

             for(ii=0;ii<(len);ii++)
		img[img_ii+ii]=((char*)data)[ii];
             img_ii+=len;


        } while (netbuf_next(buf) >= 0);
        if((buf->p->flags) & PBUF_FLAG_PUSH){
		free(img);
		push++; 

	}
        netbuf_delete(ctxt, buf);
      }
      netconn_close(ctxt, newconn);
      netconn_delete(ctxt, newconn);

    }
    //std::cout << "============netconn accepted============= " <<  OSmodel->NodeID  <<"... ..." << sc_core::sc_time_stamp().value() << std::endl;

    //std::cout<<"Cli commu time is: " << sc_core::sc_time_stamp().value() - cli_commu[schd[(push-1)%CLI_NUM]]<<" cliID "<<schd[(push-1)%CLI_NUM]<<"\n";	
    cli_commu[schd[(push-1)%CLI_NUM]] = sc_core::sc_time_stamp().value() - cli_commu[0];
    //std::cout<<"Cli commu time is: " << cli_commu[schd[(push-1)%CLI_NUM]]<<" cliID "<<schd[(push-1)%CLI_NUM]<<"\n";	
  }
  //std::cout<<"Total time is: " << sc_core::sc_time_stamp().value() ;	
/*
  std::cout<<OSmodel -> NodeID <<"   ";	
  OSmodel -> os_port -> statPrint();	
*/
}



int sc_main(int, char *[]){

    std::cout << "The sc_main should not be called ..." <<std::endl;
    return 0;
}

