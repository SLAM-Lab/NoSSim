#include "AppTasks.h"




sc_core::sc_event cli1; // systemc channel
sc_core::sc_event cli2; // systemc channel
sc_core::sc_event cli3; // systemc channel
sc_core::sc_event cli4; // systemc channel
sc_core::sc_event cli5; // systemc channel
sc_core::sc_event cli6; // systemc channel
sc_core::sc_event cli7; // systemc channel
sc_core::sc_event cli8; // systemc channel
sc_core::sc_event cli9; // systemc channel
sc_core::sc_event cli10; // systemc channel
sc_core::sc_event cli11; // systemc channel
sc_core::sc_event cli12; // systemc channel
sc_core::sc_event cli13; // systemc channel
sc_core::sc_event cli14; // systemc channel
sc_core::sc_event cli15; // systemc channel

bool flag1=0;
bool flag2=0;
bool flag3=0;
bool flag4=0;
bool flag5=0;
bool flag6=0;
bool flag7=0;
bool flag8=0;
bool flag9=0;
bool flag10=0;
bool flag11=0;
bool flag12=0;
bool flag13=0;
bool flag14=0;
bool flag15=0;

unsigned long cli_commu[CLI_NUM_MAX] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void cli_wait(void *arg){//wait for server to be available

  OSModelCtxt* OSmodel = taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() );

  struct netconn *conn, *newconn;
  err_t err;
  LwipCntxt *ctxt = (LwipCntxt *)arg;
  conn = netconn_new_with_proto_and_callback(ctxt, NETCONN_TCP, 0, NULL);
  //netconn_bind(ctxt, conn, IP_ADDR_ANY, 7);
  netconn_bind(ctxt, conn, &(((LwipCntxt* )ctxt)->ipaddr), 7 );
  netconn_listen(ctxt, conn);
  int push = 0;

  if(OSmodel->NodeID==1){cli1.notify(sc_core::SC_ZERO_TIME);flag1=1;}
  if(OSmodel->NodeID==2){cli2.notify(sc_core::SC_ZERO_TIME);flag2=1;}
  if(OSmodel->NodeID==3){cli3.notify(sc_core::SC_ZERO_TIME);flag3=1;}
  if(OSmodel->NodeID==4){cli4.notify(sc_core::SC_ZERO_TIME);flag4=1;}
  if(OSmodel->NodeID==5){cli5.notify(sc_core::SC_ZERO_TIME);flag5=1;}
  if(OSmodel->NodeID==6){cli6.notify(sc_core::SC_ZERO_TIME);flag6=1;}
  if(OSmodel->NodeID==7){cli7.notify(sc_core::SC_ZERO_TIME);flag7=1;}
  if(OSmodel->NodeID==8){cli8.notify(sc_core::SC_ZERO_TIME);flag8=1;}
  if(OSmodel->NodeID==9){cli9.notify(sc_core::SC_ZERO_TIME);flag9=1;}
  if(OSmodel->NodeID==10){cli10.notify(sc_core::SC_ZERO_TIME);flag10=1;}
  if(OSmodel->NodeID==11){cli11.notify(sc_core::SC_ZERO_TIME);flag11=1;}
  if(OSmodel->NodeID==12){cli12.notify(sc_core::SC_ZERO_TIME);flag12=1;}
  if(OSmodel->NodeID==13){cli13.notify(sc_core::SC_ZERO_TIME);flag13=1;}
  if(OSmodel->NodeID==14){cli14.notify(sc_core::SC_ZERO_TIME);flag14=1;}
  if(OSmodel->NodeID==15){cli15.notify(sc_core::SC_ZERO_TIME);flag15=1;}


  int img_ii = 0;






  //std::cout << "============cli ready=============== "   <<OSmodel->NodeID <<"   "<< sc_core::sc_time_stamp().value()<< std::endl;


  err = netconn_accept(ctxt, conn, &newconn);
  void *data;
  if (err == ERR_OK) {
      struct netbuf *buf;

      u16_t len;
      int ii;
      while ((err = netconn_recv(ctxt, newconn, &buf)) == ERR_OK) {

        do {
             netbuf_data(ctxt, buf, &data, &len);
	     img_ii+=len;
        } while (netbuf_next(buf) >= 0);
        if((buf->p->flags) & PBUF_FLAG_PUSH){
		//std::cout << "Recv data size is: " << img_ii << std::endl;
		img_ii = 0;
		push++; 
	}
        netbuf_delete(ctxt, buf);
      }
      netconn_close(ctxt, newconn);
      netconn_delete(ctxt, newconn);
  }

  netconn_close(ctxt, conn);
  netconn_delete(ctxt, conn);
  //std::cout << "============cli accepted=============== "   <<OSmodel->NodeID << std::endl;


}

