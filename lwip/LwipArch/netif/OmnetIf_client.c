/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */


#include <netif/OmnetIf_client.h>

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>


#include "lwip/debug.h"
#include "lwip/lwip_ctxt.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/ip.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"

#include "netif/etharp.h"

#if defined(LWIP_DEBUG) && defined(LWIP_TCPDUMP)
#include "netif/tcpdump.h"
#endif /* LWIP_DEBUG && LWIP_TCPDUMP */

#define IFCONFIG_BIN "/sbin/ifconfig "

#if defined(linux)
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#define DEVTAP "/dev/net/tun"
#define IFCONFIG_ARGS "tap0 inet %d.%d.%d.%d"
#elif defined(openbsd)
#define DEVTAP "/dev/tun0"
#define IFCONFIG_ARGS "tun0 inet %d.%d.%d.%d link0"
#else /* others */
#define DEVTAP "/dev/tap0"
#define IFCONFIG_ARGS "tap0 inet %d.%d.%d.%d"
#endif

#define IFNAME0 't'
#define IFNAME1 'p'

#ifndef TAPIF_DEBUG
#define TAPIF_DEBUG LWIP_DBG_OFF
#endif

#define SYSTEMC_ARCH_THREAD
//#define PTHREAD_ARCH_THREAD
#ifdef SYSTEMC_ARCH_THREAD
#include <systemc>
#endif
#ifdef PTHREAD_ARCH_THREAD
#include <pthread.h>
#endif
//#include <omnetpp.h>
#include "netif/OmnetIf_pkt.h"


struct tapif_client {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
  int fd;
};

/* Forward declarations. */
static void  tapif_client_input(LwipCntxt* ctxt);

static void tapif_client_thread(void *arg);



/*--------------------Code added as the virtual interface-----------------------*/


int client_write(void* ctxt, const void *buf, size_t len) {
    //client_write_ref(buf, len);
    //sys_sem_wait(&netif_C2S_sem);

    std::cout<< "-------------------------------client_write-------------------------------" <<std::endl;



    int err=len;


    int ii;



    static char this_str[16];
    ipaddr_ntoa_r( &(((LwipCntxt* )(ctxt))->ipaddr), this_str, 16);
    //printf("%x Client IP Addr is: %s\n", (((LwipCntxt* )(ctxt))->ipaddr).addr, this_str);
    static char dest_str[16];
    static char src_str[16];

    ipaddr_ntoa_r( &(((LwipCntxt* )(ctxt))->current_iphdr_dest), dest_str, 16);
    ipaddr_ntoa_r( &(((LwipCntxt* )(ctxt))->current_iphdr_src), src_str, 16);
    //printf("%x Dest IP Addr is: %s\n", (((LwipCntxt* )(ctxt))->current_iphdr_dest).addr, dest_str);
    //printf("%x Src IP Addr is: %s\n", (((LwipCntxt* )(ctxt))->current_iphdr_src).addr, src_str);


/*
    cSimpleModule* wrapper = (cSimpleModule*)(((LwipCntxt* )(ctxt))->OmnetWrapper);

    cContextSwitcher dummy1(wrapper); //VERY IMPORTANT
    OmnetIf_pkt* pkt = new OmnetIf_pkt();
    err = len;
    pkt->setFileBufferArraySize(len);
    for(ii=0; ii<len; ii++){
	pkt->setFileBuffer(ii, ((char*)buf)[ii]);
    }
    cMessage *startMsg = new cMessage("CliToServer");
    startMsg->setContextPointer(pkt);
    wrapper->scheduleAt(simTime(), startMsg);  //Notify immediately


    sys_sem_wait(& (((LwipCntxt* )(ctxt))->omnet_sem) ); //wait until message is sent
*/
    int taskID = ( (LwipCntxt*)(ctxt) ) -> getOSModelTaskID( sc_core::sc_get_current_process_handle());
    ((LwipCntxt* )(ctxt))->send_port[0]->SetSize(len, taskID);
    ((LwipCntxt* )(ctxt))->send_port[0]->SetData(len, (char*)buf, taskID);


   // ((LwipCntxt* )(ctxt))->recv_port[0]->GetNode( taskID);

    //printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>client_write finishing...%d\n",len);
/*

    sys_sem_wait(&w_sem_C2S);
#ifdef SYSTEMC_ARCH_THREAD
    sc_mutex_C2S.lock();
#endif
#ifdef PTHREAD_ARCH_THREAD
    pthread_mutex_lock(&mutex_C2S);
#endif


    //sys_sem_wait(&sem_C2S);
    
    printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<client_write beginning... len=%d\n", len);

    if(C2S.size()>0)
#ifdef SYSTEMC_ARCH_THREAD
	{sc_mutex_C2S.unlock();return err;}
#endif
#ifdef PTHREAD_ARCH_THREAD
	{pthread_mutex_unlock(&mutex_C2S);return err;}
#endif
    for(ii=0;ii<len;ii++)
      {C2S.push(((char*)buf)[ii]);}  
    err = C2S.size();
#ifdef SYSTEMC_ARCH_THREAD
	{sc_mutex_C2S.unlock();}
#endif
#ifdef PTHREAD_ARCH_THREAD
	{pthread_mutex_unlock(&mutex_C2S);}
#endif

    sys_sem_signal(&sem_C2S);

    printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<client_write finishing...size=%d\n", C2S.size());
*/
    return err;

}

