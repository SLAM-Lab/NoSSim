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

#include "EtherMserSrv.h"

#include "EtherMser_m.h"
#include "Ieee802Ctrl_m.h"
#include "ModuleAccess.h"
#include "NodeOperations.h"
#include "NodeStatus.h"

#include "image_file.h"
#include "OSNode/OmnetIf_pkt.h"


Define_Module(EtherMserSrv);

simsignal_t EtherMserSrv::sentPkSignal = registerSignal("sentPk");
simsignal_t EtherMserSrv::rcvdPkSignal = registerSignal("rcvdPk");


void EtherMserSrv::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == 0)
    {
        localSAP = par("localSAP");
        remoteSAP = par("remoteSAP");
        // statistics
        packetsSent = packetsReceived = 0;

        WATCH(packetsSent);
        WATCH(packetsReceived);
    }
    else if (stage == 3)
    {
        nodeStatus = dynamic_cast<NodeStatus *>(findContainingNode(this)->getSubmodule("status"));

        if (isNodeUp())
            startApp();
    }
}

bool EtherMserSrv::isNodeUp()
{
    return !nodeStatus || nodeStatus->getState() == NodeStatus::UP;
}

void EtherMserSrv::startApp()
{
    bool registerSAP = par("registerSAP");
    if (registerSAP)
        registerDSAP(localSAP);
}

void EtherMserSrv::stopApp()
{
}