void srv_wait(void *arg){//wait for server to be available
  struct netconn *conn, *newconn;
  err_t err;
  LwipCntxt *ctxt = (LwipCntxt *)arg;
  conn = netconn_new_with_proto_and_callback(ctxt, NETCONN_TCP, 0, NULL);
  //netconn_bind(ctxt, conn, IP_ADDR_ANY, 7);
  netconn_bind(ctxt, conn, &(((LwipCntxt* )ctxt)->ipaddr), 7 );
  netconn_listen(ctxt, conn);
  int push = 0;
  OSModelCtxt* OSmodel = taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() );

  //std::cout << "============cli wait=============== "   <<OSmodel->NodeID << std::endl;

  err = netconn_accept(ctxt, conn, &newconn);
  void *data;
  if (err == ERR_OK) {
      struct netbuf *buf;

      u16_t len;
      int ii;
      while ((err = netconn_recv(ctxt, newconn, &buf)) == ERR_OK) {

        do {
             netbuf_data(ctxt, buf, &data, &len);
        } while (netbuf_next(buf) >= 0);
        if((buf->p->flags) & PBUF_FLAG_PUSH){
		push++; 
	}
        netbuf_delete(ctxt, buf);
      }
      netconn_close(ctxt, newconn);
      netconn_delete(ctxt, newconn);
  }

  netconn_close(ctxt, conn);
  netconn_delete(ctxt, conn);
  //std::cout << "============cli accepted=============== "   <<OSmodel->NodeID << std::endl;

}

