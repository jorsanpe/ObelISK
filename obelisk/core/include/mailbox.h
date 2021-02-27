/*
 * $FILE: mailbox.h
 *
 * ITRON Mailbox internal kernel header
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

#ifndef _MAILBOX_H_
#define _MAILBOX_H_

#include <queue.h>

typedef struct mailbox_struct {
   /* mailbox attributes */
   int32_t    mbxatr;

   /* maximum message priority */
   int32_t    maxmpri;

   /* start address of the area for message queue headers */
   void       *mprihd;

   /* tasks waiting on the mailbox queue */
   queue_t    mbxque;

   /* mailbox message queue */
   queue_t    msgque;

   /* If the mailbox is active, insert into the active queue */
   linked_list_t actlst;
} mailbox_t;

#endif   //_MAILBOX_H_
