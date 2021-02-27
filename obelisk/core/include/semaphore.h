/*
 * $FILE: semaphore.h
 *
 * ITRON Semaphores management functions
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

#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#ifndef _KERNEL_
#error   Internal file. Do not include it in your sources.
#endif

#include <queue.h>

typedef struct semaphore_struct {
   /* extended information */
   int32_t    semid;

   /* extended information */
   void       *exinf;

   /* semaphore attributes */
   int32_t    sematr;

   /* current semaphore count */
   int32_t    semcnt;

   /* maximum semaphore count */
   int32_t    maxsem;

   /* Tasks waiting on the semaphore queue */
   queue_t semque;

   /* If the semaphore is active, insert into the active queue */
   linked_list_t actlst;
} semaphore_t;

#endif   //_SEMAPHORE_H_
