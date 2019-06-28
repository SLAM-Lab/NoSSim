#include "profile.h"
static prof_data lib_prof_data[TOTAL_LIB];
static call_stack func_stack;

static long bb_number;
static double current;
static double overhead;

static inline double now_sec(){
   struct timeval time;
   if (gettimeofday(&time,NULL)){
      return 0;
   }
   return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

static inline double now(){
   struct timeval time;
   if (gettimeofday(&time,NULL)){
      return 0;
   }
   return (double)time.tv_sec * 1000000 + (double)time.tv_usec;
}

static void acc_time(){
   double pre_duration = now() - current;
   int pre_lib_id = func_stack.lib_id[func_stack.size-1];
   int pre_fun_id = func_stack.fun_id[func_stack.size-1];
   double delta_t = (pre_duration - overhead)>0?(pre_duration - overhead):0;
   lib_prof_data[pre_lib_id].funcs[pre_fun_id].total_duration = lib_prof_data[pre_lib_id].funcs[pre_fun_id].total_duration + delta_t; 
}

static void save_profile(){
   int ii, jj;
   FILE *f = fopen("function_time.prof", "w");
   if (f == NULL){
      printf("Error opening file!\n");
      exit(1);
   }
   for(ii=0;ii<TOTAL_LIB;ii++){
      for(jj=0;jj<MAX_DEPTH;jj++){
         if(lib_prof_data[ii].funcs[jj].call_times!=0){
            fprintf(f, "%d %d %ld %f\n", ii, jj,
               lib_prof_data[ii].funcs[jj].call_times,
               lib_prof_data[ii].funcs[jj].total_duration/
                  ((double)lib_prof_data[ii].funcs[jj].call_times));
         }
      }
   }
   fclose(f);
   printf("The overhead is %f micro seconds\n", overhead);
}

void count_bb(int lib_id, int fun_id){
   printf("Counting dynamic BB numbers!");
   bb_number++;
}

void function_start(int lib_id, int fun_id){
   lib_prof_data[lib_id].funcs[fun_id].call_times = lib_prof_data[lib_id].funcs[fun_id].call_times + 1;
   if(func_stack.size!=0){
      /*Accumulate execution duration for the function on the top of calling stack*/
      acc_time();
   }
   /*Push current function onto the top of the calling stack*/ 
   func_stack.lib_id[func_stack.size] = lib_id;
   func_stack.fun_id[func_stack.size] = fun_id;
   func_stack.size++;
   current = now();
}

void function_exit(int lib_id, int fun_id){
   /*Record the execution duration and remove the function from the top of the calling stack*/
   acc_time();
   func_stack.size--;
   if(func_stack.size!=0)
   current = now();
}

void program_start(int lib_id, int fun_id){
   int ii,jj;
   for(ii=0;ii<TOTAL_LIB;ii++){
      for(jj=0;jj<MAX_DEPTH;jj++){
         lib_prof_data[ii].funcs[jj].total_bbs=0;
         lib_prof_data[ii].funcs[jj].call_times=0;
         lib_prof_data[ii].funcs[jj].total_duration=0;
      }
   }
   func_stack.size=0;
   /*Calculate the profiling overhead beforehand*/
   function_start(0, 0);
   function_exit(0, 0);
   overhead = lib_prof_data[0].funcs[0].total_duration;
   lib_prof_data[0].funcs[0].call_times=0;
   lib_prof_data[0].funcs[0].total_duration=0;
   /*Calculate the profiling overhead beforehand*/

   function_start(lib_id, fun_id);
}

void program_end(int lib_id, int fun_id){
   function_exit(lib_id, fun_id);
   save_profile();
}



