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

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define TOTAL_HEARTBEAT 20

int  to;
double *samp;
struct svm_model *model;

double interpolation, no_of_input_windows;
int  wavelet, levels, from,ac1, ac2, ac3, ac4, dc1, dc2, dc3, dc4, nac, ndc, nsig, heartbeat_window, input_signal_window, fv_size;//fix value from configuration
char detector[16];
char config_file[40];
char data_record[40];
char svm_model_file[40];

void flow(double *sig, ECG_ctxt *ctxt, char* result);

int ECG_init() {
	char line[80];
	FILE *fp;

	sprintf(config_file, "%s", "./ecg_config/configs/config_new.txt");
	sprintf(data_record, "%s", "./ecg_config/data_360/data_101.txt");
	sprintf(svm_model_file, "%s", "./ecg_config/models_360/model_config_4.txt");

	fp = fopen(config_file,"r");	//open file containing the feature vector configuration
	fgets(line,80,fp);
	if (strcmp(line,"coefficients\n")) printf("Error in config file.\n");
	fgets(line,80,fp);
	ac1 = atoi(line);
	fgets(line,80,fp);
	ac2 = atoi(line);
	fgets(line,80,fp);
	ac3 = atoi(line);
	fgets(line,80,fp);
	ac4 = atoi(line);
	fgets(line,80,fp);
	dc1 = atoi(line);
	fgets(line,80,fp);
	dc2 = atoi(line);
	fgets(line,80,fp);
	dc3 = atoi(line);
	fgets(line,80,fp);
	dc4 = atoi(line);
	fgets(line,80,fp);                                                                                                   
        if (strcmp(line,"interpolation\n")) printf("Error in config file.\n");
	fgets(line,80,fp);                                                                                                   
        interpolation = atof(line);
	fgets(line,80,fp);                                                                                                   
        if (strcmp(line,"wavelet\n")) printf("Error in config file.\n");
	fgets(line,80,fp);                                                                                                   
        wavelet = atoi(line);
	fgets(line,80,fp);                                                                                                   
        if (strcmp(line,"R peak detector\n")) printf("Error in config file.\n");
	fgets(detector,14,fp); 
	fclose(fp);



	char folder_num[6];	
	int num = interpolation*360;
	snprintf(folder_num,6,"%d",num);

	printf("folder_num: %s\n",folder_num);
	printf("filename: %s\n", config_file);
	printf("filename: %s\n",data_record);
	printf("filename: %s\n",svm_model_file);

	model = svm_load_model(svm_model_file);	

	input_signal_window = interpolation*256;//512;//256;                                                                        
        heartbeat_window = interpolation*129 - (interpolation-1); //heartbeat window                                                   
        no_of_input_windows = 650000/input_signal_window; 

	int l1 = floor((heartbeat_window-1)/2) + wavelet;
	int l2 = floor((l1-1)/2) + wavelet;
	int l3 = floor((l2-1)/2) + wavelet;
	int l4 = floor((l3-1)/2) + wavelet;
	nac = 0;
	ndc = 0;
	if (ac1==1) nac = nac + l1;//130;
	if (ac2==1) nac = nac + l2;//66;
	if (ac3==1) nac = nac + l3;//34;
	if (ac4==1) nac = nac + l4;//18;
	if (dc1==1) ndc = ndc + l1;//130;
	if (dc2==1) ndc = ndc + l2;//66;
	if (dc3==1) ndc = ndc + l3;//34;
	if (dc4==1) ndc = ndc + l4;//18;
	fv_size = nac + ndc;

	levels = 0;                                                                                       
        if (ac1==1 || dc1==1) levels = 1;                                                                 
        if (ac2==1 || dc2==1) levels = 2;                                                                 
        if (ac3==1 || dc3==1) levels = 3;                                                                 
        if (ac4==1 || dc4==1) levels = 4;


	nsig= 2;
        to = input_signal_window*250/(interpolation*360);
	return 0;
}



