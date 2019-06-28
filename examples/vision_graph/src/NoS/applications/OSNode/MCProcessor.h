/******************************************************************************
 * Multi-Core Processor TLM model
 * Parisa Razaghi, UT Austin, parisa.r@utexas.edu
 * Last update: July 2013
 ******************************************************************************/

#include <systemc>
#include "HCSim.h"
#include "config.h"
#include "AppTasks.h"

#ifndef SC_MCPROCESSOR__H
#define SC_MCPROCESSOR__H

/******************************************************************************
 * Application-specific interrupt handling interface
 *
 ******************************************************************************/
class IntHandler_if
    :virtual public sc_core::sc_interface
{
 public:
    virtual void intHandler(void) = 0;
    virtual void intHandler__HINT1(int coreID) = 0;
    virtual void intHandler__HINT2(int coreID) = 0;
};

/******************************************************************************
 * Application-specific interrupt task model
 *
 ******************************************************************************/
class IntrTask_HINT
    :public sc_core::sc_module
    ,virtual public HCSim::OS_TASK_INIT
    ,virtual public HCSim::IntrTrigger_if
{
 public:
    /*---------------------------------------------------------
       OS & Intr. interface
     ----------------------------------------------------------*/ 
    sc_core::sc_port< HCSim::OSAPI > os_port;
    sc_core::sc_port< HCSim::send_os_if > intr_ch;
    /*---------------------------------------------------------
       User-defined communication interface
       >> Put application-specific interface here... 
     ----------------------------------------------------------*/    
    sc_core::sc_port< HCSim::send_os_if > data_ch;
    /*--------------------------------------------------------*/

    SC_HAS_PROCESS(IntrTask_HINT);
    IntrTask_HINT(const sc_core::sc_module_name name, 
                            int intr_id,
                            int target_cpu, 
                            unsigned int priority, 
                            sc_dt::uint64 exe_cost)
        :sc_core::sc_module(name)
    {
        this->target_cpu = target_cpu;
        this->priority = priority;
        this->intr_id = intr_id;
        this->exe_cost = exe_cost;

        SC_THREAD(monitor);
    }    
    ~IntrTask_HINT() {}
    /*---------------------------------------------------------
       OS_TASK_INIT interface method
       >> Creates interrupt-task (i.e. Signal Handler) 
     ----------------------------------------------------------*/ 
    void OSTaskCreate(void) {   
        /*----------------------------------------------------------------------------------------------------------------------
	    os_task_id = os_port->taskCreate("intTask", HCSim::OS_INTR_TASK, priority, HCSim::OS_INFINIT_VAL, 
                                                         exe_cost, HCSim::DEFAULT_TS, HCSim::ALL_CORES, target_cpu);
            -------------------------------------------------- OR ------------------------------------------------------------*/   

	  os_task_id = os_port->createIntrTask("intTask", priority, HCSim::ALL_CORES, target_cpu, target_cpu);

    }
    /*---------------------------------------------------------
       IInitTask interface method
       >> Triggers the interrupt-task 
     ----------------------------------------------------------*/
    void start(int coreID) {  
        //std::cout<<"start intr............."<<std::endl;                                          
		os_port->intrTrigger(os_task_id, coreID);
    }
                        
 private:
	int os_task_id;
	int target_cpu;
	int intr_id;
	unsigned int priority;
	sc_dt::uint64 exe_cost;
	
	void intr_body(void) {


		os_port->timeWait(exe_cost, os_task_id);

		os_port->syncGlobalTime(os_task_id);

		//data_ch->send(os_task_id);//Adding for monitoring interruption task signals 

		intr_ch->send(os_task_id); /* Notifys the interrupt event */
	}

    void monitor(void) { 
	    os_port->taskActivate(os_task_id);

        while(1) {   
	        intr_body();
	        os_port->intrSleep(os_task_id);  
	    }
    }
};

/******************************************************************************
 * User-defined adapter for Mac Link Master Port
 *
 ******************************************************************************/
