/**
 * @file
 * Stack-internal timers implementation.
 * This file includes timer callbacks for stack-internal timers as well as
 * functions to set up or stop timers and check for expired timers.
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
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
 *         Simon Goldschmidt
 *
 */

#include "lwip/opt.h"

#include "lwip/timers.h"
#include "lwip/tcp_impl.h"

#if LWIP_TIMERS

#include "lwip/def.h"
#include "lwip/memp.h"
#include "lwip/tcpip.h"

#include "lwip/ip_frag.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"
#include "lwip/igmp.h"
#include "lwip/dns.h"
#include "lwip/sys.h"
#include "lwip/pbuf.h"
#include "lwip/lwip_ctxt.h"

/** The one and only timeout list */
//static struct sys_timeo *next_timeout;
#if NO_SYS
//static u32_t timeouts_last_time;
#endif /* NO_SYS */

#if LWIP_TCP
/** global variable that shows if the tcp timer is currently scheduled or not */
//static int tcpip_tcp_timer_active;

/**
 * Timer callback function that calls tcp_tmr() and reschedules itself.
 *
 * @param arg unused argument
 */
static void
tcpip_tcp_timer(void *arg)
{
  //LWIP_UNUSED_ARG(arg);
  //if ((((LwipCntxt*)arg)->NodeID)==1) printf("tcpip_tcp_timer ... ...%d\n", ((LwipCntxt*)arg)->NodeID);
  //printf("tcpip... ....\n");
  /* call TCP timer handler */
  tcp_tmr(arg);
  //if ((((LwipCntxt*)arg)->NodeID)==1) printf("tcpip_tcp_timer ... ...%d\n", ((LwipCntxt*)arg)->NodeID);
  /* timer still needed? */
  //if ((((LwipCntxt*)arg)->NodeID)==1) if (((LwipCntxt*)arg)->tcp_active_pcbs ){printf("tcp_active_pcbs is not NULL ... ...\n");}else{printf("tcp_active_pcbs is NULL ... ...\n");}
  //if ((((LwipCntxt*)arg)->NodeID)==1)  if ( (((LwipCntxt*)arg)->tcp_tw_pcbs) ){printf("tcp_tw_pcbs is not NULL ... ...\n");}else{printf("tcp_tw_pcbs is NULL ... ...\n");}


  if (((LwipCntxt*)arg)->tcp_active_pcbs || (((LwipCntxt*)arg)->tcp_tw_pcbs)) {
    /* restart timer */
    sys_timeout(arg, TCP_TMR_INTERVAL, tcpip_tcp_timer, arg);
  } else {
    /* disable timer */
  //if ((((LwipCntxt*)arg)->NodeID)==0) printf("disable timer??????? ... ...%d\n", ((LwipCntxt*)arg)->NodeID);
    ((LwipCntxt*)arg)->tcpip_tcp_timer_active = 0;
  }
}

/**
 * Called from TCP_REG when registering a new PCB:
 * the reason is to have the TCP timer only running when
 * there are active (or time-wait) PCBs.
 */
void
tcp_timer_needed(void* ctxt)
{

  /* timer is off but needed again? */
  if (!((LwipCntxt*)ctxt)->tcpip_tcp_timer_active && (((LwipCntxt*)ctxt)->tcp_active_pcbs || (((LwipCntxt*)ctxt)->tcp_tw_pcbs))) {
    /* enable and start timer */
    ((LwipCntxt*)ctxt)->tcpip_tcp_timer_active = 1;
    sys_timeout(ctxt, TCP_TMR_INTERVAL, tcpip_tcp_timer, ctxt);
  }
}
#endif /* LWIP_TCP */

#if IP_REASSEMBLY
/**
 * Timer callback function that calls ip_reass_tmr() and reschedules itself.
 *
 * @param arg unused argument
 */
