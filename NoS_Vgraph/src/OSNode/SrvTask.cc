
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
  int taskID = taskManager.getTaskID( sc_core::sc_get_current_process_handle());
  char img1[30];
  char img2[30];
  char recvd_img[30];
  char dptr[30];


  while(1){
            if((OSmodel -> flag_compute) == 0) {
			OSmodel-> os_port -> preWait(taskID, (OSmodel ->Blocking_taskID));
			if((OSmodel -> flag_compute)==0)
			   sc_core::wait((OSmodel->comp_sig));
			OSmodel->os_port->postWait(taskID);

            }
            if((OSmodel -> flag_compute)>0) (OSmodel -> flag_compute)--;
	    for(int i = 0; i < CLI_NUM; i++){
			if((CLI_OFFLOAD_LEVEL[schd[i]]) == 2){
			  //communication_time = communication_time  + sc_core::sc_time_stamp().value()-temp_time_recv;
			  //std::cout << "Updating vision graph ... ... ..." << communication_time << std::endl;

		            OSmodel -> os_port->timeWait( 
							imgPre_srv[recv_round_comp][i],
		                                        taskID
			    );
		            OSmodel -> os_port->statRecordApp( 
							imgPre_srv[recv_round_comp][i]
			    );
			    sprintf(recvd_img, "../input/Node%d/%d.jpg", schd[i], recv_round_comp);
			    sprintf(dptr, "../stage1/Node%d/%d.dat", schd[i], recv_round_comp);
			    std::cout<< " O-2 from cli"<<schd[i]<<" preprocessing in server: "<< recvd_img << dptr << std::endl;
			    preProcess(recvd_img, dptr);

			}

	    }


	    for(int i = 0; i < CLI_NUM; i++){
			for(int j = i+1; j < CLI_NUM; j++){
				if((CLI_OFFLOAD_LEVEL[schd[i]]) == 0){
					if( j==(i+1) ) {
					  continue;
					}		 
				}
				if((CLI_OFFLOAD_LEVEL[schd[j]]) == 0){
					if( i==((j+1)%CLI_NUM) ) {
					  continue;		
					}	 
				}


				//if (  (((i+1)%CLI_NUM)==j)  ||  (((j+1)%CLI_NUM)==i)  ) continue; //If they are next to each other, skip
				sprintf(img1, "../stage1/Node%d/%d.dat", (i+1), recv_round_comp);
				sprintf(img2, "../stage1/Node%d/%d.dat", (j+1), recv_round_comp);
				std::cout << "Matching: "<<img1<<" "<<img2<<" "<<":\n";
				featureMatch(img1, img2);
				OSmodel -> os_port->timeWait( 
								imgFeature_srv[recv_round_comp][i][j],
				                                taskID
				);	
				OSmodel -> os_port->statRecordApp( 
								imgFeature_srv[recv_round_comp][i][j]
				);	


			}
	    }
		  
            recv_round_comp++;
	    clock_gettime(CLOCK_REALTIME, &now);
	    double seconds = (double)((now.tv_sec+now.tv_nsec*1e-9) - (double)(tmstart.tv_sec+tmstart.tv_nsec*1e-9));
	    printf("wall time %fs\n", seconds/(4*CLI_NUM));

  }

}

/*
    clock_gettime(CLOCK_REALTIME, &tmstart);

    clock_gettime(CLOCK_REALTIME, &now);
    double seconds = (double)((now.tv_sec+now.tv_nsec*1e-9) - (double)(tmstart.tv_sec+tmstart.tv_nsec*1e-9));
    printf("wall time %fs\n", seconds);
*/


