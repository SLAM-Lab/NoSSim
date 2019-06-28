#ifndef PROFILE_H
#define PROFILE_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdbool.h> 

#ifndef FRAME_NUM
#define FRAME_NUM 20
#endif

#ifndef MAX_EDGE_NUM
#define MAX_EDGE_NUM 6
#endif

#ifndef NUM_OF_FUNCTIONS
#define NUM_OF_FUNCTIONS 4
#endif

typedef struct def_profile_data {
  double start_time;
  bool valid[FRAME_NUM][MAX_EDGE_NUM][MAX_EDGE_NUM];
  double total_duration[FRAME_NUM][MAX_EDGE_NUM][MAX_EDGE_NUM];
  double avg_duration[FRAME_NUM][MAX_EDGE_NUM][MAX_EDGE_NUM];
  long calling_times[FRAME_NUM][MAX_EDGE_NUM][MAX_EDGE_NUM];
} profile_data;

void dump_profile(char* filename);
void profile_start();
void profile_end(uint32_t device_type, uint32_t cores);
void start_timer(const char* function_name, uint32_t arg1, uint32_t arg2, uint32_t arg3);
void stop_timer(const char* function_name, uint32_t arg1, uint32_t arg2, uint32_t arg3);
void copy_to(const char* function_name, 
             uint32_t from1, uint32_t from2, uint32_t from3,
             uint32_t to1, uint32_t to2, uint32_t to3);

#endif 
