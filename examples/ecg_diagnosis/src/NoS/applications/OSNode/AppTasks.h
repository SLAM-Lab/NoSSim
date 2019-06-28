#include "HCSim.h"
#include "lwip_ctxt.h"

#include "netif/hcsim_if.h"
#include "OmnetIf_pkt.h"

#include "json_config.h"
#include "application.h"

#ifndef SC_TASK_MODEL__H
#define SC_TASK_MODEL__H

#define LOWPAN6_TASK 1
#define PROTO TCP

void tcpip_init_done(void *arg);

class IntrDriven_Task :public sc_core::sc_module,virtual public HCSim::OS_TASK_INIT 
{
 public:
    sc_core::sc_port< sc_core::sc_fifo_out_if<int> > ctrl_out1; 
    sc_core::sc_port< sc_core::sc_fifo_out_if<int> > ctrl_out2; 

    sc_core::sc_vector< sc_core::sc_port< sys_call_recv_if > > recv_port;
    sc_core::sc_vector< sc_core::sc_port< sys_call_send_if > > send_port;
    sc_core::sc_port< HCSim::OSAPI > os_port;
    void* g_ctxt;
    os_model_context* OSmodel;

    SC_HAS_PROCESS(IntrDriven_Task);
  	IntrDriven_Task(const sc_core::sc_module_name name, 
            sc_dt::uint64 exe_cost, sc_dt::uint64 period, 
            unsigned int priority, int id, uint8_t init_core,
            sc_dt::uint64 end_sim_time, int node_id)
    :sc_core::sc_module(name)
    {
        this->exe_cost = exe_cost;
        this->period = period;
        this->priority = priority;
        this->id = id;
        this->end_sim_time = end_sim_time;
	this->node_id = node_id;
        this->init_core = 0;
	recv_port.init(2);
	send_port.init(2);
        SC_THREAD(run_jobs);
        g_ctxt=new lwip_context();
        OSmodel = new os_model_context();
	OSmodel->node_id = node_id;
	OSmodel->os_port(this->os_port);
	OSmodel->ctrl_out1(ctrl_out1);
	OSmodel->ctrl_out2(ctrl_out2);
	OSmodel->recv_port[0](this->recv_port[0]);
	OSmodel->recv_port[1](this->recv_port[1]);
	OSmodel->send_port[0](this->send_port[0]);
	OSmodel->send_port[1](this->send_port[1]);	
    }
    
    ~IntrDriven_Task() {}

