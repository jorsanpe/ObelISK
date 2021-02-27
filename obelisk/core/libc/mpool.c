/*
 * FILE: mpool.c
 *
 * Fixed-size block memory pool
 *
 * Block structure and free blocks list:
 *
 *     --------       --------
 *    |  NEXT  | --> |  NEXT  |--->
 *     --------       --------
 *    |        |     |        |
 *    |  BLK   |     |  BLK   |
 *    |        |     |        |
 *     --------       --------
 *
 * Author: Jordi Sánchez, <jorsanp3@upvnet.upv.es>
 */

#include <mpool.h>
#include <stddef.h>

#define CONTAINER_OF(P) ((P) - sizeof(struct mpool_block *))

/*
 * mpfree()
 *
 *    Free a block from a memory pool
 */
void mpfree(void *pool, void *blk)
{
   struct mpool_block *b = CONTAINER_OF(blk);
   struct mpool *p = pool;

   b->next = p->free;
   p->free = b;
}

/*
 * mpalloc()
 *
 *    Allocate a block from a memory pool
 */
void *mpalloc(void *pool)
{
   struct mpool_block *blk;
   struct mpool *p = pool;

   if (p->free == NULL) {
      if (p->freeidx == p->poolend) {
         return NULL;
      }
      blk = p->freeidx;
      p->freeidx += p->blksz + sizeof(struct mpool_block *);
   } else {
      blk = p->free;
      p->free = p->free->next;
   }

   blk->next = NULL;

   return (void *)blk->data;
}

/*
 * init_pool()
 *
 *    Initialize a memory pool
 */
void init_pool(void *base, int blksz, int blkcnt)
{
   struct mpool *pool = base;

   pool->blksz = blksz;
   pool->blkcnt = blkcnt;
   pool->free = NULL;
   pool->freeidx = (struct mpool_block *)pool->base;
   pool->poolend = pool->freeidx + pool->blkcnt * (pool->blksz + sizeof(struct mpool_block *));
}



