#include "annotation.h"
static double duration;

void func_waitfor(double alpha, double delay, int lib_id, int fun_id){
   duration += alpha*delay;
}

void simulation_start(int lib_id, int fun_id){
   duration = 0;
}

void simulation_end(int lib_id, int fun_id){
   printf("Total execution time is: %f\n", duration);
}



