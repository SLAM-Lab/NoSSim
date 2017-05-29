#include "lwip/debug.h"

#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
//#include <pthread.h>
#include <systemc>

#include "lwip/sys.h"
#include "lwip/opt.h"
#include "lwip/stats.h"

#include "lwip/lwip_ctxt.h"
#include "os_ctxt.h"

#define SYSTEMC_ARCH_THREAD

#define UMAX(a, b)      ((a) > (b) ? (a) : (b))

//static struct timeval starttime;


GlobalRecorder taskManager;


static sc_dt::uint64 starttime;

#if !NO_SYS

static struct sys_thread *threads = NULL;
//static pthread_mutex_t threads_mutex = PTHREAD_MUTEX_INITIALIZER;
/*SystemC model*/
static sc_core::sc_mutex threads_mutex;

struct sys_mbox_msg {
  struct sys_mbox_msg *next;
  void *msg;
};

/*
#define SYS_MBOX_SIZE 128

struct sys_mbox {
  int id;
  int first, last;
  void *msgs[SYS_MBOX_SIZE];
  struct sys_sem *not_empty;
  struct sys_sem *not_full;
  struct sys_sem *mutex;
  int wait_send;
  void * ctxt;

};
*/

#define GLOBAL_SEMS 200

/*
struct sys_sem {
  unsigned int c;
  pthread_cond_t cond; // systemc channel
  pthread_mutex_t mutex; // systemc mutex
};
*/
/*systemC model*/
struct sys_sem {
  bool free;
  int id;

  unsigned int c;
  sc_core::sc_event cond; // systemc channel
  sc_core::sc_mutex mutex; // systemc mutex

  void * ctxt;

  int blocking_task_id;
  int blocked_task_id;
  //sc_core::sc_port<sc_core::sc_fifo_in_if<char> > in;
  //sc_core::sc_port<sc_core::sc_fifo_out_if<char> > out;
  //sc_core::sc_fifo<char> *fifo_inst;
} sems[GLOBAL_SEMS];

int global=0;
int sems_init=0;


/*
struct sys_thread {
  struct sys_thread *next;
  pthread_t pthread; // pointer to function ????
};
*/
/*SystemC model*/
struct sys_thread {
  struct sys_thread *next;
  sc_core::sc_process_handle sc_thread;
};

#if SYS_LIGHTWEIGHT_PROT
/*static pthread_mutex_t lwprot_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t lwprot_thread = (pthread_t)0xDEAD;*/
static sc_core::sc_mutex lwprot_mutex;
static sc_core::sc_process_handle lwprot_thread;
static sc_core::sc_process_handle SC_DEAD_PROCESS = sc_core::sc_process_handle();
static int lwprot_count = 0;
#endif /* SYS_LIGHTWEIGHT_PROT */

static struct sys_sem *sys_sem_new_internal(void* ctxt,  u8_t count);
static void sys_sem_free_internal(struct sys_sem *sem);

//OS Model integration

/*Function wrapper for OS model*/

void wrapper( void *ctxt, lwip_thread_fn function, void *arg, int taskID){
//   	sc_core::sc_process_handle taskKey = sc_core::sc_get_current_process_handle();
//	((LwipCntxt*)(ctxt))->taskIDList.push_back(taskID);
//	((LwipCntxt*)(ctxt))->taskHandlerList.push_back(taskKey);
// 	((LwipCntxt*)(ctxt))->recordOSModelTaskID(taskID, sc_core::sc_get_current_process_handle());


	taskManager.registerTask(  (OSModelCtxt*) ( ((LwipCntxt*)(ctxt))->OSmodel ), taskID, sc_core::sc_get_current_process_handle());
	
        //((LwipCntxt*)(ctxt))->os_port->debugPrint();
	OSModelCtxt* OSmodel = taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() );

	OSmodel->os_port->taskActivate(taskID);
	function(arg);//All lwip thread args are converted to context object pointers.
 	OSmodel->os_port->taskTerminate(taskID);


}

typedef void (*OS_wrapper_fn)(void *ctxt, lwip_thread_fn function, void* arg, int taskID);


