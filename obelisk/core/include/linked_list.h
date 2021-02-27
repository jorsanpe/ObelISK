/*
 * $FILE: linked_list.h
 *
 * Linked list management
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

#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#ifndef NULL
#define NULL 0
#endif

typedef struct linked_list_struct {
   struct linked_list_struct *prev, *next;
   void *this;
} linked_list_t;

/*
 * init_linked_list()
 */
static inline void
init_linked_list (linked_list_t **h)
{
   *h = NULL;
}

/*
 * insert_linked_list()
 */
static inline void
insert_linked_list (linked_list_t **h, linked_list_t *p)
{
   if (*h)
      (*h)->prev = p;

   p->next = *h;
   *h = p;
}

/*
 * remove_linked_list()
 */
static inline void
remove_linked_list (linked_list_t **h, linked_list_t *p)
{
   if (p->next)
      p->next->prev = p->prev;

   if (p->prev)
      p->prev->next = p->next;

   if (*h == p)
      *h = p->next;

   p->prev = p->next = NULL;
}

/*
 * remove_head_linked_list()
 */
static inline void *
remove_head_linked_list (linked_list_t **h)
{
   linked_list_t *p = *h;
   void *t = NULL;

   if (*h)
      *h = (*h)->next;

   if (*h)
      (*h)->prev = NULL;

   if (p) {
      p->prev = p->next = NULL;
      t = p->this;
   }

   return t;
}

#endif   //_LINKED_LIST_H_

