#include <systemc.h>
#include "lwip_ctxt.h"
double power_cli[10];
double CliEnergy[10];
unsigned char debug_flags=LWIP_DBG_OFF;
int node_choice=5;
long total_sent_pkts[10];
long total_recvd_pkts[10];

int sc_main(int, char *[]){

    std::cout << "The sc_main should not be called ..." <<std::endl;
    return 0;
}
