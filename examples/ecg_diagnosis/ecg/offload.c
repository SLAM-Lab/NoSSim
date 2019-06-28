#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "offload.h"
int file_size_in_bytes(char* filename){
   int size = 0;
   FILE *f = fopen(filename, "rb");
   if (f == NULL) 
      return -1; // -1 means file opening fail 
   fseek(f, 0, SEEK_END);
   size = ftell(f);
   fclose(f);
   return size;
}

void write_char_array(char array[], int size, char* fileName){

  FILE* file = fopen (fileName, "wb");
  int i;
  for( i = 0; i < size; i++){

    char f = array[i];
    fwrite(&f, sizeof(char), 1, file);

  }
  fclose(file);
}



char* read_char_array(int size, char* fileName){
  //char fileName[30] = "testFile.dat";
  //Hi_D = (double *)malloc(2*wavelet*sizeof(double));
 

  FILE*  file = fopen(fileName, "rb");
  char* results = (char *)malloc((size+1)*sizeof(char));
  int i=0;
  while(1){
    char f;
    int n  = fread(&f, sizeof(char), 1, file);
    if (n<1){  	
		//printf("%d\n", i);
		break;
    }
    results[i] = f;
    i++;
  }
  fclose(file);
  char* array = (char *)malloc((i)*sizeof(char));
  int ii;
  for(ii=0; ii<(i); ii++){
  	array[ii] = results[ii];
	//printf("%f\n", array[ii]);
  }
  free(results);
  return array;
}



void write_int_array(int array[], int size, char* fileName){

  FILE* file = fopen (fileName, "wb");
  int i;
  for( i = 0; i < size; i++){

    int f = array[i];
    fwrite(&f, sizeof(int), 1, file);

  }
  fclose(file);
}



int* read_int_array(int size, char* fileName){
  //char fileName[30] = "testFile.dat";
  //Hi_D = (double *)malloc(2*wavelet*sizeof(double));
 

  FILE*  file = fopen(fileName, "rb");
  int* results = (int *)malloc((size+1)*sizeof(int));
  int i=0;
  while(1){
    int f;
    int n  = fread(&f, sizeof(int), 1, file);
    if (n<1){  	
		//printf("%d\n", i);
		break;
    }
    results[i] = f;
    i++;
  }
  fclose(file);
  int* array = (int *)malloc((i)*sizeof(int));
  int ii;
  for(ii=0; ii<(i); ii++){
  	array[ii] = results[ii];
	//printf("%f\n", array[ii]);
  }
  free(results);
  return array;
}

void write_double_array(double array[], int size, char* fileName){

  FILE* file = fopen (fileName, "wb");
  int i;
  for( i = 0; i < size; i++){

    double f = array[i];
    fwrite(&f, sizeof(double), 1, file);

  }
  fclose(file);
}


double* read_double_array(int size, char* fileName){
  //char fileName[30] = "testFile.dat";
  //Hi_D = (double *)malloc(2*wavelet*sizeof(double));
 

  FILE*  file = fopen(fileName, "rb");
  double* results = (double *)malloc((size+1)*sizeof(double));
  int i=0;
  while(1){
    double f;
    int n  = fread(&f, sizeof(double), 1, file);
    if (n<1){  	
		//printf("%d\n", i);
		break;
    }
    results[i] = f;
    i++;
  }
  fclose(file);
  double* array = (double *)malloc((i)*sizeof(double));
  int ii;
  for(ii=0; ii<(i); ii++){
  	array[ii] = results[ii];
	//printf("%f\n", array[ii]);
  }
  free(results);
  return array;
}


offload_data* make_offload_data(){
   offload_data* data = (offload_data*)malloc(sizeof(offload_data));
   data->item_number = 0;
   data->rpeaks = 0;
   return data;
}

void free_offload_data(offload_data* data){
   int i;
   for(i = 0; i < data->item_number; i++){
      sig_data* prehead =  data->head;
      data->head = data->head->next;
      free(prehead->data);
      free(prehead);
   }
   free(data);
}

sig_data* get_offload_data(offload_data* data, uint32_t pos){
   int i;
   sig_data* ret =  data->head;
   for(i = 0; i < pos; i++){
      ret = ret->next;
   }
   return ret;
}

void add_offload_data(offload_data* data, char* blob, uint32_t blob_size){
   int i;
   sig_data* sig = (sig_data*)malloc(sizeof(sig_data));
   sig->size = blob_size;
   sig->data = (char*)malloc(blob_size);
   memcpy(sig->data, blob, blob_size);
   sig_data* insert_point =  data->head;
   if(data->item_number == 0) {data->item_number++; data->head = sig; data->tail = sig;return;}
   data->item_number++;
   data->tail->next = sig;
   data->tail = data->tail->next;
}

