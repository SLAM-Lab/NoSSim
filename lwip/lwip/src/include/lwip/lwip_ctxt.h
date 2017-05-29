

#ifndef __LWIPCPPWRAPPER_H__
#define __LWIPCPPWRAPPER_H__


#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>

#include "lwip/opt.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/tcpip.h"
#include "lwip/timers.h"
#include "lwip/tcp.h"
#include "lwip/tcp_impl.h"
#include "lwip/memp.h"
#include "lwip/mem.h"
#include "lwip/opt.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/ip_frag.h"
#include "netif/etharp.h"



#define SYSTEMC_ARCH_THREAD
//#include <systemc>
//#include "HCSim.h"





class LwipCntxt {
  public:
/*
   //OS model integration
	sc_core::sc_port<lwip_recv_if> recv_port[2];
	sc_core::sc_port<lwip_send_if> send_port[2]; 
	sc_core::sc_port< HCSim::OSAPI > os_port;
	std::vector< sc_core::sc_process_handle> taskHandlerList;  
	std::vector< int> taskIDList;  

	void recordOSModelTaskID(int taskID, sc_core::sc_process_handle taskHandler){
		taskIDList.push_back(taskID);
		taskHandlerList.push_back(taskHandler);
	}

	int getOSModelTaskID(sc_core::sc_process_handle taskHandler){
		std::vector< sc_core::sc_process_handle >::iterator handlerIt = taskHandlerList.begin();
		std::vector< int >::iterator idIt = taskIDList.begin();
		for(; (handlerIt!=taskHandlerList.end() && idIt!=taskIDList.end() ) ;handlerIt++, idIt++){
			if(*handlerIt == taskHandler)
				return *idIt;	
		}
		return -1;
	} 
*/
   //The destination node ID for MAC address forwarding
   int DestNodeID;

   //OSModelCtxt *OSmodel;
   void *OSmodel;


   //sys_arc.c context
        void* OmnetWrapper;
        int NodeID;
	int CntxtID;
	int SetCond;
	struct timeval starttime;
	struct sys_thread *threads;


//#ifdef PTHREAD_ARCH_THREAD
//	pthread_mutex_t threads_mutex;
//	pthread_mutex_t lwprot_mutex;
//	pthread_t lwprot_thread;
//#endif
//#ifdef SYSTEMC_ARCH_THREAD
	//sc_core::sc_mutex threads_mutex;
	//sc_core::sc_mutex lwprot_mutex;
	//sc_core::sc_process_handle lwprot_thread;

//#endif


	int lwprot_count;
	sys_sem_t *sem;



   //synchronization/buffering variables for communication between system model and OMNET++ proxy 
	sys_sem_t omnet_sem;
	sys_sem_t omnet_sem_recv;
        char *fileBuffer; // array ptr
        unsigned int fileBuffer_arraysize;
	
	unsigned int total_length; //The total length sent by netconn_write, used in the receiver to synchronously collect all the data 
	// In reality, the number of delivered bytes can not be guaranteed, must have a better way for synchronization

   //tcpip context
	//tcpip_init_done_fn tcpip_init_done;
	//void *tcpip_init_done_arg;
	sys_mbox_t mbox;  

        ip_addr_t ipaddr; 
        ip_addr_t ipaddr_dest; 
	ip_addr_t netmask;
	ip_addr_t gw;
        struct netif netif;

	sys_mutex_t lock_tcpip_core;
	tcpip_init_done_fn tcpip_init_done;
	void *tcpip_init_done_arg;

   //netif context //./lwip/src/core/netif.c
	struct netif *netif_list;
	struct netif *netif_default;
	u8_t netif_num;


   //timer.c //./lwip/src/core/timers.c
	struct sys_timeo *next_timeout;
	u32_t timeouts_last_time;
	int tcpip_tcp_timer_active;

   
   //tcp.c //lwip/src/core/tcp.c
	#ifndef TCP_LOCAL_PORT_RANGE_START
	/* From http://www.iana.org/assignments/port-numbers:
	   "The Dynamic and/or Private Ports are those from 49152 through 65535" */
	#define TCP_LOCAL_PORT_RANGE_START        0xc000
	#define TCP_LOCAL_PORT_RANGE_END          0xffff
	#define TCP_ENSURE_LOCAL_PORT_RANGE(port) (((port) & ~TCP_LOCAL_PORT_RANGE_START) + TCP_LOCAL_PORT_RANGE_START)
	#endif
	u16_t tcp_port;
	u32_t tcp_ticks;

