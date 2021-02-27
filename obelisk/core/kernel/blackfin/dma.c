/*
 * $FILE: dma.c
 *
 * DMA engine manager for Blackfin
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

#include <bitmap.h>
#include <processor.h>
#include <types.h>
#include <arch/dma.h>

/*
 * DMA Channels
 */
static const struct dma_channel *dma_channels[DMA_CHANNELS] = {
      [0] = 0xFFC00C00,
      [1] = 0xFFC00C40,
      [2] = 0xFFC00C80,
      [3] = 0xFFC00CC0,
      [4] = 0xFFC00D00,
      [5] = 0xFFC00D40,
      [6] = 0xFFC00D80,
      [7] = 0xFFC00DC0,
      [8] = 0xFFC00E00,
      [9] = 0xFFC00E40,
      [10] = 0xFFC00E80,
      [11] = 0xFFC00EC0,
      [12] = 0xFFC01C00,
      [13] = 0xFFC01C40,
      [14] = 0xFFC01C80,
      [15] = 0xFFC01CC0,
      [16] = 0xFFC01D00,
      [17] = 0xFFC01D40,
      [18] = 0xFFC01D80,
      [19] = 0xFFC01DC0,
      [20] = 0xFFC01E00,
      [21] = 0xFFC01E40,
      [22] = 0xFFC01E80,
      [23] = 0xFFC01EC0,
};
static bitmap_t dma_channel_map;

int32_t dma_allocate_channel(void)
{
   int32_t nchan;

   // Search for a free channel
   nchan = _ffc(dma_channel_map);
   if (nchan > DMA_CHANNELS || nchan < 0) {
      return -1;
   }
   dma_channel_map = setbit(dma_channel_map, nchan);

   return nchan;
}

int32_t dma_launch_transfer(int32_t ch, struct dma_descriptor *desc)
{
   // Parameter check
   if (ch > DMA_CHANNELS || ch < 0 || !isbitset(dma_channel_map, ch)) {
      return -1;
   }

   dma_channels[ch]->next_desc_ptr = desc;
   dma_channels[ch]->config |= 0x7901;
}

int32_t dma_ack_int(int32_t ch)
{
   // Parameter check
   if (ch > DMA_CHANNELS || ch < 0 || !isbitset(dma_channel_map, ch)) {
      return -1;
   }

   dma_channels[ch]->config = 0;
   dma_channels[ch]->irq_status = DMA_DONE;

   return 0;
}


/*
 * MDMA Channels
 */
const struct dma_channel *mdma_channels[MDMA_CHANNELS][2] = {
      [0] = {0xFFC00F00, 0xFFC01F00},
      [1] = {0xFFC00F40, 0xFFC01F40},
      [2] = {0xFFC00F80, 0xFFC01F80},
      [3] = {0xFFC00FC0, 0xFFC01FC0},
};
static bitmap_t mdma_channel_map;

int32_t mdma_allocate_channel(void)
{
   int32_t nchan;

   // Search for a free channel
   nchan = _ffc(mdma_channel_map);
   if (nchan > MDMA_CHANNELS || nchan < 0) {
      return NULL;
   }
   mdma_channel_map = setbit(mdma_channel_map, nchan);

   return nchan;
}

int32_t mdma_launch_transfer(int32_t ch, struct dma_descriptor *src, struct dma_descriptor *dest)
{
   // Parameter check
   if (ch > MDMA_CHANNELS || ch < 0 || !isbitset(mdma_channel_map, ch)) {
      return -1;
   }

   dma_channels[ch][0]->next_desc_ptr = src;
   dma_channels[ch][1]->next_desc_ptr = dest;
   dma_channels[ch][0]->config |= 0x7901;
   dma_channels[ch][1]->config |= 0x7901;
}

int32_t mdma_ack_int(int32_t ch)
{
   // Parameter check
   if (ch > MDMA_CHANNELS || ch < 0 || !isbitset(mdma_channel_map, ch)) {
      return -1;
   }

   dma_channels[ch][0]->config = 0;
   dma_channels[ch][1]->config = 0;
   dma_channels[ch][0]->irq_status = DMA_DONE;
   dma_channels[ch][1]->irq_status = DMA_DONE;

   return 0;
}

