#include "config_data.h"

unsigned long stage0_rpi3[HEARTBEAT_NUM] = {9580543000,9580543000,9061624000,9061624000,9580543000,9580543000,9580543000,9061624000,9061624000,9580543000,9580543000,9580543000,9061624000,9061624000,9580543000,9580543000,9580543000,9061624000,9061624000,9580543000};
unsigned long stage1_rpi3[HEARTBEAT_NUM] = {269028000,269028000,37519200,37519200,185838000,269028000,269028000,37519200,37519200,185838000,269028000,269028000,37519200,37519200,185838000,269028000,269028000,37519200,37519200,269028000};
unsigned long stage2_rpi3[HEARTBEAT_NUM] = {4126969000,4126969000,0,0,2488330,4126969000,4126969000,0,0,2488330,4126969000,4126969000,0,0,2488330,4126969000,4126969000,0,0,4126969000};
unsigned long stage3_rpi3[HEARTBEAT_NUM] = {24045036300,24045036300,17413300,17413300,45104200,24045036300,24045036300,17413300,17413300,45104200,24045036300,24045036300,17413300,17413300,45104200,24045036300,24045036300,17413300,17413300,24045036300};


unsigned long stage0_rpi0[HEARTBEAT_NUM] = {12472549180, 12472549180, 11796988019, 11796988019, 12472549180, 12472549180, 12472549180, 11796988019, 11796988019, 12472549180, 12472549180, 12472549180, 11796988019, 11796988019, 12472549180, 12472549180, 12472549180, 11796988019, 11796988019, 12472549180};
unsigned long stage1_rpi0[HEARTBEAT_NUM] = {1213058511, 1213058511, 169175643, 169175643, 837951319, 1213058511, 1213058511, 169175643, 169175643, 837951319, 1213058511, 1213058511, 169175643, 169175643, 837951319, 1213058511, 1213058511, 169175643, 169175643, 1213058511};
unsigned long stage2_rpi0[HEARTBEAT_NUM] = {8160350176, 8160350176, 0, 0, 4920231, 8160350176, 8160350176, 0, 0, 4920231, 8160350176, 8160350176, 0, 0, 4920231, 8160350176, 8160350176, 0, 0, 8160350176};
unsigned long stage3_rpi0[HEARTBEAT_NUM] = {56946317404, 56946317404, 41240250, 41240250, 106821135, 56946317404, 56946317404, 41240250, 41240250, 106821135, 56946317404, 56946317404, 41240250, 41240250, 106821135, 56946317404, 56946317404, 41240250, 41240250, 56946317404};



unsigned long stage0_srv[HEARTBEAT_NUM];
unsigned long stage1_srv[HEARTBEAT_NUM];
unsigned long stage2_srv[HEARTBEAT_NUM];
unsigned long stage3_srv[HEARTBEAT_NUM];

unsigned long stage0_cli[HEARTBEAT_NUM];
unsigned long stage1_cli[HEARTBEAT_NUM];
unsigned long stage2_cli[HEARTBEAT_NUM];
unsigned long stage3_cli[HEARTBEAT_NUM];



struct timespec now, tmstart;
int schd[CLI_NUM_MAX] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
int recv_round = 0;
unsigned char debug_flags=0;
int CLI_NUM=6;
int SRV_CORE_NUM=1;
int CLI_OFFLOAD_LEVEL[CLI_NUM_MAX+1];  //Accessed by the server device, indexed by client ID, starting from 1;
//int OFFLOAD_LEVEL=0;
//int CLI_TYPE=0;
//int SRV_TYPE=0;
//int CLI_CORE_NUM=0;
//int SRV_CORE_NUM=0;
//int Blocking_taskID;
//int flag_compute=0;
//sc_core::sc_event comp_sig; // systemc channel



void read_config_json();
void config_init(){
   std::cout<<"....................config_init.................."<<std::endl;
   read_config_json();
   OSModelCtxt* OSmodel = taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() );
