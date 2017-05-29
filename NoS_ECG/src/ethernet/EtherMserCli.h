/*
 * Copyright (C) 2003 Andras Varga; CTIE, Monash University, Australia
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __INET_ETHERMSERCLI_H
#define __INET_ETHERMSERCLI_H

#include <omnetpp.h>

#include <systemc>
#include "src/ethernet/EtherMser_m.h"

#include "NodeOperations.h"
#include "ModuleAccess.h"
#include "image_file.h"
using namespace ::sc_core;





#include "INETDefs.h"
#include "MACAddress.h"
#include "NodeStatus.h"
#include "Ieee802Ctrl_m.h"


#include "ILifecycle.h"



#include <stdint.h>
#include "OSNode/top_module.h"
#include "lwip/lwip_ctxt.h"

extern int TotalClients;


/**
 * Simple traffic generator for the Ethernet model.
 */
class INET_API EtherMserCli : public cSimpleModule, public ILifecycle
{
  protected:
    enum Kinds {START=100, NEXT};

    // send parameters
    long seqNum;
    cPar *reqLength;
    cPar *respLength;
    cPar *sendInterval;

    int localSAP;
    int remoteSAP;
    MACAddress destMACAddress;
    NodeStatus *nodeStatus;

    // self messages
    cMessage *timerMsg;
    simtime_t startTime;
    simtime_t stopTime;

    // receive statistics
    long packetsSent;
    long packetsReceived;
    static simsignal_t sentPkSignal;
    static simsignal_t rcvdPkSignal;


    MACAddress AddrNode0;
    MACAddress AddrNode1;
    MACAddress AddrNode2;

  public:

    artificial_example *System; 


    EtherMserCli() 
    {
	    reqLength = NULL;
	    respLength = NULL;
	    sendInterval = NULL;
	    timerMsg = NULL;
	    nodeStatus = NULL;
	System = new artificial_example ("mix_taskset_cli", TotalClients); 
	TotalClients++;
	System -> NetworkInterfaceCard1 -> OmnetWrapper = this;
	System -> NetworkInterfaceCard2 -> OmnetWrapper = this;

    }

    virtual ~EtherMserCli();

    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback);

  protected:
    virtual void initialize(int stage);
    virtual int numInitStages() const { return 4; }
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    virtual bool isNodeUp();
    virtual bool isGenerator();
    //virtual void scheduleNextPacket(bool start);
    virtual void cancelNextPacket();

    virtual MACAddress resolveDestMACAddress();

    virtual void sendPacket(cMessage *msg);
    virtual void receivePacket(cPacket *msg);
    virtual void registerDSAP(int dsap);
};

#endif
