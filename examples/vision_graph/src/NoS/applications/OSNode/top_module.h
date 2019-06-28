#include "MCProcessor.h"
#include "NicDriver.h"
#include "Nic.h"


#ifndef TOP_MODULE__H
#define TOP_MODULE__H

class artificial_example
    :sc_core::sc_module
{
 public:

    int NodeID; 


    Nic* NetworkInterfaceCard1;
    Nic* NetworkInterfaceCard2;



    artificial_example(const sc_core::sc_module_name name, int NodeID)
        :sc_core::sc_module(name)
        ,busSlaveMacLink_1("busSlaveMacLink_1")
        ,busSlaveMacLink_2("busSlaveMacLink_2")
	,data_send_fifo1(5000)
	,size_send_fifo1(5000)
	,data_recv_fifo1(5000)
	,size_recv_fifo1(5000)
	,data_send_fifo2(5000)
	,size_send_fifo2(5000)
	,data_recv_fifo2(5000)
    	,size_recv_fifo2(5000)
    	,control_fifo1(5000)
    	,control_fifo2(5000)
    {

        MainBus_HINT.init(CON_INTR_NUM);
        this->NodeID = NodeID;
        MainBus = new  HCSim::AmbaAhbMacTlm("MainBus");
        
	//MainBus -> ID  = NodeID;
        CPU = new MCProcessor<CON_INTR_NUM ,CON_CPU_NUM> ("DualCore_CPU", HCSim::OS_INFINIT_VAL, NodeID);
        CPU->ctrl_out1(control_fifo1);
        CPU->ctrl_out2(control_fifo2);
        CPU->MainBus_tlm_master_port(*MainBus);
        CPU->MainBus_tlm_slave_port(*MainBus);

        for (int intr = 0; intr < CON_INTR_NUM ; intr++)
            CPU->HINTR_tlm[intr](MainBus_HINT[intr]);
        
	busSlaveMacLink_1.protWMac(*MainBus);
        busSlaveMacLink_2.protWMac(*MainBus);
        
        intr_gen_1 = new NicDriver("intr_gen_1", 1, const_intr1_address, const_intr1_load, const_end_time, 1, NodeID);  
        intr_gen_1->Port_tlm(busSlaveMacLink_1);
        intr_gen_1->MainBus_HINT(MainBus_HINT[1]);

	intr_gen_2 = new NicDriver("intr_gen_2", 2, const_intr2_address, const_intr2_load, const_end_time, 1, NodeID);
	intr_gen_2->Port_tlm(busSlaveMacLink_2);
	intr_gen_2->MainBus_HINT(MainBus_HINT[2]);


	NetworkInterfaceCard1 =  new Nic("nic1", NodeID); 
	NetworkInterfaceCard2 =  new Nic("nic2", NodeID); 

        NetworkInterfaceCard1 -> ctrl_in(control_fifo1);
        NetworkInterfaceCard2 -> ctrl_in(control_fifo2);

	intr_gen_1 -> size_out(size_send_fifo1); 
	NetworkInterfaceCard1 -> size_in(size_send_fifo1); 
	intr_gen_1 -> size_in(size_recv_fifo1); 
	NetworkInterfaceCard1 -> size_out(size_recv_fifo1); 
	intr_gen_1 -> data_out(data_send_fifo1); 
	NetworkInterfaceCard1 -> data_in(data_send_fifo1); 
	intr_gen_1 -> data_in(data_recv_fifo1); 
	NetworkInterfaceCard1 -> data_out(data_recv_fifo1); 

	intr_gen_2 -> size_out(size_send_fifo2); 
	NetworkInterfaceCard2 -> size_in(size_send_fifo2); 
	intr_gen_2 -> size_in(size_recv_fifo2); 
	NetworkInterfaceCard2 -> size_out(size_recv_fifo2); 
	intr_gen_2 -> data_out(data_send_fifo2); 
	NetworkInterfaceCard2 -> data_in(data_send_fifo2); 
	intr_gen_2 -> data_in(data_recv_fifo2); 
	NetworkInterfaceCard2 -> data_out(data_recv_fifo2); 

	    
	intr_hmonitor_intr1 = new HW_Standard_interruptHandlerMonitor("handler_mon_1", 1, const_intr1_handler_address, const_intr1_load, const_end_time, 1);
	intr_hmonitor_intr1->Port_tlm(busSlaveMacLink_1);
	intr_hmonitor_intr2 = new HW_Standard_interruptHandlerMonitor("handler_mon_2", 2, const_intr2_handler_address, const_intr2_load, const_end_time,  1);
        intr_hmonitor_intr2->Port_tlm(busSlaveMacLink_2);
        
	intr_tmonitor_intr1 = new HW_Standard_signalHandlerMonitor("signal_mon_1" ,1,  const_intr1_task_address, const_intr1_load, const_end_time, 1);
	intr_tmonitor_intr1->Port_tlm(busSlaveMacLink_1);
	intr_tmonitor_intr2 = new HW_Standard_signalHandlerMonitor("signal_mon_2", 2,  const_intr2_task_address, const_intr2_load, const_end_time, 1);
	intr_tmonitor_intr2->Port_tlm(busSlaveMacLink_2);
           
    }    
    ~artificial_example()
    {
        delete CPU;
        delete MainBus;
        delete intr_gen_1;
        delete intr_gen_2;
        delete intr_hmonitor_intr1;
        delete intr_hmonitor_intr2;
        delete intr_tmonitor_intr1;
        delete intr_tmonitor_intr2;
    }
    void* getLwipCtxt(){
	return CPU->getLwipCtxt();
    }
    void dumpStatJson(){
	CPU->dumpStatJson();
    }
 private:
    NicDriver* intr_gen_1;
    NicDriver* intr_gen_2;
    sc_fifo<char* > data_send_fifo1;
    sc_fifo<int > size_send_fifo1;
    sc_fifo<char* > data_recv_fifo1;
    sc_fifo<int > size_recv_fifo1;

    sc_fifo<char* > data_send_fifo2;
    sc_fifo<int > size_send_fifo2;
    sc_fifo<char* > data_recv_fifo2;
    sc_fifo<int > size_recv_fifo2;

    sc_fifo<int > control_fifo1;
    sc_fifo<int > control_fifo2;
    // Channels 
    sc_core::sc_vector< HCSim::handshake_ch  > MainBus_HINT;
    //HCSim::handshake_ch MainBus_HINT[CON_INTR_NUM];
    // TLM AMBA AHB bus model
    HCSim::AmbaAhbMacTlm* MainBus;
    // TLM MultiCore Processpr model
    MCProcessor<CON_INTR_NUM, CON_CPU_NUM>* CPU;
    //Interrupt resources
 
    HCSim::AmbaAhbBusSlaveMacLinkPass busSlaveMacLink_1;
    HCSim::AmbaAhbBusSlaveMacLinkPass busSlaveMacLink_2;

    HW_Standard_interruptHandlerMonitor* intr_hmonitor_intr1;
    HW_Standard_interruptHandlerMonitor* intr_hmonitor_intr2;
    HW_Standard_signalHandlerMonitor* intr_tmonitor_intr1;
    HW_Standard_signalHandlerMonitor* intr_tmonitor_intr2;

};    


#endif