void EtherMserSrv::handleMessage(cMessage *msg)
{
    if (!isNodeUp())
        throw cRuntimeError("Application is not running");




    unsigned int lwip_pkt_size;
    EtherMserResp *datapacket;

    if (msg->isSelfMessage())
    {

	//Copy the content from OmnetIf_pkt to a EtherMserResp package. 
        if (strcmp(msg->getName(), "ServerToCli") == 0) {
		System -> NetworkInterfaceCard1->notify_sending();	
                //std::cout<<"sending len from server is ... ... ... ... "<<( (OmnetIf_pkt*)(msg->getContextPointer()) )->getFileBufferArraySize()<<std::endl;	
		lwip_pkt_size = ((OmnetIf_pkt*)(msg->getContextPointer()))->getFileBufferArraySize();

		datapacket = new EtherMserResp("lwip_msg", IEEE802CTRL_DATA);
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




 //   {std::cout<<" EtherMserSrv::receivePacket"<<std::endl;}
    EV << "Received packet `" << msg->getName() << "'\n";


    EtherMserResp *req = check_and_cast<EtherMserResp *>(msg);
    //std::cout<<"Received packet "<<msg->getName()<<" at simTime:"<<simTime()<<endl;
    //std::cout<<"lwip_pkt_size: "<<req->getFileBufferArraySize()<<endl;


    //=====
/*
    char* image_buf;
    int buf_size =    req->getFileBufferArraySize();
    image_buf = (char*) malloc(buf_size);
    //printf("image_buf's buf_size is ....%d\n", buf_size);
    for(int ii=0; ii<buf_size; ii++){
	image_buf[ii]=req->getFileBuffer(ii);
    }
    dump_mem_to_file(buf_size, "recv.bmp", &image_buf);
*/
    //=====

    packetsReceived++;
    emit(rcvdPkSignal, req);

/*
    Ieee802Ctrl *ctrl = check_and_cast<Ieee802Ctrl *>(req->removeControlInfo());
    MACAddress srcAddr = ctrl->getSrc();
    srcAddrTable = srcAddr;
    int srcSap = ctrl->getSsap();
    srcSapTable = srcSap;
    long requestId = req->getRequestId();
    long replyBytes = req->getResponseBytes();
    char msgname[30];
    strcpy(msgname, msg->getName());
*/



    char* image_buf;
    int buf_size =    req->getFileBufferArraySize();
    image_buf = (char*) malloc(buf_size);
    //printf("image_buf's buf_size is ....%d\n", buf_size);
    for(int ii=0; ii<buf_size; ii++){
	image_buf[ii]=req->getFileBuffer(ii);
    }
    System -> NetworkInterfaceCard1->notify_receiving(image_buf, req->getFileBufferArraySize());



    delete msg;
//    delete ctrl;
/*

    // send back packets asked by EtherMserCli side
    int k = 0;
    strcat(msgname, "-resp-");
    char *s = msgname + strlen(msgname);

    while (replyBytes > 0)
    {
        int l = replyBytes > MAX_REPLY_CHUNK_SIZE ? MAX_REPLY_CHUNK_SIZE : replyBytes;
        replyBytes -= l;

        sprintf(s, "%d", k);

        EV << "Generating packet `" << msgname << "'\n";

        EtherMserResp *datapacket = new EtherMserResp(msgname, IEEE802CTRL_DATA);
        datapacket->setRequestId(requestId);
        datapacket->setByteLength(l);
        sendPacket(datapacket, srcAddr, srcSap);

        k++;
    }

*/

}

void EtherMserSrv::sendPacket(cPacket *datapacket, const MACAddress& destAddr, int destSap)
{

   // std::cout << "Sending to the node ID :" << (((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)<<std::endl;
    


    Ieee802Ctrl *etherctrl = new Ieee802Ctrl();
    etherctrl->setSsap(localSAP);
    etherctrl->setDsap(remoteSAP);
    etherctrl->setDest(resolveDestMACAddress());

    //etherctrl->setDsap(destSap);
    //etherctrl->setDest(destAddr);

    datapacket->setControlInfo(etherctrl);
    emit(sentPkSignal, datapacket);
    send(datapacket, "out");
    packetsSent++;
}

void EtherMserSrv::registerDSAP(int dsap)
{
    EV << getFullPath() << " registering DSAP " << dsap << "\n";

    Ieee802Ctrl *etherctrl = new Ieee802Ctrl();
    etherctrl->setDsap(dsap);
    cMessage *msg = new cMessage("register_DSAP", IEEE802CTRL_REGISTER_DSAP);
    msg->setControlInfo(etherctrl);

    send(msg, "out");
}

bool EtherMserSrv::handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
{
    Enter_Method_Silent();
    if (dynamic_cast<NodeStartOperation *>(operation)) {
        if (stage == NodeStartOperation::STAGE_APPLICATION_LAYER)
            startApp();
    }
    else if (dynamic_cast<NodeShutdownOperation *>(operation)) {
        if (stage == NodeShutdownOperation::STAGE_APPLICATION_LAYER)
            stopApp();
    }
    else if (dynamic_cast<NodeCrashOperation *>(operation)) {
        if (stage == NodeCrashOperation::STAGE_CRASH)
            stopApp();
    }
    else throw cRuntimeError("Unsupported lifecycle operation '%s'", operation->getClassName());
    return true;
}


MACAddress EtherMserSrv::resolveDestMACAddress()
{
 /*   MACAddress destMACAddress;
    const char *destAddress = par("destAddress");
    if (destAddress[0])
    {
        // try as mac address first, then as a module
        if (!destMACAddress.tryParse(destAddress))
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
    return destMACAddress;*/

    MACAddress AddrNode0;
    MACAddress AddrNode1;
    MACAddress AddrNode2;
    MACAddress AddrNode3;
/*
    MACAddress AddrNode4;
    MACAddress AddrNode5;
    MACAddress AddrNode6;
    MACAddress AddrNode7;
    MACAddress AddrNode8;
    MACAddress AddrNode9;
    MACAddress AddrNode10;
*/

    MACAddress AddrAll;

    const char *addrNode0 = par("AddrNode0");
    const char *addrNode1 = par("AddrNode1");
    const char *addrNode2 = par("AddrNode2");
    const char *addrNode3 = par("AddrNode3");
/*
    const char *addrNode4 = par("AddrNode4");
    const char *addrNode5 = par("AddrNode5");
    const char *addrNode6 = par("AddrNode6");
    const char *addrNode7 = par("AddrNode7");
    const char *addrNode8 = par("AddrNode8");
    const char *addrNode9 = par("AddrNode9");
    const char *addrNode10 = par("AddrNode10");
*/

    AddrNode0.tryParse(addrNode0);
    AddrNode1.tryParse(addrNode1);
    AddrNode2.tryParse(addrNode2);
    AddrNode3.tryParse(addrNode3);
/*  AddrNode4.tryParse(addrNode4);
    AddrNode5.tryParse(addrNode5);
    AddrNode6.tryParse(addrNode6);
    AddrNode7.tryParse(addrNode7);
    AddrNode8.tryParse(addrNode8);
    AddrNode9.tryParse(addrNode9);
    AddrNode10.tryParse(addrNode10);
*/

    AddrAll.tryParse("ff:ff:ff:ff:ff:ff");


    if ((((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)==0){
	return AddrNode0;
    }
    if ((((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)==1){
	return AddrNode1;
    }
    if ((((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)==2){
	return AddrNode2;
    }
    if ((((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)==3){
	return AddrNode3;
    }  

/*
    if ((((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)==4){
	return AddrNode4;
    }
    if ((((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)==5){
	return AddrNode5;
    }
    if ((((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)==6){
	return AddrNode6;
    }
    if ((((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)==7){
	return AddrNode7;
    }  
    if ((((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)==8){
	return AddrNode8;
    }
    if ((((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)==9){
	return AddrNode9;
    }
    if ((((LwipCntxt*)   (System->getLwipCtxt()) )->DestNodeID)==10){
	return AddrNode10;
    }  
*/
    return AddrAll;

}


void EtherMserSrv::finish()
{
    std::cout<<System->NodeID<<"	"<<std::endl;
    System->dumpStatJson();
    //delete System;
}