static void
ip_reass_timer(void *arg)
{
  //if ((((LwipCntxt*)arg)->NodeID)==1) printf("ip_reass_timer ... ... %d\n", ((LwipCntxt*)arg)->NodeID);
  //printf("ip_reass_timer ... ... %d\n", ((LwipCntxt*)arg)->NodeID);
  //LWIP_UNUSED_ARG(arg);
  LWIP_DEBUGF(TIMERS_DEBUG, ("tcpip: ip_reass_tmr()\n"));
  ip_reass_tmr(arg);
  sys_timeout(arg, IP_TMR_INTERVAL, ip_reass_timer, arg);
}
#endif /* IP_REASSEMBLY */

#if LWIP_ARP
/**
 * Timer callback function that calls etharp_tmr() and reschedules itself.
 *
 * @param arg unused argument
 */
static void
arp_timer(void *arg)
{
  //if ((((LwipCntxt*)arg)->NodeID)==1) printf("arp_timer ... ...%d\n", ((LwipCntxt*)arg)->NodeID);
  //LWIP_UNUSED_ARG(arg);
  LWIP_DEBUGF(TIMERS_DEBUG, ("tcpip: etharp_tmr()\n"));
  etharp_tmr(arg);
  sys_timeout(arg, ARP_TMR_INTERVAL, arp_timer, arg);
}
#endif /* LWIP_ARP */

#if LWIP_DHCP
/**
 * Timer callback function that calls dhcp_coarse_tmr() and reschedules itself.
 *
 * @param arg unused argument
 */
static void
dhcp_timer_coarse(void *arg)
{
  //LWIP_UNUSED_ARG(arg);
  LWIP_DEBUGF(TIMERS_DEBUG, ("tcpip: dhcp_coarse_tmr()\n"));
  dhcp_coarse_tmr();
  sys_timeout(arg, DHCP_COARSE_TIMER_MSECS, dhcp_timer_coarse, arg);
}

/**
 * Timer callback function that calls dhcp_fine_tmr() and reschedules itself.
 *
 * @param arg unused argument
 */
static void
dhcp_timer_fine(void *arg)
{
  //LWIP_UNUSED_ARG(arg);
  LWIP_DEBUGF(TIMERS_DEBUG, ("tcpip: dhcp_fine_tmr()\n"));
  dhcp_fine_tmr();
  sys_timeout(arg, DHCP_FINE_TIMER_MSECS, dhcp_timer_fine, arg);
}
#endif /* LWIP_DHCP */

#if LWIP_AUTOIP
/**
 * Timer callback function that calls autoip_tmr() and reschedules itself.
 *
 * @param arg unused argument
 */
static void
autoip_timer(void *arg)
{
  //LWIP_UNUSED_ARG(arg);
  LWIP_DEBUGF(TIMERS_DEBUG, ("tcpip: autoip_tmr()\n"));
  autoip_tmr();
  sys_timeout(arg, AUTOIP_TMR_INTERVAL, autoip_timer, arg);
}
#endif /* LWIP_AUTOIP */

#if LWIP_IGMP
/**
 * Timer callback function that calls igmp_tmr() and reschedules itself.
 *
 * @param arg unused argument
 */
static void
igmp_timer(void *arg)
{
  //LWIP_UNUSED_ARG(arg);
  LWIP_DEBUGF(TIMERS_DEBUG, ("tcpip: igmp_tmr()\n"));
  igmp_tmr();
  sys_timeout(arg, IGMP_TMR_INTERVAL, igmp_timer, arg);
}
#endif /* LWIP_IGMP */

#if LWIP_DNS
/**
 * Timer callback function that calls dns_tmr() and reschedules itself.
 *
 * @param arg unused argument
 */
static void
dns_timer(void *arg)
{
  //LWIP_UNUSED_ARG(arg);
  LWIP_DEBUGF(TIMERS_DEBUG, ("tcpip: dns_tmr()\n"));
  dns_tmr();
  sys_timeout(arg, DNS_TMR_INTERVAL, dns_timer, arg);
}
#endif /* LWIP_DNS */

