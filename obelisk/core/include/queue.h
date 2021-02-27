/*
 * $FILE: queue.h
 *
 * Queues
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

#ifndef _QUEUE_H_
#define _QUEUE_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include "linked_list.h"

typedef struct queue_struct {
   linked_list_t *head, *tail;
} queue_t;

/*
 * init_queue()
 */
static inline void queue_init(queue_t *sq)
{
   sq->head = sq->tail = NULL;
}

/*
 * queue_is_empty()
 */
static inline int queue_is_empty(queue_t *sq)
{
   return sq->head == NULL;
}

/*
 * queue_insert()
 */
static inline void queue_insert(queue_t *sq, linked_list_t *p)
{
   if (sq->tail)
      sq->tail->next = p;

   p->prev = sq->tail;
   sq->tail = p;

   if (!sq->head)
      sq->head = p;
}

/*
 * queue_insert_sort
 */
static inline void queue_insert_sort(queue_t *sq, linked_list_t *p)
{
   if (sq->tail)
      sq->tail->next = p;

   p->prev = sq->tail;
   sq->tail = p;

   if (!sq->head)
      sq->head = p;
}

/*
 * queue_get_head()
 */
static inline void *queue_get_head(queue_t *sq)
{
   linked_list_t *p = sq->head;
   void *t = NULL;

   if (p) {
      t = p->this;
   }
   return t;
}

/*
 * queue_extract()
 */
static inline void *queue_extract(queue_t *sq)
{
   linked_list_t *p = sq->head;
   void *t = NULL;

   if (sq->head)
      sq->head = sq->head->next;

   if (sq->head) {
      sq->head->prev = NULL;
   } else {
      sq->tail = NULL;
   }

   if (p) {
      p->prev = p->next = NULL;
      t = p->this;
   }

   return t;
}

/*
 * queue_remove()
 */
static inline void queue_remove(queue_t *sq, linked_list_t *p)
{
   if (p->next)
      p->next->prev = p->prev;

   if (p->prev)
      p->prev->next = p->next;

   if (sq->head == p)
      sq->head = p->next;

   if (sq->tail == p)
      sq->tail = p->prev;

   p->prev = p->next = NULL;
}

#endif   //_QUEUE_H_
