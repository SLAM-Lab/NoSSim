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
#include "tcpecho.h"
#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/sys.h"
#include "lwip/api.h"
/*-----------------------------------------------------------------------------------*/
#include "image_file.h"
void 
tcpecho_thread(void *arg)
{
  struct netconn *conn, *newconn;
  err_t err;
  //LWIP_UNUSED_ARG(arg);
  LwipCntxt *ctxt = (LwipCntxt *)arg;

  /* Create a new connection identifier. */
  conn = netconn_new(ctxt, NETCONN_TCP);

  /* Bind connection to well known port number 7. */
  netconn_bind(ctxt, conn, NULL, 7);

  /* Tell connection to go into listening mode. */
  netconn_listen(ctxt, conn);

  while (1) {

    /* Grab new connection. */


    err = netconn_accept(ctxt, conn, &newconn);



    /*printf("accepted new connection %p\n", newconn);*/
    /* Process the new connection. */
    if (err == ERR_OK) {
      struct netbuf *buf;
      void *data;
      u16_t len;
      int ii;

      char *img = (char*)malloc(200000);
      int img_ii=0;
      int push = 0;


      while ((err = netconn_recv(ctxt, newconn, &buf)) == ERR_OK) {




        do {
             netbuf_data(ctxt, buf, &data, &len);

             for(ii=0;ii<(len);ii++)
		img[img_ii+ii]=((char*)data)[ii];
             img_ii+=len;

        } while (netbuf_next(buf) >= 0);

        if((buf->p->flags) & PBUF_FLAG_PUSH){
		push++; 
		printf("Pushing a received data into application %d\n", push);
		printf("length is ....%d\n", img_ii);
		
	}
        netbuf_delete(ctxt, buf);

	if(img_ii==97697){
		dump_mem_to_file(97697, "output.pgm", &img);
	}
      }
      /*printf("Got EOF, looping\n");*/ 
      /* Close connection and discard connection identifier. */
      netconn_close(ctxt, newconn);
      //netconn_disconnect(ctxt, newconn);
      netconn_delete(ctxt, newconn);
    }
  }
}
/*-----------------------------------------------------------------------------------*/
void
tcpecho_init(LwipCntxt* ctxt)
{
  sys_thread_new(ctxt, "tcpecho_thread", tcpecho_thread, ctxt, DEFAULT_THREAD_STACKSIZE, 1);
}
/*-----------------------------------------------------------------------------------*/

#endif /* LWIP_NETCONN */
