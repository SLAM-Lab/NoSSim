#ifndef _OFFLOAD_H
#define _OFFLOAD_H
#include <stdint.h>
int file_size_in_bytes(char* filename);

void write_double_array(double array[], int size, char* fileName);
double* read_double_array(int size, char* fileName);

void write_int_array(int array[], int size, char* fileName);
int* read_int_array(int size, char* fileName);

void write_char_array(char array[], int size, char* fileName);
char* read_char_array(int size, char* fileName);

typedef struct results_data {
   uint32_t rpeaks;
   uint32_t normal;
   uint32_t abnormal;
} result_data;

typedef struct signal_data {
   uint32_t size;
   char* data;
   struct signal_data* next;
} sig_data;

typedef struct offloading_data {
   uint32_t rpeaks;
   uint32_t item_number;
   sig_data* head;
   sig_data* tail;
} offload_data;

offload_data* make_offload_data();
void free_offload_data(offload_data* data);
sig_data* get_offload_data(offload_data* data, uint32_t pos);
void add_offload_data(offload_data* data, char* blob, uint32_t blob_size);

#endif
