/*
 * $FILE: bfcan.c
 *
 * Blacfin CAN Device Driver
 *
 * CANx_TIMING - SJW, TSEG2, and TSEG1 governed by:
 * SJW <= TSEG2 <= TSEG1
 *
 * CANx_CLOCK - Calculate Prescaler (BRP)
 *
 * tBIT = TQ x (1 + (TSEG1 + 1) + (TSEG2 + 1))
 *
 * Once time quantum (TQ) is known, BRP can be derived
 * from the TQ formula in the HRM. 
 *
 * TQ = (BRP + 1) / SCLK
 * BRP = (TQ * SCLK) - 1
 * 
 * Author: Jordi Sánchez, <jorsanp3@upvnet.upv.es>
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

//Physical Layer
#include <bitmap.h>
#include <cdefBF548.h>
#include <device.h>
#include <dma.h>
#include <types.h>

DECLARE_DEVICE(bfcan);

#define BFCAN_DED_MBX   8
#define CANRATE         500000
#define SJW             3
#define TSEG1           4
#define TSEG2           3
#define INVTQ           (CANRATE*(1 + (TSEG1+1) + (TSEG2+1)))
#define BRP             ((CONFIG_SCLK / INVTQ) - 1)

struct bfcan_mailbox {
};
static bitmap_t bfcan_mbxmap;

// Allocate a memory chunk for buffering. 
static struct can_message msgpool[BFCAN_MESSAGE_POOL_SIZE];
static int32_t msgidx;

/*
 * bfcan_int_handler()
 *
 *    Interrupt handler
 */
void bfcan_int_handler(void)
{
   // Critical region is protected at user level disabling interrupts
   bfin_write_CAN0_MBRMP1();
}

// Each Mailbox can be considered as a CAN channel
int32_t bfcan_alloc_rxmbx(void)
{
	uint16_t mbx;

	// Search for a free mailbox channel
	mbx = _ffc(bfcan_mbxmap);

	if (mbx > BFCAN_DED_MBX || mbx < 0) {
      // We have to allocate a configurable channel
      // For now, we only allow 8 rx channels opened
		return -1;
	}

   bfcan_mbxmap = setbit(bfcan_mbxmap, mbx);
	return mbx;
}

// Mailboxes 24-31 are dedicated transmitters
int32_t bfcan_alloc_txmbx(void)
{
	uint16_t mbx;

	// Search for a free transmit mailbox
	mbx = _ffc(bfcan_mbxmap >> 24);

	if (mbx > BFCAN_DED_MBX || mbx < 0) {
      // We have to allocate a configurable channel
      // For now, we only allow 8 tx channels
		return -1;
	}

   bfcan_mbxmap = setbit(bfcan_mbxmap, mbx);
	return mbx;
}

int32_t bfcan_open(int32_t omode)
{
/*   int32_t mbx;

   if (omode == TD_READ) {
      // Allocate a new mailbox for Receive
      mbx = bfin_alloc_rxmbx();

      // No mailboxes available
      if (mbx < 0) {
         return -1;
      }
   } else if (omode == TD_WRITE) {
      // Allocate a new mailbox for Transmit
      mbx = bfin_alloc_txmbx();

      // No mailboxes available
      if (mbx < 0) {
         return -1;
      }
   }

   return mbx;
*/
   return 0;
}

void bfcan_write(uint8_t *buf, uint32_t size)
{

}

void bfcan_config(void *data)
{
	
}

/*
 * Reads are always called on protected regions
 */
void bfcan_read(uint8_t *buf, uint32_t size)
{
	canmsg_t *msg = (canmsg_t *)buf;

	// buf contains the CAN message where the message should be stored
   msg->id;
   msg->data;
}

int32_t bfcan_add_request(device_t *dev, request_t *req)
{

}

struct dev_ops bfcan_ops = {
   .open = bfcan_open,
   .close = bfcan_close,
   .add_request = bfcan_add_request,
};

int32_t bfcan_init(void)
{
   // Set device name and attributes
   //strcpy(bfcan.name, "bfcan");
   bfcan.devatr = 0;
   bfcan.ocnt = 0;
   bfcan.devid = 0;
   bfcan.ops = bfcan_ops;
   bfcan.insts = NULL;

   // Set pin function as CAN
   bfin_write_PORTG_FER(0x3000);
   asm("ssync;");

   // Configure CAN timing registers
   bfin_write_CAN0_TIMING(0x0334);
   asm("ssync;");
   bfin_write_CAN0_CLOCK(BRP);
   asm("ssync;");
	
   // NOTE:
	// Initialize CAN mailboxes. What should the driver do
	// with CAN? At least one mailbox should be initialized
	// in order to receive messages? Or maybe the system should
	// not receive any message until the user has chosen the
	// configuration and correct filters
   // For now CAN uses Mailbox 0 for Receive operations and 
   // Mailbox 24 for Transmit operations
	
}

device_initcall(bfcan_init);