	struct tcp_pcb *tcp_bound_pcbs;
	union tcp_listen_pcbs_t tcp_listen_pcbs;
	struct tcp_pcb *tcp_active_pcbs;
	struct tcp_pcb *tcp_tw_pcbs;
	struct tcp_pcb ** tcp_pcb_lists[4];
	struct tcp_pcb *tcp_tmp_pcb;
	u8_t tcp_active_pcbs_changed;

	u8_t tcp_timer;
	u8_t tcp_timer_ctr;

   //tcp_in.c //lwip/src/core/tcp_in.c

	struct tcp_seg inseg;
	struct tcp_hdr *tcphdr;
	struct ip_hdr *iphdr;
	u32_t seqno, ackno;
	u8_t flags;
	u16_t tcplen;

	u8_t recv_flags;
	struct pbuf *recv_data;

	struct tcp_pcb *tcp_input_pcb;
   //pbuf.c //lwip/src/core/pbuf.c
	volatile u8_t pbuf_free_ooseq_pending;

   //memp.c //lwip/scr/core/memp.c




	#if !MEMP_MEM_MALLOC /* don't build if not configured for use in lwipopts.h */

	//struct memp {
	//  struct memp *next;
	#if MEMP_OVERFLOW_CHECK
	//  const char *file;
	//  int line;
	#endif /* MEMP_OVERFLOW_CHECK */
	//};

	#if MEMP_OVERFLOW_CHECK
	/* if MEMP_OVERFLOW_CHECK is turned on, we reserve some bytes at the beginning
	 * and at the end of each element, initialize them as 0xcd and check
	 * them later. */
	/* If MEMP_OVERFLOW_CHECK is >= 2, on every call to memp_malloc or memp_free,
	 * every single element in each pool is checked!
	 * This is VERY SLOW but also very helpful. */
	/* MEMP_SANITY_REGION_BEFORE and MEMP_SANITY_REGION_AFTER can be overridden in
	 * lwipopts.h to change the amount reserved for checking. */
	#ifndef MEMP_SANITY_REGION_BEFORE
	#define MEMP_SANITY_REGION_BEFORE  16
	#endif /* MEMP_SANITY_REGION_BEFORE*/
	#if MEMP_SANITY_REGION_BEFORE > 0
	#define MEMP_SANITY_REGION_BEFORE_ALIGNED    LWIP_MEM_ALIGN_SIZE(MEMP_SANITY_REGION_BEFORE)
	#else
	#define MEMP_SANITY_REGION_BEFORE_ALIGNED    0
	#endif /* MEMP_SANITY_REGION_BEFORE*/
	#ifndef MEMP_SANITY_REGION_AFTER
	#define MEMP_SANITY_REGION_AFTER   16
	#endif /* MEMP_SANITY_REGION_AFTER*/
	#if MEMP_SANITY_REGION_AFTER > 0
	#define MEMP_SANITY_REGION_AFTER_ALIGNED     LWIP_MEM_ALIGN_SIZE(MEMP_SANITY_REGION_AFTER)
	#else
	#define MEMP_SANITY_REGION_AFTER_ALIGNED     0
	#endif /* MEMP_SANITY_REGION_AFTER*/

	/* MEMP_SIZE: save space for struct memp and for sanity check */
	#define MEMP_SIZE          (LWIP_MEM_ALIGN_SIZE(sizeof(struct memp)) + MEMP_SANITY_REGION_BEFORE_ALIGNED)
	#define MEMP_ALIGN_SIZE(x) (LWIP_MEM_ALIGN_SIZE(x) + MEMP_SANITY_REGION_AFTER_ALIGNED)

	#else /* MEMP_OVERFLOW_CHECK */

