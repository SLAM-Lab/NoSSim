#ifndef _ECG_CTXT_H
#define _ECG_CTXT_H
struct ECG_ctxt {
//variables in mywqrs.c 
	int learning;// = 1;
	int T1;
	int Yn;
	int Yn1;
	int Yn2;
	int tt;
	int aet;
	int et;
};
#endif
//extern struct ECG_ctxt ctxt; 
