/*********************************************
 * Mix Task Set Example
 * Parisa Razaghi, UT Austin, parisa.r@utexas.edu
 * Last update: July 2013
 ********************************************/
 
#ifndef SC_CONFIG__H
#define SC_CONFIG__H

#define CON_CPU_NUM 2
#define CON_INTR_NUM 3

const unsigned long int const_intr1_address = 0x10000010;
const unsigned long int const_intr1_handler_address = 0x10000020;
const unsigned long int const_intr1_task_address = 0x10000040;
const unsigned long int const_intr2_address = 0x20000020;
const unsigned long int const_intr2_handler_address = 0x20000040;
const unsigned long int const_intr2_task_address = 0x20000080;

const unsigned long long const_end_time =  5000000000ull;

#define MAX_INTR 2
/* Select task set */
//#define L1
//#define S1
//#define S2
#define miniSet

/* Select interrupt priority level */
//#define INTL
//#define INTM
#define INTH
//#define NO_INTR /* with a selected intr. priority level*/



#ifdef miniSet
#define TOTAL_TASK_NUMBER 0
#define CORE0_TASK_NUMBER 0
#define CORE1_TASK_NUMBER 0
#define TOTAL_INTR_TASK_NUMBER 1

const sc_dt::uint64 const_task_delay[] = {8006000ull, 6008000ull};
const sc_dt::uint64 const_task_period[] ={49000000ull, 30000000ull};
const unsigned int const_task_priority[] ={49, 30};
const int const_init_core[] = {1, 1};
const int const_id[] = {0, 0};


const sc_dt::uint64 const_task_delay_INTH[] = {100020ull, 100020ull};
const sc_dt::uint64 const_task_period_INTH[] ={10000000ull, 10000000ull};
const unsigned int const_task_priority_INTH[] ={1, 1};
const sc_dt::uint64 const_task_delay_INTM[] = {600020ull, 350020ull};
const sc_dt::uint64 const_task_period_INTM[] ={60000000ull, 35000000ull};
const unsigned int const_task_priority_INTM[] ={60, 35};
const sc_dt::uint64 const_task_delay_INTL[] = {1000020ull, 1000020ull};
const sc_dt::uint64 const_task_period_INTL[] ={100000000ull, 100000000ull};
const unsigned int const_task_priority_INTL[] ={100, 100};
const int const_init_core_INT[] = {0, 1};
const int const_id_INT[] = {9, 9};

#endif







#ifdef L1
#define TOTAL_TASK_NUMBER 7
#define CORE0_TASK_NUMBER 3
#define CORE1_TASK_NUMBER 4
#define TOTAL_INTR_TASK_NUMBER 2

const sc_dt::uint64 const_task_delay[] = {8006000ull, 6008000ull,20011000ull, 13011000ull, 6008000ull, 5008000ull, 6008000ull };
const sc_dt::uint64 const_task_period[] ={49000000ull, 30000000ull, 70000000ull,  65000000ull, 31000000ull, 39000000ull, 40000000ull};
const unsigned int const_task_priority[] ={49, 30, 70, 65, 31, 39, 40};
const int const_init_core[] = {0,0,0, 1,1,1,1};
const int const_id[] = {0, 1, 2, 0, 1, 2, 3};

const sc_dt::uint64 const_task_delay_INTH[] = {100020ull, 100020ull};
const sc_dt::uint64 const_task_period_INTH[] ={10000000ull, 10000000ull};
const unsigned int const_task_priority_INTH[] ={1, 1};
const sc_dt::uint64 const_task_delay_INTM[] = {600020ull, 350020ull};
const sc_dt::uint64 const_task_period_INTM[] ={60000000ull, 35000000ull};
const unsigned int const_task_priority_INTM[] ={60, 35};
const sc_dt::uint64 const_task_delay_INTL[] = {1000020ull, 1000020ull};
const sc_dt::uint64 const_task_period_INTL[] ={100000000ull, 100000000ull};
const unsigned int const_task_priority_INTL[] ={100, 100};
const int const_init_core_INT[] = {0, 1};
const int const_id_INT[] = {9, 9};

#endif

#ifdef S1
#define TOTAL_TASK_NUMBER 13
#define CORE0_TASK_NUMBER 7
#define CORE1_TASK_NUMBER 6
#define TOTAL_INTR_TASK_NUMBER 2
    
const sc_dt::uint64 const_task_delay[] = {2008000ull, 1006000ull, 10010000ull,1006000ull, 1006000ull, 1006000ull, 2008000ull, 
                                                                    1006000ull, 2008000ull, 1006000ull, 1006000ull, 2008000ull, 5008000ull  };
