#ifndef _OFFLOAD_H
#define _OFFLOAD_H

void write_double_array(double array[], int size, char* fileName);
double* read_double_array(int size, char* fileName);

void write_int_array(int array[], int size, char* fileName);
int* read_int_array(int size, char* fileName);

#endif
