#ifndef __ETHERNETQUEUE_H__
#define __ETHERNETQUEUE_H__
#include <iostream>       // std::cout
#include <queue>          // std::queue
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"

#define SYSTEMC_ARCH_THREAD
//#define PTHREAD_ARCH_THREAD
#ifdef SYSTEMC_ARCH_THREAD
#include <systemc>
#endif
#ifdef PTHREAD_ARCH_THREAD
#include <pthread.h>
#endif

extern std::queue<char> C2S;
extern std::queue<char> S2C;
extern pthread_mutex_t mutex_S2C;
extern pthread_mutex_t mutex_C2S;
#ifdef SYSTEMC_ARCH_THREAD
extern sc_core::sc_mutex sc_mutex_S2C;
extern sc_core::sc_mutex sc_mutex_C2S;
#endif

extern sys_sem_t sem_S2C;
extern sys_sem_t sem_C2S;
extern sys_sem_t w_sem_S2C;
extern sys_sem_t w_sem_C2S;

#endif /* __ETHERNETQUEUE_H__ */
