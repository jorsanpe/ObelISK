/*
 * FILE: mpool.h
 *
 * Fixed-size block memory pool
 *
 * Author: Jordi Sánchez, <jorsanp3@upvnet.upv.es>
 */
 
#ifndef _MPOOL_H_
#define _MPOOL_H_

#define POOL_SIZE(BLKSZ, BLKCNT) (sizeof(struct mpool) + (BLKCNT)*(BLKSZ + sizeof(struct mpool_block *)))

struct mpool_block {
   struct mpool_block *next;
   char data[];
};

struct mpool {
   int blksz;                    // Pool block size
   int blkcnt;                   // Number of blocks of the pool
   void *poolend;                // End of the pool

   struct mpool_block *free;     // Free blocks list
   struct mpool_block *freeidx;  // Free pool index
   char base[];                  // Start of the pool
};

void *mpalloc(void *pool);
void mpfree(void *pool, void *blk);
void init_pool(void *base, int blksz, int blkcnt);

#endif //_MPOOL_H_

