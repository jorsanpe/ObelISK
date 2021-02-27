/*
 * $FILE: template.c
 *
 * ObelISK Device Driver implementation template
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
 *
 */


#include <camera.h>
#include <device.h>
#include <init.h>
#include <itron.h>
#include <processor.h>

/**
 * DECLARE_DEVICE allocates room for a device_t structure, which will hold
 * relevant information about the driver. This is simply a static global variable
 * to hold the structure. Multiple devices of the same device may be declared
 * with
 *    DECLARE_DEVICE(device[n]);
 *
 *
 */
DECLARE_DEVICE(device);

/*
 * Function to call when the device is opened
 */
static int32_t open_device(void)
{
}

/*
 * Function to call when the device is closed
 */
static int32_t close_device(void)
{
}

/*
 * The kernel communicates with the devices by means of requests. A
 * request consists of a buffer associated with a specific instance
 * of a given device and, thus, with a specific task. This request
 * has to be processed or queued if the driver is busy.
 */
static int32_t add_request(device_t *dev, request_t *req)
{
}

static struct device_ops dev_ops = {
      .open = open_device,
      .add_request = add_request,
};

int32_t init_device(void)
{
   strcpy(device.name, "device");

   device.devatr = 0;
   device.ocnt = 0;
   device.devid = 0;
   device.ops = dev_ops;
   device.insts = NULL;

   return 0;
}

__define_initcall(init_device);


