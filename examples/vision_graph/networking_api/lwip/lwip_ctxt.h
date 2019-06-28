#ifndef LWIP_CTXT_H
#define LWIP_CTXT_H

#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/tcpip.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/raw.h"
#include "lwip/ip4_addr.h"
#include "lwip/ip4_frag.h"
#include "lwip/sockets.h"

#include "lwip/timeouts.h"
#include "lwip/memp.h"
#include "lwip/mem.h"
#include "lwip/netif.h"
#include "netif/etharp.h"
#include "netif/ethernet.h"

#include "lwip/priv/tcpip_priv.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/init.h"
#include "lwip/ip.h"
#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"
#include "lwip/etharp.h"
#include "netif/ethernet.h"

#include "lwip/ip6.h"
#include "lwip/ip6_addr.h"
#include "lwip/ip6_frag.h"
#include "lwip/icmp6.h"
#include "lwip/nd6.h"
#include "lwip/priv/nd6_priv.h"
#include "lwip/prot/nd6.h"
#include "lwip/prot/icmp6.h"

#include "netif/lowpan6.h"


#ifndef UDP_LOCAL_PORT_RANGE_START
/* From http://www.iana.org/assignments/port-numbers:
   "The Dynamic and/or Private Ports are those from 49152 through 65535" */
#define UDP_LOCAL_PORT_RANGE_START  0xc000
#define UDP_LOCAL_PORT_RANGE_END    0xffff
#define UDP_ENSURE_LOCAL_PORT_RANGE(port) ((u16_t)(((port) & ~UDP_LOCAL_PORT_RANGE_START) + UDP_LOCAL_PORT_RANGE_START))
#endif

#ifndef TCP_LOCAL_PORT_RANGE_START
/* From http://www.iana.org/assignments/port-numbers:
   "The Dynamic and/or Private Ports are those from 49152 through 65535" */
#define TCP_LOCAL_PORT_RANGE_START        0xc000
#define TCP_LOCAL_PORT_RANGE_END          0xffff
#define TCP_ENSURE_LOCAL_PORT_RANGE(port) ((u16_t)(((port) & ~TCP_LOCAL_PORT_RANGE_START) + TCP_LOCAL_PORT_RANGE_START))
#endif



//-----------Some definition defined in c file, relocate those definitions here------------
//etharp.c and sockets.c must include lwip_ctxt.h file to be able to instantiate
//related global/local variables 
//--------------etharp.c //lwip/src/core/ipv4/etharp.c--------------//

struct etharp_entry {
#if ARP_QUEUEING
  /** Pointer to queue of pending outgoing packets on this ARP entry. */
  struct etharp_q_entry *q;
#else /* ARP_QUEUEING */
  /** Pointer to a single pending outgoing packet on this ARP entry. */
  struct pbuf *q;
#endif /* ARP_QUEUEING */
#if LWIP_IPV4
  ip4_addr_t ipaddr;
#endif//LWIP_IPV4
  struct netif *netif;
  struct eth_addr ethaddr;
  u16_t ctime;
  u8_t state;
};


//--------------sockets.c //lwip/src/api/sockets.c--------------//
#define NUM_SOCKETS MEMP_NUM_NETCONN

/** This is overridable for the rare case where more than 255 threads
 * select on the same socket...
 */
#ifndef SELWAIT_T
#define SELWAIT_T u8_t
#endif

/** Contains all internal pointers and states used for a socket */
struct lwip_sock {
  /** sockets currently are built on netconns, each socket has one netconn */
  struct netconn *conn;
  /** data that was left from the previous read */
  void *lastdata;
  /** offset in the data that was left from the previous read */
  u16_t lastoffset;
  /** number of times data was received, set by event_callback(),
      tested by the receive and select functions */
  s16_t rcvevent;
  /** number of times data was ACKed (free send buffer), set by event_callback(),
      tested by select */
  u16_t sendevent;
  /** error happened for this socket, set by event_callback(), tested by select */
  u16_t errevent;
  /** last error that occurred on this socket (in fact, all our errnos fit into an u8_t) */
  u8_t err;
  /** counter of how many threads are waiting for this socket using select */
  SELWAIT_T select_waiting;
};

#if LWIP_NETCONN_SEM_PER_THREAD
#define SELECT_SEM_T        sys_sem_t*
#define SELECT_SEM_PTR(sem) (sem)
#else /* LWIP_NETCONN_SEM_PER_THREAD */
#define SELECT_SEM_T        sys_sem_t
#define SELECT_SEM_PTR(sem) (&(sem))
#endif /* LWIP_NETCONN_SEM_PER_THREAD */

