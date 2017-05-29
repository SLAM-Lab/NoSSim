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
#include <math.h>


#include "EtherMserCli.h"

#include "EtherMser_m.h"
#include "Ieee802Ctrl_m.h"
#include "NodeOperations.h"
#include "ModuleAccess.h"

#include "image_file.h"
#include "OmnetIf_pkt.h"

using namespace std;

int TotalClients = 1;


Define_Module(EtherMserCli);

simsignal_t EtherMserCli::sentPkSignal = registerSignal("sentPk");
simsignal_t EtherMserCli::rcvdPkSignal = registerSignal("rcvdPk");



EtherMserCli::~EtherMserCli()
{
    cancelAndDelete(timerMsg);
}

void EtherMserCli::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == 0)
    {
        reqLength = &par("reqLength");
        respLength = &par("respLength");
        sendInterval = &par("sendInterval");

        localSAP = par("localSAP");
        remoteSAP = par("remoteSAP");




        seqNum = 0;
        WATCH(seqNum);

        // statistics
        packetsSent = packetsReceived = 0;
        WATCH(packetsSent);
        WATCH(packetsReceived);

        startTime = par("startTime");
        stopTime = par("stopTime");
        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            error("Invalid startTime/stopTime parameters");

    }
    else if (stage == 3)
    {
        if (isGenerator())
            timerMsg = new cMessage("generateNextPacket");

        nodeStatus = dynamic_cast<NodeStatus *>(findContainingNode(this)->getSubmodule("status"));

/*
        if (isNodeUp() && isGenerator())
	    {scheduleNextPacket(true);}
*/
    }
}

void EtherMserCli::handleMessage(cMessage *msg)
{
    unsigned int lwip_pkt_size;
    EtherMserResp *datapacket;
    if (!isNodeUp())
        throw cRuntimeError("Application is not running");
    if (msg->isSelfMessage())
    {
/*
        if (msg->getKind() == START)
        {
            bool registerSAP = par("registerSAP");
            if (registerSAP)
                registerDSAP(localSAP);

            destMACAddress = resolveDestMACAddress();
            // if no dest address given, nothing to do
            if (destMACAddress.isUnspecified())
                return;
        }
*/

        bool registerSAP = par("registerSAP");
        if (registerSAP)
            registerDSAP(localSAP);



	//Copy the content from OmnetIf_pkt to a EtherMserResp package. 
        if (strcmp(msg->getName(), "ServerToCli") == 0) {
		System -> NetworkInterfaceCard1->notify_sending();		
		lwip_pkt_size = ((OmnetIf_pkt*)(msg->getContextPointer()))->getFileBufferArraySize();

		datapacket = new EtherMserResp("lwip_msg", IEEE802CTRL_DATA);
		datapacket->setFileBufferArraySize(lwip_pkt_size);
		datapacket->setByteLength( lwip_pkt_size);


		for(unsigned int ii=0; ii<lwip_pkt_size; ii++){
			datapacket->setFileBuffer(ii, ((OmnetIf_pkt*)(msg->getContextPointer()))->getFileBuffer(ii));
		}

		destMACAddress = resolveDestMACAddress();
		    // if no dest address given, nothing to do
		if (destMACAddress.isUnspecified())
		    return;

//     	        if ((((LwipCntxt*)   (System->getLwipCtxt()) )->NodeID)==2){
//			std::cout << "===EtherMserCli sending to the node ID :" << lwip_pkt_size<<std::endl;    
//		}
       		sendPacket(datapacket); 
		delete msg; 
	} 




        //scheduleNextPacket(false);
    }
    else
        receivePacket(check_and_cast<cPacket*>(msg));
}

bool EtherMserCli::handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
{

    Enter_Method_Silent();
    if (dynamic_cast<NodeStartOperation *>(operation)) {
        if (stage == NodeStartOperation::STAGE_APPLICATION_LAYER && isGenerator())
	    {
		cout<<"handleOperationStage ... ... ... ... ..."<<endl;
		//scheduleNextPacket(true);
	    }
    }
    else if (dynamic_cast<NodeShutdownOperation *>(operation)) {
        if (stage == NodeShutdownOperation::STAGE_APPLICATION_LAYER)
            cancelNextPacket();
    }
    else if (dynamic_cast<NodeCrashOperation *>(operation)) {
        if (stage == NodeCrashOperation::STAGE_CRASH)
            cancelNextPacket();
    }
    else throw cRuntimeError("Unsupported lifecycle operation '%s'", operation->getClassName());
    return true;
}

bool EtherMserCli::isNodeUp()
{
    return !nodeStatus || nodeStatus->getState() == NodeStatus::UP;
}

bool EtherMserCli::isGenerator()
{
    return par("destAddress").stringValue()[0];
}
/*
void EtherMserCli::scheduleNextPacket(bool start)
{
    simtime_t cur = simTime();
    simtime_t next;
    if (start)
    {
        next = cur <= startTime ? startTime : cur;
        timerMsg->setKind(START);
    }
    else
    {
        //std::cout<<"Interval is ... : "<<sendInterval->doubleValue()<<endl;
        next = cur + sendInterval->doubleValue();
        timerMsg->setKind(NEXT);
    }
    if (stopTime < SIMTIME_ZERO || next < stopTime)
    {
        std::cout<<"Scheduling packet at simTime: "<<next<<endl;
        scheduleAt(next, timerMsg);
    }
}
*/
void EtherMserCli::cancelNextPacket()
{
    if (timerMsg)
        cancelEvent(timerMsg);
}