class MacLink_Data_Out_Adapter
    :sc_core::sc_module
    ,virtual public HCSim::send_os_if 
{
 public:   
    /*---------------------------------------------------------
       OS/HAL interface
     ----------------------------------------------------------*/ 
    sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacLink > mac_link_port;
    
    SC_HAS_PROCESS(MacLink_Data_Out_Adapter);
    MacLink_Data_Out_Adapter(const sc_core::sc_module_name name, unsigned long long addr)
        :sc_core::sc_module(name)
    {
        this->addr = addr;
    }
    ~MacLink_Data_Out_Adapter() {}
    /*---------------------------------------------------------
       send_os_if  interface method
        >> translates the high-level send method to 
             the MacLink interface methods
     ----------------------------------------------------------*/     
    void send(int task_id) {   
        mac_link_port->masterWrite(addr, &task_id, sizeof(int));
    }
    
 private:
    unsigned long long addr;    
};

/******************************************************************************
 * User-defined adapter for Mac Mem Master Port
 *
 ******************************************************************************/
class MacMem_Data_Out_Adapter
    :sc_core::sc_module
    ,virtual public HCSim::send_os_if 
{
 public:   
    /*---------------------------------------------------------
       OS/HAL interface
     ----------------------------------------------------------*/ 
    sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacMem > mac_mem_port;
    
    SC_HAS_PROCESS(MacMem_Data_Out_Adapter);
    MacMem_Data_Out_Adapter(const sc_core::sc_module_name name, unsigned long long addr)
        :sc_core::sc_module(name)
    {
        this->addr = addr;
    }
    ~MacMem_Data_Out_Adapter() {}
    /*---------------------------------------------------------
       send_os_if  interface method
        >> translates the high-level send method to 
             the MacLink interface methods
     ----------------------------------------------------------*/     
    void send(int task_id) {   
    
        unsigned char tmp[1];
        mac_mem_port->masterMemWrite(addr, tmp, 4ull);
	}
    
 private:
    unsigned long long addr;    
};
/******************************************************************************
 * Processor OS Layer
 *
 ******************************************************************************/
template< int INTR_NUM, int CPU_NUM >
class MCProcessor_OS
    :public sc_core::sc_module
    ,virtual public IntHandler_if
{
 public:
    /*---------------------------------------------------------
       Port for controling simulation progress
     ----------------------------------------------------------*/
    sc_core::sc_port< sc_core::sc_fifo_out_if<int> > ctrl_out1; 
    sc_core::sc_port< sc_core::sc_fifo_out_if<int> > ctrl_out2; 
    /*---------------------------------------------------------
       OS/HAL interface
       >> MAC LINK/MEM Master Interface
     ----------------------------------------------------------*/  
    sc_core::sc_vector< sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacLink > > mac_link_port;
    sc_core::sc_vector< sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacMem > > mac_mem_port;
    //sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacLink > mac_link_port[CPU_NUM];
    //sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacMem > mac_mem_port[CPU_NUM];
    
    /*---------------------------------------------------------
       Export OS interface to the HAL
     ----------------------------------------------------------*/
    sc_core::sc_export< HCSim::OSAPI > os_export;
    /*---------------------------------------------------------
       Export user-defined Intr. interface to the HAL
     ----------------------------------------------------------*/
    sc_core::sc_export< IntHandler_if > intr_export; 
   
    SC_HAS_PROCESS(MCProcessor_OS);
    MCProcessor_OS(const sc_core::sc_module_name name,
                                sc_dt::uint64 simulation_quantum, int NodeID);  
    ~MCProcessor_OS(); 
    /*---------------------------------------------------------
       IIntrHandler interface methods
     ----------------------------------------------------------*/    
    void intHandler(void);
    void intHandler__HINT1(int coreID);
    void intHandler__HINT2(int coreID);
 
    void* getLwipCtxt(){
	return CPU_APP->g_ctxt;
    }
    void dumpStatJson(){
	OS->statPrint();
    }
 
 private:
    /*---------------------------------------------------------
       OS Channel
     ----------------------------------------------------------*/  
    HCSim::RTOS* OS;
    /*---------------------------------------------------------
       User-defined application and interrupt tasks
     ----------------------------------------------------------*/     
//    Mix_Task_Set* CPU_APP;
    IntrDriven_Task* CPU_APP;
    IntrTask_HINT* intr_task_1;
    IntrTask_HINT* intr_task_2;
    /*---------------------------------------------------------
       Application channels and adapters
     ----------------------------------------------------------*/ 
    HCSim::handshake_os_ch flag_intr_1;
    HCSim::handshake_os_ch flag_intr_2;
    MacLink_Data_Out_Adapter intr_data_out_1;
    MacLink_Data_Out_Adapter intr_data_out_2;
    //MacLink_Data_Out_Adapter app_data_out_1;
    //MacLink_Data_Out_Adapter app_data_out_2;
    sys_call_recv_driver *recv_adapter1;
    sys_call_recv_driver *recv_adapter2;
    sys_call_send_driver *send_adapter1;
    sys_call_send_driver *send_adapter2;

    /*--------------------------------------------------------*/      
    
    void end_of_elaboration();    
    void start_of_simulation();
};