int client_read(void* ctxt, void *buf, size_t len) {
    std::cout<< "-------------------------------client read-------------------------------" <<std::endl;
    int err=0;
    int ii;
    int pkt_size;


    //sys_sem_wait(& (((LwipCntxt* )(ctxt))->omnet_sem_recv) ); //wait until message is sent


    //((LwipCntxt*)(ctxt))->os_port->timeWait(2000000, taskID);
    //sc_core::wait(100, sc_core::SC_MS);

    //if (((((LwipCntxt* )ctxt)->ipaddr).addr == 0x400a8c0)) 
//	std::cout<<"Received packet in cli1 systemC at simTime: "<<sc_core::sc_time_stamp().value()<<std::endl;



/*
    static char dest_str[16];
    static char src_str[16];
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>client_read beginning...%d\n",len);
    ipaddr_ntoa_r( &(((LwipCntxt* )(ctxt))->current_iphdr_dest), dest_str, 16);
    ipaddr_ntoa_r( &(((LwipCntxt* )(ctxt))->current_iphdr_src), src_str, 16);
    printf("%d Dest IP Addr is: %s\n", ((LwipCntxt* )(ctxt))->current_iphdr_dest, dest_str);
    printf("%d Src IP Addr is: %s\n", ((LwipCntxt* )(ctxt))->current_iphdr_src, src_str);	
*/
    int taskID = ( (LwipCntxt*)(ctxt) ) -> getOSModelTaskID( sc_core::sc_get_current_process_handle());



//receiving images from higher indexed nodes
    //printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>client_read beginning...\n");
    pkt_size = ((LwipCntxt* )(ctxt))->recv_port[0]->GetSize(taskID);
    //std::cout<<"In Task :"<<taskID<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>+++++++++receiving size in client_read: "<< pkt_size <<std::endl;
    ((LwipCntxt* )(ctxt))->recv_port[0]->GetData(pkt_size, (char*)buf, taskID);
    //std::cout<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>+++++++++receiving data in client_read"<<std::endl;
    err = pkt_size;

/*
    pkt_size = ((LwipCntxt* )(ctxt))->fileBuffer_arraysize;
    for(ii=0;ii<pkt_size;ii++)
       {((char*)buf)[ii] = (((LwipCntxt* )(ctxt))->fileBuffer)[ii];}  
    err = pkt_size;
*/



/*



    sys_sem_wait(&sem_S2C);

#ifdef SYSTEMC_ARCH_THREAD
    sc_mutex_S2C.lock();
#endif
#ifdef PTHREAD_ARCH_THREAD
    pthread_mutex_lock(&mutex_S2C);
#endif


    err = S2C.size();
    if(S2C.size()==0)
#ifdef SYSTEMC_ARCH_THREAD
	{sc_mutex_S2C.unlock(); return err;}
#endif
#ifdef PTHREAD_ARCH_THREAD
	{pthread_mutex_unlock(&mutex_S2C);return err;}
#endif


	//{sys_sem_signal(&sem_S2C); return err;}
    pkt_size=S2C.size();
    printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<client_read beginning...%d\n", pkt_size);
    for(ii=0;ii<pkt_size;ii++)
	{
	((char*)buf)[ii] = S2C.front();S2C.pop();
	}  
#ifdef SYSTEMC_ARCH_THREAD
    sc_mutex_S2C.unlock();
#endif
#ifdef PTHREAD_ARCH_THREAD
    pthread_mutex_unlock(&mutex_S2C);
#endif
    //sys_sem_signal(&sem_S2C);
    //printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<client_read finishing...\n");

    sys_sem_signal(&w_sem_S2C);
 //   int err=1;
 //   err = client_read_ref(buf, len);

*/

    return err;

}







