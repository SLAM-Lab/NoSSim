#define CLI_NUM_MAX 15
#define HEARTBEAT_NUM 20
#define HEARTBEAT_NUM_MAX 200
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

extern unsigned long stage0_srv[HEARTBEAT_NUM];
extern unsigned long stage1_srv[HEARTBEAT_NUM];
extern unsigned long stage2_srv[HEARTBEAT_NUM];
extern unsigned long stage3_srv[HEARTBEAT_NUM];

extern unsigned long stage0_cli[HEARTBEAT_NUM];
extern unsigned long stage1_cli[HEARTBEAT_NUM];
extern unsigned long stage2_cli[HEARTBEAT_NUM];
extern unsigned long stage3_cli[HEARTBEAT_NUM];


extern struct timespec now; 
extern struct timespec tmstart;
extern int schd[CLI_NUM_MAX];
extern unsigned long cli_commu[CLI_NUM_MAX];
extern int recv_round;
extern unsigned char debug_flags;
extern int CLI_OFFLOAD_LEVEL[CLI_NUM_MAX+1];  //Accessed by the server device, indexed by client ID, starting from 1;


extern int CLI_NUM;
//extern int OFFLOAD_LEVEL;
//extern int CLI_TYPE;
//extern int SRV_TYPE;
//extern int CLI_CORE_NUM;
extern int SRV_CORE_NUM;
void  g_config_init();
void config_init();
