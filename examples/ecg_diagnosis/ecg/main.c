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
#include "ecg.h"
#include "profile.h"
#include <iostream>

result_data* classification_no_output_file(offload_data* extraction_data){
	int normal = 0;
	int abnormal = 0;
	int numberof_rpeaks_in;
	double *rpeak_in;
	double *coef_in;
	int label;
	int PR_window = interpolation*54;//86;
	int QT_window = interpolation*74;//170;
	int i,j;

	numberof_rpeaks_in = extraction_data->rpeaks;
        int extraction_data_num = 0;

	for (i=0; i<numberof_rpeaks_in; i++) {//rpeak[0]
		rpeak_in = (double*)get_offload_data(extraction_data, extraction_data_num)->data;  
                extraction_data_num++;    
		if ((((int)rpeak_in[i]-PR_window)>=0) and (((int)rpeak_in[i]+QT_window)<input_signal_window)) {

			/* CLASSIFICATION */ //input: double coef[fv_size] || output: int label
                        coef_in = (double*)get_offload_data(extraction_data, extraction_data_num)->data;  
                        extraction_data_num++;
  
			label = predict(coef_in, -1, fv_size);
			if (label==1) normal = normal + 1; else abnormal = abnormal + 1;
		}
	} 
        free_offload_data(extraction_data);
        result_data* results = (result_data*)malloc(sizeof(result_data));
        results -> normal = normal;
        results -> abnormal = abnormal;
        results -> rpeaks = numberof_rpeaks_in;
        return results;
}

static inline double now_usec(){
   struct timeval time;
   if (gettimeofday(&time,NULL)) return 0;
   return (double)time.tv_sec * 1000000 + (double)time.tv_usec;
}

int main(){
  //edge_process();
  ECG_init();
  int node_id = 0;

  FILE *fresult;
  fresult = fopen("out.log","w"); 
  fclose(fresult);

  profile_start();
  int seq_num = 0;
  for(seq_num = 0; seq_num <FRAME_NUM; seq_num++){
         double* raw_sig = get_signal_block(seq_num, FRAME_NUM);

         double t = now_usec();
         start_timer("heartbeat_detection", seq_num, 0, 0);
         offload_data* tmp = heartbeat_detection(raw_sig);
         stop_timer("heartbeat_detection", seq_num, 0, 0);
         std::cout << "heartbeat_detection:" << now_usec()-t << std::endl;

         t = now_usec();
         start_timer("heartbeat_segmentation", seq_num, 0, 0);
         tmp = heartbeat_segmentation(tmp);
         stop_timer("heartbeat_segmentation", seq_num, 0, 0);
         std::cout << "heartbeat_segmentation:" << now_usec()-t << std::endl;

         t = now_usec();
         start_timer("feature_extraction", seq_num, 0, 0);
         tmp = feature_extraction(tmp);
         stop_timer("feature_extraction", seq_num, 0, 0);
         std::cout << "feature_extraction:" << now_usec()-t << std::endl;

         t = now_usec();
         start_timer("classification", seq_num, 0, 0);
         result_data* results = classification_no_output_file(tmp);
         stop_timer("classification", seq_num, 0, 0);
         std::cout << "classification:" << now_usec()-t << std::endl;

         printf("rpeaks: %d\n", results->rpeaks);
         printf("\t\t\tnormal: %d\n\t\t\tabnormal: %d\n", results->normal, results->abnormal);

  }
  profile_end(0, 1);
  return 0;
}
