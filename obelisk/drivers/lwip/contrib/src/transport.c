/*
 * FILE: transport.c
 *
 * Ethernet transport ring
 *
 * Author: Jordi Sánchez, <jorsanp3@upvnet.upv.es>
 */

#include <transport.h>
#include <arch/dma.h>



__tring static struct tring_request dmapool[DMA_POOL_SIZE];
__tring static uint32_t reqfreeidx;
__tring static struct tring_request *reqfreelist;

__tring static struct dma_descriptor dmapool[DMA_POOL_SIZE];
__tring static uint32_t dmafreeidx;
__tring static struct dma_descriptor *dmafreelist;

struct dma_descriptor *alloc_desc(void)
{
   struct dma_descriptor *desc;

   if (dmafreelist == NULL) {
      if (dmafreeidx > DMA_POOL_SIZE) {
         return NULL;
      }
      desc = &dmapool[dmafreeidx++];
   } else {
      desc = dmafreelist;
      dmafreelist = dmafreelist->next;
   }

   desc->next = NULL;
   return desc;
}

void free_desc(struct dma_descriptor *desc)
{
   desc->next = dmafreelist;
   dmafreelist = desc;
}

void init_transport(void)
{
   dmafreelist = NULL;
   dmafreeidx = 0;
}