/* SystemC version*/
sys_thread_t
sys_thread_new(void *ctxt, const char *name, lwip_thread_fn function, void *arg, int affinity, int core)
{

  struct sys_thread *thread_p;
  /*   for(thread_p=threads; thread_p!=NULL; thread_p=thread_p->next)
	{
	 std::cout<<thread_p->sc_thread.name()<<std::endl;
	 std::cout<<thread_p->sc_thread.valid()<<std::endl;
	 std::cout<<thread_p->sc_thread.dynamic()<<std::endl;
	}
  */
  int code;

  sc_core::sc_process_handle th_handle;
  struct sys_thread *thread = NULL;

  LWIP_UNUSED_ARG(name);
  LWIP_UNUSED_ARG(affinity);
  //LWIP_UNUSED_ARG(core);


/*First Hardcode Some values*/
  int child_id; 
  const sc_dt::uint64 exe_cost = 8006000ull;
  const sc_dt::uint64 period = 49000000ull;
  const unsigned int priority = 49;
  const int init_core = core;
/*First Hardcode Some values*/


  child_id =  ( taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() ))->os_port->taskCreate(
				sc_core::sc_gen_unique_name("child_task"), 
				HCSim::OS_RT_APERIODIC, priority, period, exe_cost, 
				HCSim::DEFAULT_TS, HCSim::ALL_CORES, init_core);

  (taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() ))-> os_port -> dynamicStart(init_core);



  OS_wrapper_fn OS_fn = NULL;
  OS_fn = &wrapper;


/*
  th_handle = sc_core::sc_spawn(     
				 sc_bind(  
					 (void *(*)(void *)) function,
					 arg
					 )         
				); //function with input (to do) sc_bind( )
*/
  th_handle = sc_core::sc_spawn(     
                                 sc_bind(  
                                         OS_fn,
                                         ctxt, function, arg, child_id 
                                         )         
                                ); 


  //thread = (struct sys_thread *) new ( sizeof(struct sys_thread));
  thread = new sys_thread;

  if (thread != NULL) {
    //((LwipCntxt*)ctxt)->threads_mutex.lock();
    threads_mutex.lock();
    thread->next = ((LwipCntxt*)ctxt)->threads;
    thread->sc_thread = th_handle;
    threads = thread;
    threads_mutex.unlock();
   // ((LwipCntxt*)ctxt)->threads_mutex.unlock();
  }
  //printf("/***************New thread name***************/: %s\n", name);
  //std::cout << "Thread ID in OS model is: " << child_id << std::endl; 
  //std::cout << name << std::endl;
  //std::cout << "On core: "<< core << std::endl;
  //printf("/***************New thread name***************/: %s\n", name);


  return thread;
}