template< int INTR_NUM, int CPU_NUM >
MCProcessor_OS< INTR_NUM, CPU_NUM >::MCProcessor_OS(const sc_core::sc_module_name name, 
					       sc_dt::uint64 simulation_quantum, int NodeID)
    :sc_core::sc_module(name)
    ,flag_intr_1("intr_1")
    ,flag_intr_2("intr_2")
    ,intr_data_out_1("intr_data_out_1", const_intr1_task_address)
    ,intr_data_out_2("intr_data_out_2", const_intr2_task_address)
 //   ,app_data_out_1("app_data_out_1", const_intr1_address)
 //   ,app_data_out_2("app_data_out_2", const_intr2_address)
{
    mac_link_port.init(CPU_NUM);
    mac_mem_port.init(CPU_NUM);

    intr_export(*this);
      
    OS = new HCSim::RTOS(sc_core::sc_gen_unique_name("RTOS"));
    OS->init(CPU_NUM, simulation_quantum);
    OS->NodeID = NodeID;
    os_export(*OS);
    
    flag_intr_1.os_port(*OS);   
    flag_intr_2.os_port(*OS);   
    
    intr_task_1 = new IntrTask_HINT("intr_task_1", 0, 0, const_intr1_priority, SHANDLER_DELAY);
    intr_task_1->os_port(*OS);
    intr_task_1->intr_ch(flag_intr_1);
    intr_task_1->data_ch(intr_data_out_1);
    
    intr_task_2 = new IntrTask_HINT("intr_task_2", 0, 1, const_intr2_priority, SHANDLER_DELAY);
    intr_task_2->os_port(*OS);
    intr_task_2->intr_ch(flag_intr_2);
    intr_task_2->data_ch(intr_data_out_2);


    recv_adapter1 = new sys_call_recv_driver("app_lwip_recv0", const_intr1_address);
    send_adapter1 = new sys_call_send_driver("app_lwip_send0", const_intr1_address);
    recv_adapter2 = new sys_call_recv_driver("app_lwip_recv1", const_intr2_address);
    send_adapter2 = new sys_call_send_driver("app_lwip_send1", const_intr2_address);

    send_adapter1-> mac_link_port(mac_link_port[0]);
    recv_adapter1-> mac_link_port(mac_link_port[0]);   
    send_adapter1-> intr_ch(flag_intr_1);
    recv_adapter1-> intr_ch(flag_intr_1); 

    send_adapter2-> mac_link_port(mac_link_port[1]);
    recv_adapter2-> mac_link_port(mac_link_port[1]);
    send_adapter2-> intr_ch(flag_intr_2);    
    recv_adapter2-> intr_ch(flag_intr_2);    

    //CPU_APP = new Mix_Task_Set("Application");
    CPU_APP = new IntrDriven_Task(sc_core::sc_gen_unique_name("intrdriven_task"), const_task_delay_INTH[0], 
                              const_task_period_INTH[0], const_task_priority_INTH[0], 
				const_id_INT[0], const_init_core_INT[0], const_end_time, NodeID);

    CPU_APP->ctrl_out1(ctrl_out1);
    CPU_APP->ctrl_out2(ctrl_out2);
    CPU_APP->os_port(*OS);
    CPU_APP->recv_port[0](*recv_adapter1);
    CPU_APP->send_port[0](*send_adapter1);
    CPU_APP->recv_port[1](*recv_adapter2);
    CPU_APP->send_port[1](*send_adapter2);

    //CPU_APP->intrCH[0](flag_intr_1);
    //CPU_APP->dataCH[0](app_data_out_1);
    //CPU_APP->intrCH[1](flag_intr_2);
    //CPU_APP->dataCH[1](app_data_out_2);    
    
    intr_data_out_1.mac_link_port(mac_link_port[0]);
    intr_data_out_2.mac_link_port(mac_link_port[1]);

 
}