/*-----------------------------------------------------------------------------------*/
static void
low_level_client_init(LwipCntxt * ctxt)
{
  struct netif *netif = &(((LwipCntxt *)ctxt)->netif);
  struct tapif_client *tapif_client;
  char buf[sizeof(IFCONFIG_ARGS) + sizeof(IFCONFIG_BIN) + 50];

  tapif_client = (struct tapif_client *)netif->state;

  /* Obtain MAC address from network interface. */

  /* (We just fake an address...) */
  tapif_client->ethaddr->addr[0] = 0x1;
  tapif_client->ethaddr->addr[1] = 0x2;
  tapif_client->ethaddr->addr[2] = 0x3;
  tapif_client->ethaddr->addr[3] = 0x4;
  tapif_client->ethaddr->addr[4] = 0x5;
  tapif_client->ethaddr->addr[5] = 0x6;

  /* Do whatever else is needed to initialize interface. */

  /* Nothing here by using the FIFO file */
  /* The device details are encapsulated in client_write() and client_read() function */

  //LwipCntxt* ctxt;

  sys_thread_new(ctxt, "tapif_client_thread", tapif_client_thread, ctxt, DEFAULT_THREAD_STACKSIZE, 0);

}
/*-----------------------------------------------------------------------------------*/
/*
 * low_level_client_output():
 *
 * Should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 */
/*-----------------------------------------------------------------------------------*/

