#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "svm.h"
#include "mydwt.h"
#include "svmpredict.h"
#include "mysqrs.h"
#include "mywqrs.h"
#include "noiseremoval.h"
#include "offload.h"
#include "context.h"
//#include "post_request.h"


#ifndef _ECG_H
#define _ECG_H

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

extern int  to;
extern double *samp;

extern struct svm_model *model;

extern double interpolation, no_of_input_windows;
extern int  wavelet, levels, from,ac1, ac2, ac3, ac4, dc1, dc2, dc3, dc4, nac, ndc, nsig, heartbeat_window, input_signal_window, fv_size;//fix value from configuration
extern char detector[16];
extern char config_file[40];
extern char data_record[40];
extern char svm_model_file[40];


int ECG_init();
int ECG_execute(char* result);
double* get_signal_block(int seq_num, int total_number);
void heartbeat_detection_dump_file(double *sig, ECG_ctxt *ctxt, char *in, char *out, char *other, int *numberof_rpeaks_ptr);
void heartbeat_segmentation_dump_file(char *in, char *out, char* other);
void feature_extraction_dump_file(char *in, char *out, char* other);
void classification_dump_file(char *in, char *out, char* other, char* result);
void flow(double *sig, ECG_ctxt *ctxt, char* result);
void edge_process_dump_file();



offload_data* heartbeat_detection(double *sig);
offload_data* heartbeat_segmentation(offload_data* detection_data);
offload_data* feature_extraction(offload_data* segmentation_data);
result_data* classification(offload_data* extraction_data, char* result);
void edge_process();

#endif