/** Initialize this module */
void sys_timeouts_init(void* ctxt)
{
#if IP_REASSEMBLY
  sys_timeout(ctxt, IP_TMR_INTERVAL, ip_reass_timer, ctxt);
#endif /* IP_REASSEMBLY */
#if LWIP_ARP
  sys_timeout(ctxt, ARP_TMR_INTERVAL, arp_timer, ctxt);
#endif /* LWIP_ARP */
#if LWIP_DHCP
  sys_timeout(ctxt, DHCP_COARSE_TIMER_MSECS, dhcp_timer_coarse, ctxt);
  sys_timeout(ctxt, DHCP_FINE_TIMER_MSECS, dhcp_timer_fine, NULL);
#endif /* LWIP_DHCP */
#if LWIP_AUTOIP
  sys_timeout(ctxt, AUTOIP_TMR_INTERVAL, autoip_timer, ctxt);
#endif /* LWIP_AUTOIP */
#if LWIP_IGMP
  sys_timeout(ctxt, IGMP_TMR_INTERVAL, igmp_timer, ctxt);
#endif /* LWIP_IGMP */
#if LWIP_DNS
  sys_timeout(ctxt, DNS_TMR_INTERVAL, dns_timer, ctxt);
#endif /* LWIP_DNS */

#if NO_SYS
  /* Initialise timestamp for sys_check_timeouts */
  ((LwipCntxt*)ctxt)->timeouts_last_time = sys_now();
#endif
}

/**
 * Create a one-shot timer (aka timeout). Timeouts are processed in the
 * following cases:
 * - while waiting for a message using sys_timeouts_mbox_fetch()
 * - by calling sys_check_timeouts() (NO_SYS==1 only)
 *
 * @param msecs time in milliseconds after that the timer should expire
 * @param handler callback function to call when msecs have elapsed
 * @param arg argument to pass to the callback function
 */
#if LWIP_DEBUG_TIMERNAMES
void
sys_timeout_debug(void *ctxt, u32_t msecs, sys_timeout_handler handler, void *arg, const char* handler_name)
#else /* LWIP_DEBUG_TIMERNAMES */
void
sys_timeout(void *ctxt, u32_t msecs, sys_timeout_handler handler, void *arg)
#endif /* LWIP_DEBUG_TIMERNAMES */
{
  struct sys_timeo *timeout, *t;
  //printf("sys_timeout %d\n\n", msecs); 
  timeout = (struct sys_timeo *)memp_malloc(ctxt, MEMP_SYS_TIMEOUT);
  if (timeout == NULL) {
    LWIP_ASSERT("sys_timeout: timeout != NULL, pool MEMP_SYS_TIMEOUT is empty", timeout != NULL);
    return;
  }
  timeout->next = NULL;
  timeout->h = handler;
  timeout->arg = arg;
  timeout->time = msecs;
#if LWIP_DEBUG_TIMERNAMES
  timeout->handler_name = handler_name;
  LWIP_DEBUGF(TIMERS_DEBUG, ("sys_timeout: %p msecs=%"U32_F" handler=%s arg=%p\n",
    (void *)timeout, msecs, handler_name, (void *)arg));
#endif /* LWIP_DEBUG_TIMERNAMES */

  if ((((LwipCntxt*)ctxt)->next_timeout) == NULL) {
    (((LwipCntxt*)ctxt)->next_timeout) = timeout;
    return;
  }

  if ((((LwipCntxt*)ctxt)->next_timeout)->time > msecs) {
    (((LwipCntxt*)ctxt)->next_timeout)->time -= msecs;
    timeout->next = (((LwipCntxt*)ctxt)->next_timeout);
    (((LwipCntxt*)ctxt)->next_timeout) = timeout;
  } else {
    for(t = (((LwipCntxt*)ctxt)->next_timeout); t != NULL; t = t->next) {
      timeout->time -= t->time;
      if (t->next == NULL || t->next->time > timeout->time) {
        if (t->next != NULL) {
          t->next->time -= timeout->time;
        }
        timeout->next = t->next;
        t->next = timeout;
        break;
      }
    }
  }
}

