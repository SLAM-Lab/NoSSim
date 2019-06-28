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
#include "OSNode/top_module.h"

#ifndef __INET_CLIWRAPPER_H
#define __INET_CLIWRAPPER_H

#include "inet/common/INETDefs.h"
#include "inet/linklayer/common/MACAddress.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/lifecycle/ILifecycle.h"

#include "EtherWrapper_m.h"
#include "OmnetIf_pkt.h"

using namespace ::sc_core;

extern int TotalClients;
extern long total_sent_pkts[10];
extern long total_recvd_pkts[10];

namespace inet {

/**
 * Simple traffic generator for the Ethernet model.
 */
class INET_API CliWrapper : public cSimpleModule, public ILifecycle
{
  protected:

    // send parameters
    long seqNum = 0;
    int clientID = 1;
    MACAddress destMACAddress;
    NodeStatus *nodeStatus = nullptr;

    // receive statistics
    long packetsSent = 0;
    long packetsReceived = 0;
    static simsignal_t sentPkSignal;
    static simsignal_t rcvdPkSignal;


    MACAddress AddrNode0;
    MACAddress AddrNode1;
    MACAddress AddrNode2;

  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void startApp();
    virtual void stopApp();
    virtual bool isNodeUp();
    virtual void finish() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback) override;


    virtual MACAddress resolveDestMACAddress(int dest_id);
    virtual void sendPacket(cMessage *msg, int dest_id);
    virtual void receivePacket(cPacket *msg);


  public:
    artificial_example *System; 

    CliWrapper(){
        for(int i = 0; i < 10; i++){      
          total_sent_pkts[i] = 0;
          total_recvd_pkts[i] = 0;
        }
        System = new artificial_example ("mix_taskset_cli", TotalClients); 
        TotalClients++;
        System -> NetworkInterfaceCard1 -> OmnetWrapper = this;
    }
    ~CliWrapper(){}
};

} // namespace inet

#endif // ifndef __INET_CLIWRAPPER_H


