#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "lwip/debug.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/ip.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/ethip6.h"

#include "netif/hcsim_if.h"
#include "netif/etharp.h"
#if defined(LWIP_DEBUG) && defined(LWIP_TCPDUMP)
#include "netif/tcpdump.h"
#endif /* LWIP_DEBUG && LWIP_TCPDUMP */
#ifndef HCSIM_IF_DEBUG
#define HCSIM_IF_DEBUG LWIP_DBG_OFF
#endif

#include <systemc>
#include "lwip_ctxt.h"

struct hcsim_if {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
  int fd;
};

/* Forward declarations. */
static void  hcsim_if_input(struct netif *netif);
static void hcsim_if_thread(void *arg);

/*--------------------Code added as the virtual interface-----------------------*/
static int server_write(const void *buf, size_t len){
    int err=len;
    int ii;
    int task_id = sim_ctxt.get_task_id(sc_core::sc_get_current_process_handle());
    os_model_context* os_model = sim_ctxt.get_os_ctxt(sc_core::sc_get_current_process_handle());
    os_model->send_port[0]->set_size(len, task_id);
    os_model->send_port[0]->set_data(len, (char*)buf, task_id);
    return err;
}

static int server_read(void *buf, size_t len){
    int pkt_size;
    int ii;
    int err=0;
    int task_id = sim_ctxt.get_task_id(sc_core::sc_get_current_process_handle());
    os_model_context* os_model = sim_ctxt.get_os_ctxt(sc_core::sc_get_current_process_handle());
    pkt_size = os_model->recv_port[0]->get_size(task_id);
    os_model->recv_port[0]->get_data(pkt_size, (char*)buf, task_id);
    err = pkt_size;
    return err;
}

/*-----------------------------------------------------------------------------------*/
static void
low_level_init(struct netif *netif){
  netif->hwaddr_len = 6;
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP | NETIF_FLAG_MLD6;
  netif_set_link_up(netif);
  sys_thread_new("hcsim_if_thread", hcsim_if_thread, netif, DEFAULT_THREAD_STACKSIZE, 0);
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
low_level_output(struct netif *netif, struct pbuf *p){
  struct pbuf *q;
  char buf[1514];
  char *bufptr;
  struct hcsim_if *hcsim_if;
  hcsim_if = (struct hcsim_if *)netif->state;
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
  server_write(buf, p->tot_len);
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
low_level_input(struct netif *netif)
{
  struct pbuf *p, *q;
  u16_t len;
  char buf[1514];
  char *bufptr;

  len = server_read(buf, sizeof(buf));
  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

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
hcsim_if_thread(void *arg)
{
   struct netif *netif;
   struct hcsim_if *hcsim_if;
   int ret;

   netif = (struct netif *)arg;
   hcsim_if = (struct hcsim_if *)netif->state;

   while(1) {
     //printf("hcsim_if_thread reading\n");
    /* Wait for a packet to arrive. */
    /* Handle incoming packet. */
      hcsim_if_input(netif);
    /*if nothing arrives, the server_read() in low_level_input() will be blocked*/

   }
}
/*-----------------------------------------------------------------------------------*/
/*
 * hcsim_if_input():
 *
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface.
 *
 */
/*-----------------------------------------------------------------------------------*/

static void
hcsim_if_input(struct netif *netif)
{
  struct pbuf *p = low_level_input(netif);
  if (p == NULL) {
#if LINK_STATS
    LINK_STATS_INC(link.recv);
#endif /* LINK_STATS */
    LWIP_DEBUGF(TAPIF_DEBUG, ("tapif_input: low_level_input returned NULL\n"));
    return;
  }
  printf("Input tcpip in node, p->len: %d, p->tot_len: %d\n", p->len, p->tot_len);
  if (netif->input(p, netif) != ERR_OK) {
    LWIP_DEBUGF(NETIF_DEBUG, ("tapif_input: netif input error\n"));
    pbuf_free(p);
  }
}
/*-----------------------------------------------------------------------------------*/
/*
 * hcsim_if_init():
 *
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 */
/*-----------------------------------------------------------------------------------*/
err_t
hcsim_if_init(struct netif *netif){
  void* ctxt;//HCSim
  ctxt = sim_ctxt.get_app_ctxt(sc_core::sc_get_current_process_handle())->get_context("lwIP");//HCSim
  struct hcsim_if *hcsim_if; // A holder for ethernet address and device number
  hcsim_if = (struct hcsim_if *)mem_malloc(sizeof(struct hcsim_if));
  if (!hcsim_if) {
    return ERR_MEM;
  }

  netif->state = hcsim_if;   // 
#if LWIP_IPV4
  netif->output = etharp_output;
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
  netif->linkoutput = low_level_output;
  netif->mtu = 1500;
  /* hardware address length */

  netif->hwaddr[0] = 0x00;
  netif->hwaddr[1] = 0x00;
  netif->hwaddr[2] = 0x00;
  netif->hwaddr[3] = 0x00;
  netif->hwaddr[4] = 0x00;
  netif->hwaddr[5] = 0x00 +  (((lwip_context*)ctxt)->NodeID);;

  netif->hwaddr_len = 6;
  low_level_init(netif);

  return ERR_OK;
}
#if LWIP_IPV6 && LWIP_6LOWPAN
err_t
hcsim_if_init_6lowpan(struct netif *netif){
  void* ctxt;//HCSim
  ctxt = sim_ctxt.get_app_ctxt(sc_core::sc_get_current_process_handle())->get_context("lwIP");//HCSim

  struct hcsim_if *hcsim_if; // A holder for ethernet address and device number
  hcsim_if = (struct hcsim_if *)mem_malloc(sizeof(struct hcsim_if));
  if (!hcsim_if) {
    return ERR_MEM;
  }
  netif->state = hcsim_if;
  lowpan6_if_init(netif);
  //lowpan6_set_pan_id(1);
  netif->linkoutput = low_level_output;

  /* hardware address length */
  netif->hwaddr[0] = 0x00;
  netif->hwaddr[1] = 0x00;
  netif->hwaddr[2] = 0x00;
  netif->hwaddr[3] = 0x00;
  netif->hwaddr[4] = 0x00;
  netif->hwaddr[5] = 0x00 + (((lwip_context*)ctxt)->NodeID);;
  netif->hwaddr_len = 6;
  low_level_init(netif);

  return ERR_OK;
}
#endif//LWIP_IPV6 && LWIP_6LOWPAN
/*-----------------------------------------------------------------------------------*/
