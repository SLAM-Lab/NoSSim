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

#include "inet/common/LayeredProtocolBase.h"
#include "inet/common/ModuleAccess.h"
#include "inet/linklayer/common/Ieee802Ctrl.h"
#include "inet/linklayer/configurator/Ieee8021dInterfaceData.h"
#include "SmartAP.h"
#include "inet/networklayer/common/InterfaceEntry.h"

namespace inet {

Define_Module(SmartAP);


void SmartAP::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {

        // number of ports
        portCount = gate("ifOut", 0)->size();
        if (gate("ifIn", 0)->size() != (int)portCount)
            throw cRuntimeError("the sizes of the ifIn[] and ifOut[] gate vectors must be the same");
    }
    else if (stage == INITSTAGE_LINK_LAYER_2) {
        NodeStatus *nodeStatus = dynamic_cast<NodeStatus *>(findContainingNode(this)->getSubmodule("status"));
        isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        macTable = check_and_cast<IMACAddressTable *>(getModuleByPath(par("macTablePath")));
        ifTable = check_and_cast<IInterfaceTable *>(getModuleByPath(par("interfaceTablePath")));
        if (isOperational) {
            ie = chooseInterface();
            if (ie)
                bridgeAddress = ie->getMacAddress(); // get the bridge's MAC address
            else
                throw cRuntimeError("No non-loopback interface found!");
        }
    }
}

void SmartAP::handleMessage(cMessage *msg)
{
    if (!isOperational) {
        EV_ERROR << "Message '" << msg << "' arrived when module status is down, dropped it." << endl;
        delete msg;
        return;
    }

    if (!msg->isSelfMessage()) {
        // messages from network
        if (strcmp(msg->getArrivalGate()->getName(), "ifIn") == 0) {
            EtherFrame *frame = check_and_cast<EtherFrame *>(msg);
            handleAndDispatchFrame(frame);
        }
    }
    else{
        if (strcmp(msg->getName(), "ServerToCli") == 0) {
		int lwip_pkt_size = ((OmnetIf_pkt*)(msg->getContextPointer()))->getFileBufferArraySize();
		EtherWrapperResp* datapacket = new EtherWrapperResp("lwip_msg", IEEE802CTRL_DATA);
		datapacket->setFileBufferArraySize(lwip_pkt_size);
		datapacket->setByteLength( lwip_pkt_size);
		for(int ii=0; ii<lwip_pkt_size; ii++){
			datapacket->setFileBuffer(ii, ((OmnetIf_pkt*)(msg->getContextPointer()))->getFileBuffer(ii));
		}
		sendAPPacket(datapacket);
		System -> NetworkInterfaceCard1->notify_sending();
		delete(msg);
	} 
    }
    
}



void SmartAP::handleAndDispatchFrame(EtherFrame *frame)
{
    //int arrivalGate = frame->getArrivalGate()->getIndex();
    //std::cout << "Recving infomation at AP ... ... ..." << std::endl;
    //std::cout << "IP Address of gateway is: " << bridgeAddress << std::endl;
    //std::cout << "The destination is" << frame->getDest() << std::endl;
    EtherWrapperResp *datapacket = check_and_cast<EtherWrapperResp *>(frame->getEncapsulatedPacket());
    
    char* image_buf;
    int buf_size =    datapacket->getFileBufferArraySize();
    image_buf = (char*) malloc(buf_size);
    for(int ii=0; ii<buf_size; ii++){
	image_buf[ii]=datapacket->getFileBuffer(ii);
    }
    System -> NetworkInterfaceCard1->notify_receiving(image_buf, datapacket->getFileBufferArraySize());
    delete frame;
}

void SmartAP::sendAPPacket(EtherWrapperResp * datapacket)
{
    int localSAP = 0xf0;
    int remoteSAP = 0xf1;
    char msgname[30];
    sprintf(msgname, "AP-msg");
    datapacket->setName(msgname);   
    datapacket->setRequestId(0);
    Ieee802Ctrl *etherctrl = new Ieee802Ctrl();
    etherctrl->setSsap(localSAP);
    etherctrl->setDsap(remoteSAP);
    etherctrl->setDest(MACAddress("ff:ff:ff:ff:ff:ff"));
    datapacket->setControlInfo(etherctrl);

    Ieee802Ctrl *controlInfo = check_and_cast<Ieee802Ctrl *>(datapacket->removeControlInfo());
    unsigned int portNum = 0;
    MACAddress address = controlInfo->getDest();
    delete controlInfo;

    EthernetIIFrame *frame = new EthernetIIFrame(datapacket->getName());
    frame->setSrc(bridgeAddress);
    frame->setDest(address);
    frame->setByteLength(ETHER_MAC_FRAME_BYTES);
    frame->setEtherType(-1);
    frame->encapsulate(datapacket);

    if (frame->getByteLength() < MIN_ETHERNET_FRAME_BYTES)
        frame->setByteLength(MIN_ETHERNET_FRAME_BYTES);

    send(frame, "ifOut", portNum);    
}
/*
    Ieee802Ctrl *etherctrl = new Ieee802Ctrl();
    etherctrl->setSsap(localSAP);
    etherctrl->setDsap(destSap);
    etherctrl->setDest(resolveDestMACAddress());
    datapacket->setControlInfo(etherctrl);
    emit(sentPkSignal, datapacket);
    send(datapacket, "out");
    packetsSent++;
*/

void SmartAP::start()
{
    isOperational = true;

    ie = chooseInterface();
    if (ie)
        bridgeAddress = ie->getMacAddress(); // get the bridge's MAC address
    else
        throw cRuntimeError("No non-loopback interface found!");

    macTable->clearTable();
}

void SmartAP::stop()
{
    isOperational = false;

    macTable->clearTable();
    ie = nullptr;
}

InterfaceEntry *SmartAP::chooseInterface()
{
    IInterfaceTable *ift = check_and_cast<IInterfaceTable *>(getModuleByPath(par("interfaceTablePath")));
    for (int i = 0; i < ift->getNumInterfaces(); i++) {
        InterfaceEntry *current = ift->getInterface(i);
        if (!current->isLoopback())
            return current;
    }
    return nullptr;
}

void SmartAP::finish()
{

}

bool SmartAP::handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
{
    Enter_Method_Silent();

    if (dynamic_cast<NodeStartOperation *>(operation)) {
        if ((NodeStartOperation::Stage)stage == NodeStartOperation::STAGE_LINK_LAYER) {
            start();
        }
    }
    else if (dynamic_cast<NodeShutdownOperation *>(operation)) {
        if ((NodeShutdownOperation::Stage)stage == NodeShutdownOperation::STAGE_LINK_LAYER) {
            stop();
        }
    }
    else if (dynamic_cast<NodeCrashOperation *>(operation)) {
        if ((NodeCrashOperation::Stage)stage == NodeCrashOperation::STAGE_CRASH) {
            stop();
        }
    }
    else {
        throw cRuntimeError("Unsupported operation '%s'", operation->getClassName());
    }

    return true;
}

} // namespace inet

