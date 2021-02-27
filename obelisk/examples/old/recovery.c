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
#include <processor.h>
#include <lwip/sockets.h>
#include <lwip/inet.h>
#include <flash.h>

#define RECOVERY_MAGIK_NUMBER 0x2EC0BE21
#define RECOVERY_PORT         12345

// Reflash service
void reflash_server(void)
{
   struct sockaddr_in saddr;
   struct sockaddr cliaddr;
   int clilen, listenfd, sock;

   // Acquire a socket to wait for connections
   listenfd = socket(AF_INET, SOCK_STREAM, 0);

   memset(&saddr, 0, sizeof(struct sockaddr));
   saddr.sin_family = AF_INET;
   saddr.sin_addr.s_addr = htonl(INADDR_ANY);
   saddr.sin_port = htons(RECOVERY_PORT);

   // This is a recovery application and it must not fail
   if (bind(listenfd, (struct sockaddr *) &saddr, sizeof(struct sockaddr_in)) == -1) {
      exd_tsk();
   }
   if (listen(listenfd, 0) == -1) {
      exd_tsk();
   }

   // Flash burning service
   while (1) {
      int msg, bytes, left;
      unsigned char *image;

      // Accept connections
      if ((sock = accept(listenfd, &cliaddr, &clilen)) != -1) {
         // Connection accepted. Start the protocol to burn the flash
         recv(sock, &msg, 4, 0);
         if (msg != RECOVERY_MAGIK_NUMBER) {
            close(sock);
            continue;
         }
         msg = RECOVERY_MAGIK_NUMBER;
         send(sock, &msg, 4, 0);

         // Read flash image size
         recv(sock, &msg, 4, 0);

         // Flash image too big
         if (msg > 8000000) {
            close(sock);
            continue;
         }

         // Allocate memory for the image
         get_mpl(1, msg, &image);

         // Receive flash image
         bytes = 0;
         left = msg;
         while (left > 0) {
            bytes = recv(sock, image, left & 65535, 0);
            left -= bytes;
         }

         // Burn image on flash
         flash_setup();
         msg = flash_write_buffer(0, image, msg, 0);

         // Answer to the uploader
         send(sock, &msg, 4, 0);

         // Release memory
         close(sock);
         rel_mpl(0, image);
      }
   }
}

void main(void)
{
   T_CTSK pk_ctsk;
   ID serverid;

   pk_ctsk.task = reflash_server;
   pk_ctsk.itskpri = 3;
   pk_ctsk.stksz = 512;

   serverid = acre_tsk(&pk_ctsk);
   act_tsk(serverid);

   // Tilt led
   bfin_write_PORTH_DIR_SET(PH5 | PH6);

   /*while(1) {
      bfin_write_PORTH(bfin_read_PORTH() ^ PH6);
      tslp_tsk(500);
      bfin_write_PORTH(bfin_read_PORTH() ^ PH5);
   }*/

   ext_tsk();
}

