#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "mysqrs.h"



#define abs(A)	((A) >= 0 ? (A) : -(A))

//extern double *samp;
extern double interpolation;
extern int input_signal_window, to, from;



void my_setifreq(double *samp, int *gv0, int *gv1, int *indx, long *rgvtime, long *gvtime);
int my_getvec(double *samp, int *vector, int *gv0, int *gv1, int *indx, long *rgvtime, long *gvtime);

int sqrs(double *tm, double *samp)
{

    long rgvtime; 
    long gvtime;
    int gv0, gv1, indx;
    int vector;
    int filter, i, time = 0,
        slopecrit, sign, maxslope = 0, nslope = 0,
        qtime, maxtime, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9,
        ms160, ms200, s2, scmax, scmin = 500;
    long now, spm;
    //double *tm;

    //tm = (double *)malloc(40*sizeof(double));
    for (i=0; i<20; i++) tm[i]=0;
   
    //to = to*250/(interpolation*360);

    //setifreq(250.);
    my_setifreq(samp, &gv0, &gv1, &indx, &rgvtime, &gvtime);

    spm = 15000;
    //spm = strtim("1:0");

    scmin = 100;
    //scmin = muvadu((unsigned)signal, scmin);
    slopecrit = scmax = 10 * scmin;
    now = from;

    //ms160 = strtim("0.16"); ms200 = strtim("0.2"); s2 = strtim("2");
    ms160 = 40; ms200 = 50; s2 = 500;

    i = my_getvec(samp, &vector, &gv0, &gv1, &indx, &rgvtime, &gvtime);  
    //(void)getvec(v);
    t9 = t8 = t7 = t6 = t5 = t4 = t3 = t2 = t1 = vector;
    int count =1;
    int j = 0;
    do {
        filter = (t0 = vector) + 4*t1 + 6*t2 + 4*t3 + t4
                - t5         - 4*t6 - 6*t7 - 4*t8 - t9;
	if (time % s2 == 0) {
            if (nslope == 0) {
                slopecrit -= slopecrit >> 4;
                if (slopecrit < scmin) slopecrit = scmin;
            }
            else if (nslope >= 5) {
                slopecrit += slopecrit >> 4;
                if (slopecrit > scmax) slopecrit = scmax;
            }
        }
        if (nslope == 0 && abs(filter) > slopecrit) {
            nslope = 1; maxtime = ms160;
            sign = (filter > 0) ? 1 : -1;
            qtime = time;
        }
        if (nslope != 0) {
            if (filter * sign < -slopecrit) {
                sign = -sign;
                maxtime = (++nslope > 4) ? ms200 : ms160;
            }
            else if (filter * sign > slopecrit &&
                     abs(filter) > maxslope)
                maxslope = abs(filter);
            if (maxtime-- < 0) {
                if (2 <= nslope && nslope <= 4) {
                    slopecrit += ((maxslope>>2) - slopecrit) >> 3;
                    if (slopecrit < scmin) slopecrit = scmin;
                    else if (slopecrit > scmax) slopecrit = scmax;
                    //annot.time = now - (time - qtime) - 4;
		    tm[j] = round((double)(now - (time - qtime) - 4)*(interpolation*360)/250);
		    j++;
                    time = 0;
                }
                else if (nslope >= 5) {
                    //annot.time = now - (time - qtime) - 4;
		    tm[j] = now - (time - qtime) - 4;
		    j++;
                }
                nslope = 0;
            }
        }
        t9 = t8; t8 = t7; t7 = t6; t6 = t5; t5 = t4;
        t4 = t3; t3 = t2; t2 = t1; t1 = t0; time++; now++;

    } while (my_getvec(samp, &vector, &gv0, &gv1, &indx, &rgvtime, &gvtime) > 0 && (to == 0 || now <= to));
    //wfdbquit();
    //tm[0] = (double)j;

    return(j);
}



void my_setifreq(double *samp, int *gv0, int *gv1, int *indx, long *rgvtime, long *gvtime)
{
    int sfreq = (interpolation*360), ifreq = 250;
    int f = ifreq;
    int g = sfreq;
    double error; 
   
    while ((error = f - g) > 0.005 || error < -0.005)
	    if (f > g) f -= g;
	    else g -= f;
    long mticks = (long)(sfreq/f + 0.5);
    long nticks = (long)(ifreq/f + 0.5);


    //rgvstat = rgetvec(gv0);
    //rgvstat = rgetvec(gv1);
    (*indx) = from;
    (*gv0) = samp[(*indx)]; (*indx)++;
    (*gv1) = samp[(*indx)]; (*indx)++;

    *rgvtime = nticks;
    *gvtime = 0;

    return;
}

int my_getvec(double *samp, int *vector, int *gv0, int *gv1, int *indx, long *rgvtime, long *gvtime)
{
    int sfreq = (interpolation*360), ifreq = 250;
    int f = ifreq;
    int g = sfreq;
    double error; 

    while ((error = f - g) > 0.005 || error < -0.005)
	    if (f > g) f -= g;
	    else g -= f;
    long mticks = (long)(sfreq/f + 0.5);
    long nticks = (long)(ifreq/f + 0.5);
    long mnticks = mticks * nticks;



    if ((*indx)>=input_signal_window) return(-1);    
	 
    int i;

    if (*rgvtime > mnticks) {
	*rgvtime -= mnticks;
	*gvtime  -= mnticks;
    }

    while ((*gvtime) > (*rgvtime)) {
	(*gv0) = (*gv1);
	//rgvstat = rgetvec(gv1);
        (*gv1) = samp[(*indx)]; (*indx)++;
	(*rgvtime) += nticks;
    }
    *vector = (*gv0) + ((*gvtime)%nticks)*((*gv1)-(*gv0))/nticks;
    *gv0 = *gv1;
    (*gvtime) += mticks;
    return(1);
}



