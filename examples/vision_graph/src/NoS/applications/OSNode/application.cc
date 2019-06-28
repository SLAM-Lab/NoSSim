#include "application.h"
#include "data_blob.h"

#define FRAME_NUM 5

#define EDGE_PORT 11117
#define GATEWAY_PORT 11119

//For profiling
static double sim_time_start;
static double real_time_start;
static inline double real_time_now_sec(){
   struct timeval time;
   if (gettimeofday(&time,NULL)) return 0;
   return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

uint32_t file_size_in_bytes(const char* filename){
   uint32_t size = 0;
   FILE *f = fopen(filename, "rb");
   if (f == NULL) {
      fprintf(stderr, "Cannot load image %s\n", filename);
      exit(0);
   }
   fseek(f, 0, SEEK_END);
   size = ftell(f);
   fclose(f);
   return size;
}

void write_char_array(char array[], int size, const char* filename){
  FILE* file = fopen (filename, "wb");
  int i;
  for( i = 0; i < size; i++){
    char f = array[i];
    fwrite(&f, sizeof(char), 1, file);
  }
  fclose(file);
}

uint8_t* read_char_array(int size, const char* filename){
  FILE*  file = fopen(filename, "rb");
  char* results = (char *)malloc((size+1)*sizeof(char));
  int i=0;
  while(1){
    char f;
    int n  = fread(&f, sizeof(char), 1, file);
    if (n<1){break;}
    results[i] = f;
    i++;
  }
  fclose(file);
  uint8_t* array = (uint8_t *)malloc((i)*sizeof(uint8_t));
  int ii;
  for(ii=0; ii<(i); ii++){
  	array[ii] = results[ii];
  }
  free(results);
  return array;
}

void edge_process(void* arg){
   vgraph_context* ctxt = (vgraph_context*)arg; 

   int seq_num = 0;
   for(seq_num = 0; seq_num <FRAME_NUM; seq_num++){
      uint8_t* data;
      uint32_t bytes_length;
      record_static("read_jpg", (char*)std::string(std::to_string(seq_num) + "_" + std::to_string(ctxt->node_id) + "_" + std::to_string(0)).c_str(), "application");
      sys_time_wait("read_jpg", (char*)std::string(std::to_string(seq_num) + "_" + std::to_string(ctxt->node_id) + "_" + std::to_string(0)).c_str());

      //printf("======edge_process ready for frame: %d at time %f\n", seq_num, sc_core::sc_time_stamp().to_seconds());
      if(ctxt->offload_level[ctxt->node_id] == 0){	
         std::string local_img = "data/input/Node" + std::to_string(ctxt->node_id) + "/" + std::to_string(seq_num)+".jpg";

         uint8_t* buffer = keypoints_extraction(local_img.c_str());
         record_static("keypoints_extraction", (char*)std::string(std::to_string(seq_num) + "_" + std::to_string(ctxt->node_id) + "_" + std::to_string(0)).c_str(), "application");
         sys_time_wait("keypoints_extraction", (char*)std::string(std::to_string(seq_num) + "_" + std::to_string(ctxt->node_id) + "_" + std::to_string(0)).c_str());

         bytes_length = get_kp_raw_size(buffer);
         data = get_kp_raw_data(buffer);
         blob* local_feature = new_blob_and_copy_data(ctxt->node_id, bytes_length, data);
         free(buffer);
         enqueue(ctxt->self_data, local_feature);
         blob* peer_feature = dequeue(ctxt->peer_data);

 
         std::cout << "Matching between " << ctxt->node_id << " and "<< (ctxt->node_id+1)%(ctxt->total_edge_number) << " in edge " << ctxt->node_id << ", "<< "at time " << sc_core::sc_time_stamp().to_seconds() <<std::endl;
         uint32_t matches = keypoints_matching(local_feature->data, peer_feature->data, ctxt->node_id, (ctxt->node_id+1)%(ctxt->total_edge_number), seq_num);
         record_static("keypoints_matching", (char*)std::string(std::to_string(seq_num) + "_" 
                        + std::to_string(ctxt->node_id) + "_" + std::to_string((ctxt->node_id+1)%(ctxt->total_edge_number))).c_str(), "application");
         sys_time_wait("keypoints_matching", (char*)std::string(std::to_string(seq_num) + "_" 
                        + std::to_string(ctxt->node_id) + "_" + std::to_string((ctxt->node_id+1)%(ctxt->total_edge_number))).c_str());
         free_blob(local_feature);
         free_blob(peer_feature);

         blob* result = new_blob_and_copy_data(ctxt->node_id, sizeof(uint32_t), (uint8_t *)&matches);
         enqueue(ctxt->self_data, result);
         free_blob(result);
      }
      else if(ctxt->offload_level[ctxt->node_id] == 1){		
         std::string filename = "data/input/Node" + std::to_string(ctxt->node_id) + "/" + std::to_string(seq_num)+".jpg";
         uint8_t* buffer = keypoints_extraction(filename.c_str());
         record_static("keypoints_extraction", (char*)std::string(std::to_string(seq_num) + "_" + std::to_string(ctxt->node_id) + "_" + std::to_string(0)).c_str(), "application");
         sys_time_wait("keypoints_extraction", (char*)std::string(std::to_string(seq_num) + "_" + std::to_string(ctxt->node_id) + "_" + std::to_string(0)).c_str());
         bytes_length = get_kp_raw_size(buffer);
         data = get_kp_raw_data(buffer);
         blob* local_feature = new_blob_and_copy_data(ctxt->node_id, bytes_length, data);
         free(buffer);
         printf("Enqueue local feature in edge %d at time %f\n", ctxt->node_id, sc_core::sc_time_stamp().to_seconds());
         enqueue(ctxt->self_data, local_feature);
         printf("Enqueued local feature in edge %d at time %f\n", ctxt->node_id, sc_core::sc_time_stamp().to_seconds());
         free_blob(local_feature);
      }
      else if(ctxt->offload_level[ctxt->node_id] == 2){	
         std::string filename = "data/input/Node" + std::to_string(ctxt->node_id) + "/" + std::to_string(seq_num)+".jpg";
         bytes_length = file_size_in_bytes(filename.c_str());
         data = read_char_array(bytes_length, filename.c_str());
         blob* local_feature = new_blob_and_copy_data(ctxt->node_id, bytes_length, data);
         free(data);
         enqueue(ctxt->self_data, local_feature);
         free_blob(local_feature);
      }
      else {
         //Nothing to be done
      }

   }  
}

void* edge_offload(void* srv_conn, void* app_ctxt){
   vgraph_context* ctxt = (vgraph_context*)app_ctxt;
   service_conn* conn = (service_conn*)srv_conn;
   blob* data = dequeue(ctxt->self_data);
   send_data(data, conn);
   free_blob(data);
   return NULL;
}

void* edge_peer_data(void* srv_conn, void* app_ctxt){
   vgraph_context* ctxt = (vgraph_context*)app_ctxt;
   service_conn* conn = (service_conn*)srv_conn;
   blob* data = recv_data(conn);
   enqueue(ctxt->peer_data, data);
   free_blob(data);
   return NULL;
}

void edge_service(void *arg){
   const char* request_types[]={"offload_data", "peer_data"};
   void* (*handlers[])(void*, void*) = {edge_offload, edge_peer_data};
   int service_id = service_init(EDGE_PORT, TCP);
   start_service(service_id, TCP, request_types, 2, handlers, arg);
   close_service(service_id);
}


void vgraph_edge(uint32_t edge_id){
   /*exec_barrier(START_CTRL, TCP, ctxt);*/
   /*TODO some sort of barrier must exist here*/
   vgraph_context* ctxt = (vgraph_context*)malloc(sizeof(vgraph_context));
   ctxt->node_id = edge_id;
   ctxt->total_edge_number = (sim_ctxt.cluster)->total_number;
   ctxt->offload_level[edge_id] = std::stoi((sim_ctxt.cluster)->edge_type[edge_id]);
   ctxt->self_data = new_queue(1);
   ctxt->peer_data = new_queue(1);

   sys_thread_t t1 = sys_thread_new("edge_service", edge_service, ctxt, 101, 0);
   sys_thread_t t2 = sys_thread_new("edge_process", edge_process, ctxt, 101, 0);

   sys_thread_join(t1);
   sys_thread_join(t2);
}

void gateway_process(void *arg){
   vgraph_context* ctxt = (vgraph_context*)arg; 
   #if IPV4_TASK
   const char* addr_list[MAX_EDGE_NUM] = {"192.168.4.9", "192.168.4.8", "192.168.4.4", "192.168.4.14", "192.168.4.15", "192.168.4.16"};
   #else 
   const char* addr_list[MAX_EDGE_NUM] = {"100:0:200:0:300:0:400:", "100:0:200:0:300:0:500:", "100:0:200:0:300:0:600:",
                                           "100:0:200:0:300:0:700:", "100:0:200:0:300:0:800:", "100:0:200:0:300:0:900:"};
   #endif
   blob* recv_blob[MAX_EDGE_NUM];
   uint8_t* feature_data[MAX_EDGE_NUM];
   uint32_t matches[MAX_EDGE_NUM][MAX_EDGE_NUM];
   bool covered[MAX_EDGE_NUM][MAX_EDGE_NUM];
   int node_id;

   //Recording simulated time and real time for design space exploration    
   sim_time_start = sc_core::sc_time_stamp().to_seconds();
   real_time_start = real_time_now_sec();


   int seq_num = 0;
   for(seq_num = 0; seq_num <FRAME_NUM; seq_num++){

      for(int i = 0; i < ctxt->total_edge_number; i ++){
         for(int j = 0; j < ctxt->total_edge_number; j ++){
            covered[i][j] = false;
            covered[j][i] = false;
         }
      }

      //Collect data from edge devices
      for(node_id = 0; node_id < ctxt->total_edge_number; node_id++){
         service_conn* conn = connect_service(TCP, addr_list[node_id], EDGE_PORT);
         char request_type[20] = "offload_data";
         send_request(request_type, 20, conn);
         recv_blob[node_id] = recv_data(conn);
         printf("Recv data from device %d, offload_level is %d at time %f\n", node_id, ctxt->offload_level[node_id], sc_core::sc_time_stamp().to_seconds());
         close_service_connection(conn);  

      }


      //Process collected data for each edge device
      for(node_id = 0; node_id < ctxt->total_edge_number; node_id++){
         if(ctxt->offload_level[node_id] == 2){
            printf("Process data from device %d, offload_level is 2 at time %f\n", node_id, sc_core::sc_time_stamp().to_seconds());
            std::string jpg = "Node" + std::to_string(node_id) + "_frame_" + std::to_string(seq_num)+".jpg";
            write_char_array((char*)recv_blob[node_id]->data, recv_blob[node_id]->size, jpg.c_str());
            std::string img = "data/input/Node" + std::to_string(node_id) + "/" + std::to_string(seq_num)+".jpg";
            feature_data[node_id] = keypoints_extraction(img.c_str());	
            record_static("keypoints_extraction", (char*)std::string(std::to_string(seq_num) + "_" + std::to_string(node_id) + "_" + std::to_string(0)).c_str(), "application");
            sys_time_wait("keypoints_extraction", (char*)std::string(std::to_string(seq_num) + "_" + std::to_string(node_id) + "_" + std::to_string(0)).c_str());
         }
         else if(ctxt->offload_level[node_id] == 1){
            printf("Process data from device %d, offload_level is 1 at time %f\n", node_id, sc_core::sc_time_stamp().to_seconds());
            feature_data[node_id] = recv_blob[node_id]->data;
         }
         else if(ctxt->offload_level[node_id] == 0){ 
            printf("Process data from device %d, offload_level is 0 at time %f\n", node_id, sc_core::sc_time_stamp().to_seconds());
            feature_data[node_id] = recv_blob[node_id]->data;
         }else{
            //Doing nothing
         }
      }
      for(node_id = 0; node_id < ctxt->total_edge_number; node_id++){
         if(ctxt->offload_level[node_id] == 0){
              service_conn* conn = connect_service(TCP, addr_list[node_id], EDGE_PORT);
              char request_type[20] = "peer_data";
              send_request(request_type, 20, conn);
              if(ctxt->offload_level[(node_id+1)%(ctxt->total_edge_number)] == 2){
                 uint32_t tmp_bytes_length = get_kp_raw_size(feature_data[(node_id+1)%(ctxt->total_edge_number)]);
                 uint8_t* tmp_data = get_kp_raw_data(feature_data[(node_id+1)%(ctxt->total_edge_number)]);
                 blob* feature_blob = new_blob_and_copy_data((node_id+1)%(ctxt->total_edge_number), tmp_bytes_length, tmp_data);
                 send_data(feature_blob, conn);
                 free_blob(feature_blob);
              }else {
                 send_data(recv_blob[(node_id+1)%(ctxt->total_edge_number)], conn);
              }
              close_service_connection(conn);  
         }
      }
      for(node_id = 0; node_id < ctxt->total_edge_number; node_id++){
         if(ctxt->offload_level[node_id] == 0){
            service_conn* conn = connect_service(TCP, addr_list[node_id], EDGE_PORT);
            char request_type[20] = "offload_data";
            send_request(request_type, 20, conn);
            blob* result = recv_data(conn); //result between node_id and ((node_id+1)%(ctxt->total_edge_number))
            close_service_connection(conn);  
            matches[node_id][(node_id+1)%(ctxt->total_edge_number)] = *((uint32_t*)(result->data));
            matches[(node_id+1)%(ctxt->total_edge_number)][node_id] = *((uint32_t*)(result->data));
            covered[node_id][(node_id+1)%(ctxt->total_edge_number)] = true;
            covered[(node_id+1)%(ctxt->total_edge_number)][node_id] = true;
            free_blob(result);
         }
      }


      //Construction vision graph
      for(node_id = 0; node_id < ctxt->total_edge_number; node_id++){
         for(int adj_node_id = node_id + 1; adj_node_id < ctxt->total_edge_number; adj_node_id++){
            //feature matching 
            if(covered[node_id][adj_node_id]) continue;
            std::cout << "Matching between " << node_id << " and "<< adj_node_id << " in gateway, "<< "at time " << sc_core::sc_time_stamp().to_seconds() <<std::endl;
            matches[node_id][adj_node_id] = keypoints_matching(feature_data[node_id], feature_data[adj_node_id], node_id, adj_node_id, seq_num);
            record_static("keypoints_matching", (char*)std::string(std::to_string(seq_num) + "_" 
                        + std::to_string(node_id) + "_" + std::to_string(adj_node_id)).c_str(), "application");
            sys_time_wait("keypoints_matching", (char*)std::string(std::to_string(seq_num) + "_" 
                        + std::to_string(node_id) + "_" + std::to_string(adj_node_id)).c_str());
            matches[adj_node_id][node_id] = matches[node_id][adj_node_id];
            covered[node_id][adj_node_id] = true;
            covered[adj_node_id][node_id] = true;
         }
      }

      for(node_id = 0; node_id < ctxt->total_edge_number; node_id++){
         free_blob(recv_blob[node_id]);
      }
   }

   (sim_ctxt.result)->set_gateway_result("latency", (double)(sc_core::sc_time_stamp().to_seconds() - sim_time_start)/(double)(FRAME_NUM));
   (sim_ctxt.result)->set_gateway_result("sim_time", (double)(sc_core::sc_time_stamp().to_seconds() - sim_time_start));
   (sim_ctxt.result)->set_gateway_result("real_time", (double)(real_time_now_sec() - real_time_start));
   (sim_ctxt.result)->set_gateway_result("simsec_per_sec", (double)(sc_core::sc_time_stamp().to_seconds() - sim_time_start)/(double)(real_time_now_sec() - real_time_start));
   os_model_context* os_model = sim_ctxt.get_os_ctxt( sc_core::sc_get_current_process_handle() );
   os_model -> ctrl_out1->write(0);


}

void vgraph_gateway(uint32_t gateway_id){
   /*exec_barrier(START_CTRL, TCP, ctxt);*/
   /*TODO some sort of barrier must exist here*/
   vgraph_context* ctxt = (vgraph_context*)malloc(sizeof(vgraph_context));
   ctxt->node_id = gateway_id;
   ctxt->total_edge_number = (sim_ctxt.cluster)->total_number;
   for(int node_id = 0; node_id < ctxt->total_edge_number; node_id++){
      ctxt->offload_level[node_id] = std::stoi((sim_ctxt.cluster)->edge_type[node_id]);
   }
   sys_thread_t t1 = sys_thread_new("gateway_process", gateway_process, ctxt, 101, 0);
   sys_thread_join(t1);
}



