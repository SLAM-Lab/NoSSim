#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "offload.h"

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
