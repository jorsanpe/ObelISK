/*
 * $FILE: obk_console.c
 *
 * Remote console for ObelISK
 *
 * Author: Jordi Sánchez, <jorsanp3@upvnet.upv.es>
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


#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>

#define CONSOLE_MAGIK_NUMBER 0x0C045013
#define CONSOLE_PORT         40000
#define MAX_CMD_LENGTH       64

#define OBK_CONSOLE_INFO     1
#define OBK_CONSOLE_STATUS   2
#define OBK_CONSOLE_ACTION   3

int main(int argc, char *argv[])
{
   struct sockaddr_in console_server;
   int data, sock, size, ret;
   char cmd[MAX_CMD_LENGTH];
   char *response;

   if (argc != 2) {
      fprintf (stderr, "USAGE: obk-console <server_ip>\n");
      exit(1);
   }
   // Create the TCP socket
   if ((sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      printf ("Failed to create socket\n");
      exit(1);
   }

   // Construct the server sockaddr_in structure
   memset (&console_server, 0, sizeof (console_server)); // Clear struct
   console_server.sin_family = AF_INET;                  // Internet/IP
   console_server.sin_addr.s_addr = inet_addr (argv[1]); // IP address
   console_server.sin_port = htons (CONSOLE_PORT);       // server port

   // Establish connection
   printf("Connecting to console... ");
   if (connect(sock, (struct sockaddr *) &console_server, sizeof (console_server)) < 0) {
      printf("Failed to connect with server %s:%d\n", argv[1], CONSOLE_PORT);
      exit(1);
   }

   // We send an echo to ensure that the ObelISK console
   // server is listening at the other side
   data = CONSOLE_MAGIK_NUMBER;
   send(sock, &data, 4, 0);
   data = 0;
   recv(sock, &data, 4, 0);
   if (data != CONSOLE_MAGIK_NUMBER) {
      printf("Error, no console detected. Closing connection\n");
      close (sock);
      exit(1);
   }
   printf("Connected\n");

   // Main loop
   while (1) {
      fprintf(stdout, "obk@%s$ ", argv[1]);
      fgets(cmd, MAX_CMD_LENGTH, stdin);
      send(sock, cmd, MAX_CMD_LENGTH, 0); // Send command
      recv(sock, &size, 4, 0);  // Response length
      if (size > 0) {
         response = malloc(size);
         recv(sock, response, size, 0);
         // Parse response --> info   :
         //                --> status
         //                --> action
         switch ((size >> 28) & 0xf) {
            case OBK_CONSOLE_INFO:
            case OBK_CONSOLE_STATUS:
               printf("%s\n", response);
               break;
            case OBK_CONSOLE_ACTION:
               break;
            default:
               break;
         }

         free(response);
      }
   }

   close (sock);
   exit (0);
}
