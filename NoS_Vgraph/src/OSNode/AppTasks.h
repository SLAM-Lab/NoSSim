/*********************************************                                                       
 * VisionGraph Tasks Based on Interrupt-Driven Task Model                                                                     
 * Zhuoran Zhao, UT Austin, zhuoran@utexas.edu                                                    
 * Last update: July 2016                                                                            
 ********************************************/
#include <omnetpp.h>
#include <systemc>

#include "HCSim.h"
#include "image_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <time.h>
#include <math.h>       /* sqrt */




#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/ip_addr.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/tcp_impl.h"
#include "lwip/inet_chksum.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "lwip/lwip_ctxt.h"
#include "lwip/sys.h"
#include "lwip/api.h" //netconn_api libraries 



#include "netif/OmnetIf_server.h"
#include "netif/unixif.h"
#include "netif/dropif.h"
#include "netif/pcapif.h"
#include "netif/tcpdump.h"

#include "arch/perf.h"

#include "NetworkConfig.h"
#include "surf_features.h"
#include "os_ctxt.h"
#include "annotation.h"
#include "OmnetIf_pkt.h"
#include "config_data.h"

#ifndef SC_VISION_TASK__H
#define SC_VISION_TASK__H




void computeTask(void *arg);
void tcpip_init_done(void *arg);
void recv_imgs(void *arg);
void send_imgs(void *arg);
void send_img(void *arg, int image_count);

void cli_wait(void *arg);
void srv_wait(void *arg);
bool srv_send(void *arg, int cliID);
void cli_send(void *arg, int cliID);
void srv_send_dat(void *arg,  int from_cliID, int cliID,  int image_count);

class IntrDriven_Task
    :public sc_core::sc_module
  	,virtual public HCSim::OS_TASK_INIT 
{
 public:

    sc_core::sc_port<lwip_recv_if> recv_port[2];
    sc_core::sc_port<lwip_send_if> send_port[2];  
    sc_core::sc_port< HCSim::OSAPI > os_port;
    void* g_ctxt;

   
    VisionGraph *GraphRecorder;

    SC_HAS_PROCESS(IntrDriven_Task);
  	IntrDriven_Task(const sc_core::sc_module_name name, 
            sc_dt::uint64 exe_cost, sc_dt::uint64 period, 
            unsigned int priority, int id, uint8_t init_core,
            sc_dt::uint64 end_sim_time, int NodeID)
    :sc_core::sc_module(name)
    {
        this->exe_cost = exe_cost;
        this->period = period;
        this->priority = priority;
        this->id = id;
        this->end_sim_time = end_sim_time;
	this->GraphRecorder = new VisionGraph(TOTAL_NODES);
	this->NodeID = NodeID;


        g_config_init();

	if(SRV_CORE_NUM==2){
          if(NodeID==0)
            this->init_core = SRV_COMM_CORE;
	  else
            this->init_core = 1;
	}
        else{
            this->init_core = 1;
	}

        SC_THREAD(run_jobs);

        g_ctxt=new LwipCntxt();

	((LwipCntxt* )g_ctxt) -> OSmodel = (new OSModelCtxt());

	((OSModelCtxt*)(((LwipCntxt* )g_ctxt)->OSmodel))->NodeID = NodeID;
	((OSModelCtxt*)(((LwipCntxt* )g_ctxt)->OSmodel))->os_port(this->os_port);
	((OSModelCtxt*)(((LwipCntxt* )g_ctxt)->OSmodel))->recv_port[0](this->recv_port[0]);
	((OSModelCtxt*)(((LwipCntxt* )g_ctxt)->OSmodel))->recv_port[1](this->recv_port[1]);
	((OSModelCtxt*)(((LwipCntxt* )g_ctxt)->OSmodel))->send_port[0](this->send_port[0]);
	((OSModelCtxt*)(((LwipCntxt* )g_ctxt)->OSmodel))->send_port[1](this->send_port[1]);


		
    }
    
    ~IntrDriven_Task() {}

    void OSTaskCreate(void)
    {
	IP4_ADDR(&(((LwipCntxt* )g_ctxt)->gw), 192,168,0,1);
	IP4_ADDR(&(((LwipCntxt* )g_ctxt)->netmask), 255,255,255,0);
	IP4_ADDR(&(((LwipCntxt* )g_ctxt)->ipaddr), 192,168,0,NodeID+2);
	printf("Setting up NodeID %d ............... \n", NodeID);


	((LwipCntxt* )g_ctxt)->NodeID = NodeID;

        os_task_id = os_port->taskCreate(sc_core::sc_gen_unique_name("intrdriven_task"), 
                                HCSim::OS_RT_APERIODIC, priority, period, exe_cost, 
                                HCSim::DEFAULT_TS, HCSim::ALL_CORES, init_core);



    }

 private:
  
    int id;
    uint8_t init_core;
    sc_dt::uint64 exe_cost;
    sc_dt::uint64 period;
    unsigned int priority;
    sc_dt::uint64 end_sim_time;
    int os_task_id;
    int NodeID;

    void run_jobs(void)
    {

//      struct in_addr inaddr;
//      char ip_str[16] = {0}, nm_str[16] = {0}, gw_str[16] = {0};
//	inaddr.s_addr = (((LwipCntxt* )g_ctxt)->ipaddr).addr;
//	strncpy(ip_str,inet_ntoa(inaddr),sizeof(ip_str));
//	inaddr.s_addr = (((LwipCntxt* )g_ctxt)->netmask).addr;
//	strncpy(nm_str,inet_ntoa(inaddr),sizeof(nm_str));
//	inaddr.s_addr = (((LwipCntxt* )g_ctxt)->gw).addr;
//	strncpy(gw_str,inet_ntoa(inaddr),sizeof(gw_str));
//	printf("Client at %s mask %s gateway %s\n", ip_str, nm_str, gw_str);
//        total_client_OS = CLI_NUM;
	os_port->taskActivate(os_task_id);
	os_port->timeWait(0, os_task_id);
	os_port->syncGlobalTime(os_task_id);

        taskManager.registerTask( (OSModelCtxt*)(((LwipCntxt*)(g_ctxt))->OSmodel ), os_task_id, sc_core::sc_get_current_process_handle());

//	program_start_BA(0, 0);
//	taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() )
//	((LwipCntxt*)(g_ctxt))->recordOSModelTaskID(os_task_id, sc_core::sc_get_current_process_handle()); 
	config_init();//must be invoked after task registration
	netif_init();
	tcpip_init(g_ctxt, tcpip_init_done, g_ctxt);
	printf("TCP/IP initialized.\n");
	printf("Applications started, NodeID is %d\n", ((LwipCntxt* )g_ctxt)->NodeID);
	sys_thread_new(((LwipCntxt* )g_ctxt),"send_imgs", send_imgs, ((LwipCntxt* )g_ctxt), DEFAULT_THREAD_STACKSIZE, init_core);
	if(SRV_CORE_NUM==2){
	   sys_thread_new(((LwipCntxt* )g_ctxt),"compute", computeTask, ((LwipCntxt* )g_ctxt), DEFAULT_THREAD_STACKSIZE, SRV_APP_CORE);
	}
	recv_imgs(g_ctxt);


        os_port->taskTerminate(os_task_id);





    }



};

#endif // SC_VISION_TASK__H 
