/*
 * $FILE: transport.h
 *
 * Transport ring for the LAN9218 packets on CM-BF548
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

#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

#include <lwipopts.h>
#include <arch/dma.h>
#include <mpool.h>
#include <queue.h>

#define __tring __attribute__((section(".tring")))

#define LWIP_DPOOL_SIZE   PBUF_POOL_SIZE // lwIP Descriptor pool size. Must be power of 2
#define DMA_POOL_SIZE     (2*(LWIP_DPOOL_SIZE))
#define MAX_REQUESTS      LWIP_DPOOL_SIZE

#define TRING_CHAIN_RX    1              // Input chain. Else, output.

struct tring_request {
   uint32_t flags;             // Receive of transmit
   struct dma_descriptor *src; // DMA source descriptors
   struct dma_descriptor *dst; // DMA destination descriptors
   struct pbuf *pbuf;          // Involved pbuf chain
   linked_list_t tring_list;   // Tring request queue
};

extern struct mpool *eth_dma_desc_pool;
extern struct mpool *eth_request_pool;
extern queue_t eth_request_queue;      // Ethernet request queue
extern int32_t eth_dma_channel;        // Ethernet DMA channel
extern struct tring_request *eth_active_request;

static inline void clear_request(struct tring_request *__r)
{
   struct dma_descriptor *__d;
   for (__d=__r->src; __d!=NULL; __d=__d->next) {
      mpfree(eth_dma_desc_pool, __d);
   }
   for (__d=__r->dst; __d!=NULL; __d=__d->next) {
      mpfree(eth_dma_desc_pool, __d);
   }
   mpfree(eth_request_pool, __r);
}

#endif //_TRANSPORT_H_

