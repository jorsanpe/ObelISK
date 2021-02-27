/*
 * $FILE: ov7660.c
 *
 * ov7660 camera driver
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

#include <dma.h>
#include <ov7660.h>
#include <processor.h>
#include <time.h>

DECLARE_DEVICE(ov7660);

#define SIOC_SET_OUTPUT()  bfin_write_PORTF_DIR_SET(OV7660_SIOC_PIN);
#define SIOD_SET_OUTPUT()  bfin_write_PORTF_DIR_SET(OV7660_SIOD_PIN);
#define PWDN_SET_OUTPUT()  bfin_write_PORTF_DIR_SET(OV7660_PWDN_PIN);
#define SIOC_SET_INPUT()   bfin_write_PORTF_DIR_CLEAR(OV7660_SIOC_PIN);
#define SIOD_SET_INPUT()   bfin_write_PORTF_DIR_CLEAR(OV7660_SIOD_PIN);
#define PWDN_SET_INPUT()   bfin_write_PORTF_DIR_CLEAR(OV7660_PWDN_PIN);

#define SIOD_SET()         bfin_write_PORTF_SET(OV7660_SIOD_PIN)
#define SIOD_CLEAR()       bfin_write_PORTF_CLEAR(OV7660_SIOD_PIN)
#define SIOC_SET()         bfin_write_PORTF_SET(OV7660_SIOC_PIN)
#define SIOC_CLEAR()       bfin_write_PORTF_CLEAR(OV7660_SIOC_PIN)
#define PWDN_SET()         bfin_write_PORTF_SET(OV7660_PWDN_PIN)
#define PWDN_CLEAR()       bfin_write_PORTF_CLEAR(OV7660_PWDN_PIN)

#define SIOD_READ()        ((bfin_read_PORTF() & OV7660_SIOD_PIN == 0) ? 0 : 1)

static uint16_t config_table[] = {
   #include <ov7660cfg.h>
};

/*
 * clear_lines()
 *
 *    Called at the beggining of a transmission
 */
static inline void clear_lines(void)
{
   SIOD_CLEAR();
   usleep(100);
   SIOC_CLEAR();
}

/*
 * clock_pulse()
 *                                     ,200 us,
 *    Standard SCCB clock pulse ->  ___|^^^^^^|___
 */
static inline void clock_pulse(void)
{
   usleep(100);
   SIOC_SET();
   usleep(200);
   SIOC_CLEAR();
   usleep(100);
}

/*
 * stop_transmission()
 *
 *    Signal a transmission stop
 */
static inline void stop_transmission(int time)
{
   SIOD_CLEAR();
   usleep(100);
   SIOC_SET();
   usleep(100);
   SIOD_SET();
   usleep(time);
}

/*
 * write_byte()
 *
 *    Send a byte over the bus
 */
static inline void write_byte(unsigned char byte)
{
   uint8_t i;

   SIOD_SET_OUTPUT();
   for (i = 0; i < 8; i++) {
      SIOD_WRITE(byte & (0x0080 >> i));
      clock_pulse();
   }
   // Ack
   SIOD_SET_INPUT();
   clock_pulse();
   SIOD_SET_OUTPUT();
}

/*
 * read_byte()
 *
 *    Read a byte from the bus
 */
static inline unsigned char read_byte(void)
{
   uint8_t rb;
   int8_t i;

   rb = 0;

   SIOD_SET_INPUT();

   usleep(100);
   SIOC_SET();
   usleep(200);

   for (i=7; i >= 0; i--) {
      SIOC_CLEAR();
      usleep(200);
      SIOC_SET();
      usleep(100);

      rb = rb | (SIOD_READ() << i);
   }
   SIOC_SET();
   usleep(100);

   SIOD_SET_OUTPUT();

   return rb;
}

/*
 * ov7660_write()
 *
 *    Procedure to write a camera register
 */
static void ov7660_write_reg(unsigned char address, unsigned char byte)
{
   // Clear interrupts to ensure atomic operation. Otherwise,
   __hw_cli();

   clear_lines();

   // Writing ov7660 address
   write_byte(OV7660_ADDRESS);

   // Writing register address
   write_byte(address);

	// Writing data
	write_byte(byte);

	__hw_sti();

	// Signal end of transmission
	stop_transmission(2000);
}

/*
 * ov7660_read_reg()
 *
 *    Procedure to read a camera register
 */
static uint16_t ov7660_read_reg(unsigned char address)
{
   uint16_t byte;

   clear_lines();

   // Writing ov7660 address
   write_byte(OV7660_ADDRESS);

   // Writing register address
   write_byte(address);

   // Stop of transmission
   stop_transmission(500);

   // Phase 2
   clear_lines();

   // Writing register address
   write_byte(OV7660_ADDRESS | 1);

   // Read byte
   byte = read_byte();

   // Send Ack
   SIOD_SET();
   clock_pulse();

   // Signal end of transmission
   stop_transmission(2000);

   return byte;
}

/*
 * ov7660_reset()
 *
 *    Reset camera registers
 */
static void ov7660_reset(void)
{
   ov7660_write(0x12, 0x80);
   usleep(5000);
}

int32_t ov7660_open(void)
{

}

void ov7660_close(void)
{
}

int32_t ov7660_capture_frame(char *buf)
{

}

int32_t ov7660_add_request(device_t *dev, request_t *req)
{
   if (req->flags & REQ_READ) {
      if (dev->stat & DEV_BUSY) {
         req->list.this = req;

         __hw_cli();
         queue_insert(&dev->request_queue, req->list);
         __hw_sti();
      } else {
         // Initiate frame transfer
         ov7660_capture_frame(req->buf);
      }
   } else if (req->flags & REQ_WRITE) {
      // Write configuration register


      req->status = REQ_COMPLETE;
   }

   return 0;
}

void ov7660_callback(device_t *dev, request_t *req)
{

}

struct dev_ops ov7660_ops = {
   .open = ov7660_open,
   .close = ov7660_close,
   .add_request = ov7660_add_request,
	.callback = ov7660_callback,
};

/*
 * ov7660_init()
 */
int32_t ov7660_init(void)
{
   uint32_t idx;
   uint8_t address, value;

	// Set device name
	strcpy(ov7660.name, "ov7660");

	// Initialize camera with the specific configuration selected by the user
   idx = 0;
   while ((config_table[idx]!=0xffff) && (idx < sizeof(config_table))) {
      address = (uint8_t) ((config_table[idx] >> 8) & 0x00ff);
      value   = (uint8_t) ((config_table[idx]) & 0x00ff);
      ov7660_write_reg(address, value);
      idx++;
   }

	// Initialize camera signals
   // SIO_C, SIO_D and PWDN as output
   SIOC_SET_OUTPUT();
   SIOD_SET_OUTPUT();
   PWDN_SET_OUTPUT();

   SIOC_SET();
   SIOD_SET();
   PWDN_WRITE(0);

   usleep(800);

   ov7660_reset();
}


device_initcall(ov7660_init);