int ECG_execute(char* result) {


	int gain, baseline, i, j;
	char line[80];
	double rp;
	FILE *fp;

	double *full_sig;
	gain = 200;
	baseline = 1024;
	ECG_ctxt ctxt;
	ctxt.learning = 1;

	full_sig = (double *)malloc((input_signal_window)*sizeof(double));



	FILE *fp_result;
	char result_file[50];
	sprintf(result_file, "%s/out.log", result);
	fp_result = fopen(result_file,"w"); 
	fclose(fp_result);


	fp = fopen(data_record,"r"); 

	if (fp == 0) printf("Error: could not open data file.\n");

	i=0;
	for (j=0; j<no_of_input_windows*input_signal_window; j++) {
		if ((TOTAL_HEARTBEAT*input_signal_window)== j) {printf("%d\n", j); break;}
		fgets(line,80,fp);
		sscanf(line,"%lf", &rp);
		full_sig[i] = (rp - baseline)/gain;
		i = i + 1;
		if (i==input_signal_window) { 
			flow(full_sig, &ctxt, result); 
			i=0;
		}
	}
	fclose(fp);
	free(full_sig);                                  
	//svm_free_and_destroy_model(&model);


	return 0;
	//exit(0);

}


void heartbeat_detection_dump_file(double *sig, char *in, char *out, char *other, int *numberof_rpeaks_ptr){

	int i,j,k,m,n,nl,freq,gain,baseline,numberof_rpeaks;
	double *rpeak,*beat,*coef;

	char dat_file[50];
	int iter;

	freq = 360;
	gain = 200;
	baseline = 1024;
	nl = 0;
	from = 0;
	//to = input_signal_window;

    
	samp = (double *)malloc((input_signal_window)*sizeof(double));
	rpeak = (double *)malloc(20*sizeof(double));
	
	struct timeval t0, t1;
	double elapsed, nr_elapsed, rp_elapsed, dwt_elapsed, svm_elapsed;
	int len = input_signal_window;


	/* NOISE REMOVAL */ //input: double sig[input_signal_window], output: double sig[input_signal_window], double samp[input_signal_window]

	sig = noiseremoval(sig, len);
	for (i=0; i<len; i++) {
		samp[i] = sig[i]*gain + baseline;
	}
	
	/* HEARTBEAT DETECTION */ //input: samp[input_signal_window], output: double rpeak[20], int numberof_rpeaks, double sig[input_signal_window]
	ECG_ctxt ctxt;
	ctxt.learning = 1;
	ctxt.T1=0;
	ctxt.Yn=0;
	ctxt.Yn1=0;
	ctxt.Yn2=0;
	ctxt.tt=0;
	ctxt.aet=0;
	ctxt.et=0;

	if (strcmp(detector,"sqrs_detector")) numberof_rpeaks = wqrs(rpeak, samp, &ctxt);
	else numberof_rpeaks = sqrs(rpeak, samp);

	(*numberof_rpeaks_ptr) = numberof_rpeaks;

	//Dumping data
	for(iter = 0; iter<numberof_rpeaks; iter ++){	
		sprintf(dat_file, "%s/sig%d.dat", out, iter);
		write_double_array(sig, input_signal_window, dat_file);
		sprintf(dat_file, "%s/rpeak%d.dat", other, iter);
		write_double_array(rpeak, 20, dat_file);

	}
	sprintf(dat_file, "%s/numberof_rpeaks.dat", other);
	write_int_array(&numberof_rpeaks, 1, dat_file);

	//read_double_array(input_signal_window, dat_file);
	//read_int_array(input_signal_window, dat_file);

	//int normal = 0;
	//int abnormal = 0;
	//int PR_window = interpolation*54;//86;
	//int QT_window = interpolation*74;//170;
	//heartbeat_window = PR_window + QT_window + 1



	//int numberof_rpeaks_in;
	//double *rpeak_in;
	//double *sig_in;
	//double *beat_in;
	//double *coef_in;
	free(samp);
	free(rpeak);
	//free(sig);
} 