/**
 * Go through timeout list (for this task only) and remove the first matching
 * entry, even though the timeout has not triggered yet.
 *
 * @note This function only works as expected if there is only one timeout
 * calling 'handler' in the list of timeouts.
 *
 * @param handler callback function that would be called by the timeout
 * @param arg callback argument that would be passed to handler
*/
void
sys_untimeout(void *ctxt, sys_timeout_handler handler, void *arg)
{
  struct sys_timeo *prev_t, *t;

  if ((((LwipCntxt*)ctxt)->next_timeout) == NULL) {
    return;
  }

  for (t = (((LwipCntxt*)ctxt)->next_timeout), prev_t = NULL; t != NULL; prev_t = t, t = t->next) {
    if ((t->h == handler) && (t->arg == arg)) {
      /* We have a match */
      /* Unlink from previous in list */
      if (prev_t == NULL) {
        (((LwipCntxt*)ctxt)->next_timeout) = t->next;
      } else {
        prev_t->next = t->next;
      }
      /* If not the last one, add time of this one back to next */
      if (t->next != NULL) {
        t->next->time += t->time;
      }
      memp_free(ctxt, MEMP_SYS_TIMEOUT, t);
      return;
    }
  }
  return;
}

#if NO_SYS

/** Handle timeouts for NO_SYS==1 (i.e. without using
 * tcpip_thread/sys_timeouts_mbox_fetch(). Uses sys_now() to call timeout
 * handler functions when timeouts expire.
 *
 * Must be called periodically from your main loop.
 */
void
sys_check_timeouts(void* ctxt)
{
  if ((((LwipCntxt*)ctxt)->next_timeout)) {
    struct sys_timeo *tmptimeout;
    u32_t diff;
    sys_timeout_handler handler;
    void *arg;
    u8_t had_one;
    u32_t now;

    now = sys_now();
    /* this cares for wraparounds */
    diff = now - ((LwipCntxt*)ctxt) -> timeouts_last_time;
    do
    {
#if PBUF_POOL_FREE_OOSEQ
      PBUF_CHECK_FREE_OOSEQ(ctxt);
#endif /* PBUF_POOL_FREE_OOSEQ */
      had_one = 0;
      tmptimeout = (((LwipCntxt*)ctxt)->next_timeout);
      if (tmptimeout && (tmptimeout->time <= diff)) {
        /* timeout has expired */
        had_one = 1;
        ((LwipCntxt*)ctxt)->timeouts_last_time = now;
        diff -= tmptimeout->time;
        (((LwipCntxt*)ctxt)->next_timeout) = tmptimeout->next;
        handler = tmptimeout->h;
        arg = tmptimeout->arg;
#if LWIP_DEBUG_TIMERNAMES
        if (handler != NULL) {
          LWIP_DEBUGF(TIMERS_DEBUG, ("sct calling h=%s arg=%p\n",
            tmptimeout->handler_name, arg));
        }
#endif /* LWIP_DEBUG_TIMERNAMES */
        memp_free(MEMP_SYS_TIMEOUT, tmptimeout);
        if (handler != NULL) {
          handler(arg);
        }
      }
    /* repeat until all expired timers have been called */
    }while(had_one);
  }
}

/** Set back the timestamp of the last call to sys_check_timeouts()
 * This is necessary if sys_check_timeouts() hasn't been called for a long
 * time (e.g. while saving energy) to prevent all timer functions of that
 * period being called.
 */
void
sys_restart_timeouts(void *ctxt)
{
  ((LwipCntxt*)ctxt)->timeouts_last_time = sys_now();
}

#else /* NO_SYS */

/**
 * Wait (forever) for a message to arrive in an mbox.
 * While waiting, timeouts are processed.
 *
 * @param mbox the mbox to fetch the message from
 * @param msg the place to store the message
 */
