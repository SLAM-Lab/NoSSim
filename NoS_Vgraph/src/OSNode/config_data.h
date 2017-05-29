#define CLI_NUM_MAX 10
#define IMG_NUM_MAX 4

#define IMG_NUM 4
#define SRV_COMM_CORE 0
#define SRV_APP_CORE 1

#include <time.h>
#include "rapidjson/reader.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/en.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "os_ctxt.h"

extern long long imgPre_cli[IMG_NUM_MAX][CLI_NUM_MAX];
extern long long imgFeature_cli[IMG_NUM_MAX][CLI_NUM_MAX][CLI_NUM_MAX];

extern long long imgPre_srv[IMG_NUM_MAX][CLI_NUM_MAX];
extern long long imgFeature_srv[IMG_NUM_MAX][CLI_NUM_MAX][CLI_NUM_MAX];



extern struct timespec now; 
extern struct timespec tmstart;
extern int schd[CLI_NUM_MAX];
extern unsigned long cli_commu[CLI_NUM_MAX];
extern int recv_round;
extern unsigned char debug_flags;
extern int CLI_OFFLOAD_LEVEL[CLI_NUM_MAX+1];  //Accessed by the server device, indexed by client ID, starting from 1;

extern int CLI_NUM;
extern int SRV_CORE_NUM;

//extern int offload1_phase;//execution phase for offloading level 1
extern int recv_round_comp;
//extern int offload1_phase_comp;



//Variables in the os_ctxt object
//int OFFLOAD_LEVEL;
//int CLI_TYPE;
//int SRV_TYPE;
//int CLI_CORE_NUM;

//Var for  profiling
//long long communication_time = 0; //lwip + channel
//long long temp_time_recv = 0;


void  g_config_init();
void config_init();