template< int INTR_NUM, int CPU_NUM >
MCProcessor_OS< INTR_NUM, CPU_NUM >::~MCProcessor_OS()
{
    delete intr_task_1;
    delete intr_task_2; 
    delete CPU_APP;
    delete OS;
    delete recv_adapter1;
    delete recv_adapter2;
    delete send_adapter1;
    delete send_adapter2;

}

template< int INTR_NUM, int CPU_NUM >
void MCProcessor_OS< INTR_NUM, CPU_NUM >::end_of_elaboration()
{
    /*---------------------------------------------------------
       Create user-define interrupt tasks 
     ----------------------------------------------------------*/ 
    intr_task_1->OSTaskCreate();
    intr_task_2->OSTaskCreate();
    CPU_APP->OSTaskCreate();

}

template< int INTR_NUM, int CPU_NUM >
void MCProcessor_OS< INTR_NUM, CPU_NUM >::start_of_simulation()
{
    OS->start();
}

template< int INTR_NUM, int CPU_NUM >
void MCProcessor_OS< INTR_NUM, CPU_NUM >::intHandler(void)
{   
}

template< int INTR_NUM, int CPU_NUM >
void MCProcessor_OS< INTR_NUM, CPU_NUM >::intHandler__HINT1(int coreID)
{  
    intr_task_1->start(coreID);
}

template< int INTR_NUM, int CPU_NUM >
void MCProcessor_OS< INTR_NUM, CPU_NUM >::intHandler__HINT2(int coreID)
{ 
    intr_task_2->start(coreID);
}
   
/******************************************************************************
 * Interrupt Handler Model
 *
 ******************************************************************************/
