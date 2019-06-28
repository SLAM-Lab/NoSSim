// Copyright (C) 2013 OpenSim Ltd.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
// Author: Benjamin Martin Seregi


#ifndef __INET_SMARTAP_H
#define __INET_SMARTAP_H

#include "inet/common/INETDefs.h"
#include "inet/networklayer/common/InterfaceTable.h"
#include "inet/linklayer/ethernet/switch/IMACAddressTable.h"
#include "inet/linklayer/ethernet/EtherFrame_m.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/common/lifecycle/NodeStatus.h"

#include "EtherWrapper_m.h"
#include "OmnetIf_pkt.h"
#include "OSNode/top_module.h"

namespace inet {

class INET_API SmartAP : public cSimpleModule, public ILifecycle
{
  public:
    artificial_example *System; 
    SmartAP(){
        System = new artificial_example ("mix_taskset_cli", 6); 
        System -> NetworkInterfaceCard1 -> OmnetWrapper = this;
    }


  protected:
    MACAddress bridgeAddress;
    IInterfaceTable *ifTable = nullptr;
    IMACAddressTable *macTable = nullptr;
    InterfaceEntry *ie = nullptr;
    bool isOperational = false;
    unsigned int portCount = 0;    // number of ports in the switch


  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;
    void sendAPPacket(EtherWrapperResp * datapacket);
    void handleAndDispatchFrame(EtherFrame *frame);

    virtual void start();
    virtual void stop();
    bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback) override;

    virtual InterfaceEntry *chooseInterface();
    virtual void finish() override;
};

} // namespace inet

#endif // ifndef __INET_SMARTAP_H

