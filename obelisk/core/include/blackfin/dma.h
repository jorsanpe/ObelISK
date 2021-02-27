/*
 * $FILE: dma.h
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

#ifndef _DMA_H_
#define _DMA_H_

#ifndef __ASSEMBLY__

#define DMA_CHANNELS    24
#define MDMA_CHANNELS    4

struct dma_descriptor
{
   struct dma_descriptor *next;
   void *start_addr;
   uint16_t config;
   uint16_t x_count;
   uint16_t x_modify;
   uint16_t y_count;
   uint16_t y_modify;
};

// This is provided with the Blackfin processor documentation
struct dma_channel
{
   void *next_desc_ptr;    //0x00
   void *start_addr;       //0x04
   uint16_t config;        //0x08
   uint16_t res1;
   uint32_t res2;          //0x0C
   uint16_t x_count;       //0x10
   uint16_t res3;
   uint16_t x_modify;      //0x14
   uint16_t res4;
   uint16_t y_count;       //0x18
   uint16_t res5;
   uint16_t y_modify;      //0x1C
   uint16_t res6;
   void *curr_desc_ptr;    //0x20
   void *curr_addr;        //0x24
   uint16_t irq_status;    //0x28
   uint16_t res7;
   uint16_t peripheral_map;//0x2C
   uint16_t res8;
   uint16_t curr_x_count;  //0x30
   uint16_t res9;
   uint32_t res10;         //0x34
   uint16_t curr_y_count;  //0x38
};

extern int32_t dma_allocate_channel(void);
extern int32_t dma_launch_transfer(int32_t ch, struct dma_descriptor *desc);
extern int32_t dma_ack_int(int32_t ch);

extern int32_t mdma_allocate_channel(void);
extern int32_t mdma_launch_transfer(int32_t ch, struct dma_descriptor *src, struct dma_descriptor *dest);
extern int32_t mdma_ack_int(int32_t ch);

#endif

#endif //_DMA_H_