class Interrupt_Handler
    :public sc_core::sc_module
    ,virtual public HCSim::OS_TASK_INIT
    ,virtual public HCSim::HAL_if
{
 public:
     /*---------------------------------------------------------
        HW interface
       >> MAC TLM PROT Master Interface, nIRQ
     ----------------------------------------------------------*/ 
    sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacTlmProt > bus_master_port;
    /*---------------------------------------------------------
       OS interface 
     ----------------------------------------------------------*/    
    sc_core::sc_port< HCSim::OSAPI> os_port;
    /*---------------------------------------------------------
       User-defined interrupt task interface 
     ----------------------------------------------------------*/  
    sc_core::sc_port< IntHandler_if > intr_port;
    sc_core::sc_export< HCSim::HAL_if > hal_export;

    SC_HAS_PROCESS(Interrupt_Handler);
    Interrupt_Handler(const sc_core::sc_module_name name, int coreID)
	    :sc_core::sc_module(name)
	    ,core_id(coreID)
		{  
		    hal_export(*this);
		    
		    SC_THREAD(intr_handler);
		}
    ~Interrupt_Handler(){}
    
    /*---------------------------------------------------------
       OS_TASK_INIT interface method
       >> Creates interrupt-handler (called by HAL) 
     ----------------------------------------------------------*/ 
    void OSTaskCreate(void) 
	{   
	    intr_handler_id = os_port->createIntrHandler(core_id, 0);
	} 
    void IRQEnter(int coreID) 
	{   
	    assert (coreID == core_id);
	    os_port->iEnter(core_id, intr_handler_id);
	}

 private:
    int intr_handler_id;
    int core_id;
    
    void intr_handler(void)
    {
	    unsigned int INTSRC;
	    unsigned long int addrPic;

	   while(1) {

		os_port->taskActivate(intr_handler_id);

	#ifdef INTR_TRACE_ON             
		    printf("%llu: C%d Interrupt handler HAL layer start. \n", sc_core::sc_time_stamp().value(), core_id);
	#endif
		sc_core::wait(IHANDLER_DELAY_1, SIM_RESOLUTION); 
		 /*---------------------------------------------------------
		    Reads interrupt source ID 
		 ----------------------------------------------------------*/ 	    	
		addrPic = (unsigned long int)(HCSim::GIC_BASE_ADDRESS + (HCSim::GIC_CPUADDR_STEP*core_id)+HCSim::GIC_IAR_OFFSET);
		bus_master_port->masterRead(addrPic,  &INTSRC, sizeof(INTSRC));
		    
	#ifdef INTR_TRACE_ON 
				printf("%llu: C%d- Interrupt handler HAL layer -- source %d. \n", sc_core::sc_time_stamp().value(), core_id, INTSRC);
	#endif	
			/*---------------------------------------------------------
		    Triggers interrupt tasks 
		 ----------------------------------------------------------*/ 	
		if (INTSRC != HCSim::GIC_SPURIOUS_INTERRUPT) {
		    switch(INTSRC) {
		    	    case 0:
		    		    intr_port->intHandler();
		    			break;
		        case 1:
					  	addrPic = const_intr1_handler_address;
		            intr_port->intHandler__HINT1(core_id);
		            break;
		        case 2:
					  addrPic = const_intr2_handler_address;

		            intr_port->intHandler__HINT2(core_id);
		            break;
		        default:
		            intr_port->intHandler();
		            break;
		    }
		    /* To measure interrupt handler response time (in mix_task_set example) */
		    /* Note: needs to be removed in other examples...*/
				//bus_master_port->masterWrite(addrPic,  &INTSRC, sizeof(INTSRC));

		    sc_core::wait(IHANDLER_DELAY_2, SIM_RESOLUTION); 
		    /*---------------------------------------------------------
		        Writes End-Of-Interrupt register
		    ----------------------------------------------------------*/   	
		    addrPic = (unsigned long int)(HCSim::GIC_BASE_ADDRESS + (HCSim::GIC_CPUADDR_STEP*core_id)+ HCSim::GIC_EOIR_OFFSET);
		    bus_master_port->masterWrite(addrPic,  &INTSRC, sizeof(INTSRC));
			 
		}
		os_port->iReturn(core_id);
	#ifdef INTR_TRACE_ON            
		   printf("%llu: C%d- Interrupt handler HAL layer end on source %d. \n", sc_core::sc_time_stamp().value(), core_id, INTSRC);
	#endif           
	    }
    }  
};







  
/******************************************************************************
 * Processor HAL Layer
 *
 ******************************************************************************/
template< int INTR_NUM, int CPU_NUM >
class MCProcessor_HAL
    :public sc_core::sc_module
    ,virtual public HCSim::HAL_if
{
 public:
    /*---------------------------------------------------------
       Port for controling simulation progress
     ----------------------------------------------------------*/
    sc_core::sc_port< sc_core::sc_fifo_out_if<int> > ctrl_out1; 
    sc_core::sc_port< sc_core::sc_fifo_out_if<int> > ctrl_out2; 
      /*---------------------------------------------------------
       HAL/HW interface
       >> MAC TLM PROT Master Interface
     ----------------------------------------------------------*/ 
    sc_core::sc_vector< sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacTlmProt > > bus_master_port;
    //sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacTlmProt > bus_master_port[CPU_NUM];
    /*---------------------------------------------------------
       Export OS interface to the HW layer
     ----------------------------------------------------------*/
    //sc_core::sc_export< HCSim::OSAPI > os_export;
    sc_core::sc_export< HCSim::HAL_if>  hal_export;
    MCProcessor_HAL(const sc_core::sc_module_name name, 
                                  sc_dt::uint64 simulation_quantum, int NodeID);
    ~MCProcessor_HAL();
    
    void end_of_elaboration();
    void IRQEnter(int coreID);
    void* getLwipCtxt(){
	return CPU_OS->getLwipCtxt();
    }
    void dumpStatJson(){
	CPU_OS->dumpStatJson();
    }

 private:
    /*---------------------------------------------------------
       CPU OS LAYER
     ----------------------------------------------------------*/  
    MCProcessor_OS< INTR_NUM, CPU_NUM >* CPU_OS;
    /*---------------------------------------------------------
       CPU Interrupt Handlers
     ----------------------------------------------------------*/     
    std::vector< Interrupt_Handler* > Intr_Handler;
    /*---------------------------------------------------------
       MAC-(Link,Mem) / MAC-TLM Transactors
     ----------------------------------------------------------*/     
	sc_core::sc_vector< HCSim::AmbaAhbBusMasterMacLinkPass > mac_link2tlm;
	sc_core::sc_vector< HCSim::AmbaAhbBusMasterMacMemPass > mac_mem2tlm;
	//HCSim::AmbaAhbBusMasterMacLinkPass mac_link2tlm[CPU_NUM];
	//HCSim::AmbaAhbBusMasterMacMemPass mac_mem2tlm[CPU_NUM];
};

