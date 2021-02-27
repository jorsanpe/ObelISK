/*
 * $FILE: sys_arch.h
 *
 * Wrapper header for the lwip TCP/IP stack
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

#ifndef _LWIP_SYS_ARCH_H_
#define _LWIP_SYS_ARCH_H_

#include "itron.h"
#include "stddef.h"

#define SYS_SEM_NULL       (-1)
#define SYS_MBOX_NULL      (-1)

struct thread_struct_wrapper {
  struct thread_struct_wrapper *next;
  ID thread;
  //struct sys_timeouts timeouts;
  void *timeouts;
};

typedef ID sys_sem_t, sys_mbox_t;
typedef struct thread_struct_wrapper *sys_thread_t;

#endif   //_LWIP_SYS_ARCH_H_