offload_data* heartbeat_detection(double *sig){
	int i,j,k,m,n,nl,freq,gain,baseline,numberof_rpeaks;
	double *rpeak,*beat,*coef;
	int iter;

	freq = 360;
	gain = 200;
	baseline = 1024;
	nl = 0;
	from = 0;
    
	samp = (double *)malloc((input_signal_window)*sizeof(double));
	rpeak = (double *)malloc(20*sizeof(double));
	
	struct timeval t0, t1;
	double elapsed, nr_elapsed, rp_elapsed, dwt_elapsed, svm_elapsed;
	int len = input_signal_window;

	/* NOISE REMOVAL */ //input: double sig[input_signal_window], output: double sig[input_signal_window], double samp[input_signal_window]
	sig = noiseremoval(sig, len);
	for (i=0; i<len; i++) {
		samp[i] = sig[i]*gain + baseline;
	}
	
	/* HEARTBEAT DETECTION */ //input: samp[input_signal_window], output: double rpeak[20], int numberof_rpeaks, double sig[input_signal_window]
	ECG_ctxt ctxt;
	ctxt.learning = 1;
	ctxt.T1=0;
	ctxt.Yn=0;
	ctxt.Yn1=0;
	ctxt.Yn2=0;
	ctxt.tt=0;
	ctxt.aet=0;
	ctxt.et=0;

	if (strcmp(detector,"sqrs_detector")) numberof_rpeaks = wqrs(rpeak, samp, &ctxt);
	else numberof_rpeaks = sqrs(rpeak, samp);

        offload_data* stage_data = make_offload_data();

	//Dumping data
	for(iter = 0; iter<numberof_rpeaks; iter ++){	
             add_offload_data(stage_data, (char*)sig, input_signal_window*(sizeof(double)));               
             add_offload_data(stage_data, (char*)rpeak, 20*(sizeof(double)));    
	}
        stage_data->rpeaks = numberof_rpeaks; 
	free(samp);
	free(rpeak);
        free(sig);
        return stage_data;
} 

offload_data* heartbeat_segmentation(offload_data* detection_data){
	/* HEARTBEAT SEGMENTATION *///input: double sig[input_signal_window], double rpeak[20] || output: double beat[PR_window + QT_window+1]
	int numberof_rpeaks_in;
	double *rpeak_in;
	double *sig_in;

	double *beat;
	int PR_window = interpolation*54;//86;
	int QT_window = interpolation*74;//170;

	int i,j;

  	beat = (double *)malloc((PR_window + QT_window+1)*sizeof(double));

        offload_data* segmentation_data = make_offload_data();
	numberof_rpeaks_in = detection_data->rpeaks;
        segmentation_data->rpeaks = numberof_rpeaks_in; 
        int detection_data_num = 0;
	for (i=0; i<numberof_rpeaks_in; i++) {//rpeak[0]
		sig_in = (double*)get_offload_data(detection_data, detection_data_num)->data;      
                detection_data_num++;         
		rpeak_in = (double*)get_offload_data(detection_data, detection_data_num)->data;  
                detection_data_num++;   
                add_offload_data(segmentation_data, (char*)rpeak_in, 20*(sizeof(double)));               
		if ((((int)rpeak_in[i]-PR_window)>=0) and (((int)rpeak_in[i]+QT_window)<input_signal_window)) {

			for (j=PR_window; j>0; j--) {
				beat[PR_window - j] = sig_in[(int)rpeak_in[i] - j];
			}	
			for (j=0; j<=QT_window; j++) {
				beat[PR_window + j] = sig_in[(int)rpeak_in[i] + j];
			}		
                        add_offload_data(segmentation_data, (char*)beat, (PR_window + QT_window+1)*(sizeof(double)));               
		}

	}
	free(beat);
        free_offload_data(detection_data);
        return segmentation_data;
} 

offload_data* feature_extraction(offload_data* segmentation_data){
	int numberof_rpeaks_in;
	double *rpeak_in;
	double *beat_in;
	double *coef;
	int PR_window = interpolation*54;//86;
	int QT_window = interpolation*74;//170;
	int i,j;
        offload_data* extraction_data = make_offload_data();
        extraction_data->rpeaks = segmentation_data->rpeaks; 
	numberof_rpeaks_in = segmentation_data->rpeaks;
        int segmentation_data_num = 0;
	for (i=0; i<numberof_rpeaks_in; i++) {//rpeak[0]
		rpeak_in = (double*)get_offload_data(segmentation_data, segmentation_data_num)->data;      
                segmentation_data_num++;
                add_offload_data(extraction_data, (char*)rpeak_in, 20*(sizeof(double)));               
		if ((((int)rpeak_in[i]-PR_window)>=0) and (((int)rpeak_in[i]+QT_window)<input_signal_window)) {
			/* FEATURE EXTRACTION *///input: double beat[(PR_window + QT_window+1)] || output: double coef[fv_size]
			//coef = (double *)malloc(fv_size*sizeof(double));
		        beat_in = (double*)get_offload_data(segmentation_data, segmentation_data_num)->data;      
                        segmentation_data_num++;
			coef = dbwavelet(beat_in);
                        add_offload_data(extraction_data, (char*)coef, (fv_size)*(sizeof(double)));               
			//free(coef);
		}
	}
        free_offload_data(segmentation_data);
        return extraction_data;
} 
result_data* classification(offload_data* extraction_data, char* result){
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

	FILE *fp_result;
	fp_result = fopen(result,"a"); 
	fprintf(fp_result, "rpeaks: %d\n",numberof_rpeaks_in);
	fprintf(fp_result, "\t\t\tnormal: %d\n\t\t\tabnormal: %d\n",normal,abnormal);
	fclose(fp_result);
        free_offload_data(extraction_data);

        result_data* results = (result_data*)malloc(sizeof(result_data));
        results -> normal = normal;
        results -> abnormal = abnormal;
        results -> rpeaks = numberof_rpeaks_in;
        return results;
}