void mdma_release_channel(uint32_t channel)
{
   mdma_channel_map = clearbit(mdma_channel_map, channel);
}

/*
uint32_t mdma_config_transfer(uint32_t channel, void *src, void *dst, uint32_t count)
{
   uint16_t dmacfg;

   // Parameter check
   if (channel > 3 || channel < 0 || !isbitset(mdma_channel_map, channel)) {
      return -1;
   }

   bfin_write32(MDMA_S_START_ADDR(channel), src);
   bfin_write32(MDMA_D_START_ADDR(channel), dst);

   // DMA config
   if (count & 0x1) {         // DMA 8 bit
      dmacfg = WDSIZE_8;
   } else if (count & 0x10) { // DMA 16 bit
      dmacfg = WDSIZE_16;
      count >>= 1;
   } else {                   // DMA 32 bit
      dmacfg = WDSIZE_32;
      count >>= 2;
   }
   bfin_write32(MDMA_S_X_COUNT(channel), count);
   bfin_write32(MDMA_D_X_COUNT(channel), count);
   bfin_write16(MDMA_S_X_MODIFY(channel), dmacfg>>1);
   bfin_write16(MDMA_D_X_MODIFY(channel), dmacfg>>1);
   bfin_write16(MDMA_S_CONFIG(channel), dmacfg);
   bfin_write16(MDMA_D_CONFIG(channel), dmacfg | WNR);

   return 0;
}

uint32_t mdma_relocate_transfer(uint32_t channel, void *src, void *dst)
{
   // Parameter check
   if (channel > 3 || channel < 0 || !isbitset(mdma_channel_map, channel)) {
      return -1;
   }

   bfin_write32(MDMA_S_START_ADDR(channel), src);
   bfin_write32(MDMA_D_START_ADDR(channel), dst);

   return 0;
}

uint32_t mdma_start_transfer(uint32_t channel)
{
   // Parameter check
   if (channel > 3 || channel < 0 || !isbitset(mdma_channel_map, channel)) {
      return -1;
   }

   bfin_write16(MDMA_D_CONFIG(channel), bfin_read16(MDMA_D_CONFIG(channel)) | DMAEN);
   bfin_write16(MDMA_S_CONFIG(channel), bfin_read16(MDMA_S_CONFIG(channel)) | DMAEN);

   return 0;
}

uint32_t mdma_wait_transfer(uint32_t channel, uint32_t tmo)
{
   // Parameter check
   if (channel > 3 || channel < 0 || !isbitset(mdma_channel_map, channel)) {
      return -1;
   }

   while (--tmo) {
      if (bfin_read16(MDMA_D_IRQ_STATUS(channel)) & DMA_DONE) {
         // Ack interrupts
         bfin_write16(MDMA_S_IRQ_STATUS(channel), DMA_DONE);
         bfin_write16(MDMA_D_IRQ_STATUS(channel), DMA_DONE);
         // Disable MDMAs
         bfin_write16(MDMA_D_CONFIG(channel), bfin_read16(MDMA_D_CONFIG(channel)) & ~DMAEN);
         bfin_write16(MDMA_S_CONFIG(channel), bfin_read16(MDMA_S_CONFIG(channel)) & ~DMAEN);
         return 0;
      }
   }

   return -1;
}

void mdma_ack_interrupt(uint32_t channel)
{
   uint32_t dmareg;

   // Disable DMAs
   dmareg = MDMA_D_CONFIG(channel);
   bfin_write16(dmareg, 0);
   dmareg = MDMA_S_CONFIG(channel);
   bfin_write16(dmareg, 0);

   // Ack interrupt
   // Disable DMAs
   dmareg = MDMA_D_IRQ_STATUS(channel);
   bfin_write16(dmareg, 0);
   dmareg = MDMA_S_IRQ_STATUS(channel);
   bfin_write16(dmareg, 0);
}*/

