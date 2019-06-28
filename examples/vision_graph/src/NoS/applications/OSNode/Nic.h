#include <omnetpp.h>
#include <systemc>
#include <string.h>

#include "HCSim.h"
#include "config.h"
#include "OmnetIf_pkt.h"

#include "netif/hcsim_if.h"

using namespace omnetpp;
#ifndef SC_NIC__H
#define SC_NIC__H

class Nic : public sc_module {
	bool stop_flag;
   public:
	int syn_flag;
   	sc_core::sc_event syn_cond; 
	int NodeID;
   	sc_core::sc_event recvd; 
   	sc_core::sc_event sent; 
   	sc_core::sc_event stop; 
	char* data_send;
	int size_send;
	char* data_recv;
	int size_recv;
	cSimpleModule* OmnetWrapper;
	sc_port< sc_fifo_out_if<int> > size_out;
	sc_port< sc_fifo_in_if<int> > size_in;
	sc_port< sc_fifo_out_if<char*> > data_out;
	sc_port< sc_fifo_in_if<char*> > data_in;
        
        sc_port< sc_fifo_in_if<int> > ctrl_in;

	SC_HAS_PROCESS(Nic);

	Nic(sc_module_name name, int NodeID) : sc_module(name){
		syn_flag = 0;
                stop_flag = false;
		this -> NodeID = NodeID;
		SC_THREAD(NicRecv);
		SC_THREAD(NicSend);
		SC_THREAD(NicCtrl);
	}
	void NicSend()
	{
	  while (1) {
		size_send = size_in -> read();
		data_send = data_in -> read();
                if(size_send==6 && strcmp(data_send, "Stop!")==0){
		   cSimpleModule* wrapper = (cSimpleModule*)(OmnetWrapper);
		   cContextSwitcher dummy1(wrapper); //VERY IMPORTANT
		   cMessage *startMsg = new cMessage("StopSimulation");
		   wrapper->scheduleAt(simTime(), startMsg);  //Notify immediately
                   break;	
                }

                //std::cout << "dest is" << get_dest_device_id(data_send, size_send) << std::endl;
		cSimpleModule* wrapper = (cSimpleModule*)(OmnetWrapper);
		cContextSwitcher dummy1(wrapper); //VERY IMPORTANT
		OmnetIf_pkt* pkt = new OmnetIf_pkt();
                pkt->DestNode = get_dest_device_id(data_send, size_send, NodeID);
		pkt->setFileBufferArraySize(size_send);
		for(int ii=0; ii<size_send; ii++){
			pkt->setFileBuffer(ii, ((char*)data_send)[ii]);
		}
		free(data_send);
		cMessage *startMsg = new cMessage("ServerToCli");
		startMsg->setContextPointer(pkt);
		wrapper->scheduleAt(simTime(), startMsg);  //Notify immediately
                if(!stop_flag){
		   wait(this->sent | this->stop);
                }
                if(stop_flag){
		   cSimpleModule* wrapper = (cSimpleModule*)(OmnetWrapper);
		   cContextSwitcher dummy1(wrapper); //VERY IMPORTANT
		   cMessage *startMsg = new cMessage("StopSimulation");
		   wrapper->scheduleAt(simTime(), startMsg);  //Notify immediately
                }
	  }
	}
	void NicRecv(){
            while (1) {
               sc_core::wait(recvd);
               size_out -> write(size_recv);
               data_out -> write(data_recv);
            }   
	}
        void NicCtrl(){
	     ctrl_in -> read();
             stop_flag = true;
	     stop.notify(); 
             size_out -> write(6);
             char* msg = (char*)malloc(6);
             strcpy(msg, "Stop!");
             data_out -> write(msg);
        }

	void notify_sending(){  
		sent.notify(); 
	}

	void notify_receiving(char* fileBuffer, unsigned int size){  
        	data_recv = fileBuffer;
		size_recv = size;
		recvd.notify(); 
	}


};



#endif // SC_NIC__H