template< int INTR_NUM, int CPU_NUM >
MCProcessor_HAL< INTR_NUM, CPU_NUM >::MCProcessor_HAL(const sc_core::sc_module_name name,
						sc_dt::uint64 simulation_quantum, int NodeID)
    :sc_core::sc_module(name)
{
    mac_link2tlm.init(CPU_NUM);
    mac_mem2tlm.init(CPU_NUM);
    bus_master_port.init(CPU_NUM);
    
  	hal_export(*this);
  	
    std::stringstream module_name;
    module_name << name << "_OS";
    CPU_OS = new MCProcessor_OS< INTR_NUM, CPU_NUM >(module_name.str().c_str(), simulation_quantum, NodeID);
    CPU_OS->ctrl_out1(ctrl_out1);
    CPU_OS->ctrl_out2(ctrl_out2);
    for (int cpu = 0; cpu < CPU_NUM; cpu++) {
    	CPU_OS->mac_link_port[cpu](mac_link2tlm[cpu]);
    	CPU_OS->mac_mem_port[cpu](mac_mem2tlm[cpu]);
    }
		
	for (int cpu = 0; cpu < CPU_NUM; cpu++) {
	    Interrupt_Handler* intr_h;
	    intr_h = new Interrupt_Handler(sc_core::sc_gen_unique_name("intr_handler"), cpu);
	    intr_h->bus_master_port(bus_master_port[cpu]);
	    intr_h->os_port(CPU_OS->os_export);
	    intr_h->intr_port(CPU_OS->intr_export);
	    Intr_Handler.push_back(intr_h);
	}
	
    for (int cpu = 0; cpu < CPU_NUM; cpu++) {
	    mac_link2tlm[cpu].protWMac(bus_master_port[cpu]);
        mac_mem2tlm[cpu].protWMac(bus_master_port[cpu]);
    }
}

template< int INTR_NUM, int CPU_NUM >
MCProcessor_HAL< INTR_NUM, CPU_NUM >::~MCProcessor_HAL()
{
    delete CPU_OS;
    for(std::vector< Interrupt_Handler* >::iterator iter =  Intr_Handler.begin();
          iter !=  Intr_Handler.end(); ++iter) {
        delete *iter;
    }
    Intr_Handler.clear(); 
}

template< int INTR_NUM, int CPU_NUM >
void MCProcessor_HAL< INTR_NUM, CPU_NUM >::end_of_elaboration()
{
    for (int cpu = 0; cpu < CPU_NUM; cpu++) 
	    Intr_Handler[cpu]->OSTaskCreate();
}

template< int INTR_NUM, int CPU_NUM >
void MCProcessor_HAL< INTR_NUM, CPU_NUM >::IRQEnter(int coreID) 
{   
    Intr_Handler[coreID]->IRQEnter(coreID);
}

/******************************************************************************
 * CPU Interrupt Interface
 *
 ******************************************************************************/
class Interrupt_Interface
    :public sc_core::sc_module
{
 public:
    sc_core::sc_in< bool > nIRQ;
    sc_core::sc_port< HCSim::HAL_if > hal_port;
     
    SC_HAS_PROCESS(Interrupt_Interface); 
    Interrupt_Interface(const sc_core::sc_module_name name, int core_id)
        :sc_core::sc_module(name)
    {
        this->core_id = core_id;
        
        SC_THREAD(intr_monitor);
    }      
    ~Interrupt_Interface() {}
 
 private:
    int core_id;
    
    void intr_monitor(void)
    {
        while (1) {       
            while (nIRQ->read() == 1){
		//std::cout<<               "BEFORE sc_core::wait(nIRQ->value_changed_event());"<<std::endl;
                sc_core::wait(nIRQ->value_changed_event());
		//std::cout<<               "AFTER sc_core::wait(nIRQ->value_changed_event());"<<std::endl;
		}
            hal_port->IRQEnter(core_id);
            sc_core::wait(nIRQ->posedge_event());
        }
    }
};