/*-----------------------------------------------------------------------------------*/
err_t
sys_mbox_new(void * ctxt, struct sys_mbox **mb, int size)
{


  //Debug Mark: OS Model Integration

  int taskID = taskManager.getTaskID(sc_core::sc_get_current_process_handle());
//( (LwipCntxt*)(ctxt) ) -> getOSModelTaskID( sc_core::sc_get_current_process_handle());


  //std::cout << " ************ "<<taskID<<" ************Creating mbox task ID is: ... ... .." << taskID <<std::endl;
  //Debug mark: OS Model Integration
  
  struct sys_mbox *mbox;
  LWIP_UNUSED_ARG(size);

  //mbox = (sys_mbox *)new(sizeof( sys_mbox));
  mbox = new sys_mbox;

  if (mbox == NULL) {
    return ERR_MEM;
  }
  mbox->id = taskID;
  mbox->ctxt = ctxt;
  mbox->first = mbox->last = 0;

  mbox->not_empty = sys_sem_new_internal(ctxt, 0);

  mbox->not_full = sys_sem_new_internal(ctxt, 0);

  mbox->mutex = sys_sem_new_internal(ctxt, 1);
  mbox->wait_send = 0;

  SYS_STATS_INC_USED(mbox);
  *mb = mbox;

  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
void
sys_mbox_free(struct sys_mbox **mb)
{
  if ((mb != NULL) && (*mb != SYS_MBOX_NULL)) {
    struct sys_mbox *mbox = *mb;
    SYS_STATS_DEC(mbox.used);


    sys_sem_free_internal(mbox->not_empty);
    sys_sem_free_internal(mbox->not_full);
    sys_sem_free_internal(mbox->mutex);
    mbox->not_empty = mbox->not_full = mbox->mutex = NULL;
    /*  LWIP_DEBUGF("sys_mbox_free: mbox 0x%lx\n", mbox); */
    delete mbox;
  }
}
/*-----------------------------------------------------------------------------------*/
err_t
sys_mbox_trypost(struct sys_mbox **mb, void *msg)
{

  u8_t first;
  struct sys_mbox *mbox;
  LWIP_ASSERT("invalid mbox", (mb != NULL) && (*mb != NULL));

  mbox = *mb;

  int taskID = taskManager.getTaskID(sc_core::sc_get_current_process_handle());
  //( (LwipCntxt*)(mbox->ctxt) ) -> getOSModelTaskID( sc_core::sc_get_current_process_handle());
  //if(mbox->id == 111)std::cout << " ************ "<<taskID<<"w************try writing mbox ID is: ... ... .." << mbox->id <<std::endl;


  LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_trypost: mbox %p msg %p\n",
                          (void *)mbox, (void *)msg));

  if ((mbox->last + 1) >= (mbox->first + SYS_MBOX_SIZE)) {
    //sys_sem_signal(&mbox->mutex);
    printf("Returning with memory error!\n");
    return ERR_MEM;
  }

  mbox->msgs[mbox->last % SYS_MBOX_SIZE] = msg;

  if (mbox->last == mbox->first) {
    first = 1;
  } else {
    first = 0;
  }

  mbox->last++;

  if (first) {
    //if(( (LwipCntxt*)((mbox->not_empty) -> ctxt) )-> NodeID == 1) printf("sys_sem_signal(&mbox->not_empty);%d sem %d mbox %p msg %p\n", ( (LwipCntxt*)((mbox->not_empty) -> ctxt) )-> NodeID ,(mbox->not_empty)->id,(void *)mbox, (void *)msg);
    sys_sem_signal(&mbox->not_empty);

  }

 // sys_sem_signal(&mbox->mutex);

  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