static err_t
low_level_client_output(void* ctxt, struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  char buf[1514];
  char *bufptr;
  struct tapif_client *tapif;



  tapif = (struct tapif_client *)netif->state;
#if 0
    if(((double)rand()/(double)RAND_MAX) < 0.2) {
    printf("drop output\n");
    return ERR_OK;
    }
#endif
  /* initiate transfer(); */

  bufptr = &buf[0];

  for(q = p; q != NULL; q = q->next) {
    /* Send the data from the pbuf to the interface, one pbuf at a
       time. The size of the data in each pbuf is kept in the ->len
       variable. */
    /* send data from(q->payload, q->len); */
    memcpy(bufptr, q->payload, q->len);
    bufptr += q->len;
  }

  /* signal that packet should be sent(); */
  //if(client_write(buf, p->tot_len) == -1) {
    //perror("tapif_client: write");
  //}
  //printf("Client output ... ... ... ...\n");
  client_write(ctxt, buf, p->tot_len);
  //while((client_write(buf, p->tot_len) == 0)&&(p->tot_len!=0) ){printf("client writing\n");};
  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
/*
 * low_level_client_input():
 *
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 */
/*-----------------------------------------------------------------------------------*/
static struct pbuf *
low_level_client_input(LwipCntxt* ctxt, struct tapif_client *tapif)
{
  struct pbuf *p, *q;
  u16_t len;
  char buf[1514];
  char *bufptr;

  len = client_read(ctxt,buf, sizeof(buf));


  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(ctxt, PBUF_RAW, len, PBUF_POOL);

  if(p != NULL) {
    /* We iterate over the pbuf chain until we have read the entire
       packet into the pbuf. */
    bufptr = &buf[0];
    for(q = p; q != NULL; q = q->next) {
      /* Read enough bytes to fill this pbuf in the chain. The
         available data in the pbuf is given by the q->len
         variable. */
      /* read data into(q->payload, q->len); */
      memcpy(q->payload, bufptr, q->len);
      bufptr += q->len;
    }
    /* acknowledge that packet has been read(); */
  } else {
    printf("Could not allocate pbufs, or nothing is read out of the device\n");
    /* drop packet(); */
  }

  return p;
}
/*-----------------------------------------------------------------------------------*/
static void
tapif_client_thread(void *arg)
{

  struct netif *netif;
  struct tapif_client *tapif;
  int ret;

  netif = &(((LwipCntxt*)arg)->netif);
  tapif = (struct tapif_client *)netif->state;

  while(1) {

    /* Wait for a packet to arrive. */
    /* Handle incoming packet. */
      tapif_client_input((LwipCntxt*)arg);
    /*if nothing arrives, the client_read() in low_level_client_input() will be blocked*/

  }
}
/*-----------------------------------------------------------------------------------*/
/*
 * tapif_client_input():
 *
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_client_input() that
 * should handle the actual reception of bytes from the network
 * interface.
 *
 */
/*-----------------------------------------------------------------------------------*/
static void
tapif_client_input(LwipCntxt* ctxt)
{
  struct tapif_client *tapif;
  struct eth_hdr *ethhdr;
  struct pbuf *p;
  struct netif *netif = &(ctxt->netif);

  tapif = (struct tapif_client *)netif->state;

  p = low_level_client_input(ctxt, tapif);

  if(p == NULL) {
    LWIP_DEBUGF(TAPIF_DEBUG, ("tapif_client_input: low_level_client_input returned NULL\n"));
    return;
  }
  ethhdr = (struct eth_hdr *)p->payload;

  switch(htons(ethhdr->type)) {
  /* IP or ARP packet? */
  case ETHTYPE_IP:
  case ETHTYPE_ARP:
#if PPPOE_SUPPORT
  /* PPPoE packet? */
  case ETHTYPE_PPPOEDISC:
  case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
    /* full packet send to tcpip_thread to process */
    if (netif->input(ctxt, p, netif) != ERR_OK) {
      LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
       pbuf_free(ctxt, p);
       p = NULL;
    }
    break;
  default:
    pbuf_free(ctxt,p);
    break;
  }
}
/*-----------------------------------------------------------------------------------*/
/*
 * tapif_client_init():
 *
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_client_init() to do the
 * actual setup of the hardware.
 *
 */
/*-----------------------------------------------------------------------------------*/
err_t
tapif_client_init(void *ctxt)
{

  struct tapif_client *tapif;
  struct netif *netif = &(((LwipCntxt*)ctxt)->netif);
  tapif = (struct tapif_client *)mem_malloc(sizeof(struct tapif_client));
  if (!tapif) {
    return ERR_MEM;
  }
  netif->state = tapif;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  netif->output = etharp_output;
  netif->linkoutput = low_level_client_output;
  netif->mtu = 1500;
  /* hardware address length */
  netif->hwaddr_len = 6;

  tapif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;

  low_level_client_init((LwipCntxt*)ctxt);

  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
