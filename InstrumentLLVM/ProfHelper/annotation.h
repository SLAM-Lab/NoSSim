#include "profile.h"

#ifndef ANNOTATION_H
#define ANNOTATION_H
/*#define OS_MODEL*/

#ifdef __cplusplus
extern "C" {
#endif

void func_waitfor(double alpha, double delay, int lib_id, int fun_id);
void simulation_start(int lib_id, int fun_id);
void simulation_end(int lib_id, int fun_id);

#ifdef __cplusplus
}//extern "C"
#endif

#endif
