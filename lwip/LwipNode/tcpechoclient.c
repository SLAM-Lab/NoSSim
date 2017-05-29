/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#include "tcpechoclient.h"

#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/sys.h"
#include "lwip/api.h"
/*-----------------------------------------------------------------------------------*/
#include "image_file.h"
void 
tcpecho_client_thread(void *arg)
{
  bool debug;

  struct netconn *conn;
  //ip_addr_t ipaddr_local;
  ip_addr_t ipaddr_server;
  err_t err;
  LWIP_UNUSED_ARG(arg);
  LwipCntxt *ctxt = (LwipCntxt *)arg;
  //IP4_ADDR(&ipaddr_local, 192,168,0,3);
  IP4_ADDR(&ipaddr_server, 192,168,0,2);

  /* Create a new connection identifier. */

/*  if  ((((LwipCntxt* )ctxt)->ipaddr).addr == 0x300a8c0){
	debug=true;
  }
  else debug=false;


  if(debug)IP4_ADDR(&ipaddr_server, 192,168,0,2);
  else IP4_ADDR(&ipaddr_server, 192,168,0,2);
*/
//  if(debug) sc_core::wait(500, sc_core::SC_MS);

  conn = netconn_new(ctxt, NETCONN_TCP);

  /* Bind connection to well known port number 7. */
  //netconn_bind(ctxt, conn, &ipaddr_local, 7);

  netconn_bind(ctxt, conn, &(((LwipCntxt* )ctxt)->ipaddr), 7 );

  //if(debug) {printf("Probing --- 1\n");}
  netconn_connect(ctxt, conn, &ipaddr_server, 7);
  //if(debug) {printf("Probing --- 2\n");}

  size_t *bytes_written;
  unsigned int buf_size;

  //char buf[]="success";
  //buf_size = 8;

  char* buf;
  buf_size = load_file_to_memory("book.pgm", &buf);
  printf("buf_size is ....%d\n", buf_size);

  while (1) {

    err = netconn_write_partly(ctxt, conn, buf, buf_size, NETCONN_COPY, bytes_written);

    //printf("Thread finishing up ... ... ...\n");
//  err = netconn_write_partly(ctxt, conn, buf, buf_size, NETCONN_COPY, bytes_written);
    /* Process the new connection. */
//  netconn_close(ctxt, conn);

    if (err == ERR_OK) 
    {
      netconn_close(ctxt, conn);
      //netconn_disconnect(ctxt, conn);
      netconn_delete(ctxt, conn);
      break;
    }

  }


  //buf_size = load_file_to_memory("book.pgm", &buf);
  //printf("buf_size is ....%d\n", buf_size);
/*
  if(!debug){
    conn = netconn_new(ctxt, NETCONN_TCP);
    netconn_connect(ctxt, conn, &ipaddr_server, 7);
    printf("==========================Writing second time .... 0===========================\n");
    err = netconn_write_partly(ctxt, conn, buf, buf_size, NETCONN_COPY, bytes_written);
    printf("==========================Writing second time .... 0===========================\n");
  }
*/

}
/*-----------------------------------------------------------------------------------*/
void
tcpecho_client_init(LwipCntxt* ctxt)
{
  sys_thread_new(ctxt, "tcpecho_client_thread", tcpecho_client_thread, ctxt, DEFAULT_THREAD_STACKSIZE, 1);
}
/*-----------------------------------------------------------------------------------*/

#endif /* LWIP_NETCONN */


