/*
 * $FILE: obkconsole.c
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

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>
#include <stdio.h>

#define CONSOLE_MAGIK_NUMBER 0x0C045013
#define CONSOLE_PORT         40000
#define MAX_CMD_LENGTH       64

#define OBK_CONSOLE_INFO     1
#define OBK_CONSOLE_STATUS   2
#define OBK_CONSOLE_ACTION   3

struct isolation_msg {
    union {
        struct {
            xm_u32_t cmd;
            xm_u32_t size;
            xm_s8_t *addr;
            char data[];
        };
        char msg[PORT_SIZE];
    };
};

void main(void)
{
   int serversock, consock, clilen;
   struct sockaddr_in consoleserver, consoleclient;
   char cmd[MAX_CMD_LENGTH];

   // Acquire a socket to wait for connections
   if ((serversock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
       EPRINT("Error. Failed to Acquire socket\n");
   }
   printf("Socket acquired\n");

   memset(&consoleserver, 0, sizeof(consoleserver));
   consoleserver.sin_family = AF_INET;
   consoleserver.sin_addr.s_addr = htonl(INADDR_ANY);
   consoleserver.sin_port = htons(CONSOLE_PORT);

   if (bind(serversock, (struct sockaddr *) &consoleserver, sizeof(consoleserver)) < 0) {
      EPRINT("Error. Failed to Bind socket\n");
   }
   printf("Socket binded\n");

   if (listen(serversock, 10) < 0) {
      EPRINT("Error. Could not Listen on socket\n");
   }

   printf("Listening on port %d. Waiting for connections... ", CONSOLE_PORT);
   while (1) {
      clilen = sizeof(consoleclient);
      // Accept connections
      if ((consock = accept(serversock, (struct sockaddr *) &consoleclient, &clilen)) < 0) {
         EPRINT("Error. Error in Accept\n");
      } else {
         int msg;
         recv(consock, &msg, 4, 0); // Protocol on console. Echo magic number

         if (msg != CONSOLE_MAGIK_NUMBER) {
            msg = -1;
            send(consock, &msg, 4, 0); // Error in protocol. Not received a magic number
            close(consock);
            continue;
         } else {
            send(consock, &msg, 4, 0); // Protocol OK. Console ready
         }

         printf("Connection accepted\n");

         while (recv(consock, cmd, MAX_CMD_LENGTH, 0) != -1) {
            //printf("Command received: %s\n", cmd);
            //sprintf(cmd, "Response OK");
            msg = strlen(cmd);
            send(consock, &msg, 4, 0);
            send(consock, cmd, msg, 0);
         }

         close(consock);            // Close socket
      }
   }
}

