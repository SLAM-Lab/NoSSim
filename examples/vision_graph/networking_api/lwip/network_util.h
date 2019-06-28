#ifndef NETWORK_UTIL_H
#define NETWORK_UTIL_H
#include "lwip_ctxt.h"
/*Assgin port number for different services*/
#define PORTNO 11111 //Service for job stealing and sharing
#define SMART_GATEWAY 11112 //Service for a smart gateway 
#define START_CTRL 11113 //Control the start and stop of a service
#define RESULT_COLLECT_PORT 11114 //Control the start and stop of a service
#define WORK_STEAL_PORT 11115 //Control the start and stop of a service

#define IPV4_TASK LWIP_IPV4
#define IPV6_TASK !(IPV4_TASK)

#if IPV4_TASK
#define ADDRSTRLEN INET_ADDRSTRLEN
#elif IPV6_TASK/*IPV4_TASK*/
#define ADDRSTRLEN INET6_ADDRSTRLEN
#endif/*IPV4_TASK*/   

#include "data_blob.h"/*Data blob function is defined by the DeepThings*/

#if LWIP_IPV4 && LWIP_IPV6
/** @ingroup socket */
#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET6) ? ip6addr_ntoa_r((const ip6_addr_t*)(src),(dst),(size)) \
     : (((af) == AF_INET) ? ip4addr_ntoa_r((const ip4_addr_t*)(src),(dst),(size)) : NULL))
/** @ingroup socket */
#define inet_pton(af,src,dst) \
    (((af) == AF_INET6) ? ip6addr_aton((src),(ip6_addr_t*)(dst)) \
     : (((af) == AF_INET) ? ip4addr_aton((src),(ip4_addr_t*)(dst)) : 0))
#elif LWIP_IPV4 /* LWIP_IPV4 && LWIP_IPV6 */
#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET) ? ip4addr_ntoa_r((const ip4_addr_t*)(src),(dst),(size)) : NULL)
#define inet_pton(af,src,dst) \
    (((af) == AF_INET) ? ip4addr_aton((src),(ip4_addr_t*)(dst)) : 0)
#else /* LWIP_IPV4 && LWIP_IPV6 */
#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET6) ? ip6addr_ntoa_r((const ip6_addr_t*)(src),(dst),(size)) : NULL)
#define inet_pton(af,src,dst) \
    (((af) == AF_INET6) ? ip6addr_aton((src),(ip6_addr_t*)(dst)) : 0)
#endif /* LWIP_IPV4 && LWIP_IPV6 */



typedef enum proto{
   TCP,
   UDP
} ctrl_proto;

typedef struct service_connection{
   int sockfd;
   ctrl_proto proto;
   #if IPV4_TASK
   struct sockaddr_in* serv_addr_ptr;
   #elif IPV6_TASK/*IPV4_TASK*/
   struct sockaddr_in6* serv_addr_ptr;
   #endif/*IPV4_TASK*/   
} service_conn;

/*Networking API on service client side*/
service_conn* connect_service(ctrl_proto proto, const char *dest_ip, int portno);
void close_service_connection(service_conn* conn);
void send_request(char* meta, uint32_t meta_size, service_conn* conn);

/*Networking API on service server side*/
int service_init(int portno, ctrl_proto proto);
void start_service_for_n_times(int sockfd, ctrl_proto proto, const char* handler_name[], uint32_t handler_num, void* (*handlers[])(void*, void*), void* arg, uint32_t times);
void start_service(int sockfd, ctrl_proto proto, const char* handler_name[], uint32_t handler_num, void* (*handlers[])(void*, void*), void* arg);
void close_service(int sockfd);

/*Data exchanging API on both sides*/
blob* recv_data(service_conn* conn);
void send_data(blob *temp, service_conn* conn);

/*IP address parsing API*/
void get_dest_ip_string(char* ip_string, service_conn* conn);

#endif
