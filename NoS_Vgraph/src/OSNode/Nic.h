/*********************************************
 * Network Interface Card Model
 * Zhuoran Zhao, UT Austin, zhuoran@utexas.edu
 * Last update: July 2015
 ********************************************/
#include <systemc>
#include <string.h>

#include "HCSim.h"
#include "image_file.h"
#include "config.h"

#include <omnetpp.h>
#include "OmnetIf_pkt.h"


#ifndef SC_NIC__H
#define SC_NIC__H

class Nic : public sc_module {
   public:
	int syn_flag;
   	sc_core::sc_event syn_cond; 

	int NodeID;
   	sc_core::sc_event recvd; 
   	sc_core::sc_event sent; 

	char* data_send;
	int size_send;
	char* data_recv;
	int size_recv;
	cSimpleModule* OmnetWrapper;

	sc_port< sc_fifo_out_if<int> > size_out;
	sc_port< sc_fifo_in_if<int> > size_in;

	sc_port< sc_fifo_out_if<char*> > data_out;
	sc_port< sc_fifo_in_if<char*> > data_in;




	SC_HAS_PROCESS(Nic);

	Nic(sc_module_name name, int NodeID) : sc_module(name)
	{
		syn_flag = 0;
		this -> NodeID = NodeID;
		SC_THREAD(NicRecv);
		SC_THREAD(NicSend);
	}

	void NicSend()
	{

		while(1){
			size_send = size_in -> read();
			data_send = data_in -> read();
			//Sending packets out to MAC layer

			cSimpleModule* wrapper = (cSimpleModule*)(OmnetWrapper);
			cContextSwitcher dummy1(wrapper); //VERY IMPORTANT
			OmnetIf_pkt* pkt = new OmnetIf_pkt();
			pkt->setFileBufferArraySize(size_send);
			    for(int ii=0; ii<size_send; ii++){
				pkt->setFileBuffer(ii, ((char*)data_send)[ii]);
			    }
			cMessage *startMsg = new cMessage("ServerToCli");
			startMsg->setContextPointer(pkt);
			wrapper->scheduleAt(simTime(), startMsg);  //Notify immediately
			sc_core::wait(sent);
		}




	}
	void NicRecv()
	{
		while (1) {
			sc_core::wait(recvd);
			size_out -> write(size_recv);
			data_out -> write(data_recv);
	    	}   
	}



	void notify_sending(){  
		sent.notify(sc_core::SC_ZERO_TIME); 
	}
	void notify_receiving(char* fileBuffer, unsigned int size){  
        	data_recv = fileBuffer;
		size_recv = size;
		recvd.notify(sc_core::SC_ZERO_TIME); 
	}


	void syn_wait()
	{
		if(syn_flag == 0)
			sc_core::wait(syn_cond);
		syn_flag = 0;





	}
	void syn_send()
	{
			cSimpleModule* wrapper = (cSimpleModule*)(OmnetWrapper);
			cContextSwitcher dummy1(wrapper); //VERY IMPORTANT
			OmnetIf_pkt* pkt = new OmnetIf_pkt();
			int size_send = 1;
			pkt->setFileBufferArraySize(size_send);
			    for(int ii=0; ii<size_send; ii++){
				pkt->setFileBuffer(ii, 1);
			    }
			cMessage *startMsg = new cMessage("syn_wait");
			startMsg->setContextPointer(pkt);
			wrapper->scheduleAt(simTime(), startMsg);  //Notify immediately
	}



};



#endif // SC_NIC__H