void
sys_mbox_post(struct sys_mbox **mb, void *msg)
{
  //printf("sys_mbox_post\n");
  u8_t first;
  struct sys_mbox *mbox;
  LWIP_ASSERT("invalid mbox", (mb != NULL) && (*mb != NULL));
  mbox = *mb;
  int taskID = taskManager.getTaskID(sc_core::sc_get_current_process_handle());
//( (LwipCntxt*)(mbox->ctxt) ) -> getOSModelTaskID( sc_core::sc_get_current_process_handle());
  //if(mbox->id == 111)std::cout << " ************ "<<taskID<<"w************Keep writing mbox ID is: ... ... .." << mbox->id <<std::endl;


  LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_post: mbox %p msg %p\n", (void *)mbox, (void *)msg));

  while ((mbox->last + 1) >= (mbox->first + SYS_MBOX_SIZE)) {
    mbox->wait_send++;
    sys_arch_sem_wait(&mbox->not_full, 0);
    mbox->wait_send--;
  }

  mbox->msgs[mbox->last % SYS_MBOX_SIZE] = msg;

  if (mbox->last == mbox->first) {
    first = 1;
  } else {
    first = 0;
  }

  mbox->last++;

  if (first) {
    sys_sem_signal(&mbox->not_empty);
  }

  //sys_sem_signal(&mbox->mutex);


}
/*-----------------------------------------------------------------------------------*/
u32_t
sys_arch_mbox_tryfetch(struct sys_mbox **mb, void **msg)
{
  struct sys_mbox *mbox;
  LWIP_ASSERT("invalid mbox", (mb != NULL) && (*mb != NULL));
  mbox = *mb;
  int taskID = taskManager.getTaskID(sc_core::sc_get_current_process_handle());

  if (mbox->first == mbox->last) {
    //sys_sem_signal(&mbox->mutex);
    return SYS_MBOX_EMPTY;
  }

  if (msg != NULL) {
    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_tryfetch: mbox %p msg %p\n", (void *)mbox, *msg));
    *msg = mbox->msgs[mbox->first % SYS_MBOX_SIZE];
  }
  else{
    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_tryfetch: mbox %p, null msg\n", (void *)mbox));
  }

  mbox->first++;

  if (mbox->wait_send) {
    sys_sem_signal(&mbox->not_full);
  }

  //sys_sem_signal(&mbox->mutex);

  return 0;
}
/*-----------------------------------------------------------------------------------*/
u32_t
sys_arch_mbox_fetch(struct sys_mbox **mb, void **msg, u32_t timeout)
{

  u32_t time_needed = 0;
  struct sys_mbox *mbox;
  LWIP_ASSERT("invalid mbox", (mb != NULL) && (*mb != NULL));
  mbox = *mb;
  int taskID = taskManager.getTaskID(sc_core::sc_get_current_process_handle());

  //( (LwipCntxt*)(mbox->ctxt) ) -> getOSModelTaskID( sc_core::sc_get_current_process_handle());
  //std::cout << " ************ "<<taskID<<"r************Keep reading mbox ID is: ... ... .." << mbox->id <<" Time out is: "<< timeout <<std::endl;
  /* The mutex lock is quick so we don't bother with the timeout
     stuff here. */

  //if(( (LwipCntxt*)((mbox->not_empty) -> ctxt) )-> NodeID == 1) printf("sys_arch_mbox_fetch\n");
  while (mbox->first == mbox->last) {
   //sys_sem_signal(&mbox->mutex);

    /* We block while waiting for a mail to arrive in the mailbox. We
       must be prepared to timeout. */
    if (timeout != 0) {
      //if(( (LwipCntxt*)((mbox->not_empty) -> ctxt) )-> NodeID == 1) printf("sys_arch_sem_wait(&mbox->not_empty, timeout) sem %d;\n", mbox->not_empty->id);
      time_needed = sys_arch_sem_wait(&mbox->not_empty, timeout);
      //if(( (LwipCntxt*)((mbox->not_empty) -> ctxt) )-> NodeID == 1) printf("sys_arch_sem_wait(&mbox->not_empty, timeout needed %d) sem %d;\n",time_needed, mbox->not_empty->id);
      if (time_needed == SYS_ARCH_TIMEOUT) {
        //if(( (LwipCntxt*)((mbox->not_empty) -> ctxt) )-> NodeID == 1) printf("return .. .... .....SYS_ARCH_TIMEOUT sys_arch_mbox_fetch\n");
        return SYS_ARCH_TIMEOUT;
      }
    } else {
      //if(( (LwipCntxt*)((mbox->not_empty) -> ctxt) )-> NodeID == 1) printf("sys_arch_sem_wait(&mbox->not_empty, ================) sem %d;\n", mbox->not_empty->id);
      sys_arch_sem_wait(&mbox->not_empty, 0);
    }



  }

  if (msg != NULL) {
    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_fetch: mbox %p msg %p\n", (void *)mbox, *msg));
    *msg = mbox->msgs[mbox->first % SYS_MBOX_SIZE];
  }
  else{
    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_fetch: mbox %p, null msg\n", (void *)mbox));
  }

  mbox->first++;

  if (mbox->wait_send) {
    sys_sem_signal(&mbox->not_full);
  }

  //sys_sem_signal(&mbox->mutex);
  //if(( (LwipCntxt*)((mbox->not_empty) -> ctxt) )-> NodeID == 1) printf("return .. .... ..... sys_arch_mbox_fetch\n");
  return time_needed;
}
/*-----------------------------------------------------------------------------------*/
/*static struct sys_sem *
sys_sem_new_internal(u8_t count)
{
  struct sys_sem *sem;
  sem = (struct sys_sem *)malloc(sizeof(struct sys_sem));
  if (sem != NULL) {
    sem->c = count;
    pthread_cond_init(&(sem->cond), NULL);
    pthread_mutex_init(&(sem->mutex), NULL);
  }
  return sem;
}*/
/*SystemC Model*/
static struct sys_sem *
sys_sem_new_internal(void* ctxt, u8_t count)
{ 
  int i = 0;
  if(sems_init == 0){
	sems_init = 1;
	for(i = 0; i<GLOBAL_SEMS; i++){
		sems[i].free = 1;
	}
  }


 //
  for(i = 0; i<GLOBAL_SEMS; i++){
	if(sems[i].free == 1){
		break;
	}
  }

  global=i;

  struct sys_sem *sem;
  sem = sems + global;
  sem->id = global;
  sem->blocking_task_id = -1;
  sem->blocked_task_id = -1; 
  sem->free=0;
  //global++;


  if (GLOBAL_SEMS==global){printf("global  is %d \n", global);}

  //printf("global  is %d \n", global);


  if (sem != NULL) {
    sem->c = count;
    sem->ctxt = ctxt;
  }
  return sem;
}

