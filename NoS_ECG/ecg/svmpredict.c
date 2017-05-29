#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "svm.h"
#include "svmpredict.h"

extern struct svm_model *model;





int predict(double *coef, int target_label, int fv_size)
{
	struct svm_node *x;
	int predict_probability=0;
	int correct = 0;
	int total = 0;
	double error = 0;
	double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;
	int i;
	int predict_label;

	//struct svm_model *model,*createmodel();

	//model = svm_load_model("training_data.model");
	//model = createmodel();
	int svm_type=svm_get_svm_type(model);
	int nr_class=svm_get_nr_class(model);
	double *prob_estimates=NULL;
	int j;

	x = (struct svm_node *) malloc((fv_size+1)*sizeof(struct svm_node));

		for (i=0; i<fv_size; i++){
			x[i].value = coef[i]; 
			x[i].index = i+1;
		}
		x[i].index = -1;

		
		predict_label = svm_predict(model,x);


		if(predict_label == target_label)
			++correct;
		error += (predict_label-target_label)*(predict_label-target_label);
		sump += predict_label;
		sumt += target_label;
		sumpp += predict_label*predict_label;
		sumtt += target_label*target_label;
		sumpt += predict_label*target_label;
		++total;

	if (svm_type==NU_SVR || svm_type==EPSILON_SVR)
	{
		//info("Mean squared error = %g (regression)\n",error/total);
		//info("Squared correlation coefficient = %g (regression)\n",
		//	((total*sumpt-sump*sumt)*(total*sumpt-sump*sumt))/
		//	((total*sumpp-sump*sump)*(total*sumtt-sumt*sumt))
		//	);
	}
	else
		//info("Accuracy = %g%% (%d/%d) (classification)\n",
			//(double)correct/total*100,correct,total);
	if(predict_probability)
		free(prob_estimates);

	//for(i=0; i<(model->l); i++){                                               
        //        free(model->SV[i]);   
        //        model->SV[i]=NULL;   
        //}

	//svm_free_and_destroy_model(&model);
	free(x);

	return(predict_label);
}
