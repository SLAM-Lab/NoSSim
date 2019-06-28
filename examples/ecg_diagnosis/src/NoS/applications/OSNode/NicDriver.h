/*********************************************
 * Network Interface Card Driver Model
 * Zhuoran Zhao, UT Austin, zhuoran@utexas.edu
 * Last update: July 2015
 ********************************************/
#include <systemc>
#include <string.h>

#include "HCSim.h"
#include "config.h"

#ifndef SC_NICPROXY__H
#define SC_NICPROXY__H


using namespace sc_core;
class NicDriver
    :public sc_core::sc_module
{
 public:

    unsigned int MacAddr;


/*
    sc_core::sc_port< HCSim::IAmbaAhbMacTlm_MasterSlave > NetworkPortCli2Srv;
    sc_core::sc_port< HCSim::IAmbaAhbMacTlm_MasterSlave > NetworkPortSrv2Cli;
*/
    sc_core::sc_port< sc_fifo_out_if<int> > size_out;
    sc_core::sc_port< sc_fifo_in_if<int> > size_in;

    sc_core::sc_port< sc_fifo_out_if<char*> > data_out;
    sc_core::sc_port< sc_fifo_in_if<char*> > data_in;

    
    sc_core::sc_port< HCSim::IAmbaAhbBusSlaveMacLink > Port_tlm;
    sc_core::sc_port < HCSim::send_if > MainBus_HINT;
    
    SC_HAS_PROCESS(NicDriver );
    NicDriver (const sc_core::sc_module_name name, int id, unsigned int addr, 
            sc_dt::uint64 intr_period, sc_dt::uint64 end_time, int enabled, int NetworkNodeID)
        :sc_core::sc_module(name)
    {
        this->id = id;
        this->addr = addr;
        this->intr_period = intr_period;
        this->end_time = end_time;
        this->enabled = enabled;
	this->NetworkNodeID = NetworkNodeID;
        SC_THREAD(dataBuffer);
        SC_THREAD(generator);  

    }          
    ~NicDriver() {}
    void PacketArrived(void){
      PacketArrival.notify(sc_core::SC_ZERO_TIME);
    }

     
 private:
    int id;
    unsigned long addr;
    sc_dt::uint64 intr_period;
    sc_dt::uint64 end_time;
    int enabled;
    sc_core::sc_event PacketArrival;    
    int NetworkNodeID;


    void dataBuffer(void){
        char* data;
	int size;
	while(1){
			Port_tlm->slaveRead(addr, &size, sizeof(int) );
			data = (char*) malloc(size);
			Port_tlm->slaveRead(addr, data, size*sizeof(char) );
			//std::cout << "Writing size is: " << size << std::endl;
			size_out->write(size);
			data_out->write(data);
	}
    }


    void generator(void){
	char* data;
	int size;
     	while (1) {
			size = size_in->read();
			data = data_in->read();
			if(size==6 && strcmp(data, "Stop!")==0){
			   size_out->write(size);
			   data_out->write(data);	
			   break;
                        }
	    		MainBus_HINT->send(); // send an interrupt
	    		Port_tlm->slaveWrite(addr+1, &size, sizeof(int) );
	    		MainBus_HINT->send(); // send an interrupt
	    		Port_tlm->slaveWrite(addr+1, data, size*sizeof(char) );

    	}   

    }


};

class HW_Standard_interruptHandlerMonitor //Reserve for debugging purpose 
    :sc_core::sc_module
{   
 public:
    sc_core::sc_port< HCSim::IAmbaAhbBusSlaveMacLink > Port_tlm;
    
    SC_HAS_PROCESS(HW_Standard_interruptHandlerMonitor);
    HW_Standard_interruptHandlerMonitor(const sc_core::sc_module_name name, int id, unsigned int addr, 
            sc_dt::uint64 intr_period, sc_dt::uint64 end_time, int enabled)
        :sc_core::sc_module(name)
    {
        this->id = id;
        this->addr = addr;
        this->intr_period = intr_period;
        this->end_time = end_time;
        this->enabled = enabled;
        
        SC_THREAD(monitor);
    }    
    ~HW_Standard_interruptHandlerMonitor() {}
    
 private:    
	int id;
    sc_dt::uint64 end_time;
    sc_dt::uint64 intr_period;
    unsigned long addr;
    int enabled;
    
	
    void monitor(void)
    {
    }
    
};
 /*--------------------------------------------------------------------------------------------------------------------------*/
class HW_Standard_signalHandlerMonitor //Reserve for debugging purpose
    :sc_core::sc_module
{
 public:
    sc_core::sc_port< HCSim::IAmbaAhbBusSlaveMacLink > Port_tlm;

    SC_HAS_PROCESS(HW_Standard_signalHandlerMonitor);
    HW_Standard_signalHandlerMonitor(const sc_core::sc_module_name name,
            int id, unsigned int addr, sc_dt::uint64 intr_period, sc_dt::uint64 end_time, int enabled)
        :sc_core::sc_module(name)
    {
        this->id = id;
        this->addr = addr;
        this->intr_period = intr_period;
        this->end_time = end_time;
        this->enabled = enabled;

        SC_THREAD(monitor);
    }        
    ~HW_Standard_signalHandlerMonitor() {}
    
 private:    
    int id;
    sc_dt::uint64 end_time;
    sc_dt::uint64 intr_period;
    unsigned long addr;
    int enabled;
	
    void monitor(void)
    {
    }
    
};
/*--------------------------------------------------------------------------------------------------------------------------*/

#endif // SC_NICPROXY__H


