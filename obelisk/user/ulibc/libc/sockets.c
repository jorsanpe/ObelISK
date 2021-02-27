/*
 * $FILE: sockets.c
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

#include <itron.h>
#include <syscalls.h>
#include <sockets.h>

int accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
   int res;
   res = _syscall3(-TFN_ACCEPT, s, addr, addrlen);
   return res;
}

int bind(int s, const struct sockaddr *name, socklen_t namelen)
{
   int res;
   res = _syscall3(-TFN_BIND, s, name, namelen);
   return res;
}

int shutdown(int s, int how)
{
   int res;
   res = _syscall2(-TFN_SHUTDOWN, s, how);
   return res;
}

int getpeername (int s, struct sockaddr *name, socklen_t *namelen)
{
   int res;
   res = _syscall3(-TFN_GETPEERNAME, s, name, namelen);
   return res;
}

int getsockname (int s, struct sockaddr *name, socklen_t *namelen)
{
   int res;
   res = _syscall3(-TFN_GETSOCKNAME, s, name, namelen);
   return res;
}

int getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen)
{
   int res;
   res = _syscall5(-TFN_GETSOCKOPT, s, level, optname, optval, optlen);
   return res;
}

int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen)
{
   int res;
   res = _syscall5(-TFN_SETSOCKOPT, s, level, optname, optval, optlen);
   return res;
}

int close(int s)
{
   int res;
   res = _syscall1(-TFN_CLOSE, s);
   return res;
}

int connect(int s, const struct sockaddr *name, socklen_t namelen)
{
   int res;
   res = _syscall3(-TFN_CONNECT, s, name, namelen);
   return res;
}

int listen(int s, int backlog)
{
   int res;
   res = _syscall2(-TFN_LISTEN, s, backlog);
   return res;
}

int recv(int s, void *mem, size_t len, int flags)
{
   int res;
   res = _syscall4(-TFN_RECV, s, mem, len, flags);
   return res;
}

int read(int s, void *mem, size_t len)
{
   int res;
   res = _syscall3(-TFN_READ, s, mem, len);
   return res;
}

int recvfrom(int s, void *mem, size_t len, int flags,
      struct sockaddr *from, socklen_t *fromlen)
{
   int res;
   res = _syscall6(-TFN_RECVFROM, s, mem, len, flags, from, fromlen);
   return res;
}

int send(int s, const void *dataptr, size_t size, int flags)
{
   int res;
   res = _syscall4(-TFN_SEND, s, dataptr, size, flags);
   return res;
}

int sendto(int s, const void *dataptr, size_t size, int flags,
    const struct sockaddr *to, socklen_t tolen)
{
   int res;
   res = _syscall6(-TFN_SENDTO, s, dataptr, size, flags, to, tolen);
   return res;
}

int socket(int domain, int type, int protocol)
{
   int res;
   res = _syscall3(-TFN_SOCKET, domain, type, protocol);
   return res;
}

int write(int s, const void *dataptr, size_t size)
{
   int res;
   res = _syscall3(-TFN_WRITE, s, dataptr, size);
   return res;
}

int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                struct timeval *timeout)
{
   int res;
   res = _syscall5(-TFN_SELECT, maxfdp1, readset, writeset, exceptset, timeout);
   return res;
}

int ioctl(int s, long cmd, void *argp)
{
   int res;
   res = _syscall3(-TFN_IOCTL, s, cmd, argp);
   return res;
}

