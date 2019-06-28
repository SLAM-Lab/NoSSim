#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <float.h>
#include <limits.h>
#include <time.h>

#include <sys/time.h>

#ifndef PROFILE_H
#define PROFILE_H
#define TOTAL_LIB 20
#define MAX_DEPTH 1000

#ifdef __cplusplus
extern "C" {
#endif

void count_bb(int lib_id, int fun_id);
void function_start(int lib_id, int fun_id);
void function_exit(int lib_id, int fun_id);
void program_start(int lib_id, int fun_id);
void program_end(int lib_id, int fun_id);

typedef struct calling_stack {
  int fun_id[MAX_DEPTH];
  int lib_id[MAX_DEPTH];
  int size;
} call_stack;

typedef struct func_profile_data {
  long total_bbs;
  long call_times;
  double total_duration;
} func_prof_data;

typedef struct profile_data {
  func_prof_data funcs[MAX_DEPTH]; //indexed by function ID
} prof_data;

#ifdef __cplusplus
}//extern "C"
#endif

#endif
