/*
 * $FILE: can.c
 *
 * ObelISK Logical CAN Device Driver
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

//Interface Layer

#include <camera.h>
#include <device.h>
#include <init.h>
#include <itron.h>
#include <processor.h>

DECLARE_LOGDEV(can);

static int32_t do_request(physdev_t *pdev, request_t *req)
{
   pdev->ops->hw_read();
}

static void callback(physdev_t *pdev, request_t *req)
{
   dev_reqdone(reqid, size, stat);
}

/**
 * open_can()
 */
static int32_t open_can(physdev_t *pdev, int32_t omode)
{
   pdev->ops->hw_open(omode);
}

/**
 * add_request()
 *
 *    New request for CAN communication
 *
 * NOTE 1
 * Writes are directly transmitted to the physical driver
 * Reads are asynchronous. If the device is busy (a request is pending),
 * we enqueue the read, otherwise we send the request to the device
 *
 * NOTE 2
 * On CAN the size of the requests is ignored
 */
static int32_t add_request(physdev_t *pdev, request_t *req)
{
   logdev_t *ldev = pdev->parent;

	// 
   if (req->flags & REQ_READ) {
		__hw_cli();
      if (pdev->stat & DEV_BUSY) {
         req->list.this = req;
         queue_insert(&ldev->request_queue, req->list);
      } else {
         pdev->ops->hw_read(req->buf, 0);
      }
		__hw_sti();
   } else if (req->flags & REQ_WRITE) {
      pdev->ops->hw_write(req->buf, 0);
      req->status = REQ_COMPLETE;
   }

   return 0;
}

static struct logdev_ops can_ops = {
      .open = open_can,
      .add_request = add_request,
      .callback = callback,
};

int32_t init_can(void)
{
   // Set device logical type
   strcpy(dev->name, "can");

   can.devatr = 0;
   can.ocnt = 0;
   can.devid = 0;
   can.ops = can_ops;
   can.insts = NULL;

   return 0;
}

logdev_initcall(init_can);