void recv_imgs(void *arg)
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
  char img1[30];
  char img2[30];
  char recvd_img[30];
  char dptr[30];

  int taskID = taskManager.getTaskID( sc_core::sc_get_current_process_handle());

  (OSmodel ->Blocking_taskID) = taskID;

  int push = 0;
  struct netbuf *buf;
  void *data;
  u16_t len;
  int ii;
  char *img = (char*)malloc( 4000000 );
  int img_ii = 0;

  while(1){

	//=====================================================
	//=====================collect data====================
	//=====================================================
	for(int i = 0; i < CLI_NUM; i++){
            srv_send(arg, schd[i]);
	    err = netconn_accept(ctxt, conn, &newconn);
	    if (err == ERR_OK) {
	      img_ii = 0;
	      while ((err = netconn_recv(ctxt, newconn, &buf)) == ERR_OK) {
		do {
		     netbuf_data(ctxt, buf, &data, &len);

		     for(ii=0;ii<(len);ii++)
			img[img_ii+ii]=((char*)data)[ii];
		     img_ii+=len;


		} while (netbuf_next(buf) >= 0);
		if((buf->p->flags) & PBUF_FLAG_PUSH){
		   if((CLI_OFFLOAD_LEVEL[schd[i]]) == 0){
			sprintf(recvd_img, "../stage1/Node0/Node%d/%d.dat",   schd[i], recv_round);
			dump_mem_to_file(img_ii, recvd_img, &img);

		   }
		   if((CLI_OFFLOAD_LEVEL[schd[i]]) == 1){
			sprintf(recvd_img, "../stage1/Node0/Node%d/%d.dat",   schd[i], recv_round);
			dump_mem_to_file(img_ii, recvd_img, &img);

		   }	
		   if((CLI_OFFLOAD_LEVEL[schd[i]]) == 2){
			sprintf(recvd_img, "../output/Node0/Node%d/%d.jpg",   schd[i], recv_round);
			dump_mem_to_file(img_ii, recvd_img, &img);

		   }
		}
		netbuf_delete(ctxt, buf);
	      }

	      netconn_close(ctxt, newconn);
	      netconn_delete(ctxt, newconn);

	    }
        }//for(int i = 0; i < CLI_NUM; i++)
	//=====================================================
	//=====================================================


	//std::cout << "==================="<<recv_round<<"======================" << sc_core::sc_time_stamp().value() << "======================"<<recv_round<<"====================" <<std::endl;
	//=====================================================
	//======================distr data=====================
	//=====================================================
	for(int i = 0; i < CLI_NUM; i++){


	  if((CLI_OFFLOAD_LEVEL[schd[i]]) == 0){
		srv_send_dat(arg, schd[(i+1)%CLI_NUM], schd[i], recv_round);
		err = netconn_accept(ctxt, conn, &newconn);
		      while ((err = netconn_recv(ctxt, newconn, &buf)) == ERR_OK) {
			do {
			     netbuf_data(ctxt, buf, &data, &len);

			     for(ii=0;ii<(len);ii++)
				img[img_ii+ii]=((char*)data)[ii];
			     img_ii+=len;


			} while (netbuf_next(buf) >= 0);
			netbuf_delete(ctxt, buf);
		      }
		      netconn_close(ctxt, newconn);
		      netconn_delete(ctxt, newconn);



	   }
	  
	  
	}//for(int i = 0; i < CLI_NUM; i++)
	//=====================================================
	//=====================================================
	//std::cout << "==================="<<recv_round<<"======================" << sc_core::sc_time_stamp().value() << "======================"<<recv_round<<"====================" <<std::endl;

	//=====================================================
	//======================calculate======================
	//=====================================================
	if((OSmodel->SRV_CORE_NUM)==2){

		(OSmodel -> flag_compute)++;
		std::cout << "2-s notifying "  << sc_core::sc_time_stamp().value() <<"  "<<(OSmodel -> flag_compute)<< std::endl;
		(OSmodel->comp_sig).notify(sc_core::SC_ZERO_TIME);


	}else{//OSmodel->SRV_CORE_NUM)==2
		for(int i = 0; i < CLI_NUM; i++){
			if((CLI_OFFLOAD_LEVEL[schd[i]]) == 2){
		            OSmodel -> os_port->timeWait( 
							imgPre_srv[recv_round][i],
		                                        taskID
			    );
			    OSmodel -> os_port->statRecordApp( 
							imgPre_srv[recv_round][i]
			    );
			    sprintf(recvd_img, "../input/Node%d/%d.jpg", schd[i], recv_round);
			    sprintf(dptr, "../stage1/Node%d/%d.dat", schd[i], recv_round);
			    std::cout<< " O-2 from cli"<<schd[i]<<" preprocessing in server: "<< recvd_img << dptr << std::endl;
			    preProcess(recvd_img, dptr);
			}
		}

		for(int i = 0; i < CLI_NUM; i++){
			for(int j = i+1; j < CLI_NUM; j++){
				if((CLI_OFFLOAD_LEVEL[schd[i]]) == 0){
					if( j==(i+1) ) {
					  continue;
					}		 
				}
				if((CLI_OFFLOAD_LEVEL[schd[j]]) == 0){
					if( i==((j+1)%CLI_NUM) ) {
					  continue;		
					}	 
				}


				//if (  (((i+1)%CLI_NUM)==j)  ||  (((j+1)%CLI_NUM)==i)  ) continue; //If they are next to each other, skip
				sprintf(img1, "../stage1/Node%d/%d.dat", (i+1), recv_round);
				sprintf(img2, "../stage1/Node%d/%d.dat", (j+1), recv_round);
				std::cout << "Matching: "<<img1<<" "<<img2<<" "<<":\n";
				featureMatch(img1, img2);
				OSmodel -> os_port->timeWait( 
								imgFeature_srv[recv_round][i][j],
				                                taskID
				);	
				OSmodel -> os_port->statRecordApp( 
								imgFeature_srv[recv_round][i][j]
				);	
			}
		}
	}//OSmodel->SRV_CORE_NUM)==2
	//=====================================================
	//=====================================================

	std::cout << "==================="<<recv_round<<"======================" << sc_core::sc_time_stamp().value() << "======================"<<recv_round<<"====================" <<std::endl;



	clock_gettime(CLOCK_REALTIME, &now);
	double seconds = (double)((now.tv_sec+now.tv_nsec*1e-9) - (double)(tmstart.tv_sec+tmstart.tv_nsec*1e-9));
	printf("wall time %fs\n", seconds/(4*CLI_NUM));

	recv_round++;	

  }//while(1)
}




int sc_main(int, char *[]){

    std::cout << "The sc_main should not be called ..." <<std::endl;
    return 0;
}


