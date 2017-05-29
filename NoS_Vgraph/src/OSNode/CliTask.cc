
#include "AppTasks.h"



void send_img(void *arg, int image_count)
{

  struct netconn *conn;
  err_t err;

  LwipCntxt *ctxt = (LwipCntxt *)arg;


  /* Create a new connection identifier. */

  conn = netconn_new(ctxt, NETCONN_TCP);

  /* Bind connection to well known port number 7. */
  //std::cout << "Before Connection: "<< (((LwipCntxt* )ctxt)->NodeID)<<" time: " << sc_core::sc_time_stamp().value() << std::endl;
  //char this_str[16];
  //ipaddr_ntoa_r(&(((LwipCntxt* )ctxt)->ipaddr_dest), this_str, 16);


  err = netconn_connect(ctxt, conn, &(((LwipCntxt* )ctxt)->ipaddr_dest), 7);
  //std::cout << "Connected in node " << (((LwipCntxt* )ctxt)->NodeID) << " connect to "<< this_str << std::endl;
  if (err != ERR_OK) {
    std::cout << "Connection refused" << std::endl;
    return;
  }


  size_t *bytes_written=NULL;
  unsigned int buf_size;
  char* buf;
  char local_img[30];   
  sprintf(local_img, "../input/Node%d/%d.jpg", ((LwipCntxt* )ctxt)->NodeID, image_count);//large


  buf_size = load_file_to_memory(local_img, &buf);
  while (1) {
    err = netconn_write_partly(ctxt, conn, buf, buf_size, NETCONN_COPY, bytes_written);
    if (err == ERR_OK) 
    {
      netconn_close(ctxt, conn);
      netconn_delete(ctxt, conn);
      break;
    }
  }

}


void send_dat(void *arg, int image_count)
{

  struct netconn *conn;
  err_t err;

  LwipCntxt *ctxt = (LwipCntxt *)arg;


  /* Create a new connection identifier. */

  conn = netconn_new(ctxt, NETCONN_TCP);

  /* Bind connection to well known port number 7. */
 // std::cout << "Before Connection: "<< (((LwipCntxt* )ctxt)->NodeID)<<" time: " << sc_core::sc_time_stamp().value() << std::endl;
  //char this_str[16];
  //ipaddr_ntoa_r(&(((LwipCntxt* )ctxt)->ipaddr_dest), this_str, 16);


  err = netconn_connect(ctxt, conn, &(((LwipCntxt* )ctxt)->ipaddr_dest), 7);
  //std::cout << "Connected in node " << (((LwipCntxt* )ctxt)->NodeID) << " connect to "<< this_str << std::endl;
  if (err != ERR_OK) {
    std::cout << "Connection refused" << std::endl;
    return;
  }


  size_t *bytes_written=NULL;
  unsigned int buf_size;
  char* buf;
  char local_img[30];   
  sprintf(local_img, "../stage1/Node%d/%d.dat", ((LwipCntxt* )ctxt)->NodeID, image_count);//large


  buf_size = load_file_to_memory(local_img, &buf);
  while (1) {
    err = netconn_write_partly(ctxt, conn, buf, buf_size, NETCONN_COPY, bytes_written);
    if (err == ERR_OK) 
    {
      netconn_close(ctxt, conn);
      netconn_delete(ctxt, conn);
      break;
    }
  }

}









