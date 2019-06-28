#ifndef APPLICATION_H
#define APPLICATION_H
#include "vgraph.h"
#include "darkiot.h"
#ifndef MAX_EDGE_NUM
#define MAX_EDGE_NUM 6
#endif

typedef struct vgraph_ctxt{
   int node_id;
   int offload_level[MAX_EDGE_NUM];
   int total_edge_number;
   thread_safe_queue* self_data;
   thread_safe_queue* peer_data;
} vgraph_context;

void vgraph_gateway(uint32_t gateway_id);
void vgraph_edge(uint32_t edge_id);
#endif