	/* No sanity checks
	 * We don't need to preserve the struct memp while not allocated, so we
	 * can save a little space and set MEMP_SIZE to 0.
	 */
	#define MEMP_SIZE           0
	#define MEMP_ALIGN_SIZE(x) (LWIP_MEM_ALIGN_SIZE(x))

	#endif /* MEMP_OVERFLOW_CHECK */

	/** This array holds the first free element of each pool.
	 *  Elements form a linked list. */
	struct memp *memp_tab[MEMP_MAX];

	#else /* MEMP_MEM_MALLOC */

	#define MEMP_ALIGN_SIZE(x) (LWIP_MEM_ALIGN_SIZE(x))

	#endif /* MEMP_MEM_MALLOC */

	u8_t memp_memory[MEM_ALIGNMENT - 1 
		#define LWIP_MEMPOOL(name,num,size,desc) + ( (num) * (MEMP_SIZE + MEMP_ALIGN_SIZE(size) ) )
		#include "lwip/memp_std.h"
	];

   //ip.c //lwip/scr/core/ipv4/ip.c
	struct netif *current_netif;

	/**
	 * Header of the input packet currently being processed.
	 */
	struct ip_hdr *current_header;
	/** Source IP address of current_header */
	ip_addr_t current_iphdr_src;
	/** Destination IP address of current_header */
	ip_addr_t current_iphdr_dest;

	/** The IP header ID of the next outgoing IP packet */
	u16_t ip_id;
   //ip_frag.c //lwip/scr/core/ipv4/ip_frag.c
	struct ip_reassdata *reassdatagrams;
	u16_t ip_reass_pbufcount;


  LwipCntxt(){
        DestNodeID = -1;
   	CntxtID = 0;
	lwprot_count = 0;
        tcp_port = TCP_LOCAL_PORT_RANGE_START;
        threads = NULL;
	reassdatagrams=NULL;
	current_netif=NULL;
	current_header=NULL;
	netif_list=NULL;
	netif_default=NULL;
	SetCond=0;
	//OSmodel = new OSModelCtxt();
        next_timeout=NULL;
	tcp_active_pcbs=NULL;
	tcp_tw_pcbs=NULL;
	tcp_tmp_pcb=NULL;
	tcp_bound_pcbs=NULL;
	tcphdr=NULL;
	iphdr=NULL;
	recv_data=NULL;
	tcp_input_pcb=NULL;
	tcp_listen_pcbs.pcbs=NULL;
	tcp_listen_pcbs.listen_pcbs=NULL;

	tcp_pcb_lists[0] = &tcp_listen_pcbs.pcbs; 
	tcp_pcb_lists[1] = &tcp_bound_pcbs;
	tcp_pcb_lists[2] = &tcp_active_pcbs; 
	tcp_pcb_lists[3] = &tcp_tw_pcbs;
  }
/*
  void threads_mutex_lock(){
    pthread_mutex_lock(&(threads_mutex));
  }

  void threads_mutex_unlock(){
    pthread_mutex_unlock(&(threads_mutex));
  }
*/
  ~LwipCntxt(){
//	pthread_mutex_destroy(&threads_mutex);
  }


};

/*
//#include <systemc>
class LwipCntxt {
  public:
   //sys_arc.c context
	sc_dt::uint64 starttime;
	struct sys_thread *threads;
	sc_core::sc_mutex threads_mutex;
	int SetCond;
	sc_core::sc_mutex lwprot_mutex;
	sc_core::sc_process_handle lwprot_thread;
	sc_core::sc_process_handle SC_DEAD_PROCESS;
	int lwprot_count;

   //tcpip context
	tcpip_init_done_fn tcpip_init_done;
	void *tcpip_init_done_arg;
	sys_mbox_t mbox;

	#if LWIP_TCPIP_CORE_LOCKING
	sys_mutex_t lock_tcpip_core;
	#endif 


  LwipCntxt(){
	lwprot_count = 0;
	SC_DEAD_PROCESS = sc_core::sc_process_handle();
        threads = NULL;
	SetCond=0;
  }
  ~LwipCntxt(){

  }


};
*/




#endif /* __LWIPCPPWRAPPER_H__ */
