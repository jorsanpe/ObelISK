/*
 * $FILE: heap.c
 *
 * Heap implementation for scheduling purposes. It is used to
 * keep track of the remaining time of the tasks put to sleep.
 * It can be used to implement the EDF scheduler.
 *
 * TODO: Consider a heap modification, so that tasks with the
 * same keys are stored on the same tree node with a queue.
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

#include "heap.h"
#include "tlsf.h"

#define LCHILD(X)       (X<<1)
#define RCHILD(X)       ((X<<1)+1)
#define PARENT(X)       (X>>1)

/*
 * heap_create()
 */
heap_t *heap_create(int capacity)
{
   heap_t *heap;

   heap = malloc_ex(sizeof(heap_t), system_pool);
   heap->elements = calloc_ex(capacity, sizeof(heapnode_t), system_pool);
   heap->capacity = capacity + 1;
   heap->elements[0].key = -1;

   return heap;
}

/*
 * heap_insert()
 */
void heap_insert(heap_t *heap, void *item, SYSTIM key)
{
   int i;

   // Insert and Heapify
   for (i=++heap->size; heap->elements[PARENT(i)].key > key; i=PARENT(i))
      heap->elements[i] = heap->elements[PARENT(i)];
   heap->elements[i].item = item;
   heap->elements[i].key = key;
}

/*
 * heap_min()
 *
 *    Find minimum key
 */
SYSTIM heap_getmin(heap_t *heap)
{
   if (heap->size > 0) {
      return heap->elements[1].key;
   }
   return -1;
}

/*
 * heap_extract()
 *
 *    Find and extract minimum key
 */
void *heap_extract(heap_t *heap)
{
   int i, child;
   void *min;
   heapnode_t *last;

   // Heap is empty
   if (heap->size == 0) {
      return NULL;
   }

   // Minimum is on the top
   min = heap->elements[1].item;
   last = &heap->elements[heap->size--];

   // Heapify
   for (i=1; LCHILD(i)<=heap->size; i=child) {
      child = LCHILD(i);
      // Get smaller child
      if (child != heap->size && heap->elements[child+1].key < heap->elements[child].key) {
         child++;
      }
      if (last->key > heap->elements[child].key) {
         heap->elements[i] = heap->elements[child];
      } else {
         break;
      }
   }

   heap->elements[i].item = last->item;
   heap->elements[i].key = last->key;
   return min;
}

/*
 * heap_destroy()
 */
void heap_destroy(heap_t *heap)
{
   free_ex(heap->elements, system_pool);
   free_ex(heap, system_pool);
}



