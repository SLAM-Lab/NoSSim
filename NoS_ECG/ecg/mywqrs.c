#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "mywqrs.h"

//extern double *samp;
extern int to, from;

#define BUFLN  16384	/* must be a power of 2, see ltsamp() */
#define EYE_CLS 0.25    /* eye-closing period is set to 0.25 sec (250 ms) */ 
#define MaxQRSw 0.13    /* maximum QRS width (130ms) */                        
#define NDP	 2.5    /* adjust threshold if no QRS found in NDP seconds */
#define PWFreqDEF 60    /* power line (mains) frequency, in Hz (default) */
#define TmDEF	 100	/* minimum threshold value (default) */




//double lfsc;		/* length function scale constant */
//int *ebuf;
//int LPn, LP2n;          /* filter parameters (dependent on sampling rate) */
//int LTwindow;           /* LT window size */
//int PWFreq;		/* power line (mains) frequency, in Hz */
//int Tm;			/* minimum threshold value */
//double *lbuf;




//static int Yn, Yn1, Yn2;
//static int tt;
//static int aet, et;


//static int learning = 1, T1;

double ltsamp(int t, double *samp, double lfsc, int LPn, int LP2n, int *ebuf, double *lbuf, int LTwindow, ECG_ctxt *ctxt)
{
    int dy,sig=0;

    double v0, v1, v2;

    if (lbuf == NULL) {

	(ctxt->aet) = 0;
	(ctxt->tt) = (int)-1L;


	
	    for (ebuf[0] = sqrt(lfsc), (ctxt->tt) = 1L; (ctxt->tt) < BUFLN; (ctxt->tt)++)
		ebuf[(ctxt->tt)] = ebuf[0];
	    if (t > BUFLN) (ctxt->tt) = (int)(t - BUFLN);
	    else (ctxt->tt) = (int)-1L;
	    (ctxt->Yn) = (ctxt->Yn1) = (ctxt->Yn2) = 0;
    }
    
    while (t > (ctxt->tt)) {
	(ctxt->Yn2) = (ctxt->Yn1);
	(ctxt->Yn1) = (ctxt->Yn);
	 if ( ((ctxt->tt)<to) && ((ctxt->tt)-LPn<to) && ((ctxt->tt)-LP2n<to) && ((ctxt->tt)>=from) && ((ctxt->tt)-LPn>=from) && ((ctxt->tt)-LP2n>=from) ) {
	   v0 = samp[(ctxt->tt)];
	   v1 = samp[(ctxt->tt)-LPn];
	   v2 = samp[(ctxt->tt)-LP2n];
	   (ctxt->Yn) = 2*(ctxt->Yn1) - (ctxt->Yn2) + v0 - 2*v1 + v2;
	}
	dy = ((ctxt->Yn) - (ctxt->Yn1)) / LP2n;		/* lowpass derivative of input */
	(ctxt->et) = ebuf[(++(ctxt->tt))&(BUFLN-1)] = sqrt(lfsc +dy*dy); /* length transform */
	lbuf[((ctxt->tt))&(BUFLN-1)] = (ctxt->aet) += (ctxt->et) - ebuf[((ctxt->tt)-LTwindow)&(BUFLN-1)];
	/* lbuf contains the average of the length-transformed samples over
	   the interval from tt-LTwindow+1 to tt */
    }
    return (lbuf[t&(BUFLN-1)]);
}