/******************************************************************************
 * Processor HW Layer
 * 
 ******************************************************************************/
template< int INTR_NUM, int CPU_NUM >
class MCProcessor_HW
    :public sc_core::sc_module
{
 public:
    /*---------------------------------------------------------
       Port for controling simulation progress
     ----------------------------------------------------------*/
    sc_core::sc_port< sc_core::sc_fifo_out_if<int> > ctrl_out1; 
    sc_core::sc_port< sc_core::sc_fifo_out_if<int> > ctrl_out2; 
    /*---------------------------------------------------------
       HW/TLM interface
       >> MAC TLM PROT Master Interface
     ----------------------------------------------------------*/
    sc_core::sc_vector< sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacTlmProt > > bus_master_port;
    //sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacTlmProt > bus_master_port[CPU_NUM];
    /*---------------------------------------------------------
       nIRQ signals
     ----------------------------------------------------------*/    
    sc_core::sc_vector< sc_core::sc_in< bool > > nIRQ;
    //sc_core::sc_in< bool > nIRQ[CPU_NUM];
        
    MCProcessor_HW(const sc_core::sc_module_name name,
		    sc_dt::uint64 simulation_quantum, int NodeID);
    ~MCProcessor_HW();
    void* getLwipCtxt(){
	return CPU_HAL->getLwipCtxt();
    }
    void dumpStatJson(){
	CPU_HAL->dumpStatJson();
    }
private:
    /*---------------------------------------------------------
       CPU HAL LAYER
     ----------------------------------------------------------*/ 
    MCProcessor_HAL< INTR_NUM, CPU_NUM >* CPU_HAL;
    /*---------------------------------------------------------
       CPU Interrupt Interfaces
     ----------------------------------------------------------*/     
    std::vector< Interrupt_Interface* > CPU_Interrupt_Interface;

};

template< int INTR_NUM, int CPU_NUM >
MCProcessor_HW< INTR_NUM, CPU_NUM >::MCProcessor_HW(const sc_core::sc_module_name name,
					      sc_dt::uint64 simulation_quantum, int NodeID)
   :sc_core::sc_module(name)
{
    bus_master_port.init(CPU_NUM);
    nIRQ.init(CPU_NUM);
  
  	std::stringstream module_name;
    module_name << name << "_HAL";    
    CPU_HAL = new MCProcessor_HAL< INTR_NUM, CPU_NUM >(module_name.str().c_str(), simulation_quantum, NodeID);
    CPU_HAL->ctrl_out1(ctrl_out1);
    CPU_HAL->ctrl_out2(ctrl_out2);
    for (int cpu = 0; cpu < CPU_NUM; cpu++)   
       CPU_HAL->bus_master_port[cpu](bus_master_port[cpu]);
     
    for (int cpu = 0; cpu < CPU_NUM; cpu++)  {
        Interrupt_Interface* cpu_if;
        cpu_if = new Interrupt_Interface(sc_core::sc_gen_unique_name("Interrupt_Interface"), cpu);
        cpu_if->nIRQ(nIRQ[cpu]); 
        cpu_if->hal_port(CPU_HAL->hal_export);
        CPU_Interrupt_Interface.push_back(cpu_if);
    }     
}

template< int INTR_NUM, int CPU_NUM >
MCProcessor_HW< INTR_NUM, CPU_NUM >::~MCProcessor_HW()
{
    delete CPU_HAL;
    for(std::vector< Interrupt_Interface* >::iterator iter =  CPU_Interrupt_Interface.begin();
          iter !=  CPU_Interrupt_Interface.end(); ++iter) {
        delete *iter;
    }
    CPU_Interrupt_Interface.clear(); 
}

/******************************************************************************
 *    Processor TLM Layer << Processor + Interrupt Controller >>
 *   >> interfaces:: Bus master/slave ports + hardware interrupt signals <<
 ******************************************************************************/