bool srv_send(void *arg, int cliID) //send availibility info from server side
{
  struct netconn *conn;
  err_t err;
  LwipCntxt *ctxt = (LwipCntxt *)arg;
  conn = netconn_new(ctxt, NETCONN_TCP);
  //netconn_bind(ctxt, conn, &(((LwipCntxt* )ctxt)->ipaddr), 7 );

 // char this_str[16];
 // ipaddr_ntoa_r(&(((LwipCntxt* )ctxt)->ipaddr_dest), this_str, 16);
  IP4_ADDR(&(((LwipCntxt*)(ctxt))->ipaddr_dest), 192, 168, 0, (2+cliID));


  OSModelCtxt* OSmodel = taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() );


  //if(cliID==1)	temp_time_recv = sc_core::sc_time_stamp().value();
  //std::cout << "============srv check=============== "  <<cliID <<" "<< flag1 <<"   "<< sc_core::sc_time_stamp().value()<<  std::endl;


  unsigned long temp = sc_core::sc_time_stamp().value();

  if(cliID==1&&flag1==0) { sc_core::wait(cli1);}
  if(cliID==2&&flag2==0) { sc_core::wait(cli2);}
  if(cliID==3&&flag3==0) { sc_core::wait(cli3);}
  if(cliID==4&&flag4==0) { sc_core::wait(cli4);}
  if(cliID==5&&flag5==0) { sc_core::wait(cli5);}
  if(cliID==6&&flag6==0) { sc_core::wait(cli6);}
  if(cliID==7&&flag7==0) { sc_core::wait(cli7);}
  if(cliID==8&&flag8==0) { sc_core::wait(cli8);}
  if(cliID==9&&flag9==0) { sc_core::wait(cli9);}
  if(cliID==10&&flag10==0) {sc_core::wait(cli10);}
  if(cliID==11&&flag11==0) {sc_core::wait(cli11);}
  if(cliID==12&&flag12==0) {sc_core::wait(cli12);}
  if(cliID==13&&flag13==0) {sc_core::wait(cli13);}
  if(cliID==14&&flag14==0) {sc_core::wait(cli14);}
  if(cliID==15&&flag15==0) {sc_core::wait(cli15);}


  if(cliID==1)  flag1=0;
  if(cliID==2)  flag2=0;
  if(cliID==3)  flag3=0;
  if(cliID==4)  flag4=0;
  if(cliID==5)  flag5=0;
  if(cliID==6)  flag6=0;
  if(cliID==7)  flag7=0;
  if(cliID==8)  flag8=0;
  if(cliID==9)  flag9=0;
  if(cliID==10) flag10=0;
  if(cliID==11) flag11=0;
  if(cliID==12) flag12=0;
  if(cliID==13) flag13=0;
  if(cliID==14) flag14=0;
  if(cliID==15) flag15=0;

  OSmodel -> os_port -> adjustBusyTime(SRV_COMM_CORE, sc_core::sc_time_stamp().value() - temp);

  cli_commu[0] = sc_core::sc_time_stamp().value();

  //communication_time = communication_time-(sc_core::sc_time_stamp().value() - temp);

  //std::cout << "============srv check=============== "  <<cliID <<"   "<< sc_core::sc_time_stamp().value()<<  std::endl;
  err = netconn_connect(ctxt, conn, &(((LwipCntxt* )ctxt)->ipaddr_dest), 7);
  //std::cout << "============srv sent=============== "  <<cliID  <<  std::endl;
  if (err != ERR_OK) {
    std::cout << "Connection refused... ..." << std::endl;
    return 0;
  }
  size_t *bytes_written=NULL;
  unsigned int msg_size=10;
  char msg[10]="srv avail";

/*
  char* buf;
  char local_img[30];   
  sprintf(local_img, "../stage1/Node%d/%d.dat", 1, 1);//large
  unsigned int buf_size = load_file_to_memory(local_img, &buf);
*/

  while (1) {
    err = netconn_write_partly(ctxt, conn, msg, msg_size, NETCONN_COPY, bytes_written);
    //err = netconn_write_partly(ctxt, conn, buf, buf_size, NETCONN_COPY, bytes_written);
    if (err == ERR_OK) 
    {
      netconn_close(ctxt, conn);

      break;
    }
  }

  netconn_disconnect(ctxt, conn);
  //netconn_close(ctxt, conn);
  netconn_delete(ctxt, conn);
  IP4_ADDR(&(((LwipCntxt*)(ctxt))->ipaddr_dest), 0, 0, 0, 0);
  return 1;
}





void cli_send(void *arg, int cliID) //send availibility info from server side
{
  struct netconn *conn;
  err_t err;
  LwipCntxt *ctxt = (LwipCntxt *)arg;
  conn = netconn_new(ctxt, NETCONN_TCP);
  //netconn_bind(ctxt, conn, &(((LwipCntxt* )ctxt)->ipaddr), 7 );

 // char this_str[16];
 // ipaddr_ntoa_r(&(((LwipCntxt* )ctxt)->ipaddr_dest), this_str, 16);
  IP4_ADDR(&(((LwipCntxt*)(ctxt))->ipaddr_dest), 192, 168, 0, (2+cliID));


  OSModelCtxt* OSmodel = taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() );

  //std::cout << "============srv send=============== "  <<cliID  <<  std::endl;
  err = netconn_connect(ctxt, conn, &(((LwipCntxt* )ctxt)->ipaddr_dest), 7);
  //std::cout << "============srv sent=============== "  <<cliID  <<  std::endl;
  if (err != ERR_OK) {
    std::cout << "Connection refused... ..." << std::endl;
    return;
  }
  size_t *bytes_written=NULL;
  unsigned int msg_size=10;
  char msg[10]="srv avail";

  while (1) {
    err = netconn_write_partly(ctxt, conn, msg, msg_size, NETCONN_COPY, bytes_written);
    if (err == ERR_OK) 
    {
      netconn_close(ctxt, conn);
      netconn_delete(ctxt, conn);
      break;
    }
  }

}





