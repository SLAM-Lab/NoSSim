#ifndef THREAD_UTIL_H
#define THREAD_UTIL_H
#include <systemc>
#include "HCSim.h"
#include <cstdint>
#include <string>

typedef void (*thread_fn)(void *arg);
struct sys_thread;
typedef struct sys_thread* sys_thread_t;

/*multithreading APIs*/
sys_thread_t sys_thread_new(const char *name, thread_fn function, void *arg, int stacksize, int prio);
void sys_thread_join(sys_thread_t thread);

/*Semaphore APIs*/
struct sys_sem;
typedef struct sys_sem* sys_sem_t;
int8_t sys_sem_new(sys_sem_t *sem, uint8_t count);
void sys_sem_signal(sys_sem_t *s);
uint32_t sys_arch_sem_wait(sys_sem_t *s, uint32_t timeout);
void sys_sem_free(sys_sem_t *sem);
void sys_sleep();
uint32_t sys_now(void);
double sys_now_in_sec(void);

void sys_init(void);

#endif
