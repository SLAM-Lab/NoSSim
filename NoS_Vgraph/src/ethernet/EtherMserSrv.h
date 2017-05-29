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

#ifndef __INET_ETHERMSERSRV_H
#define __INET_ETHERMSERSRV_H


#include <omnetpp.h>

#include <systemc>
#include "src/ethernet/EtherMser_m.h"

#include "NodeOperations.h"
#include "ModuleAccess.h"
#include "image_file.h"
using namespace ::sc_core;


//#include "mix_taskset/simhost.h"

#include "INETDefs.h"

#include "ILifecycle.h"
#include "LifecycleOperation.h"
#include "Ieee802Ctrl_m.h"
#include "MACAddress.h"
#include "NodeStatus.h"
#include "OSNode/top_module.h"
#include "lwip/lwip_ctxt.h"

#define MAX_REPLY_CHUNK_SIZE   1497


/**
 * Server-side process EtherMserCli.
 */
class INET_API EtherMserSrv : public cSimpleModule, public ILifecycle
{
  protected:
    int localSAP;
    int remoteSAP;
    NodeStatus *nodeStatus;

    // statistics
    long packetsSent;
    long packetsReceived;
    static simsignal_t sentPkSignal;
    static simsignal_t rcvdPkSignal;


    int srcSapTable;
    MACAddress srcAddrTable;

    MACAddress AddrNode0;
    MACAddress AddrNode1;
    MACAddress AddrNode2;


  public:
    //sc_model
    //sc_srv *test;
    artificial_example *System; 

    EtherMserSrv() 
    {

	//test = new sc_srv("sc_srv", this);
	System = new artificial_example ("mix_taskset_srv",  0); 
	System -> NetworkInterfaceCard1 -> OmnetWrapper = this;
	System -> NetworkInterfaceCard2 -> OmnetWrapper = this;

    }

    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback);

  protected:
    virtual void initialize(int stage);
    virtual int  numInitStages() const { return 4; }
    virtual void startApp();
    virtual void stopApp();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    virtual bool isNodeUp();

    virtual MACAddress resolveDestMACAddress();
    void registerDSAP(int dsap);
    void sendPacket(cPacket *datapacket, const MACAddress& destAddr, int destSap);
};

#endif
