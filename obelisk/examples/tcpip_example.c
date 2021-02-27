/*
 * $FILE: tcpip_example.c
 *
 * lwIP example. This application implements a TCP/IP
 * echo server
 *
 * Author: Jordi Sánchez <jorsanp3@upvnet.upv.es>
 *
 * $LICENSE:
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include <itron.h>
#include <sockets.h>
#include <string.h>
#include <arch/processor.h>

enum {
   ECHO_SERVER_TID = 1,
   ECHO_WORKER_TID
};

void echo_server(void);
void echo_worker(void *arg);

void echo_server(void)
{
   struct sockaddr_in saddr;
   struct sockaddr cliaddr;
   int clilen, listenfd, sock;
   T_CTSK pk_ctsk;

   // Acquire a socket to wait for connections
   listenfd = socket(AF_INET, SOCK_STREAM, 0);

   memset(&saddr, 0, sizeof(struct sockaddr));
   saddr.sin_family = AF_INET;
   saddr.sin_addr.s_addr = htonl(INADDR_ANY);
   saddr.sin_port = htons(12345);

   if (bind(listenfd, (struct sockaddr *) &saddr, sizeof(struct sockaddr_in)) == -1) {
      exd_tsk();
   }

   if (listen(listenfd, 0) == -1) {
      exd_tsk();
   }

   /*memset(&pk_ctsk, 0, sizeof(pk_ctsk));
   pk_ctsk.task = echo_worker;
   pk_ctsk.itskpri = 6;
   pk_ctsk.stksz = 512;
   pk_ctsk.exinf = TSK_VPARG;
   pk_ctsk.extdat = (void *) sock;
   get_mpl(0, pk_ctsk.stksz, &pk_ctsk.stk);*/
   while (1) {
      // Accept connections
      if ((sock = accept(listenfd, &cliaddr, &clilen)) != -1) {
         int msg;
         recv(sock, &msg, 4, 0); // Receive data
         send(sock, &msg, 4, 0); // Send echo
         close(sock);            // Close socket
         /*ID workerid;
         workerid = acre_tsk(&pk_ctsk);
         act_tsk(workerid);*/
      } else {
         return;
      }
   }
}

void echo_worker(void *arg)
{
   int msg;
   int sock = (int) arg;

   // Receive data
   recv(sock, &msg, 4, 0);

   // Send echo
   send(sock, &msg, 4, 0);

   // Close socket
   close(sock);

   // Die
   exd_tsk();
}

void main(void)
{
   T_CTSK pk_ctsk;
   ID serverid;
   UH val=0;

   memset(&pk_ctsk, 0, sizeof(pk_ctsk));
   pk_ctsk.task = echo_server;
   pk_ctsk.itskpri = 5;
   pk_ctsk.stksz = 512;
   get_mpl(1, pk_ctsk.stksz, &pk_ctsk.stk);

   serverid = acre_tsk(&pk_ctsk);
   act_tsk(serverid);

   wri16_mmr(PORTH_DIR_SET, PH5);
   while (1) {
      tslp_tsk(500);
      rea16_mmr(PORTH, &val);
      wri16_mmr(PORTH, val ^ PH5);
   }
   exd_tsk();
}

