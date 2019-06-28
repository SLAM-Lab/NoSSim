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

#include <stdio.h>
#include <string.h>

#include "SrvWrapper.h"


#include "inet/linklayer/common/Ieee802Ctrl.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/common/lifecycle/NodeStatus.h"


#include "inet/common/LayeredProtocolBase.h"
#include "inet/common/ModuleAccess.h"
#include "inet/linklayer/configurator/Ieee8021dInterfaceData.h"
#include "inet/linklayer/ieee8021d/relay/Ieee8021dRelay.h"
#include "inet/networklayer/common/InterfaceEntry.h"

namespace inet {

Define_Module(SrvWrapper);

simsignal_t SrvWrapper::sentPkSignal = registerSignal("sentPk");
simsignal_t SrvWrapper::rcvdPkSignal = registerSignal("rcvdPk");

void SrvWrapper::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        localSAP = par("localSAP");
        remoteSAP = par("remoteSAP");
        // statistics
        packetsSent = packetsReceived = 0;

        WATCH(packetsSent);
        WATCH(packetsReceived);
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        nodeStatus = dynamic_cast<NodeStatus *>(findContainingNode(this)->getSubmodule("status"));

        if (isNodeUp())
            startApp();
    }
}

bool SrvWrapper::isNodeUp()
{
    return !nodeStatus || nodeStatus->getState() == NodeStatus::UP;
}

void SrvWrapper::startApp()
{
    EV_INFO << "Starting application\n";
    bool registerSAP = par("registerSAP");
    if (registerSAP)
        registerDSAP(localSAP);
}

void SrvWrapper::stopApp()
{
    EV_INFO << "Stop the application\n";
}

void SrvWrapper::handleMessage(cMessage *msg)
{
    if (!isNodeUp())
        throw cRuntimeError("Application is not running");

     unsigned int lwip_pkt_size;
    EtherWrapperResp *datapacket;

    if (msg->isSelfMessage())
    {

        if (strcmp(msg->getName(), "ServerToCli") == 0) {
		System -> NetworkInterfaceCard1->notify_sending();	
                //std::cout<<"sending len from server is ... ... ... ... "<<( (OmnetIf_pkt*)(msg->getContextPointer()) )->getFileBufferArraySize()<<std::endl;	
		lwip_pkt_size = ((OmnetIf_pkt*)(msg->getContextPointer()))->getFileBufferArraySize();

		datapacket = new EtherWrapperResp("lwip_msg", IEEE802CTRL_DATA);
		datapacket->setFileBufferArraySize(lwip_pkt_size);
		datapacket->setByteLength( lwip_pkt_size);
		for(unsigned int ii=0; ii<lwip_pkt_size; ii++){
			datapacket->setFileBuffer(ii, ((OmnetIf_pkt*)(msg->getContextPointer()))->getFileBuffer(ii));
		}
        	sendPacket(datapacket, srcAddrTable, srcSapTable);
		delete msg; 
	} 
	return;
    }

    EV << "Received packet `" << msg->getName() << "'\n";

    std::cout << "Received packet `" << msg->getName() << "'\n";

//For packets recving in access point 
//    EtherFrame *frame = check_and_cast<EtherFrame *>(msg);
//    EtherWrapperResp *req = check_and_cast<EtherWrapperResp *>(frame->decapsulate());
//


    EtherWrapperResp *req = check_and_cast<EtherWrapperResp *>(msg);


    //std::cout << "req->getRequestId()... ... ... :       "<< req->getRequestId()<<std::endl;

    packetsReceived++;
    emit(rcvdPkSignal, req);


    char* image_buf;
    int buf_size =    req->getFileBufferArraySize();
    image_buf = (char*) malloc(buf_size);
    for(int ii=0; ii<buf_size; ii++){
	image_buf[ii]=req->getFileBuffer(ii);
    }
    System -> NetworkInterfaceCard1->notify_receiving(image_buf, req->getFileBufferArraySize());



    delete msg;

}

void SrvWrapper::sendPacket(cPacket *datapacket, const MACAddress& destAddr, int destSap)
{
    Ieee802Ctrl *etherctrl = new Ieee802Ctrl();
    etherctrl->setSsap(localSAP);
    etherctrl->setDsap(destSap);
    etherctrl->setDest(resolveDestMACAddress());
    datapacket->setControlInfo(etherctrl);
    emit(sentPkSignal, datapacket);
    send(datapacket, "out");
    packetsSent++;
}

void SrvWrapper::registerDSAP(int dsap)
{
    EV_DEBUG << getFullPath() << " registering DSAP " << dsap << "\n";

    Ieee802Ctrl *etherctrl = new Ieee802Ctrl();
    etherctrl->setDsap(dsap);
    cMessage *msg = new cMessage("register_DSAP", IEEE802CTRL_REGISTER_DSAP);
    msg->setControlInfo(etherctrl);

    send(msg, "out");
}

bool SrvWrapper::handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
{
    Enter_Method_Silent();
    if (dynamic_cast<NodeStartOperation *>(operation)) {
        if ((NodeStartOperation::Stage)stage == NodeStartOperation::STAGE_APPLICATION_LAYER)
            startApp();
    }
    else if (dynamic_cast<NodeShutdownOperation *>(operation)) {
        if ((NodeShutdownOperation::Stage)stage == NodeShutdownOperation::STAGE_APPLICATION_LAYER)
            stopApp();
    }
    else if (dynamic_cast<NodeCrashOperation *>(operation)) {
        if ((NodeCrashOperation::Stage)stage == NodeCrashOperation::STAGE_CRASH)
            stopApp();
    }
    else
        throw cRuntimeError("Unsupported lifecycle operation '%s'", operation->getClassName());
    return true;
}


MACAddress SrvWrapper::resolveDestMACAddress()
{
    MACAddress AddrNode0;
    MACAddress AddrNode1;
    MACAddress AddrNode2;
    MACAddress AddrNode3;
    MACAddress AddrAll;

    const char *addrNode0 = par("AddrNode0");
    const char *addrNode1 = par("AddrNode1");
    const char *addrNode2 = par("AddrNode2");
    const char *addrNode3 = par("AddrNode3");

    AddrNode0.tryParse(addrNode0);
    AddrNode1.tryParse(addrNode1);
    AddrNode2.tryParse(addrNode2);
    AddrNode3.tryParse(addrNode3);

    AddrAll.tryParse("ff:ff:ff:ff:ff:ff");



    return   AddrAll;

}


void SrvWrapper::finish()
{
    std::cout<<System->NodeID<<"	"<<std::endl;
}

} // namespace inet