    void OSTaskCreate(void){
#if IPV4_TASK
	//IP_ADDR4(&((lwip_context* )g_ctxt)->gw, 192,168,4,1);
	//IP_ADDR4(&((lwip_context* )g_ctxt)->netmask, 255,255,255,0);
        ipaddr_aton((simulation_config.cluster)->gateway_ipv4_address.c_str(), &((lwip_context* )g_ctxt)->gw);
        ipaddr_aton("255.255.255.0", &((lwip_context* )g_ctxt)->netmask);
        if(node_id == (sim_ctxt.cluster)->gateway_id){ 
           //IP_ADDR4(&((lwip_context* )g_ctxt)->ipaddr, 192, 168, 4, 1);
           ipaddr_aton((simulation_config.cluster)->gateway_ipv4_address.c_str(), &((lwip_context* )g_ctxt)->ipaddr);
           printf("IP address is: %s\n", ipaddr_ntoa(&((lwip_context* )g_ctxt)->ipaddr));
 	}else {
           //IP_ADDR4(&((lwip_context* )g_ctxt)->ipaddr, 192, 168, 4, client_id[node_id]);
           ipaddr_aton((simulation_config.cluster)->edge_ipv4_address[node_id].c_str(), &((lwip_context* )g_ctxt)->ipaddr);
           printf("IP address is: %s\n", ipaddr_ntoa(&((lwip_context* )g_ctxt)->ipaddr));
        }

#elif IPV6_TASK//IPV4_TASK
        if(node_id == (sim_ctxt.cluster)->gateway_id){ 
           IP_ADDR6(&((lwip_context* )g_ctxt)->ipaddr,  1, 2, 3, 3);
        } else {
           IP_ADDR6(&((lwip_context* )g_ctxt)->ipaddr,  1, 2, 3, (4 + node_id));
        }
#endif//IPV4_TASK
	((lwip_context* )g_ctxt)->node_id = node_id;
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
    int node_id;

    void run_jobs(void)
    {
	os_port->taskActivate(os_task_id);
	os_port->timeWait(0, os_task_id);
	os_port->syncGlobalTime(os_task_id);
        app_context* app_ctxt = new app_context(); 
        app_ctxt -> add_context("lwIP", g_ctxt);
        sim_ctxt.register_task(OSmodel, app_ctxt, os_task_id, sc_core::sc_get_current_process_handle());
	tcpip_init(tcpip_init_done, g_ctxt);
	printf("Applications started, node_id is %d %d\n", ((lwip_context* )g_ctxt)->node_id, sim_ctxt.get_task_id(sc_core::sc_get_current_process_handle()));
	printf("TCP/IP initialized at time %f\n", sc_core::sc_time_stamp().to_seconds());

        if(node_id == (simulation_config.cluster)->gateway_id) {
            //std::cout << "Core number is:" << (simulation_config.cluster)->gateway_core_number << std::endl;
            int cores =  (simulation_config.cluster)->gateway_core_number;
            if(cores==1){
	       OSmodel->profile->load_profile("./profile/1_core/pi0.prof");
               OSmodel->profile->load_profile("./profile/1_core/lwip_pi0.prof");
            }else{
	       OSmodel->profile->load_profile("./profile/4_core/pi3.prof");
               OSmodel->profile->load_profile("./profile/4_core/lwip_pi3.prof");
            }
            ecg_gateway(node_id);
        }else{ 
            //std::cout << "Core number is:" << (simulation_config.cluster)->edge_core_number[node_id] << std::endl;
            int cores =  (simulation_config.cluster)->edge_core_number[node_id];
            if(cores==1){
	       OSmodel->profile->load_profile("./profile/1_core/pi0.prof");
               OSmodel->profile->load_profile("./profile/1_core/lwip_pi0.prof");
            }else{
	       OSmodel->profile->load_profile("./profile/4_core/pi3.prof");
               OSmodel->profile->load_profile("./profile/4_core/lwip_pi3.prof");
            }
            ecg_edge(node_id);
        }

        os_port->taskTerminate(os_task_id);
    }
};


void inline tcpip_init_done(void *arg){
   lwip_context* ctxt = (lwip_context*)arg;
#if IPV4_TASK
   netif_set_default(
		netif_add( &(ctxt->netif), (ip_2_ip4(&(ctxt->ipaddr))),	(ip_2_ip4(&(ctxt->netmask))), (ip_2_ip4(&(ctxt->gw))), NULL, hcsim_if_init, tcpip_input)
   );
   netif_set_up(&(ctxt->netif));
#elif IPV6_TASK//IPV4_TASK
#if LOWPAN6_TASK
   netif_add(&(ctxt->netif), NULL, hcsim_if_init_6lowpan, tcpip_6lowpan_input);
   lowpan6_set_pan_id(1);
#else //LOWPAN6_TASK
   netif_add(&(ctxt->netif), NULL, hcsim_if_init, tcpip_input);
#endif //LOWPAN6_TASK
   (ctxt->netif).ip6_autoconfig_enabled = 1;
   netif_create_ip6_linklocal_address(&(ctxt->netif), 1);
   netif_add_ip6_address(&(ctxt->netif), ip_2_ip6(&(ctxt->ipaddr)), NULL);
   netif_set_default(&(ctxt->netif));
   netif_set_up(&(ctxt->netif));
   netif_ip6_addr_set_state(&(ctxt->netif), 0,  IP6_ADDR_PREFERRED);
   netif_ip6_addr_set_state(&(ctxt->netif), 1,  IP6_ADDR_PREFERRED);
#endif//IPV4_TASK   
}

#endif // SC_TASK_MODEL__H 

