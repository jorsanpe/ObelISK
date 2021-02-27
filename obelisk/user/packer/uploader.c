/*
 * $FILE: uploader.c
 *
 * This program connects to the reflash service
 * installed on the node. It uploads a new flash
 * image based on the established communication protocol
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

#define RECOVERY_MAGIK_NUMBER 0x2EC0BE21
#define RECOVERY_PORT         12345

static inline int get_file_size(FILE *stream)
{
   int size;

   fseek(stream, 0L, SEEK_END);
   size = ftell(stream);
   fseek(stream, 0L, SEEK_SET);

   return size;
}

int main (int argc, char *argv[])
{
   struct sockaddr_in reflash_server;
   unsigned char *image;
   int data, sock, size;
   FILE *file;

   if (argc != 3) {
      fprintf (stderr, "USAGE: uploader <server_ip> file\n");
      exit(1);
   }
   // Create the TCP socket
   if ((sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      printf ("Failed to create socket\n");
      exit(1);
   }

   // Open the image file
   if ((file = fopen(argv[2], "rb")) == NULL) {
      printf("Failed to open file %s\n", argv[2]);
      exit(1);
   }
   size = get_file_size(file);
   image = (unsigned char *)malloc(size);
   fread(image, 1, size, file);
   fclose(file);

   // Construct the server sockaddr_in structure
   memset (&reflash_server, 0, sizeof (reflash_server));      // Clear struct
   reflash_server.sin_family = AF_INET;                   // Internet/IP
   reflash_server.sin_addr.s_addr = inet_addr (argv[1]);  // IP address
   reflash_server.sin_port = htons (RECOVERY_PORT);      // server port

   // Establish connection
   printf("Connecting to reflash server\n");
   if (connect(sock, (struct sockaddr *) &reflash_server, sizeof (reflash_server)) < 0) {
      printf("Failed to connect with server %s:%d\n", argv[1], RECOVERY_PORT);
      free(image);
      exit(1);
   }

   // Protocol Phase 1
   data = RECOVERY_MAGIK_NUMBER;
   printf("Echoing Magic Number: %d\n", data);
   send(sock, &data, 4, 0);
   data = 0;
   recv(sock, &data, 4, 0);
   if (data != RECOVERY_MAGIK_NUMBER) {
      printf("Failed. Closing connection\n");
      free(image);
      exit(1);
   }
   printf("Success\n");

   // Protocol Phase 2
   printf("Sending flash image, size %d\n", size);
   send(sock, &size, 4, 0);
   send(sock, image, size, 0);
   recv(sock, &data, 4, 0);

   free(image);
   if (data < 0) {
      printf("Flash burn failed\n");
      exit(1);
   }

   printf("Successfully burned flash\n");
   close (sock);
   exit (0);
}