/** Description for a task waiting in select */
struct lwip_select_cb {
  /** Pointer to the next waiting task */
  struct lwip_select_cb *next;
  /** Pointer to the previous waiting task */
  struct lwip_select_cb *prev;
  /** readset passed to select */
  fd_set *readset;
  /** writeset passed to select */
  fd_set *writeset;
  /** unimplemented: exceptset passed to select */
  fd_set *exceptset;
  /** don't signal the same semaphore twice: set to 1 when signalled */
  int sem_signalled;
  /** semaphore to wake up a task waiting for select */
  SELECT_SEM_T sem;
};

/** A struct sockaddr replacement that has the same alignment as sockaddr_in/
 *  sockaddr_in6 if instantiated.
 */
union sockaddr_aligned {
   struct sockaddr sa;
#if LWIP_IPV6
   struct sockaddr_in6 sin6;
#endif /* LWIP_IPV6 */
#if LWIP_IPV4
   struct sockaddr_in sin;
#endif /* LWIP_IPV4 */
};


/* Define the number of IPv4 multicast memberships, default is one per socket */
#ifndef LWIP_SOCKET_MAX_MEMBERSHIPS
#define LWIP_SOCKET_MAX_MEMBERSHIPS NUM_SOCKETS
#endif

/* This is to keep track of IP_ADD_MEMBERSHIP calls to drop the membership when
   a socket is closed */
struct lwip_socket_multicast_pair {
  /** the socket */
  struct lwip_sock* sock;
  /** the interface address */
#if LWIP_IPV4
  ip4_addr_t if_addr;
  /** the group address */
  ip4_addr_t multi_addr;
#endif
};
//-----------Some definition defined in c file, relocate those definitions as listed above------------

#if LWIP_IPV6
//--------------lowpan6.c //lwip/src/netif/lowpan6.c--------------//
struct lowpan6_reass_helper {
  struct lowpan6_reass_helper *next_packet;
  struct pbuf *reass;
  struct pbuf *frags;
  u8_t timer;
  struct lowpan6_link_addr sender_addr;
  u16_t datagram_size;
  u16_t datagram_tag;
};

/** This struct keeps track of per-netif state */
struct lowpan6_ieee802154_data {
  /** fragment reassembly list */
  struct lowpan6_reass_helper *reass_list;
#if LWIP_6LOWPAN_NUM_CONTEXTS > 0
  /** address context for compression */
  ip6_addr_t lowpan6_context[LWIP_6LOWPAN_NUM_CONTEXTS];
#endif
  /** Datagram Tag for fragmentation */
  u16_t tx_datagram_tag;
  /** local PAN ID for IEEE 802.15.4 header */
  u16_t ieee_802154_pan_id;
  /** Sequence Number for IEEE 802.15.4 transmission */
  u8_t tx_frame_seq_num;
};
#endif
//---------------The definitions for 6LoWPAN----------------//



class lwip_context{
  public:

   //The destination node ID for MAC address forwarding
   int DestNodeID;

   //sys_arc.c context
   void* OmnetWrapper;
   int node_id;
   int CntxtID;
   int SetCond;
   struct timeval starttime;
   struct sys_thread *threads;
   int lwprot_count;
   sys_sem_t *sem;

   //synchronization/buffering variables for communication between system model and OMNET++ proxy 
   sys_sem_t omnet_sem;
   sys_sem_t omnet_sem_recv;
   char *fileBuffer; // array ptr
   unsigned int fileBuffer_arraysize;
   unsigned int total_length; //The total length sent by netconn_write, used in the receiver to synchronously collect all the data 
   // In reality, the number of delivered bytes can not be guaranteed, must have a better way for synchronization
   ip_addr_t ipaddr; 
   ip_addr_t ipaddr_dest; 
   ip_addr_t netmask;
   ip_addr_t gw;
   struct netif netif;


   //tcpip context //./lwip/src/api/tcpip.c
   tcpip_init_done_fn tcpip_init_done;
   void *tcpip_init_done_arg;
   sys_mbox_t mbox; 
   sys_mutex_t lock_tcpip_core;

   //netif context //./lwip/src/core/netif.c
   struct netif *netif_list;//NOT static
   struct netif *netif_default;//NOT static
   u8_t netif_num;
   u8_t netif_client_id;


   //timeouts.c //./lwip/src/core/timeouts.c
   struct sys_timeo *next_timeout;
   u32_t timeouts_last_time;
   int tcpip_tcp_timer_active;

   //udp.c //lwip/src/core/udp.c
   /* last local UDP port */
   u16_t udp_port;