/*-----------------------------------------------------------------------------------*/
err_t
sys_sem_new(void *ctxt, struct sys_sem **sem, u8_t count)
{ 
  SYS_STATS_INC_USED(sem);
  *sem = sys_sem_new_internal(ctxt, count);
  if (*sem == NULL) {
    return ERR_MEM;
  }
  return ERR_OK;
}

/*-----------------------------------------------------------------------------------*/
/*static u32_t
cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex, u32_t timeout)
{
  int tdiff;
  unsigned long sec, usec;
  struct timeval rtime1, rtime2;
  struct timespec ts;
  int retval;

  if (timeout > 0) {
    /* Get a timestamp and add the timeout value. */
/*    gettimeofday(&rtime1, NULL);
    sec = rtime1.tv_sec;
    usec = rtime1.tv_usec;
    usec += timeout % 1000 * 1000;
    sec += (int)(timeout / 1000) + (int)(usec / 1000000);
    usec = usec % 1000000;
    ts.tv_nsec = usec * 1000;
    ts.tv_sec = sec;

    retval = pthread_cond_timedwait(cond, mutex, &ts);

    if (retval == ETIMEDOUT) {
      return SYS_ARCH_TIMEOUT;
    } else {
      /* Calculate for how long we waited for the cond. */
/*      gettimeofday(&rtime2, NULL);
      tdiff = (rtime2.tv_sec - rtime1.tv_sec) * 1000 +
        (rtime2.tv_usec - rtime1.tv_usec) / 1000;

      if (tdiff <= 0) {
        return 0;
      }

      return tdiff;
    }
  } else {
    pthread_cond_wait(cond, mutex);
    return SYS_ARCH_TIMEOUT;
  }
}
*/
/*-----------------------------------------------------------------------------------*//*u32_t
sys_arch_sem_wait(struct sys_sem **s, u32_t timeout)
{
  u32_t time_needed = 0;
  struct sys_sem *sem;
  LWIP_ASSERT("invalid sem", (s != NULL) && (*s != NULL));
  sem = *s;

  pthread_mutex_lock(&(sem->mutex));
  while (sem->c <= 0) {
    if (timeout > 0) {
      time_needed = cond_wait(&(sem->cond), &(sem->mutex), timeout);

      if (time_needed == SYS_ARCH_TIMEOUT) {
        pthread_mutex_unlock(&(sem->mutex));
        return SYS_ARCH_TIMEOUT;
      }timeout
      /*      pthread_mutex_unlock(&(sem->mutex));
              return time_needed; */
