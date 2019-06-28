/*
Author: Zhuoran Zhao
Edge device linklayer wrapper is implemented based on EtherAppCli.cc from INET  
*/

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "CliWrapper.h"
#include "json_config.h"

#include "inet/linklayer/common/Ieee802Ctrl.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/common/ModuleAccess.h"


int TotalClients = 0;

namespace inet {

Define_Module(CliWrapper);

simsignal_t CliWrapper::sentPkSignal = registerSignal("sentPk");
simsignal_t CliWrapper::rcvdPkSignal = registerSignal("rcvdPk");

void CliWrapper::initialize(int stage)
{
   cSimpleModule::initialize(stage);

   if (stage == INITSTAGE_LOCAL) {
        clientID = par("clientID");
        seqNum = 0;
        WATCH(seqNum);

        // statistics
        packetsSent = packetsReceived = 0;
        WATCH(packetsSent);
        WATCH(packetsReceived);


   }
   else if (stage == INITSTAGE_APPLICATION_LAYER) {
        nodeStatus = dynamic_cast<NodeStatus *>(findContainingNode(this)->getSubmodule("status"));
   }
}

bool CliWrapper::isNodeUp()
{
    return !nodeStatus || nodeStatus->getState() == NodeStatus::UP;
}

void CliWrapper::startApp()
{
    EV_INFO << "Starting application\n";
}

void CliWrapper::stopApp()
{
    EV_INFO << "Stop the application\n";
}

void CliWrapper::handleMessage(cMessage *msg)
{
   unsigned int lwip_pkt_size;
   EtherWrapperResp *datapacket;
   if (!isNodeUp())
        throw cRuntimeError("Application is not running");
   if (msg->isSelfMessage()) {
      if (strcmp(msg->getName(), "ServerToCli") == 0) {
         lwip_pkt_size = ((OmnetIf_pkt*)(msg->getContextPointer()))->getFileBufferArraySize();
         datapacket = new EtherWrapperResp("lwip_msg", IEEE802CTRL_DATA);
         datapacket->setFileBufferArraySize(lwip_pkt_size);
         datapacket->setByteLength( lwip_pkt_size);
         for(unsigned int ii=0; ii<lwip_pkt_size; ii++){
            datapacket->setFileBuffer(ii, ((OmnetIf_pkt*)(msg->getContextPointer()))->getFileBuffer(ii));
         }
         //if(System->NodeID == 0) std::cout << "Send packet to device: " << ((OmnetIf_pkt*)(msg->getContextPointer()))->DestNode << " at time:" << simTime().dbl() <<"  size is:"<< lwip_pkt_size << std::endl;
         destMACAddress = resolveDestMACAddress(((OmnetIf_pkt*)(msg->getContextPointer()))->DestNode);
         if (destMACAddress.isUnspecified())
            return;
         sendPacket(datapacket, ((OmnetIf_pkt*)(msg->getContextPointer()))->DestNode); 
         System -> NetworkInterfaceCard1->notify_sending();		
         delete msg; 
      }
      else if (strcmp(msg->getName(), "StopSimulation") == 0){
         delete msg; 
         endSimulation();
      }
   }
   else
      receivePacket(check_and_cast<cPacket *>(msg));
}

bool CliWrapper::handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
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


MACAddress CliWrapper::resolveDestMACAddress(int dest_id)
{

    //std::cout <<"NodeID:"<< System->NodeID << ", resolveDestMACAddress:" << dest_id << std::endl;
    MACAddress dest_mac_addr;
    //std::cout << "This id is: "<< System->NodeID << " CliWrapper::resolveDestMACAddress dest ID: " 
    //         << (simulation_config.cluster)->get_dest_id(System->NodeID) << " passed in "<< dest_id << "equal?" << (dest_id==(simulation_config.cluster)->get_dest_id(System->NodeID))
    //         << "At time:" << sc_core::sc_time_stamp().to_seconds()
    //         << std::endl;
    //dest_id = (simulation_config.cluster)->get_dest_id(System->NodeID);
    //std::cout << "dequeue_dest_id: " << (sim_ctxt.cluster)->dequeue_dest_id( System->NodeID ) << std::endl;
    char* dest = (simulation_config.cluster)->get_mac_address_from_device_id(dest_id);
    assert(dest != NULL);
    dest_mac_addr.tryParse(dest);
    if(dest_id!=BROAD_CAST_MAC_ADDR) {
       total_sent_pkts[dest_id]++;
    }else{
       for(int i : (simulation_config.cluster)->edge_id){
          if(i == System->NodeID) continue;
          total_sent_pkts[i]++;
       }
    }
    return  dest_mac_addr;
/*

    if(dest_id==0){
       Dest.tryParse("00:01:00:00:00:00");
       return Dest;
    }
    if(dest_id==1){
       Dest.tryParse("00:02:00:00:00:00");
       return Dest;
    }
    if(dest_id==2){
       Dest.tryParse("00:03:00:00:00:00");
       return Dest;
    }
    if(dest_id==3){
       Dest.tryParse("00:04:00:00:00:00");
       return Dest;
    }
    if(dest_id==4){
       Dest.tryParse("00:05:00:00:00:00");
       return Dest;
    }
    if(dest_id==5){
       Dest.tryParse("00:06:00:00:00:00");
       return Dest;
    }
    if(dest_id==6){
       Dest.tryParse("00:07:00:00:00:00");
       return Dest;
    }
    if(dest_id==255)
       return  AddrAll;

    return  AddrAll;
*/
}

void CliWrapper::sendPacket(cMessage *msg, int dest_node)
{
    EtherWrapperResp *datapacket = check_and_cast<EtherWrapperResp *>(msg);
    seqNum++;

    char msgname[30];
    sprintf(msgname, "req-%d-%ld", getId(), seqNum);
    EV << "Generating packet `" << msgname << "'\n";
    //std::cout << "Sender is " << System->NodeID << "Dest Node ID is " << dest_node << ", Dest destMACAddress is " << destMACAddress << ", Packet name is: " << msgname << std::endl;
    datapacket->setName(msgname);   
    datapacket->setRequestId(seqNum);

    Ieee802Ctrl *etherctrl = new Ieee802Ctrl();
    etherctrl->setDest(destMACAddress);
    datapacket->setControlInfo(etherctrl);

    emit(sentPkSignal, datapacket);

    send(datapacket, "out");
    packetsSent++;
}

void CliWrapper::receivePacket(cPacket *msg)
{

    EtherWrapperResp *datapacket = check_and_cast<EtherWrapperResp *>(msg);
    //std::cout << "Receive Node ID is " << System->NodeID << "Packet name is: " << datapacket->getName() << std::endl;
    int buf_size = datapacket->getFileBufferArraySize();
    char* buf = (char*) malloc(buf_size);
    for(int ii=0; ii<buf_size; ii++){
	      buf[ii]=datapacket->getFileBuffer(ii);
    }
    System -> NetworkInterfaceCard1->notify_receiving(buf, datapacket->getFileBufferArraySize());
    packetsReceived++;
    total_recvd_pkts[System->NodeID]++;
    emit(rcvdPkSignal, msg);
    delete msg;
}

void CliWrapper::finish()
{
    std::cout<<System->NodeID<<"	"<<std::endl;
    if(System->NodeID == 0){
        for(int i = 0; i < 7; i++){
           std::cout <<" In Node "<< i <<", recvd: " <<total_recvd_pkts[i] <<", sent: " << total_sent_pkts[i] << std::endl;
        }
        simulation_config.write_result_json("result.json");
    }


}

} // namespace inet