void heartbeat_segmentation_dump_file(char *in, char *out, char* other){
	/* HEARTBEAT SEGMENTATION *///input: double sig[input_signal_window], double rpeak[20] || output: double beat[PR_window + QT_window+1]
	int numberof_rpeaks_in;
	double *rpeak_in;
	double *sig_in;

	double *beat;
	int PR_window = interpolation*54;//86;
	int QT_window = interpolation*74;//170;

	char dat_file[50];
	int i,j;

  	beat = (double *)malloc((PR_window + QT_window+1)*sizeof(double));

	sprintf(dat_file, "%s/numberof_rpeaks.dat", other);
	numberof_rpeaks_in = *(read_int_array(1, dat_file));

	for (i=0; i<numberof_rpeaks_in; i++) {//rpeak[0]

		sprintf(dat_file, "%s/sig%d.dat", in, i);
		sig_in = read_double_array(input_signal_window, dat_file);
		sprintf(dat_file, "%s/rpeak%d.dat", in, i);
		rpeak_in = read_double_array(20, dat_file);

		if ((((int)rpeak_in[i]-PR_window)>=0) and (((int)rpeak_in[i]+QT_window)<input_signal_window)) {

			for (j=PR_window; j>0; j--) {
				beat[PR_window - j] = sig_in[(int)rpeak_in[i] - j];
			}	
			for (j=0; j<=QT_window; j++) {
				beat[PR_window + j] = sig_in[(int)rpeak_in[i] + j];
			}			

			sprintf(dat_file, "%s/beat%d.dat", out,  i);
			write_double_array(beat, (PR_window + QT_window+1), dat_file);

		}

	}
	free(beat);
} 
void feature_extraction_dump_file(char *in, char *out, char* other){

	int numberof_rpeaks_in;
	double *rpeak_in;
	double *beat_in;
	double *coef;
	char dat_file[50];
	int PR_window = interpolation*54;//86;
	int QT_window = interpolation*74;//170;
	int i,j;
	sprintf(dat_file, "%s/numberof_rpeaks.dat", other);
	numberof_rpeaks_in = *(read_int_array(1, dat_file));
	for (i=0; i<numberof_rpeaks_in; i++) {//rpeak[0]
		sprintf(dat_file, "%s/rpeak%d.dat", other, i);
		rpeak_in = read_double_array(20, dat_file);
		if ((((int)rpeak_in[i]-PR_window)>=0) and (((int)rpeak_in[i]+QT_window)<input_signal_window)) {

		
			/* FEATURE EXTRACTION *///input: double beat[(PR_window + QT_window+1)] || output: double coef[fv_size]
			//coef = (double *)malloc(fv_size*sizeof(double));
			sprintf(dat_file, "%s/beat%d.dat", in, i);
			beat_in = read_double_array((PR_window + QT_window+1), dat_file);
			coef = dbwavelet(beat_in);
			sprintf(dat_file, "%s/coef%d.dat", out, i);
			write_double_array(coef, (fv_size), dat_file);
			//free(coef);
		}
	}
} 
void classification_dump_file(char *in, char *out, char* other, char* result){
	int normal = 0;
	int abnormal = 0;
	int numberof_rpeaks_in;
	double *rpeak_in;
	double *coef_in;
	int label;
	char data_to_post[50];
	char dat_file[50];
	int PR_window = interpolation*54;//86;
	int QT_window = interpolation*74;//170;
	int i,j;

	sprintf(dat_file, "%s/numberof_rpeaks.dat",  other);
	numberof_rpeaks_in = *(read_int_array(1, dat_file));

	for (i=0; i<numberof_rpeaks_in; i++) {//rpeak[0]
		sprintf(dat_file, "%s/rpeak%d.dat", other, i);
		rpeak_in = read_double_array(20, dat_file);

		if ((((int)rpeak_in[i]-PR_window)>=0) and (((int)rpeak_in[i]+QT_window)<input_signal_window)) {

			/* CLASSIFICATION */ //input: double coef[fv_size] || output: int label
			sprintf(dat_file, "%s/coef%d.dat", in, i);
			coef_in = read_double_array((fv_size), dat_file);


			label = predict(coef_in, -1, fv_size);
			if (label==1) strcpy(data_to_post, "Heartbeat detected and labeled as NORMAL."); 
			else strcpy(data_to_post, "Heartbeat detected and labeled as ABNORMAL.");
			//j = post_req(data_to_post);

			if (label==1) normal = normal + 1; else abnormal = abnormal + 1;

		}

	} 

	FILE *fp_result;
	char result_file[50];
	sprintf(result_file, "%s/out.log", result);
	fp_result = fopen(result_file,"a"); 
	fprintf(fp_result, "rpeaks: %d\n",numberof_rpeaks_in);
	fprintf(fp_result, "\t\t\tnormal: %d\n\t\t\tabnormal: %d\n",normal,abnormal);
	fclose(fp_result);



	//printf("rpeaks: %d\n",numberof_rpeaks_in);
	//printf("\t\t\tnormal: %d\n\t\t\tabnormal: %d\n",normal,abnormal);
}