const sc_dt::uint64 const_task_period[] ={34000000ull, 27000000ull, 98000000ull, 17000000ull, 56000000ull, 29000000ull, 95000000ull,
                                                                     35000000ull, 29000000ull, 45000000ull, 67000000ull, 25000000ull, 50000000ull};
const unsigned int const_task_priority[] ={34, 27, 98, 17, 56, 29, 95, 35, 29, 45, 67, 25, 50};
const int const_init_core[] = {0,0,0,0,0,0,0, 1,1,1,1,1,1};
const int const_id[] = {0, 1, 2,3, 4, 5, 6, 0, 1, 2, 3, 4, 5};

const sc_dt::uint64 const_task_delay_INTH[] = {100020ull, 100020ull};
const sc_dt::uint64 const_task_period_INTH[] ={10000000ull, 10000000ull};
const unsigned int const_task_priority_INTH[] ={1, 1};
const sc_dt::uint64 const_task_delay_INTM[] = {500020ull, 400020ull};
const sc_dt::uint64 const_task_period_INTM[] ={50000000ull, 40000000ull};
const unsigned int const_task_priority_INTM[] ={50, 40};
const sc_dt::uint64 const_task_delay_INTL[] = {1000020ull, 1000020ull};
const sc_dt::uint64 const_task_period_INTL[] ={100000000ull, 100000000ull};
const unsigned int const_task_priority_INTL[] ={100, 100};
const int const_init_core_INT[] = {0, 1};
const int const_id_INT[] = {9, 9};

#endif

#ifdef S2
#define TOTAL_TASK_NUMBER 18
#define CORE0_TASK_NUMBER 9
#define CORE1_TASK_NUMBER 9
#define TOTAL_INTR_TASK_NUMBER 2
    
const sc_dt::uint64 const_task_delay[] = {1006000ull, 8008000ull, 3008000ull, 8008000ull, 1006000ull, 2008000ull,  2008000ull, 7008000ull, 1006000ull,
                                                                    3008000ull, 1006000ull, 5008000ull, 4008000ull, 1006000ull, 5008000ull, 1006000ull, 3008000ull, 3008000ull};
const sc_dt::uint64 const_task_period[] ={98000000ull, 85000000ull, 89000000ull, 75000000ull, 17000000ull, 33000000ull, 59000000ull, 69000000ull, 34000000ull, 
                                                                     33000000ull, 68000000ull, 77000000ull, 75000000ull, 54000000ull, 45000000ull, 15000000ull, 71000000ull, 86000000ull};
const unsigned int const_task_priority[] ={98, 85, 89, 75, 17, 33, 59, 69, 34, 33, 68, 77, 75, 54, 45, 15, 71, 86};
const int const_init_core[] = {0,0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,1};
const int const_id[] = {0, 1, 2,3, 4, 5, 6, 7, 8, 0, 1, 2, 3, 4,  5, 6, 7, 8};

const sc_dt::uint64 const_task_delay_INTH[] = {100020ull, 100020ull};
const sc_dt::uint64 const_task_period_INTH[] ={10000000ull, 10000000ull};
const unsigned int const_task_priority_INTH[] ={1, 1};
const sc_dt::uint64 const_task_delay_INTM[] = {600020ull, 600020ull};
const sc_dt::uint64 const_task_period_INTM[] ={60000000ull, 60000000ull};
const unsigned int const_task_priority_INTM[] ={60, 60};
const sc_dt::uint64 const_task_delay_INTL[] = {1000020ull, 1000020ull};
const sc_dt::uint64 const_task_period_INTL[] ={100000000ull, 100000000ull};
const unsigned int const_task_priority_INTL[] ={100, 100};
const int const_init_core_INT[] = {0, 1};
const int const_id_INT[] = {9, 9};

#endif
    
#ifdef INTL
const int const_intr1_priority = const_task_priority_INTL[0];
const int const_intr2_priority = const_task_priority_INTL[1];
const unsigned long long const_intr1_load =  const_task_period_INTL[0];
const unsigned long long const_intr2_load = const_task_period_INTL[1];
#endif
#ifdef INTM
const int const_intr1_priority = const_task_priority_INTM[0];
const int const_intr2_priority = const_task_priority_INTM[1];
const unsigned long long const_intr1_load =  const_task_period_INTM[0];
const unsigned long long const_intr2_load = const_task_period_INTM[1];
#endif
#ifdef INTH
const int const_intr1_priority = const_task_priority_INTH[0];
const int const_intr2_priority = const_task_priority_INTH[1];
const unsigned long long const_intr1_load =  const_task_period_INTH[0];
const unsigned long long const_intr2_load = const_task_period_INTH[1];
#endif

#endif