int wqrs(double* tm, double *samp, ECG_ctxt *ctxt){
 
    double lfsc;
    int LPn, LP2n;          /* filter parameters (dependent on sampling rate) */

    int *ebuf;
    double *lbuf;
    int LTwindow;
    int PWFreq;
    int Tm;

    lbuf = (double *)malloc((unsigned)BUFLN*sizeof(double));
    ebuf = (int *)malloc((unsigned)BUFLN * sizeof(int));


    lbuf = NULL;
    Tm = TmDEF;
    PWFreq = PWFreqDEF;
    char *p;
    float sps;			     /* sampling frequency, in Hz (SR) */
    float samplingInterval;          /* sampling interval, in milliseconds */
    int i, max, min, minutes = 0, onset, timer, vflag = 0;
    int dflag = 0;		     /* if non-zero, dump raw and filtered
					samples only;  do not run detector */
    int jflag = 0;		     /* if non-zero, annotate J-points */
    int Rflag = 0;		     /* if non-zero, resample at 120 or 150 Hz
				      */
    int EyeClosing;                  /* eye-closing period, related to SR */
    int ExpectPeriod;                /* if no QRS is detected over this period,
					the threshold is automatically reduced
					to a minimum value;  the threshold is
					restored upon a detection */
    double Ta, T0;//, *tm;		     /* high and low detection thresholds */

    int gain;
    int next_minute, spm, t, tj, tpq, tt, t1;
    
    //tm = (double *)malloc(4000*sizeof(double));
    for (i=0; i<20; i++) tm[i] = 0;

    int sig = 0;

    sps = 360.000000;
    gain = 200;

    Tm = 20;	//Tm = muvadu((unsigned)sig, Tm);
    samplingInterval = 1000.0/sps;
    lfsc = 1.25*gain*gain/sps;	/* length function scale constant */
    spm = 60 * sps;
    next_minute = from + spm;
    LPn = sps/PWFreq; 		/* The LP filter will have a notch at the power line (mains) frequency */
    if (LPn > 8)  LPn = 8;	/* avoid filtering too agressively */
    LP2n = 2 * LPn;
    EyeClosing = sps * EYE_CLS; /* set eye-closing period */
    ExpectPeriod = sps * NDP;	/* maximum expected RR interval */
    LTwindow = sps * MaxQRSw;   /* length transform window size */

    if ((t1 = 2880) > BUFLN*0.9)
	t1 = BUFLN/2;
    t1 += from;
    //for (T0 = 0, t = from; t < t1 && sample_valid(); t++)
    for (T0 = 0, t = from; t < t1; t++)
	T0 += ltsamp(t, samp, lfsc, LPn, LP2n, ebuf, lbuf, LTwindow, ctxt);
    T0 /= t1 - from;
    Ta = 3 * T0;

    /* Main loop */
    int j = 0;
    //for (t = from; t < to || (to == 0L && sample_valid()); t++) {
    for (t = from; t < to; t++) {

	
	if ((ctxt->learning)) {
	    if (t > t1) {
		(ctxt->learning) = 0;
		(ctxt->T1) = T0;
		t = from;	/* start over */
	    }
	    else
		(ctxt->T1) = 2*T0;
	}	
	
	/* Compare a length-transformed sample against T1. */
	if (ltsamp(t, samp, lfsc, LPn, LP2n, ebuf, lbuf, LTwindow, ctxt) > (ctxt->T1)) {	/* found a possible QRS near t */
	    timer = 0; /* used for counting the time after previous QRS */
	    max = min = ltsamp(t, samp, lfsc, LPn, LP2n, ebuf, lbuf, LTwindow, ctxt);
	    for (tt = t+1; tt < t + EyeClosing/2; tt++)
		if (ltsamp(tt, samp, lfsc, LPn, LP2n, ebuf, lbuf, LTwindow, ctxt) > max) max = ltsamp(tt, samp, lfsc, LPn, LP2n, ebuf, lbuf, LTwindow, ctxt);
	    for (tt = t-1; tt > t - EyeClosing/2; tt--)
		if (ltsamp(tt, samp, lfsc, LPn, LP2n, ebuf, lbuf, LTwindow, ctxt) < min) min = ltsamp(tt, samp, lfsc, LPn, LP2n, ebuf, lbuf, LTwindow, ctxt);
	    if (max > min+10) { /* There is a QRS near tt */
		/* Find the QRS onset (PQ junction) */
		onset = max/100 + 2;
		tpq = t - 5;
		for (tt = t; tt > t - EyeClosing/2; tt--) {
		    if (ltsamp(tt, samp, lfsc, LPn, LP2n, ebuf, lbuf, LTwindow, ctxt)   - ltsamp(tt-1, samp, lfsc, LPn, LP2n, ebuf, lbuf, LTwindow, ctxt) < onset &&
			ltsamp(tt-1, samp, lfsc, LPn, LP2n, ebuf, lbuf, LTwindow, ctxt) - ltsamp(tt-2, samp, lfsc, LPn, LP2n, ebuf, lbuf, LTwindow, ctxt) < onset &&
			ltsamp(tt-2, samp, lfsc, LPn, LP2n, ebuf, lbuf, LTwindow, ctxt) - ltsamp(tt-3, samp, lfsc, LPn, LP2n, ebuf, lbuf, LTwindow, ctxt) < onset &&
			ltsamp(tt-3, samp, lfsc, LPn, LP2n, ebuf, lbuf, LTwindow, ctxt) - ltsamp(tt-4, samp, lfsc, LPn, LP2n, ebuf, lbuf, LTwindow, ctxt) < onset) {
			tpq = tt - LP2n;	/* account for phase shift */
			break;
		    }
		}

		if (!(ctxt->learning)) {
		    /* Check that we haven't reached the end of the record. */
		    //(void)sample(sig, tpq);
		    //if (sample_valid() == 0) break;
		    if (tpq>=to) break;
		    tm[j] = (double)tpq; 
		    j = j + 1; 
		    if (jflag) {
			/* Find the end of the QRS */
			for (tt = t, tj = t + 5; tt < t + EyeClosing/2; tt++) {
			    if (ltsamp(tt, samp, lfsc, LPn, LP2n, ebuf, lbuf, LTwindow, ctxt) > max - (max/10)) {
				tj = tt;
				break;
			    }
			}
			//(void)sample(sig, tj);
			//if (sample_valid() == 0) break;
			if (tj>=to) break;
		    }
		}

		/* Adjust thresholds */
		Ta += (max - Ta)/10;
		(ctxt->T1) = Ta / 3;


		/* Lock out further detections during the eye-closing period */
		t += EyeClosing;
	    }
	}
	else if (!(ctxt->learning)) {
	    /* Once past the learning period, decrease threshold if no QRS
	       was detected recently. */
	    if (++timer > ExpectPeriod && Ta > Tm) {
		Ta--;
		(ctxt->T1) = Ta / 3;
	    }      
	}

	/* Keep track of progress by printing a dot for each minute analyzed */
	if (t >= next_minute) {
	    next_minute += spm;
	    //(void)fprintf(stderr, ".");
	    //(void)fflush(stderr);
	    if (++minutes >= 60) {
		//(void)fprintf(stderr, " %s\n", timstr(t));
		minutes = 0;
	    }
	}
    }
    //if (minutes) (void)fprintf(stderr, " %s\n", timstr(t));

    (void)free(lbuf);
    (void)free(ebuf);
    //tm[0] = (double)j;
  	
    return(j);
}