//Simulation setup
   int i;
   if(OSmodel->NodeID>0)
	   if((OSmodel->CLI_TYPE) == 0){// RPi0
	       for(i=0;i<HEARTBEAT_NUM;i++){
		   stage0_cli[i] = stage0_rpi0[i];
		   stage1_cli[i] = stage1_rpi0[i];
		   stage2_cli[i] = stage2_rpi0[i];
		   stage3_cli[i] = stage3_rpi0[i];
	       }
	   }else if((OSmodel->CLI_TYPE) == 1){// RPi3
	       for(i=0;i<HEARTBEAT_NUM;i++){
		   stage0_cli[i] = stage0_rpi3[i];
		   stage1_cli[i] = stage1_rpi3[i];
		   stage2_cli[i] = stage2_rpi3[i];
		   stage3_cli[i] = stage3_rpi3[i];
	       }
	   }else if((OSmodel->CLI_TYPE) == 2){// RPi4
	       for(i=0;i<HEARTBEAT_NUM;i++){
		   stage0_cli[i] = stage0_rpi3[i]/2;
		   stage1_cli[i] = stage1_rpi3[i]/2;
		   stage2_cli[i] = stage2_rpi3[i]/2;
		   stage3_cli[i] = stage3_rpi3[i]/2;
	       }
	   }else{
	       std::cout << "CLI_TYPE: "<< (OSmodel->CLI_TYPE)<<" is invalid." << std::endl;
	   }
   else{
	   if((OSmodel->SRV_TYPE) == 0){// RPi0
	       for(i=0;i<HEARTBEAT_NUM;i++){
		   stage0_srv[i] = stage0_rpi0[i];
		   stage1_srv[i] = stage1_rpi0[i];
		   stage2_srv[i] = stage2_rpi0[i];
		   stage3_srv[i] = stage3_rpi0[i];
	       }
	   }else if((OSmodel->SRV_TYPE) == 1){// RPi3
	       for(i=0;i<HEARTBEAT_NUM;i++){
		   stage0_srv[i] = stage0_rpi3[i];
		   stage1_srv[i] = stage1_rpi3[i];
		   stage2_srv[i] = stage2_rpi3[i];
		   stage3_srv[i] = stage3_rpi3[i];
	       }
	   }else if((OSmodel->SRV_TYPE) == 2){// RPi4
	       for(i=0;i<HEARTBEAT_NUM;i++){
		   stage0_srv[i] = stage0_rpi3[i]/2;
		   stage1_srv[i] = stage1_rpi3[i]/2;
		   stage2_srv[i] = stage2_rpi3[i]/2;
		   stage3_srv[i] = stage3_rpi3[i]/2;
	       }
	   }else{
	       std::cout << "SRV_TYPE: "<< (OSmodel->SRV_TYPE)<<" is invalid." << std::endl;
	   }
   }
   std::cout<<"....................config_init done.................."<<std::endl;
}
using namespace rapidjson;

void  g_config_init(){
   std::cout<<"....................g_config_init.................."<<std::endl;
   std::ifstream ifs("./conf.json");
   std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
   ifs.close();
   Document d;
   d.Parse(content.c_str());

   Value& item = d["cli_num"];
   CLI_NUM = item.GetInt();
   item = d["srv_core_num"];
   SRV_CORE_NUM = item.GetInt();
   total_input_sets = HEARTBEAT_NUM;//Stat value defined in OS model
   total_cli_num = CLI_NUM;//Stat value defined in OS model
   std::cout<<"....................g_config_init done.................."<<std::endl;
}


void read_config_json(){

   //{
   //    "cli_core_num": 1,
   //    "protocol": 0,
   //    "offloading": [1,1,1,1,1,1],
   //    "srv_core_num": 1,
   //    "srv_type": 1,
   //    "cli_type": 0
   //}
   std::ifstream ifs("./conf.json");
   std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
   ifs.close();
   Document d;

   d.Parse(content.c_str());


   OSModelCtxt* OSmodel = taskManager.getTaskCtxt( sc_core::sc_get_current_process_handle() );
   Value& item = d["cli_num"];
   OSmodel->CLI_NUM = item.GetInt();

   item = d["srv_type"];
   OSmodel->SRV_TYPE = item.GetInt();

   item = d["srv_core_num"];
   OSmodel->SRV_CORE_NUM = item.GetInt();
/*
   item = d["cli_type"];
   OSmodel->CLI_TYPE = item.GetInt();

   item = d["cli_core_num"];
   OSmodel->CLI_CORE_NUM = item.GetInt();

   item = d["offloading"];
   OSmodel->OFFLOAD_LEVEL = item.GetInt();
   CLI_OFFLOAD_LEVEL[OSmodel->NodeID] = OSmodel->OFFLOAD_LEVEL;
*/

   if((OSmodel->NodeID)>0){
	   item = d["cli_core_num"];
	   OSmodel->CLI_CORE_NUM = (item.GetArray())[(OSmodel->NodeID)-1].GetInt();
	   item = d["cli_type"];
	   OSmodel->CLI_TYPE = (item.GetArray())[(OSmodel->NodeID)-1].GetInt();
	   item = d["offloading"];
	   //OSmodel->OFFLOAD_LEVEL = item.GetInt();
	   OSmodel->OFFLOAD_LEVEL = (item.GetArray())[(OSmodel->NodeID)-1].GetInt();
	   CLI_OFFLOAD_LEVEL[OSmodel->NodeID] = OSmodel->OFFLOAD_LEVEL;
           std::cout << "OSmodel->CLI_CORE_NUM" << OSmodel->CLI_CORE_NUM << std::endl;
           std::cout << "OSmodel->CLI_TYPE" << OSmodel->CLI_TYPE << std::endl;
           std::cout << "OSmodel->OFFLOAD_LEVEL" << OSmodel->OFFLOAD_LEVEL << std::endl;
   }
}