void srv_send_dat(void *arg,  int from_cliID, int cliID,  int image_count)
{

  struct netconn *conn;
  err_t err;

  LwipCntxt *ctxt = (LwipCntxt *)arg;


  /* Create a new connection identifier. */

  conn = netconn_new(ctxt, NETCONN_TCP);

  /* Bind connection to well known port number 7. */

  OSModelCtxt* OSmodel = taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() );


  IP4_ADDR(&(((LwipCntxt*)(ctxt))->ipaddr_dest), 192, 168, 0, (2+cliID));



  unsigned long temp = sc_core::sc_time_stamp().value();

  if(cliID==1&&flag1==0) { sc_core::wait(cli1);}
  if(cliID==2&&flag2==0) { sc_core::wait(cli2);}
  if(cliID==3&&flag3==0) { sc_core::wait(cli3);}
  if(cliID==4&&flag4==0) { sc_core::wait(cli4);}
  if(cliID==5&&flag5==0) { sc_core::wait(cli5);}
  if(cliID==6&&flag6==0) { sc_core::wait(cli6);}
  if(cliID==7&&flag7==0) { sc_core::wait(cli7);}
  if(cliID==8&&flag8==0) { sc_core::wait(cli8);}
  if(cliID==9&&flag9==0) { sc_core::wait(cli9);}
  if(cliID==10&&flag10==0) {sc_core::wait(cli10);}
  if(cliID==11&&flag11==0) {sc_core::wait(cli11);}
  if(cliID==12&&flag12==0) {sc_core::wait(cli12);}
  if(cliID==13&&flag13==0) {sc_core::wait(cli13);}
  if(cliID==14&&flag14==0) {sc_core::wait(cli14);}
  if(cliID==15&&flag15==0) {sc_core::wait(cli15);}


  if(cliID==1)  flag1=0;
  if(cliID==2)  flag2=0;
  if(cliID==3)  flag3=0;
  if(cliID==4)  flag4=0;
  if(cliID==5)  flag5=0;
  if(cliID==6)  flag6=0;
  if(cliID==7)  flag7=0;
  if(cliID==8)  flag8=0;
  if(cliID==9)  flag9=0;
  if(cliID==10) flag10=0;
  if(cliID==11) flag11=0;
  if(cliID==12) flag12=0;
  if(cliID==13) flag13=0;
  if(cliID==14) flag14=0;
  if(cliID==15) flag15=0;

  OSmodel -> os_port -> adjustBusyTime(SRV_COMM_CORE, sc_core::sc_time_stamp().value() - temp);

  //communication_time = communication_time-(sc_core::sc_time_stamp().value() - temp);
  cli_commu[0] = sc_core::sc_time_stamp().value();

  //std::cout << "============srv check phase1=============== from "  <<from_cliID <<" to "<< cliID <<"   "<< sc_core::sc_time_stamp().value()<<  std::endl;
  err = netconn_connect(ctxt, conn, &(((LwipCntxt* )ctxt)->ipaddr_dest), 7);

  if (err != ERR_OK) {
    std::cout << "Connection refused" << std::endl;
    return;
  }


  size_t *bytes_written=NULL;
  unsigned int buf_size;
  char* buf;
  char local_img[30];   
  sprintf(local_img, "../stage1/Node%d/%d.dat", from_cliID, image_count);//large
  std::cout <<  local_img << std::endl;

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

