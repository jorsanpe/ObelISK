/*
 * $FILE: camera.c
 *
 * ObelISK Camera Device Driver for BF548
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

#include <camera.h>
#include <device.h>
#include <init.h>
#include <itron.h>
#include <processor.h>

DECLARE_LOGDEV(camera);

static void int_handler(void)
{
   uint16_t tmpreg;

   // Ack interrupt
   bfin_write_DMA12_IRQ_STATUS(0x0001);

   // Disable EPPI0
   tmpreg = bfin_read_EPPI0_CONTROL();
   tmpreg &= 0xfffe;
   bfin_write_EPPI0_CONTROL(tmpreg);

   // Disable DMA
   tmpreg = bfin_read_DMA12_CONFIG();
   tmpreg &= 0xfffe;
   bfin_write_DMA12_CONFIG(tmpreg);

   // Signal arrival of new frame
   sig_sem_sys(frame_sem);
}

void config_dma(int32_t xcount, int32_t ycount, int32_t xmod, int32_t ymod, int32_t color)
{
   bfin_write_DMA12_X_COUNT(xcount);
   bfin_write_DMA12_Y_COUNT(ycount);
   bfin_write_DMA12_X_MODIFY(xmod);
   bfin_write_DMA12_Y_MODIFY(ymod);

   if (color) {
      bfin_write_DMA12_CONFIG(0x0092 | WDSIZE_32);
   } else {
      bfin_write_DMA12_CONFIG(0x0092);
   }
}

void config_ppi(int32_t lines, int32_t color)
{
	bfin_write_EPPI0_FRAME(lines);
	bfin_write_EPPI0_CONTROL(FLD_SEL);
	if (color) {
		bfin_write_EPPI0_CONTROL(PACKEN | FLD_SEL);
	} else {
		bfin_write_EPPI0_CONTROL(SKIP_EN | SKIP_EO | FLD_SEL);
	}
}

void config_ppi_dma(int32_t mode, int32_t col)
{
   int32_t ppi_lines;

   __hw_cli();
   if (mode == OV7660_CCIR656_QVGA_YUV_15FPS) { // 320x240
      ppi_lines = 240;
      if (col == COLOR) { // Color
         config_dma(160, 240, 4, 4, COLOR);

      } else { // Gray. Stored in short type
         config_dma(320, 240, 1, 1, GRAY);
      }
   }

   if (mode == OV7660_CCIR656_VGA_YUV_15FPS) { // 640x480
      ppi_lines = 480;
      if (col == COLOR) { // Color
         config_dma(320, 480, 4, 4, COLOR);
      } else { // Gray. Stored in short type
         config_dma(640, 480, 1, 1, GRAY);
      }
   }
   config_ppi(ppi_lines, col);

   __hw_sti();
}

void read_frame(unsigned char *pframe)
{
   __hw_cli();

   //config_ppi(PpiLines, Color);

   bfin_write_DMA12_START_ADDR(pframe);
   bfin_write_DMA12_CONFIG(bfin_read_DMA12_CONFIG() | 0x0001);
   bfin_write_EPPI0_CONTROL(bfin_read_EPPI0_CONTROL() | 0x0001);

   __hw_sti();
}

static int32_t do_request(physdev_t *pdev, request_t *req)
{
   pdev->ops->hw_read();
}

static void callback(physdev_t *pdev, request_t *req)
{
   dev_reqdone(reqid, size, stat);
}


/**
 * add_request()
 *
 *    On the camera the request is a new frame capture
 */
static int32_t add_request(physdev_t *pdev, request_t *req)
{
	logdev_t *ldev = pdev->parent;

	if (req->flags & REQ_READ) {
		if (pdev->stat & DEV_BUSY) {
			req->list.this = req;

			__hw_cli();
			queue_insert(&ldev->request_queue, req->list);
			__hw_sti();
		} else {
			pdev->ops->hw_read(req->buf, 0);
		}
	} else if (req->flags & REQ_WRITE) {
		// A write request to the camera should be a configuration message
		pdev->ops->hw_write(req->buf, 0);
		req->status = REQ_COMPLETE;
	}

	return 0;
}


int32_t open_camera(void)
{
	return 0;
}

static struct logdev_ops cam_ops = {
      .open = open_camera,
      .add_request = add_request,
      .callback = callback,
};

int32_t init_camera(void)
{
   // Set device logical type
   strncpy(camera.name, "camera", L_DEVNM);

   camera.devid = 0;
   camera.devatr = 0;
   camera.ocnt = 0;

   camera.ops = cam_ops;
   camera.insts = NULL;
}

logdev_initcall(init_camera);

/*
int32_t init_camera(void)
{
   T_DINH pk_dinh;
   extern void (*led_handler)(void);
   uint32_t tmpreg;

   ov7660_setup();

   config_ppi_dma(mode, col);

   // Interrupt handler
   pk_dinh.inthdr = led_handler;
   pk_dinh.inhatr = 0;
   def_inh_sys(9, &pk_dinh);

   // System interrupt assignment register
   tmpreg = (bfin_read_SIC_IAR1() & 0xfffffff0) | 0x00000002;
   bfin_write_SIC_IAR1(tmpreg);

   // Sistem interrupt mask register
   tmpreg = bfin_read_SIC_IMASK0() | (1 << 8);
   bfin_write_SIC_IMASK0(tmpreg);
}*/