template< int INTR_NUM, int CPU_NUM >
class MCProcessor
	:public sc_core::sc_module
{
 public:
    /*---------------------------------------------------------
       Port for controling simulation progress
     ----------------------------------------------------------*/
    sc_core::sc_port< sc_core::sc_fifo_out_if<int> > ctrl_out1; 
    sc_core::sc_port< sc_core::sc_fifo_out_if<int> > ctrl_out2; 
    /*---------------------------------------------------------
       Hardware interface
       >> MAC TLM PROT Master/Slave Interface
     ----------------------------------------------------------*/
    sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacTlmProt > MainBus_tlm_master_port; 
    sc_core::sc_port< HCSim::IAmbaAhbBusSlaveMacTlmProt > MainBus_tlm_slave_port;
    /*---------------------------------------------------------
       Interrupt interface
       >> Hardware interrupts 
     ----------------------------------------------------------*/    
	sc_core::sc_vector< sc_core::sc_port< HCSim::receive_if > > HINTR_tlm;
	//sc_core::sc_port< HCSim::receive_if > HINTR_tlm[INTR_NUM];
	
    MCProcessor(const sc_core::sc_module_name name, 
                                  sc_dt::uint64 simulation_quantum, int NodeID);
    ~MCProcessor();
    void* getLwipCtxt(){
	return CPU_HW->getLwipCtxt();
    }

    void dumpStatJson(){
	CPU_HW->dumpStatJson();
    }
 private:
    /*---------------------------------------------------------
       Interrupt Controller
     ----------------------------------------------------------*/ 
   HCSim::GenericIntrController< INTR_NUM, CPU_NUM >* GIC;
    /*---------------------------------------------------------
       CPU HW LAYER
     ----------------------------------------------------------*/ 
   MCProcessor_HW< INTR_NUM, CPU_NUM >* CPU_HW;
    /*---------------------------------------------------------
       Internal communication channels
       >> Master port wrapper / nIRQ signals
     ----------------------------------------------------------*/    
   HCSim::AmbaAhbMacTlm_MasterTLM_Wrap master_port_wrapper;
   sc_core::sc_vector< sc_core::sc_signal< bool > > nIRQ;
    //sc_core::sc_signal< bool > nIRQ[CPU_NUM];
};

template< int INTR_NUM, int CPU_NUM >
MCProcessor< INTR_NUM, CPU_NUM >::MCProcessor(const sc_core::sc_module_name name,
				 sc_dt::uint64 simulation_quantum, int NodeID)
   :sc_core::sc_module(name)
   ,master_port_wrapper("tlm_master_port_wrapper")
{
   HINTR_tlm.init(INTR_NUM);
   nIRQ.init(CPU_NUM);
   GIC = new HCSim::GenericIntrController<INTR_NUM, CPU_NUM> (sc_core::sc_gen_unique_name("GIC")) ;
   GIC->bus_slave_port(MainBus_tlm_slave_port);
   for (int intr = 0; intr < INTR_NUM; intr++) 
      GIC->HINT_tlm[intr](HINTR_tlm[intr]);   
   for (int cpu = 0; cpu < CPU_NUM; cpu++)      
      GIC->nIRQ[cpu](nIRQ[cpu]);
   /* Set interrupt target CPU IDs */
   GIC->setIntrTargetCPU(1, 0);
   GIC->setIntrTargetCPU(2, 1);    
   std::stringstream module_name;
   module_name << name << "_HW";
   CPU_HW = new MCProcessor_HW< INTR_NUM, CPU_NUM >(module_name.str().c_str(), simulation_quantum, NodeID);
   CPU_HW->ctrl_out1(ctrl_out1);
   CPU_HW->ctrl_out2(ctrl_out2);
   for (int cpu = 0; cpu < CPU_NUM; cpu++) {
        CPU_HW->bus_master_port[cpu](master_port_wrapper);
        CPU_HW->nIRQ[cpu](nIRQ[cpu]);
   }  
   master_port_wrapper.tlm_port(MainBus_tlm_master_port);
}

template< int INTR_NUM, int CPU_NUM >
MCProcessor< INTR_NUM, CPU_NUM >::~MCProcessor()
{
    delete CPU_HW;
    delete GIC;
}

#endif //SC_MCPROCESSOR__H
