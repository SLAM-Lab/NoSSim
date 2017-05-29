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
#include <netif/OmnetIf_server.h>
#include "os_ctxt.h"

#include "netif/etharp.h"

#if defined(LWIP_DEBUG) && defined(LWIP_TCPDUMP)
#include "netif/tcpdump.h"
#endif /* LWIP_DEBUG && LWIP_TCPDUMP */


#ifndef TAPIF_DEBUG
#define TAPIF_DEBUG LWIP_DBG_OFF
#endif



#define SYSTEMC_ARCH_THREAD

#ifdef SYSTEMC_ARCH_THREAD
#include <systemc>
#endif
#ifdef PTHREAD_ARCH_THREAD
#include <pthread.h>
#endif








struct tapif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
  int fd;
};

/* Forward declarations. */
static void  tapif_input(LwipCntxt* ctxt);

static void tapif_thread(void *arg);



/*--------------------Code added as the virtual interface-----------------------*/



int server_write(void* ctxt, const void *buf, size_t len) {

    int err=len;

    int ii;
/*
    static char this_str[16];
    ipaddr_ntoa_r( &(((LwipCntxt* )(ctxt))->ipaddr_dest), this_str, 16);

    static char dest_str[16];
    static char src_str[16];
    ipaddr_ntoa_r( &(((LwipCntxt* )(ctxt))->current_iphdr_dest), dest_str, 16);
    ipaddr_ntoa_r( &(((LwipCntxt* )(ctxt))->current_iphdr_src), src_str, 16);
*/
    int taskID =  ( taskManager.getTaskID( sc_core::sc_get_current_process_handle() ));
    ( taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() ))->send_port[0]->SetSize(len, taskID);
    ( taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() ))->send_port[0]->SetData(len, (char*)buf, taskID);
    return err;

}

int server_read(void* ctxt, void *buf, size_t len) {

    int pkt_size;
    int ii;
    int err=0;
    int taskID = taskManager.getTaskID( sc_core::sc_get_current_process_handle() );
    pkt_size =   ( taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() ))->recv_port[0]->GetSize(taskID);
     ( taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() ))->recv_port[0]->GetData(pkt_size, (char*)buf, taskID);
    err = pkt_size;
    return err;

}





/*-----------------------------------------------------------------------------------*/
static void
low_level_init(LwipCntxt * ctxt)
{
  struct netif *netif = &(((LwipCntxt *)ctxt)->netif);
  struct tapif *tapif;


  tapif = (struct tapif *)netif->state;

  /* Obtain MAC address from network interface. */

  /* (We just fake an address...) */
  tapif->ethaddr->addr[0] = 0x1;
  tapif->ethaddr->addr[1] = 0x2;
  tapif->ethaddr->addr[2] = 0x3;
  tapif->ethaddr->addr[3] = 0x4;
  tapif->ethaddr->addr[4] = 0x5;
  tapif->ethaddr->addr[5] = 0x6;

  /* Do whatever else is needed to initialize interface. */

  /* Nothing here by using the FIFO file */
  /* The device details are encapsulated in server_write() and server_read() function */

  //LwipCntxt* ctxt;
  sys_thread_new(ctxt, "tapif_thread", tapif_thread, ctxt, DEFAULT_THREAD_STACKSIZE, 0);

}
/*-----------------------------------------------------------------------------------*/
/*
 * low_level_output():
 *
 * Should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 */
/*-----------------------------------------------------------------------------------*/

static err_t
low_level_output(void* ctxt, struct netif *netif, struct pbuf *p)
{

  struct pbuf *q;
  char buf[1514];
  char *bufptr;
  struct tapif *tapif;
//  ((LwipCntxt* )(ctxt));

  tapif = (struct tapif *)netif->state;
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
  //if(server_write(buf, p->tot_len) == -1) {
    //perror("tapif: write");
  //}
  server_write(ctxt, buf, p->tot_len);

  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
/*
 * low_level_input():
 *
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 */
/*-----------------------------------------------------------------------------------*/
static struct pbuf *
low_level_input(LwipCntxt* ctxt, struct tapif *tapif)
{
  struct pbuf *p, *q;
  u16_t len;
  char buf[1514];
  char *bufptr;

  len = server_read(ctxt, buf, sizeof(buf));


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
tapif_thread(void *arg)
{
  struct netif *netif;
  struct tapif *tapif;
  int ret;

  netif = (struct netif *)(&(((LwipCntxt*)arg)->netif));
  tapif = (struct tapif *)netif->state;

  while(1) {
     //printf("tapif_thread reading\n");
    /* Wait for a packet to arrive. */
    /* Handle incoming packet. */
      tapif_input((LwipCntxt*)arg);
    /*if nothing arrives, the server_read() in low_level_input() will be blocked*/

  }
}
/*-----------------------------------------------------------------------------------*/
/*
 * tapif_input():
 *
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface.
 *
 */
/*-----------------------------------------------------------------------------------*/
static void
tapif_input(LwipCntxt* ctxt)
{
  struct tapif *tapif;
  struct eth_hdr *ethhdr;
  struct pbuf *p;
  struct netif *netif = &(ctxt->netif);

  tapif = (struct tapif *)netif->state;

  p = low_level_input(ctxt, tapif);

  if(p == NULL) {
    LWIP_DEBUGF(TAPIF_DEBUG, ("tapif_input: low_level_input returned NULL\n"));
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
    pbuf_free(ctxt, p);
    break;
  }
}
/*-----------------------------------------------------------------------------------*/
/*
 * tapif_init():
 *
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 */
/*-----------------------------------------------------------------------------------*/
err_t
tapif_init(void *ctxt)
{

  struct tapif *tapif;
  struct netif *netif = &(((LwipCntxt*)ctxt)->netif);
  tapif = (struct tapif *)mem_malloc(sizeof(struct tapif));
  if (!tapif) {
    return ERR_MEM;
  }
  netif->state = tapif;
//  netif->name[0] = IFNAME0;
//  netif->name[1] = IFNAME1;
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;
  netif->mtu = 1500;
  /* hardware address length */
  netif->hwaddr_len = 6;

  tapif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;

  low_level_init((LwipCntxt*)ctxt);

  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