   /* The list of UDP PCBs */
   /* exported in udp.h (was static) */
   struct udp_pcb *udp_pcbs;//NOT static 


   //tcp.c //lwip/src/core/tcp.c
   u16_t tcp_port;
   u32_t tcp_ticks;//NOT static

   struct tcp_pcb *tcp_bound_pcbs;//NOT static
   union tcp_listen_pcbs_t tcp_listen_pcbs;//NOT static
   struct tcp_pcb *tcp_active_pcbs;//NOT static
   struct tcp_pcb *tcp_tw_pcbs;//NOT static
   struct tcp_pcb ** tcp_pcb_lists[4];//NOT static
   u8_t tcp_active_pcbs_changed;//NOT static

   u8_t tcp_timer;
   u8_t tcp_timer_ctr;

   //tcp_in.c //lwip/src/core/tcp_in.c
   struct tcp_seg inseg;
   struct tcp_hdr *tcphdr;
   u16_t tcphdr_optlen;
   u16_t tcphdr_opt1len;
   u8_t* tcphdr_opt2;
   u16_t tcp_optidx;
   u32_t seqno, ackno;
   tcpwnd_size_t recv_acked;
   u16_t tcplen;
   u8_t flags;

   u8_t recv_flags;
   struct pbuf *recv_data;
   struct tcp_pcb *tcp_input_pcb;//NOT static 

   //etharp.c //lwip/src/core/ipv4/etharp.c
   struct etharp_entry arp_table[ARP_TABLE_SIZE];
   u8_t etharp_cached_entry;

   //sockets.c //lwip/src/api/sockets.c
   //The variable in lwip source codes 
   struct lwip_socket_multicast_pair socket_ipv4_multicast_memberships[LWIP_SOCKET_MAX_MEMBERSHIPS];
	
   /** The global array of available sockets */
   struct lwip_sock sockets[NUM_SOCKETS];

   /** The global list of tasks waiting for select */
   struct lwip_select_cb *select_cb_list;

   /** This counter is increased from lwip_select when the list is changed
      and checked in event_callback to see if it has changed. */
   int select_cb_ctr; //Why defined as volatile here

   //api_msg.c //lwip/src/api/api_msg.c
   u8_t netconn_aborted;

   //ip4.c //lwip/src/core/ipv4/ip4.c
   //ip.c //lwip/src/core/ip.c
   struct ip_globals ip_data;//NOT static 
   u16_t ip_id;
   struct netif* ip4_default_multicast_netif;

   //ip4_frag.c //lwip/src/core/ipv4/ip4_frag.c
   struct ip_reassdata *reassdatagrams;
   u16_t ip_reass_pbufcount;
#if LWIP_IPV6
   //ip6_frag.c //lwip/src/core/ipv6/ip6_frag.c
   struct ip6_reassdata *reassdatagrams6;
   u16_t ip6_reass_pbufcount;

   //nd6.c //lwip/src/core/ipv6/nd6.c
   /* Router tables. */
   struct nd6_neighbor_cache_entry neighbor_cache[LWIP_ND6_NUM_NEIGHBORS];
   struct nd6_destination_cache_entry destination_cache[LWIP_ND6_NUM_DESTINATIONS];
   struct nd6_prefix_list_entry prefix_list[LWIP_ND6_NUM_PREFIXES];
   struct nd6_router_list_entry default_router_list[LWIP_ND6_NUM_ROUTERS];

   /* Default values, can be updated by a RA message. */
   u32_t reachable_time;
   u32_t retrans_timer; /* @todo implement this value in timer */

   /* Index for cache entries. */
   u8_t nd6_cached_neighbor_index;
   u8_t nd6_cached_destination_index;

   /* Multicast address holder. */
   ip6_addr_t multicast_address;

   /* Static buffer to parse RA packet options (size of a prefix option, biggest option) */
   u8_t nd6_ra_buffer[sizeof(struct prefix_option)];

   //lowpan6.c //lwip/src/netif/lowpan6.c
   struct lowpan6_ieee802154_data lowpan6_data;
   struct lowpan6_link_addr short_mac_addr;
#endif

   //pbuf.c //lwip/src/core/pbuf.c
   volatile u8_t pbuf_free_ooseq_pending;

   //memp.c //lwip/src/core/memp.c
	//const struct memp_desc* const memp_pools[MEMP_MAX] = {
	//#define LWIP_MEMPOOL(name,num,size,desc) &memp_ ## name,
	//#include "lwip/priv/memp_std.h"
	//};

