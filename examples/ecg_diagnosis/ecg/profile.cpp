#include "profile.h"

static profile_data prof_data[NUM_OF_FUNCTIONS];

static char function_list[NUM_OF_FUNCTIONS][40]={
   "heartbeat_detection",
   "heartbeat_segmentation",
   "feature_extraction",
   "classification"
};

static inline double now_sec(){
   struct timeval time;
   if (gettimeofday(&time,NULL)) return 0;
   return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

static inline double now_usec(){
   struct timeval time;
   if (gettimeofday(&time,NULL)) return 0;
   return (double)time.tv_sec * 1000000 + (double)time.tv_usec;
}

static inline uint32_t get_function_id(const char* function_name){
   uint32_t id = 0; 
   for(id = 0; id < NUM_OF_FUNCTIONS; id++){
      if(strcmp(function_name, function_list[id]) == 0) return id;
   }
   return 0;
}

static inline char* get_function_name(uint32_t id){
   if(id < NUM_OF_FUNCTIONS) return function_list[id];
   return NULL;
}

void dump_profile(char* filename){
   FILE *f = fopen(filename, "w");
   if (f == NULL){
      printf("Error opening file!\n");
      exit(1);
   }
   uint32_t function_id;
   uint32_t arg1;
   uint32_t arg2;
   uint32_t arg3;
   fprintf(f, "name_of_function	arg1	arg2	arg3	calling#	avg_time\n");
   for(function_id = 0; function_id < NUM_OF_FUNCTIONS; function_id++){
      for(arg1 = 0; arg1 < FRAME_NUM; arg1++){
         for(arg2 = 0; arg2 < MAX_EDGE_NUM; arg2++){
            for(arg3 = 0; arg3 < MAX_EDGE_NUM; arg3++){
               if(prof_data[function_id].valid[arg1][arg2][arg3] == false) continue;
               fprintf(f, "%s	%d	%d	%d	%ld	%f\n", 
                    get_function_name(function_id), 
                    arg1, 
                    arg2,
                    arg3,
                    prof_data[function_id].calling_times[arg1][arg2][arg3], 
                    prof_data[function_id].avg_duration[arg1][arg2][arg3]      
               );
            }
         }
      }
   }
   fclose(f);
}

void profile_start(){
   uint32_t function_id;
   uint32_t arg1;
   uint32_t arg2;
   uint32_t arg3;
   for(function_id = 0; function_id < NUM_OF_FUNCTIONS; function_id++){
      for(arg1 = 0; arg1 < FRAME_NUM; arg1++){
         for(arg2 = 0; arg2 < MAX_EDGE_NUM; arg2++){
            for(arg3 = 0; arg3 < MAX_EDGE_NUM; arg3++){
               prof_data[function_id].valid[arg1][arg2][arg3] = false;
               prof_data[function_id].total_duration[arg1][arg2][arg3] = 0.0;
               prof_data[function_id].avg_duration[arg1][arg2][arg3] = 0.0;
               prof_data[function_id].calling_times[arg1][arg2][arg3] = 0;
            }
         }
      }
   }
}

void profile_end(uint32_t device_type, uint32_t cores){
   char filename[50];
   sprintf(filename, "pi_%d_%d_core.prof", device_type, cores);
   dump_profile(filename);
}

void start_timer(const char* function_name, uint32_t arg1, uint32_t arg2, uint32_t arg3){
   uint32_t function_id = get_function_id(function_name);
   prof_data[function_id].start_time = now_usec();
}

void stop_timer(const char* function_name, uint32_t arg1, uint32_t arg2, uint32_t arg3){
   uint32_t function_id = get_function_id(function_name);
   prof_data[function_id].total_duration[arg1][arg2][arg3] += 
                          now_usec() - prof_data[function_id].start_time;
   prof_data[function_id].calling_times[arg1][arg2][arg3]++;
   prof_data[function_id].avg_duration[arg1][arg2][arg3] =
                     prof_data[function_id].total_duration[arg1][arg2][arg3]/
                      ((double)(prof_data[function_id].calling_times[arg1][arg2][arg3]));
   prof_data[function_id].valid[arg1][arg2][arg3] = true;
}

void copy_to(const char* function_name, 
             uint32_t from1, uint32_t from2, uint32_t from3,
             uint32_t to1, uint32_t to2, uint32_t to3){
   uint32_t function_id = get_function_id(function_name);
   prof_data[function_id].total_duration[to1][to2][to3] = prof_data[function_id].total_duration[from1][from2][from3];
   prof_data[function_id].calling_times[to1][to2][to3] = prof_data[function_id].calling_times[from1][from2][from3];
   prof_data[function_id].avg_duration[to1][to2][to3] = prof_data[function_id].avg_duration[from1][from2][from3];
   prof_data[function_id].valid[to1][to2][to3] = prof_data[function_id].valid[from1][from2][from3];
}