void
sys_timeouts_mbox_fetch(void *ctxt, sys_mbox_t *mbox, void **msg)
{
  u32_t time_needed;
  struct sys_timeo *tmptimeout;
  sys_timeout_handler handler;
  void *arg;
  //printf("In Node %d\n", (((LwipCntxt*)ctxt)->NodeID)); 


  //if (!(((LwipCntxt*)ctxt)->next_timeout)) {
  //    printf("sys_timeouts_mbox_fetch, no time out in Node %d\n", (((LwipCntxt*)ctxt)->NodeID)); 
  //}
  //else{
  //    printf("sys_timeouts_mbox_fetch,timeout is %d\n", (((LwipCntxt*)ctxt)->next_timeout)->time); 
  //}

 again:

  if (!(((LwipCntxt*)ctxt)->next_timeout)) {
    time_needed = sys_arch_mbox_fetch(mbox, msg, 0);
  } else {
    if ((((LwipCntxt*)ctxt)->next_timeout)->time > 0) {
      //if((((LwipCntxt*)ctxt)->NodeID)==1 ) printf("time out in timers is  : %ld\n", (((LwipCntxt*)ctxt)->next_timeout)->time); 
      time_needed = sys_arch_mbox_fetch(mbox, msg, (((LwipCntxt*)ctxt)->next_timeout)->time);
      //printf("time needed in timers is  : %ld\n", time_needed ); 
    } else {
      time_needed = SYS_ARCH_TIMEOUT;
    }

    if (time_needed == SYS_ARCH_TIMEOUT) {
      //printf("time out is triggered in timer\n"); 
      /* If time == SYS_ARCH_TIMEOUT, a timeout occured before a message
         could be fetched. We should now call the timeout handler and
         deallocate the memory allocated for the timeout. */



      tmptimeout = (((LwipCntxt*)ctxt)->next_timeout);
      (((LwipCntxt*)ctxt)->next_timeout) = tmptimeout->next;
      handler = tmptimeout->h;
      arg = tmptimeout->arg;
#if LWIP_DEBUG_TIMERNAMES
      if (handler != NULL) {
        LWIP_DEBUGF(TIMERS_DEBUG, ("stmf calling h=%s arg=%p\n",
          tmptimeout->handler_name, arg));
      }
#endif /* LWIP_DEBUG_TIMERNAMES */
      memp_free(ctxt, MEMP_SYS_TIMEOUT, tmptimeout);
      if (handler != NULL) {


        /* For LWIP_TCPIP_CORE_LOCKING, lock the core before calling the
           timeout handler function. */
        LOCK_TCPIP_CORE();
        handler(arg);
        UNLOCK_TCPIP_CORE();
	//if ((((LwipCntxt*)ctxt)->NodeID)==1) 
        //printf("=====================: %d\n", (((LwipCntxt*)ctxt)->next_timeout)->time ); 

      }
      LWIP_TCPIP_THREAD_ALIVE();

      /* We try again to fetch a message from the mbox. */
      //printf("In Node try again%d\n", (((LwipCntxt*)ctxt)->NodeID));
	  //if ((((LwipCntxt*)ctxt)->next_timeout)) {
		//   printf("sys_timeouts_mbox_fetch,timeout is %d\n", (((LwipCntxt*)ctxt)->next_timeout)->time); 
	  //} 
      goto again;
    } else {

      //printf("(((LwipCntxt*)ctxt)->next_timeout)->time = %ld\n", (((LwipCntxt*)ctxt)->next_timeout)->time); 
      /* If time != SYS_ARCH_TIMEOUT, a message was received before the timeout
         occured. The time variable is set to the number of
         milliseconds we waited for the message. */
      if (time_needed < (((LwipCntxt*)ctxt)->next_timeout)->time) {
        (((LwipCntxt*)ctxt)->next_timeout)->time -= time_needed;
      } else {
        (((LwipCntxt*)ctxt)->next_timeout)->time = 0;
      }
    }
  }
}

#endif /* NO_SYS */

#else /* LWIP_TIMERS */
/* Satisfy the TCP code which calls this function */
void
tcp_timer_needed(void* ctxt)
{
}
#endif /* LWIP_TIMERS */
