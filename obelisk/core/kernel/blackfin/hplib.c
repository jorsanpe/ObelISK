/*
 * $FILE: hplib.c
 *
 * High performance library using DMA transfers
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

#ifdef CONFIG_HIGH_PERFORMANCE_LIBRARY

#include <types.h>
#include <hplib.h>

#define HP_L1_BLOCK_SIZE   0x1000
#define HP_MODE_2D         0x1
#define SWAP_BLOCK(BLK)    ((BLK) = 1 - (BLK))

struct hp_param_config {
   void (*algorithm)(void *, void *, uint32_t);
   void *src, *dst;
   uint32_t length;
   uint32_t mode;

   // Only for 2D Modes
   uint32_t winw, winh; // Window width and height
   uint32_t imgw, imgh; // Image width and height
};

/*static void (*algorithm)(void *, void *, uint32_t);
static void *hp_src, *hp_dst;
static uint32_t hp_length, hp_width, hp_height, hp_mode;*/

static struct hp_param_config local_config;
static uint32_t inbank[2] = {0xFF900000, 0xFF901000};
static uint32_t outbank[2] = {0xFF902000, 0xFF903000};

/*
 * hp_config()
 *
 *    Config the high performance system
 */
void hp_config(struct hp_param_config * cfg)//uint32_t mode, void *in, void *out, uint32_t bytes, void (*alg)(void *, void *, uint32_t))
{
   // 1D or 2D algorithm configuration
   local_config.src = cfg->src;
   local_config.dst = cfg->dst;
   local_config.length = cfg->length;
   local_config.algorithm = cfg->algorithm;
   local_config.width = cfg->width;
   local_config.height = cfg->height;
}

/*
 * hp_run()
 *
 *    Run the configured high performance algorithm
 */
void hp_run(void)
{
   uint32_t inch, outch;
   uint32_t inidx, outidx, blkidx;

   blkidx = inidx = outidx = 0;

   inch = mdma_allocate_channel();
   if (mdma_config_transfer(inch, hp_src, inbank[blkidx], hp_length) < 0) {
      return;
   }
   outch = mdma_allocate_channel();
   if (mdma_config_transfer(outch, outbank[blkidx], hp_dst, hp_length) < 0) {
      return;
   }

   // Initialization step
   mdma_start_transfer(inch);
   mdma_wait_transfer(inch, hp_length);

   SWAP_BLOCK(blkidx);
   inidx += HP_L1_BLOCK_SIZE;
   mdma_relocate_transfer(inch, hp_src + inidx, inbank[blkidx]);
   mdma_start_transfer(inch);
   algorithm(inbank[1-blkidx], outbank[1-blkidx], HP_L1_BLOCK_SIZE);
   mdma_wait_transfer(inch, hp_length);


   // Main Loop
   while (inidx < hp_length) {
      SWAP_BLOCK(blkidx);
      inidx += HP_L1_BLOCK_SIZE;
      mdma_relocate_transfer(inch, hp_src + inidx, inbank[blkidx]);
      mdma_relocate_transfer(outch, outbank[blkidx], hp_dst + outidx);

      mdma_start_transfer(inch);
      mdma_start_transfer(outch);
      algorithm(inbank[1-blkidx], outbank[1-blkidx], HP_L1_BLOCK_SIZE);  // Algorithm processing
      mdma_wait_transfer(inch, hp_length);
      mdma_wait_transfer(outch, hp_length);

      outidx += HP_L1_BLOCK_SIZE;
   }


   // Finalization step
   SWAP_BLOCK(blkidx);
   mdma_relocate_transfer(outch, outbank[blkidx], hp_dst + outidx);

   mdma_start_transfer(outch);  // DMA output transfer
   algorithm(inbank[1-blkidx], outbank[1-blkidx], HP_L1_BLOCK_SIZE);  // Algorithm processing
   mdma_wait_transfer(outch, hp_length);

   outidx += HP_L1_BLOCK_SIZE;
   SWAP_BLOCK(blkidx);
   mdma_relocate_transfer(outch, outbank[blkidx], hp_dst + outidx);
   mdma_start_transfer(outch);  // DMA output transfer
   mdma_wait_transfer(outch, hp_length);
}

#endif


