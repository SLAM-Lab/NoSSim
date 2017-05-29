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

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))


#ifdef SET1
#define TOTAL_HEARTBEAT 200
#endif

#ifdef SET2
#define TOTAL_HEARTBEAT 400
#endif



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


void heartbeat_detection(double *sig, ECG_ctxt *ctxt, char *in, char *out, char *other, int *numberof_rpeaks_ptr){

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
	

	if (strcmp(detector,"sqrs_detector")) numberof_rpeaks = wqrs(rpeak, samp, ctxt);
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
void heartbeat_segmentation(char *in, char *out, char* other){
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
void feature_extraction(char *in, char *out, char* other){

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
void classification(char *in, char *out, char* other, char* result){
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
	heartbeat_detection(sig, ctxt, detection,detection,detection, &numberof_rpeaks);
	heartbeat_segmentation(detection, segmentation,  detection);
	feature_extraction(segmentation, extraction, detection);
	classification(extraction, classification_dir, detection, result);

	return;
}





/*
int main(){

  ECG_init();
  ECG_execute();

  return 0;
}

*/