MACAddress EtherMserCli::resolveDestMACAddress()
{
//        if ((((LwipCntxt*)   (System->getLwipCtxt()) )->NodeID)==2)
//	   std::cout << "resolveDestMACAddress " << (((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID) << std::endl;    
//    std::cout << "Sending to the node ID :" << (((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)<<std::endl;
//    MACAddress destMACAddress;
//    const char *destAddress = par("destAddress");
//    if (destAddress[0])
//    {

//        if (!destMACAddress.tryParse(destAddress))

/*
        {
            cModule *destStation = simulation.getModuleByPath(destAddress);
            if (!destStation)
                error("cannot resolve MAC address '%s': not a 12-hex-digit MAC address or a valid module path name", destAddress);

            cModule *destMAC = destStation->getSubmodule("mac");
            if (!destMAC)
                error("module '%s' has no 'mac' submodule", destAddress);

            destMACAddress.setAddress(destMAC->par("address"));
        }
    }
*/

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

   // return AddrAll;
    if ((((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)==0){
	//(((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)=-1;
	return AddrNode0;
    }

    if ((((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)==1){
	//(((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)=-1;
	return AddrNode1;
	//return AddrAll;
    }
    if ((((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)==2){
	//(((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)=-1;
	//printf("==============================Address is node 2===============================\n");
	//return AddrAll;
	return AddrNode2;
    }
      if ((((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)==3){
	//(((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)=-1;
	//printf("==============================Address is node 2===============================\n");
	//return AddrAll;
	return AddrNode3;
    }  

    return AddrAll;
}

void EtherMserCli::registerDSAP(int dsap)
{
    EV << getFullPath() << " registering DSAP " << dsap << "\n";

    Ieee802Ctrl *etherctrl = new Ieee802Ctrl();
    etherctrl->setDsap(dsap);
    cMessage *msg = new cMessage("register_DSAP", IEEE802CTRL_REGISTER_DSAP);
    msg->setControlInfo(etherctrl);

    send(msg, "out");
}

void EtherMserCli::sendPacket(cMessage *msg)
{
//    if ((((LwipCntxt*)   (System->getLwipCtxt()) )->NodeID)==2)
//    std::cout << "Sending to the node ID :" << (((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)<<std::endl;
//    std::cout << "EtherMserCli sending to the node ID :" << (((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)<<std::endl;    

    EtherMserResp *datapacket = check_and_cast<EtherMserResp *>(msg);
    //EtherMserResq *datapacket = new EtherMserResq(msgname, IEEE802CTRL_DATA);
    seqNum++;

    char msgname[30];
    sprintf(msgname, "req-%d-%ld", getId(), seqNum);
    EV << "Generating packet `" << msgname << "'\n";

    datapacket->setName(msgname);   
    datapacket->setRequestId(seqNum);

    //long len = reqLength->longValue();

    //datapacket->setByteLength(len);

    //long respLen = respLength->longValue();
    //datapacket->setResponseBytes(respLen);

    Ieee802Ctrl *etherctrl = new Ieee802Ctrl();
    etherctrl->setSsap(localSAP);
    etherctrl->setDsap(remoteSAP);
    etherctrl->setDest(destMACAddress);
    datapacket->setControlInfo(etherctrl);



    emit(sentPkSignal, datapacket);

    //std::cout<<"Sending packet "<<msgname<<" at simTime:"<<simTime()<<std::endl;

    send(datapacket, "out");
    packetsSent++;
}

void EtherMserCli::receivePacket(cPacket *msg)
{
    //if ((((LwipCntxt*)   (System->getLwipCtxt()) )->NodeID)==1)
//    {std::cout<<" EtherMserCli::receivePacket"<<std::endl;}
    EV << "Received packet `" << msg->getName() << "'\n";


    EtherMserResp *datapacket = check_and_cast<EtherMserResp *>(msg);
    char* image_buf;
    int buf_size =    datapacket->getFileBufferArraySize();
    image_buf = (char*) malloc(buf_size);
    //printf("image_buf's buf_size is ....%d\n", buf_size);
    for(int ii=0; ii<buf_size; ii++){
	image_buf[ii]=datapacket->getFileBuffer(ii);
    }
    System -> NetworkInterfaceCard1->notify_receiving(image_buf, datapacket->getFileBufferArraySize());



    packetsReceived++;
    emit(rcvdPkSignal, msg);
    delete msg;
}

void EtherMserCli::finish()
{
 //   std::cout<<System->NodeID<<"	";
 //    delete System;
 //   cancelAndDelete(timerMsg);
 //   timerMsg = NULL;
    std::cout<<System->NodeID<<"	"<<std::endl;
    System->dumpStatJson();
}

