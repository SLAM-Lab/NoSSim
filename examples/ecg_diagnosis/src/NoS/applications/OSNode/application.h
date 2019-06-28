#ifndef APPLICATION_H
#define APPLICATION_H
#include "ecg.h"
#include "darkiot.h"

#ifndef MAX_EDGE_NUM
#define MAX_EDGE_NUM 6
#endif

void send_offload_data(offload_data* data, service_conn* conn);
void recv_offload_data(offload_data* data, service_conn* conn);
void send_result(result_data* data, service_conn* conn);

typedef struct ecg_ctxt{
   int node_id;
   int offload_level[MAX_EDGE_NUM];
   int total_edge_number;
   thread_safe_queue* ready;
} ecg_context;

void ecg_gateway(uint32_t gateway_id);
void ecg_edge(uint32_t edge_id);
#endif

