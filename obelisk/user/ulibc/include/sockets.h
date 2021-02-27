/*
 * $FILE: sockets.h
 *
 * Sockets API
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
#ifndef _SOCKETS_H_
#define _SOCKETS_H_

#include <kernel_inc/arch/arch_types.h>
#include <lwip/sockets.h>
#include <lwip/inet.h>

extern int accept(int s, struct sockaddr *addr, socklen_t *addrlen);
extern int bind(int s, const struct sockaddr *name, socklen_t namelen);
extern int shutdown(int s, int how);
extern int getpeername (int s, struct sockaddr *name, socklen_t *namelen);
extern int getsockname (int s, struct sockaddr *name, socklen_t *namelen);
extern int getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen);
extern int setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen);
extern int close(int s);
extern int connect(int s, const struct sockaddr *name, socklen_t namelen);
extern int listen(int s, int backlog);
extern int recv(int s, void *mem, size_t len, int flags);
extern int read(int s, void *mem, size_t len);
extern int recvfrom(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
extern int send(int s, const void *dataptr, size_t size, int flags);
extern int sendto(int s, const void *dataptr, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);
extern int socket(int domain, int type, int protocol);
extern int write(int s, const void *dataptr, size_t size);
extern int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout);
extern int ioctl(int s, long cmd, void *argp);

#endif //_SOCKETS_H_