void flow(double *sig, ECG_ctxt *ctxt, char* result) {
        //printf("input_signal_window: %d, heartbeat_window: %d, interp: %f\n",input_signal_window, heartbeat_window, interpolation);
	//printf("%f%f\n", interpolation, no_of_input_windows);
	//printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", wavelet, levels, from, ac1, ac2, ac3, ac4, dc1, dc2, dc3, dc4, nac, ndc, nsig, heartbeat_window, input_signal_window, fv_size);


	char detection[50];
	char segmentation[50];
	char extraction[50];
	char classification_dir[50];

	sprintf(detection, "%s/detection", result);
	sprintf(segmentation, "%s/segmentation", result);
	sprintf(extraction, "%s/extraction", result);
	sprintf(classification_dir, "%s/classification", result);

	int numberof_rpeaks;
	heartbeat_detection_dump_file(sig, detection,detection,detection, &numberof_rpeaks);
	heartbeat_segmentation_dump_file(detection, segmentation,  detection);
	feature_extraction_dump_file(segmentation, extraction, detection);
	classification_dump_file(extraction, classification_dir, detection, result);

	return;
}

void edge_process_dump_file()
{
  ECG_init();

  int HEARTBEAT_NUM = 1;
  int OFFLOAD_LEVEL = 0;
  int node_id = 0;

  char result_dir[20];
  sprintf(result_dir, ".", node_id);

//Prepare for the ECG processing 
  int gain, baseline, i, j;
  char line[80];
  double rp;
  FILE *fp;
  double *full_sig;
  gain = 200;
  baseline = 1024;
  ECG_ctxt ecg_ctxt;
  ecg_ctxt.learning = 1;
  full_sig = (double *)malloc((input_signal_window)*sizeof(double));
  FILE *fp_result;
  char result_file[50];
  sprintf(result_file, "%s/out.log", result_dir);
  fp_result = fopen(result_file,"w"); 
  fclose(fp_result);
  fp = fopen(data_record,"r"); 
  if (fp == 0) printf("Error: could not open data file.\n");

  int total_heartbeat=HEARTBEAT_NUM;
  int beat_count=0;
  char detection[50];
  char segmentation[50];
  char extraction[50];
  char classification_dir[50];
  int numberof_rpeaks=1;
  i=0;
  for (j=0; j<no_of_input_windows*input_signal_window; j++) {
     if(j==(total_heartbeat*input_signal_window)) break;
     fgets(line,80,fp);
     sscanf(line,"%lf", &rp);
     full_sig[i] = (rp - baseline)/gain;
     i = i + 1;
     if (i==input_signal_window) {
         sprintf(detection, "%s/detection", result_dir, beat_count);
         sprintf(segmentation, "%s/segmentation", result_dir, beat_count);
         sprintf(extraction, "%s/extraction", result_dir, beat_count);
         sprintf(classification_dir, "%s/classification", result_dir, beat_count);
         beat_count++;

         if(OFFLOAD_LEVEL == 0){		
			  heartbeat_detection_dump_file(full_sig, detection,detection,detection, &numberof_rpeaks);
			  heartbeat_segmentation_dump_file(detection, segmentation,  detection);
			  feature_extraction_dump_file(segmentation, extraction, detection);
			  classification_dump_file(extraction, classification_dir, detection, result_dir);

         }
         if(OFFLOAD_LEVEL == 1){		
			  heartbeat_detection_dump_file(full_sig, detection,detection,detection, &numberof_rpeaks);
			  heartbeat_segmentation_dump_file(detection, segmentation,  detection);
			  feature_extraction_dump_file(segmentation, extraction, detection);
         }
         if(OFFLOAD_LEVEL == 2){	
			  heartbeat_detection_dump_file(full_sig, detection,detection,detection, &numberof_rpeaks);
			  heartbeat_segmentation_dump_file(detection, segmentation,  detection);
         }
         if(OFFLOAD_LEVEL == 3){		
			  heartbeat_detection_dump_file(full_sig, detection,detection,detection, &numberof_rpeaks);
         }

         i=0;
     }
  }
  fclose(fp);
  free(full_sig);    
}