/*    } else {
      cond_wait(&(sem->cond), &(sem->mutex), 0);
    }
  }
  sem->c--;
  pthread_mutex_unlock(&(sem->mutex));
  return time_needed;
}
*/
/*SystemC Model */
u32_t /* problem here */
sys_arch_sem_wait(struct sys_sem **s, u32_t timeout)
{

  sc_dt::uint64 start_time;

  u32_t time_needed = 0;
  struct sys_sem *sem;

  LWIP_ASSERT("invalid sem", (s != NULL) && (*s != NULL)); 
  sem = *s;

  int taskID = taskManager.getTaskID(sc_core::sc_get_current_process_handle());


  //printf("sys_arch_sem_wait timeout %d ... ...%d %d\n",timeout, ((LwipCntxt*)(sem->ctxt)) -> NodeID, taskID);
 
  sem->blocked_task_id = taskID;
  while (sem->c <= 0) {
        { ( taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() ) ) -> os_port -> preWait(taskID, sem->blocking_task_id);}	
        if(sem->c > 0){
                (  taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() ) ) ->os_port->postWait(taskID);
                break;
        }

        if (timeout > 0) {
           start_time = sc_core::sc_time_stamp().value();
	   //printf("start_time is %ld\n", start_time);
           sc_core::wait(timeout, sc_core::SC_MS, sem->cond);
           //sc_core::wait(sem->cond);
           start_time = (sc_core::sc_time_stamp().value() - start_time);
	   		
	   //if((sem->id)==7)printf("time_needed is %ld\n", (u32_t)(start_time/1000000000));
	   time_needed = (u32_t)(start_time/1000000000);

	   if((time_needed == timeout) && (sem->c <= 0)){
        	(  taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() ) ) ->os_port->postWait(taskID);
	   	return SYS_ARCH_TIMEOUT;
           }

	}else{
	   //printf("sc_core::wait(sem->cond) ... ...%d \n\n", ((LwipCntxt*)(sem->ctxt)) -> NodeID);
	   sc_core::wait(sem->cond);	
	   //printf("sc_core::wait(sem->cond) --- ---%d \n\n", ((LwipCntxt*)(sem->ctxt)) -> NodeID);
	}

        (  taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() ) ) ->os_port->postWait(taskID);
    
  }
 // printf("sys_arch_sem_wait after %d ... ...%d %d\n",timeout, ((LwipCntxt*)(sem->ctxt)) -> NodeID, taskID);
  sem->c--;
  //printf("Return timeout %ld\n", time_needed);
  return time_needed;
}
/*-----------------------------------------------------------------------------------*/
/*void sys_sem_signal(struct sys_sem **s)
{
  struct sys_sem *sem;
  LWIP_ASSERT("invalid sem", (s != NULL) && (*s != NULL));
  sem = *s;

  pthread_mutex_lock(&(sem->mutex));
  sem->c++;

  if (sem->c > 1) {
    sem->c = 1;
  }

  pthread_cond_broadcast(&(sem->cond));
  pthread_mutex_unlock(&(sem->mutex));
}*/
/*SystemC Model*/
void
sys_sem_signal(struct sys_sem **s)
{

  struct sys_sem *sem;
  LWIP_ASSERT("invalid sem", (s != NULL) && (*s != NULL));
  sem = *s;

  int taskID = taskManager.getTaskID(sc_core::sc_get_current_process_handle());
//( (LwipCntxt*)(sem->ctxt) ) -> getOSModelTaskID( sc_core::sc_get_current_process_handle());
  sem->blocking_task_id = taskID;

  sem->c++;
  if (sem->c > 1) {
    sem->c = 1;
  }
  //( (LwipCntxt*)(sem->ctxt) ) -> os_port->preNotify(taskID, sem->blocked_task_id);
  sem->cond.notify(sc_core::SC_ZERO_TIME);
  //( (LwipCntxt*)(sem->ctxt) ) -> os_port->postNotify(taskID, sem->blocked_task_id);

}

/*-----------------------------------------------------------------------------------*/
/*static void
sys_sem_free_internal(struct sys_sem *sem)
{
  pthread_cond_destroy(&(sem->cond));
  pthread_mutex_destroy(&(sem->mutex));
  free(sem);
}*/
/*SystemC Model*/
static void
sys_sem_free_internal(struct sys_sem *sem)
{
  //pthread_cond_destroy(&(sem->cond));
  //pthread_mutex_destroy(&(sem->mutex));
  //delete sem;
  sem->free=1;
}
/*-----------------------------------------------------------------------------------*/
void
sys_sem_free(struct sys_sem **sem)
{
  if ((sem != NULL) && (*sem != SYS_SEM_NULL)) {
    SYS_STATS_DEC(sem.used);
    sys_sem_free_internal(*sem);
  }
}
#endif /* !NO_SYS */
/*-----------------------------------------------------------------------------------*/
/*u32_t
sys_now(void)
{
  struct timeval tv;
  u32_t sec, usec, msec;
  gettimeofday(&tv, NULL);

  sec = (u32_t)(tv.tv_sec - starttime.tv_sec);
  usec = (u32_t)(tv.tv_usec - starttime.tv_usec);
  msec = sec * 1000 + usec / 1000;

  return msec;
}*/
/*SystemC Model*/
u32_t
sys_now(void)
{
  //struct timeval tv;
  //double sec;
  sc_dt::uint64 msec;
  //gettimeofday(&tv, NULL);

  //sec = (u32_t)(tv.tv_sec - starttime.tv_sec);
  msec = (sc_core::sc_time_stamp().value()/1000000000) - starttime;

  return (u32_t)msec;
}
/*-----------------------------------------------------------------------------------*/
/*void
sys_init(void)
{
  gettimeofday(&starttime, NULL);
}
*/
void
sys_init(void)
{
  //gettimeofday(&starttime, NULL);
  starttime =(sc_dt::uint64) (sc_core::sc_time_stamp().value()/1000000000);
}

