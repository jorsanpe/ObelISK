/*
 * $FILE: heap.h
 *
 * Heap implementation for scheduling purposes
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
 */

#ifndef _HEAP_H_
#define _HEAP_H_

#ifndef _KERNEL_
#error   Internal file. Do not include it in your sources.
#endif

#include <itron.h>

typedef struct heapnode_struct {
   // TODO: consider the item as a queue so that a 
   // node can store items with the same key
   SYSTIM key;
   void *item;
} heapnode_t;

typedef struct heap_struct {
   heapnode_t *elements;
   int capacity;
   int size;
} heap_t;

extern heap_t *heap_create(int maxsize);
extern void heap_insert(heap_t *heap, void *item, SYSTIM key);
SYSTIM heap_getmin(heap_t *heap);
extern void *heap_extract(heap_t *heap);
extern void heapify(heap_t *heap);

#endif   //_HEAP_H_