void send_imgs(void *arg)
{
  if( (((LwipCntxt*)(arg))->NodeID) == 0){
	return;
  }

  LwipCntxt *ctxt = (LwipCntxt *)arg;
  OSModelCtxt* OSmodel = taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() );
  int taskID = taskManager.getTaskID( sc_core::sc_get_current_process_handle());
  int total_imgs = (((LwipCntxt*)(arg))->NodeID);  //sent images to lower indexed nodes

  char img[30];
  char dptr[30];
  char img1[30];
  char img2[30];
  for(int i = 0; i < IMG_NUM; i++){
	  if((OSmodel->OFFLOAD_LEVEL) != 0)
		{

	  		cli_wait(arg);//wait for server to be available
		}
	  if((OSmodel->OFFLOAD_LEVEL) == 2){
	  	IP4_ADDR(&(((LwipCntxt*)(ctxt))->ipaddr_dest), 192, 168, 0, (2));

		std::cout<< " O-2 in cli"<<((LwipCntxt*)(arg))->NodeID<< "\n";
	  	send_img(arg, i);		

	  }
	  if((OSmodel->OFFLOAD_LEVEL) == 0){

	  	cli_wait(arg);//wait for server to be available



	  	IP4_ADDR(&(((LwipCntxt*)(ctxt))->ipaddr_dest), 192, 168, 0, (2));

		send_dat(arg, i);
		cli_commu[(((LwipCntxt*)(arg))->NodeID)] = sc_core::sc_time_stamp().value() - cli_commu[0];

		sprintf(img, "../input/Node%d/%d.jpg", ((LwipCntxt*)(arg))->NodeID, i);
		sprintf(dptr, "../stage1/Node%d/%d.dat", ((LwipCntxt*)(arg))->NodeID, i);
		std::cout<< " O-0 in cli"<<((LwipCntxt*)(arg))->NodeID<<" preprocessing in client: "<< img << dptr << std::endl;

		preProcess(img, dptr);
		//std::cout<<"Last cli commu time is: " << cli_commu[(((LwipCntxt*)(arg))->NodeID)]<<"\n";
		  //preProcess(img, dptr);
		if((OSmodel->CLI_CORE_NUM)==2){
			std::cout<<"2-c";
			if(imgPre_cli[i][((LwipCntxt*)(arg))->NodeID-1] >cli_commu[(((LwipCntxt*)(arg))->NodeID)]  ){
		        	OSmodel -> os_port->timeWait( 
							imgPre_cli[i][((LwipCntxt*)(arg))->NodeID-1] -cli_commu[(((LwipCntxt*)(arg))->NodeID)] ,
		                                        taskID
				);
			        OSmodel -> os_port->incBusyTime(1,
						cli_commu[(((LwipCntxt*)(arg))->NodeID)]
				);	
			}
			else
			{
		        	OSmodel -> os_port->timeWait( 
							0,
		                                        taskID
				);	
			        OSmodel -> os_port->incBusyTime(1,
							imgPre_cli[i][((LwipCntxt*)(arg))->NodeID-1]
				);	
			}
		}else{


		
		        OSmodel -> os_port->timeWait( 
							imgPre_cli[i][((LwipCntxt*)(arg))->NodeID-1],
		                                        taskID
			);

		}

	        OSmodel -> os_port->statRecordApp( 
				imgPre_cli[i][((LwipCntxt*)(arg))->NodeID-1]
		);

	  	cli_wait(arg);//wait for server to be available

		cli_send(arg, 0);
		cli_commu[(((LwipCntxt*)(arg))->NodeID)] = sc_core::sc_time_stamp().value() - cli_commu[0];
		//std::cout<<"Last cli commu time is: " << cli_commu[(((LwipCntxt*)(arg))->NodeID)]<<"\n";

		sprintf(img1, "../stage1/Node%d/%d.dat", schd[((LwipCntxt*)(arg))->NodeID-1], i);
		sprintf(img2, "../stage1/Node%d/%d.dat", schd[(((LwipCntxt*)(arg))->NodeID)%CLI_NUM], i);
		std::cout<< " O-0 in cli"<<((LwipCntxt*)(arg))->NodeID<< " matching in client: "<<img1<<" "<<img2<<" "<<":\n";
		featureMatch(img1, img2);

		if((OSmodel->CLI_CORE_NUM)==2){
			if(imgFeature_cli[i][((LwipCntxt*)(arg))->NodeID-1][(((LwipCntxt*)(arg))->NodeID)%CLI_NUM] >cli_commu[(((LwipCntxt*)(arg))->NodeID)]  ){
		        	OSmodel -> os_port->timeWait( 
				imgFeature_cli[i][((LwipCntxt*)(arg))->NodeID-1][(((LwipCntxt*)(arg))->NodeID)%CLI_NUM] -cli_commu[(((LwipCntxt*)(arg))->NodeID)] , 
		                taskID
				);
				OSmodel -> os_port->incBusyTime(1,
					cli_commu[(((LwipCntxt*)(arg))->NodeID)]
				);	
			}
			else
			{
		        	OSmodel -> os_port->timeWait( 0, taskID);
				OSmodel -> os_port->incBusyTime(1,
					imgFeature_cli[i][((LwipCntxt*)(arg))->NodeID-1][(((LwipCntxt*)(arg))->NodeID)%CLI_NUM]
				);		
			}

	  	}else{

			OSmodel -> os_port->timeWait( 
					imgFeature_cli[i][((LwipCntxt*)(arg))->NodeID-1][(((LwipCntxt*)(arg))->NodeID)%CLI_NUM],
					taskID
			);


		}
		OSmodel -> os_port->statRecordApp( 
					imgFeature_cli[i][((LwipCntxt*)(arg))->NodeID-1][(((LwipCntxt*)(arg))->NodeID)%CLI_NUM]
		);

	  }
	  if((OSmodel->OFFLOAD_LEVEL) == 1){
	  	//cli_send(arg, 0);//wait for server to be available
	  	IP4_ADDR(&(((LwipCntxt*)(ctxt))->ipaddr_dest), 192, 168, 0, (2));

		send_dat(arg, i);
		cli_commu[(((LwipCntxt*)(arg))->NodeID)] = sc_core::sc_time_stamp().value() - cli_commu[0];

		//std::cout<<"Last cli commu time is: " << cli_commu[(((LwipCntxt*)(arg))->NodeID)]<<"\n";
		sprintf(img,  "../input/Node%d/%d.jpg", ((LwipCntxt*)(arg))->NodeID, i);
		sprintf(dptr, "../stage1/Node%d/%d.dat", ((LwipCntxt*)(arg))->NodeID, i);
		std::cout<< " O-1 in cli"<<((LwipCntxt*)(arg))->NodeID<<" preprocessing in client: "<< img << dptr << std::endl;
		preProcess(img, dptr);

		if((OSmodel->CLI_CORE_NUM)==2){
			std::cout<<"2-c";
			if(imgPre_cli[i][((LwipCntxt*)(arg))->NodeID-1] >cli_commu[(((LwipCntxt*)(arg))->NodeID)]  ){
		        	OSmodel -> os_port->timeWait( 
							imgPre_cli[i][((LwipCntxt*)(arg))->NodeID-1] -cli_commu[(((LwipCntxt*)(arg))->NodeID)] ,
		                                        taskID
				);
		     		OSmodel -> os_port->incBusyTime(1, 
							cli_commu[(((LwipCntxt*)(arg))->NodeID)]
				);
			}
			else
			{
		        	OSmodel -> os_port->timeWait( 
							0,
		                                        taskID
				);	
		     		OSmodel -> os_port->incBusyTime(1,
							imgPre_cli[i][((LwipCntxt*)(arg))->NodeID-1]
				);	
			}
		}else{
		        OSmodel -> os_port->timeWait( 
							imgPre_cli[i][((LwipCntxt*)(arg))->NodeID-1],
		                                        taskID
			);


		}
	        OSmodel -> os_port->statRecordApp( 
							imgPre_cli[i][((LwipCntxt*)(arg))->NodeID-1]
		);
	  	//visionGraphApp("../input/Node1/1.jpg", "../input/Node2/1.jpg");
	  }


  }

  //std::cout<<OSmodel -> NodeID <<"   ";	
  //OSmodel -> os_port -> statPrint();	

}