   lwip_context(): ip_data(), inseg(), netif(){
      DestNodeID = -1;
      CntxtID = 0;
      lwprot_count = 0;
      threads = NULL;
      SetCond=0;

   //udp.c //lwip/src/core/udp.c
      udp_port = UDP_LOCAL_PORT_RANGE_START;
      udp_pcbs=NULL;




   //tcpip context //./lwip/src/api/tcpip.c
      mbox=NULL;
      lock_tcpip_core=NULL;



   //netif context //./lwip/src/core/netif.c
      netif_list=NULL;//NOT static
      netif_default=NULL;//NOT static
      netif_num = 0;
      netif_client_id = 0;


   //timer.c //./lwip/src/core/timeouts.c
      next_timeout = NULL;
      timeouts_last_time = 0;
      tcpip_tcp_timer_active = 0;



   //ip4.c //lwip/src/core/ipv4/ip4.c
   //ip4.c //lwip/src/core/ip.c
      ip_id = 0;
      ip4_default_multicast_netif = NULL;
   //ip4_frag.c //lwip/src/core/ipv4/ip4_frag.c
      reassdatagrams = NULL;
      ip_reass_pbufcount = 0;
   //pbuf.c //lwip/src/core/pbuf.c
      pbuf_free_ooseq_pending = 0;

   //tcp.c //lwip/src/core/tcp.c
      tcp_port = TCP_LOCAL_PORT_RANGE_START;
      tcp_ticks = 0;

      tcp_bound_pcbs=NULL;
      tcp_listen_pcbs.pcbs=NULL;
      tcp_listen_pcbs.listen_pcbs=NULL;
      tcp_active_pcbs=NULL;
      tcp_tw_pcbs=NULL;
      tcp_pcb_lists[0] = &tcp_listen_pcbs.pcbs; 
      tcp_pcb_lists[1] = &tcp_bound_pcbs;
      tcp_pcb_lists[2] = &tcp_active_pcbs; 
      tcp_pcb_lists[3] = &tcp_tw_pcbs;
	//tcp_tmp_pcb=NULL;

      tcp_active_pcbs_changed = 0;

      tcp_timer = 0;
      tcp_timer_ctr = 0;


   //tcp_in.c //lwip/src/core/tcp_in.c
      tcphdr = NULL;
      tcphdr_optlen = 0;
      tcphdr_opt1len = 0;
      tcphdr_opt2 = NULL;
      tcp_optidx = 0;
      seqno = 0; 
      ackno = 0;
      recv_acked = 0;
      tcplen = 0;
      flags = 0;
      recv_flags = 0;
      recv_data=NULL;
      tcp_input_pcb=NULL;

   //api_msg.c //lwip/src/api/api_msg.c
      netconn_aborted = 0;
   //sockets.c //lwip/src/api/sockets.c
      /** The global list of tasks waiting for select */
      select_cb_list = NULL;
      /** This counter is increased from lwip_select when the list is changed
	    and checked in event_callback to see if it has changed. */
      select_cb_ctr = 0; 
   //etharp.c //lwip/src/core/ipv4/etharp.c
      etharp_cached_entry = 0;

#if LWIP_IPV6
   //ipv6
   //ip6_frag.c //lwip/src/core/ipv6/ip6_frag.c
      reassdatagrams6=NULL;
      ip6_reass_pbufcount=0;

   //nd6.c //lwip/src/core/ipv6/nd6.c
      /* Default values, can be updated by a RA message. */
      reachable_time = LWIP_ND6_REACHABLE_TIME;
      retrans_timer = LWIP_ND6_RETRANS_TIMER; 

      /* Index for cache entries. */
      nd6_cached_neighbor_index = 0;
      nd6_cached_destination_index = 0;
   //lowpan6.c //lwip/src/netif/lowpan6.c
      lowpan6_data.reass_list = NULL;
      lowpan6_data.ieee_802154_pan_id = 0;
      lowpan6_data.tx_frame_seq_num = 0; //static variable in lowpan6_frag function 
      lowpan6_data.tx_datagram_tag = 0;  //static variable in lowpan6_frag function
      short_mac_addr.addr_len = 2;
      short_mac_addr.addr[0] = 0;
      short_mac_addr.addr[1] = 0;
      short_mac_addr.addr[2] = 0;
      short_mac_addr.addr[3] = 0;
      short_mac_addr.addr[4] = 0;
      short_mac_addr.addr[5] = 0;
      short_mac_addr.addr[6] = 0;
      short_mac_addr.addr[7] = 0;
#endif
  }

  ~lwip_context(){

  }

};







#endif /* __LWIPCPPWRAPPER_H__ */