double* get_signal_block(int seq_num, int total_number){
   //Prepare for the ECG processing 
  int gain, baseline, i, j;
  char line[80];
  double rp;
  FILE *fp;

  gain = 200;
  baseline = 1024;
  fp = fopen(data_record,"r"); 
  if (fp == 0) printf("Error: could not open data file.\n");

  int total_heartbeat=total_number;
  int numberof_rpeaks=1;

  double *full_sig;
  full_sig = (double *)malloc((input_signal_window)*sizeof(double));
  i=0;
  for (j=0; j<no_of_input_windows*input_signal_window; j++) {
     if(j==(total_heartbeat*input_signal_window)) break;
     fgets(line,80,fp);
     sscanf(line,"%lf", &rp);
     full_sig[i] = (rp - baseline)/gain;
     i = i + 1;
     if (i==input_signal_window) {
         if(j/input_signal_window == seq_num) break;
         i=0;
     }
  }
  return full_sig;
}

void edge_process()
{
  ECG_init();

  int FRAME_NUM = 20;
  int OFFLOAD_LEVEL = 0;
  int node_id = 0;

  FILE *fresult;
  fresult = fopen("out.log","w"); 
  fclose(fresult);


  int seq_num = 0;
  for(seq_num = 0; seq_num <FRAME_NUM; seq_num++){
         if(OFFLOAD_LEVEL == 0){	
		offload_data* tmp = heartbeat_detection(get_signal_block(seq_num, FRAME_NUM));
		tmp = heartbeat_segmentation(tmp);
		tmp = feature_extraction(tmp);
		result_data* results = classification(tmp, "./out.log");
		printf("rpeaks: %d\n", results->rpeaks);
		printf("\t\t\tnormal: %d\n\t\t\tabnormal: %d\n", results->normal, results->abnormal);
         }
         if(OFFLOAD_LEVEL == 1){		
		offload_data* tmp = heartbeat_detection(get_signal_block(seq_num, FRAME_NUM));
		tmp = heartbeat_segmentation(tmp);
		tmp = feature_extraction(tmp);
         }
         if(OFFLOAD_LEVEL == 2){	
		offload_data* tmp = heartbeat_detection(get_signal_block(seq_num, FRAME_NUM));
		tmp = heartbeat_segmentation(tmp);
         }
         if(OFFLOAD_LEVEL == 3){		
		offload_data* tmp = heartbeat_detection(get_signal_block(seq_num, FRAME_NUM));
         }

  }

}


/*
int main(){

  ECG_init();
  ECG_execute();

  return 0;
}

*/