/*-----------------------------------------------------------------------------------*/
/*#if SYS_LIGHTWEIGHT_PROT
/** sys_prot_t sys_arch_protect(void)

This optional function does a "fast" critical region protection and returns
the previous protection level. This function is only called during very short
critical regions. An embedded system which supports ISR-based drivers might
want to implement this function by disabling interrupts. Task-based systems
might want to implement this by using a mutex or disabling tasking. This
function should support recursive calls from the same task or interrupt. In
other words, sys_arch_protect() could be called while already protected. In
that case the return value indicates that it is already protected.

sys_arch_protect() is only required if your port is supporting an operating
system.
*/
/*sys_prot_t
sys_arch_protect(void)
{
    /* Note that for the UNIX port, we are using a lightweight mutex, and our
     * own counter (which is locked by the mutex). The return code is not actually
     * used. */
/*    if (lwprot_thread != pthread_self())
    {
        /* We are locking the mutex where it has not been locked before *
        * or is being locked by another thread */
/*        pthread_mutex_lock(&lwprot_mutex);
        lwprot_thread = pthread_self();
        lwprot_count = 1;
    }
    else
        /* It is already locked by THIS thread */
/*        lwprot_count++;
    return 0;
}*/
/*-----------------------------------------------------------------------------------*/
/** void sys_arch_unprotect(sys_prot_t pval)

This optional function does a "fast" set of critical region protection to the
value specified by pval. See the documentation for sys_arch_protect() for
more information. This function is only required if your port is supporting
an operating system.
*/
/*void
sys_arch_unprotect(sys_prot_t pval)
{
    LWIP_UNUSED_ARG(pval);
    //TODO ???
    /*if (lwprot_thread == pthread_self())
    {
        if (--lwprot_count == 0)
        {
            lwprot_thread = (pthread_t) 0xDEAD;
            pthread_mutex_unlock(&lwprot_mutex);
        }
    }*/
/*}
#endif /* SYS_LIGHTWEIGHT_PROT */

/*-----------------------------------------------------------------------------------*/
sys_prot_t
sys_arch_protect(void)
{
    /* Note that for the UNIX port, we are using a lightweight mutex, and our
     * own counter (which is locked by the mutex). The return code is not actually
     * used. */
    if (lwprot_thread != sc_core::sc_get_current_process_handle())
    {
        /* We are locking the mutex where it has not been locked before *
        * or is being locked by another thread */
        lwprot_mutex.lock();
        lwprot_thread = sc_core::sc_get_current_process_handle();
        lwprot_count = 1;
    }
    else
        /* It is already locked by THIS thread */
        lwprot_count++;
    return 0;
}
void
sys_arch_unprotect(sys_prot_t pval)
{
    LWIP_UNUSED_ARG(pval);
    if (lwprot_thread == sc_core::sc_get_current_process_handle())
    {
        if (--lwprot_count == 0)
        {
            lwprot_thread = SC_DEAD_PROCESS;
            lwprot_mutex.unlock();
        }
    }
}
/*-----------------------------------------------------------------------------------*/

#ifndef MAX_JIFFY_OFFSET
#define MAX_JIFFY_OFFSET ((~0U >> 1)-1)
#endif

#ifndef HZ
#define HZ 100
#endif
