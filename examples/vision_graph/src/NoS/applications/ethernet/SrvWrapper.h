//
// Copyright (C) 2003 Andras Varga; CTIE, Monash University, Australia
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INET_SRVWRAPPER_H
#define __INET_SRVWRAPPER_H

#include "inet/common/INETDefs.h"

#include "inet/linklayer/common/MACAddress.h"
#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/common/lifecycle/LifecycleOperation.h"
#include "inet/common/lifecycle/NodeStatus.h"

#include "top_module.h"
#include "OmnetIf_pkt.h"
#include "EtherWrapper_m.h"

using namespace ::sc_core;

namespace inet {

#define MAX_REPLY_CHUNK_SIZE    1497


class INET_API SrvWrapper : public cSimpleModule, public ILifecycle
{
  protected:
    int localSAP = 0;
    int remoteSAP = 0;
    NodeStatus *nodeStatus = nullptr;

    // statistics
    long packetsSent = 0;
    long packetsReceived = 0;
    static simsignal_t sentPkSignal;
    static simsignal_t rcvdPkSignal;


    int srcSapTable;
    MACAddress srcAddrTable;

    MACAddress AddrNode0;
    MACAddress AddrNode1;
    MACAddress AddrNode2;

  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void startApp();
    virtual void stopApp();
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback) override;

    virtual bool isNodeUp();

    virtual MACAddress resolveDestMACAddress();
    void registerDSAP(int dsap);
    void sendPacket(cPacket *datapacket, const MACAddress& destAddr, int destSap);

  public:
    //sc_model
    //sc_srv *test;
    artificial_example *System; 

    SrvWrapper() 
    {

	//test = new sc_srv("sc_srv", this);
	System = new artificial_example ("mix_taskset_srv",  0); 
	System -> NetworkInterfaceCard1 -> OmnetWrapper = this;
	//System -> NetworkInterfaceCard2 -> OmnetWrapper = this;

    }
    ~SrvWrapper(){}

};

} // namespace inet

#endif // ifndef __INET_SRVWRAPPER_H

