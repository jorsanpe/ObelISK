/*
 * $FILE: device.c
 *
 * ObelISK Device Driver Engine
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
#include <device.h>
#include <itron.h>
#include <mempool.h>
#include <mpool.h>
#include <processor.h>
#include <string.h>
#include <task.h>
#include <tlsf.h>

// Device management globals
extern device_t __device_pool[], __devices_end[];
__kobject static bitmap_t devbmap;
__kobject static device_instance_t devins_pool[CONFIG_MAX_DEVICES];

// Request management globals
__kobject static request_t request_pool[CONFIG_MAX_DEVICE_REQUESTS];
__kobject static int16_t free_req_pool[CONFIG_MAX_DEVICE_REQUESTS];
__kobject static int32_t req_idx, free_req;

static void init_requests(void)
{
   memset(request_pool, 0, sizeof(request_pool));
   memset(free_req_pool, 0, sizeof(free_req_pool));
   req_idx = 0;
   free_req = -1;
}

/**
 * The requests have to be managed by means of its ID. Thus, we need
 * to relate a request ID with the structure that defines it. Thus, we
 * cannot use the internal fixed-size memory pool manager. Instead, we
 * have implemented a different approach of fixed-size block memory pool.
 */
static int32_t alloc_request(void)
{
   int32_t req;

   if (free_req < 0) {
      if (req_idx >= CONFIG_MAX_DEVICE_REQUESTS) {
         return -1;
      }
      req = req_idx++;
   } else {
      req = free_req_pool[free_req--];
   }

   return req;
}

static void free_request(int32_t req)
{
   // Check if request is already free
   if (!request_pool[req].buf) {
      return;
   }
   // Use the buffer field to mark this request as free
   request_pool[req].buf = NULL;
   free_req_pool[++free_req] = req;
}

/*
 * init_devices()
 *
 *    Eventflag pool initialization
 */
void init_devices(void)
{
   devbmap = clearbitmap();
   memset(devins_pool, 0, sizeof(devins_pool));
   init_requests();
}

int32_t dev_open_sys(const char *name, uint32_t omode)
{
   device_t *dev;
   device_instance_t *devins;
   int32_t desc, ret;

   __hw_cli();

   // Search for the device on the device pool
   for (dev=__device_pool; dev<__devices_end; dev+=sizeof(device_t)) {
      if (strncmp(dev->name, name, L_DEVNM) == 0) {
         goto found;
      }
   }
   __hw_sti();
   return E_NOEXS;

found:
   // Check device exclusive open mode
   if (dev->ocnt > 0) {
      if (dev->devatr & TD_EXCL) {
         goto devbusy;
      }
      if ((dev->devatr & TD_WEXCL) && (omode & TD_WRITE)) {
         goto devbusy;
      }
      if ((dev->devatr & TD_REXCL) && (omode & TD_READ)) {
         goto devbusy;
      }
   }

   // Update exclusive open mode
   dev->devatr |= omode & 0x0F00;

   // Get free descriptor
   desc = _ffc(devbmap);
   if (desc < 0 || desc > CONFIG_MAX_DEVICES) {
      __hw_sti();
      return E_NOMEM;
   }

   ret = 0;

   // Call open function of the device
   if (dev->ops->open) {
      ret = dev->ops->open(omode);
   }

   // Return device descriptor or error if the call to the device went wrong
   if (ret >= 0) {
      // Ret OK. Reference the structure with the descriptor
      ret = desc;

      // Set device instance features
      devins = &devins_pool[desc];
      devins->dev = dev;
      devins->tskid = current_task->tskid;
      devins->omode = omode;

      // Insert instance on the list
      devins->next = dev->insts;
      dev->insts = devins;

      // Increase open count
      dev->ocnt++;

      // Mark instance as used
      devbmap = setbit(devbmap, desc);
   } 

   __hw_sti();
   return ret;

devbusy:
#ifdef CONFIG_SYSLOG_ACTIVE
   //write_log(TFN_OPN_DEV, dev->devid, E_BUSY);
#endif
   __hw_sti();
   return E_BUSY;
}

/*
 * dev_close_sys()
 *
 *    Close device
 */
int32_t dev_close_sys(int32_t dd, uint32_t option)
{

}

/*
 * dev_read_sys()
 *
 *    Read device
 */
int32_t dev_read_sys(int32_t dd, int32_t start, void *buf, int32_t size, uint32_t tmout)
{
   device_t *dev;
   device_instance_t *devins;
   int32_t reqid;

   __hw_cli();

   if (!isbitset(devbmap, dd)) {
      __hw_sti();
      return E_ID;
   }

   devins = &devins_pool[dd];

   if (!devins) {
      //  SYSTEM ERROR  //
      // Device instance bit is active, but the pointer to the
      // instance is invalid. Something bad happened on the system.
      // ¡DO SOMETHING! //
      __hw_sti();
      return E_ID;
   }

   // Is current task the owner of this device instance?
   if (devins->tskid != current_task->tskid) {
      __hw_sti();
      return E_ID;
   }

   // Is the device opened in read mode?
   if (!(devins->omode & TD_READ)) {
      __hw_sti();
      return E_OAVC;
   }

   // Allocate a new request and initialize it
   reqid = alloc_request();
   if (reqid < 0) {
      __hw_sti();
      return E_LIMIT;
   }
   request_pool[reqid].buf = buf;
   request_pool[reqid].size = size;
   request_pool[reqid].flags = REQ_READ;
   request_pool[reqid].desc = dd;
   request_pool[reqid].tmout = tmout;

   // Add request to the device. Leave critical section
   __hw_sti();
   dev = devins->dev;
   if (dev->ops->add_request) {
      // add_request() is called unprotected, so that the device
      // driver can manage concurrency itself. This function calls
      // something which will lay outside the kernel, and may be
      // implemented by third parties. Thus, as we don't know
      // how is the code at the other side, it's preferable to leave
      // the task to the driver developer.
      dev->ops->add_request(dev, &request_pool[reqid]);
   }

   // Return request ID
   return reqid+1;
}

/*
 * dev_syncread_sys()
 *
 *    Read device
 */
int32_t dev_syncread_sys(int32_t dd, int32_t start, void *buf, int32_t size, int32_t *asize)
{
   device_t *dev;
   device_instance_t *devins;
   int32_t reqidx, ret;

   __hw_cli();

   if (!isbitset(devbmap, dd)) {
      __hw_sti();
      return E_ID;
   }
   devins = &devins_pool[dd];

   if (!devins) {
      //  SYSTEM ERROR  //
      // Device instance bit is active, but the pointer to the
      // instance is invalid. Something bad happened on the system.
      // ¡DO SOMETHING! //
      __hw_sti();
      return E_ID;
   }

   // Is current task the owner of this device instance?
   if (devins->tskid != current_task->tskid) {
      __hw_sti();
      return E_ID;
   }

   // Is the device opened in read mode?
   if (!(devins->omode & TD_READ)) {
      __hw_sti();
      return E_OAVC;
   }

   // Allocate a new request and initialize it
   reqidx = alloc_request();
   if (reqidx < 0) {
      __hw_sti();
      return E_LIMIT;
   }
   request_pool[reqidx].buf = buf;
   request_pool[reqidx].size = size;
   request_pool[reqidx].flags = REQ_READ;
   request_pool[reqidx].desc = dd;
   request_pool[reqidx].tmout = tmout;

   // Add request to the device. Leave critical section
   __hw_sti();
   dev = devins->dev;
   if (dev->ops->add_request) {
      // add_request() is called unprotected, so that the device
      // driver can manage concurrency itself
      dev->ops->add_request(dev, &request_pool[reqidx]);
   }

   __hw_cli();

   // Wait on request accepted
   if (request_pool[reqidx].flags & REQ_COMPLETED) {
      ret = request_pool[reqidx].stat;
      if (asize) {
         *asize = request_pool[reqidx].size;
      }
      free_request(reqidx);
      __hw_sti();
      return ret;
   } else {
      request_pool[reqidx].status |= REQ_WAITING;
      request_pool[reqidx].tskid = current_task->tskid;

      // Change task state to WAITING
      current_task->tskstat = TTS_WAI;
      current_task->wobjid = reqidx;
      rq_remove(current_task);
   }

   __hw_sti();

   scheduler();

   __hw_cli();

   // Release request and finish
   free_request(reqidx);
   if (request_pool[reqidx].flags & REQ_COMPLETED) {
      ret = request_pool[reqidx].stat;
      if (asize) {
         *asize = request_pool[reqidx].size;
      }
      __hw_sti();
      return ret;
   }

   // Should never get here
   __hw_sti();
   return -1;
}

/*
 * dev_read_sys()
 *
 *    Write device
 */
int32_t dev_write_sys(int32_t dd, int32_t start, void *buf, int32_t size, uint32_t tmout)
{
   device_t *dev;
   device_instance_t *devins;
   int32_t reqid;

   __hw_cli();

   if (!isbitset(devbmap, dd)) {
      __hw_sti();
      return E_ID;
   }

   devins = &devins_pool[dd];

   if (!devins) {
      //  SYSTEM ERROR  //
      // Device instance bit is active, but the pointer to the
      // instance is invalid. Something bad happened on the system.
      // ¡DO SOMETHING! //
      __hw_sti();
      return E_ID;
   }

   // Is current task the owner of this device instance?
   if (devins->tskid != current_task->tskid) {
      __hw_sti();
      return E_ID;
   }

   // Is the device read only?
   if (devins->dev->devatr & TD_RDONLY) {
      __hw_sti();
      return E_RONLY;
   }

   // Is the device opened in write mode?
   if (!(devins->omode & TD_WRITE)) {
      __hw_sti();
      return E_OAVC;
   }

   // Allocate a new request and initialize it
   reqid = alloc_request();
   if (reqid < 0) {
      __hw_sti();
      return E_LIMIT;
   }
   request_pool[reqid].buf = buf;
   request_pool[reqid].size = size;
   request_pool[reqid].flags = REQ_WRITE;
   request_pool[reqid].desc = dd;
   request_pool[reqid].tmout = tmout;

   // Add request to the device. Leave critical section
   __hw_sti();
   dev = devins->dev;
   if (dev->ops->add_request) {
      // add_request() is called unprotected, so that the device
      // driver can manage concurrency itself
      dev->ops->add_request(dev, &request_pool[reqid]);
   }

   // Return request ID
   return reqid+1;
}

int32_t dev_wait_sys(int32_t dd, int32_t reqid, int32_t *asize, int32_t *ioer, uint32_t tmout)
{
   device_t *dev;
   device_instance_t *devins;
   int32_t reqidx = reqid - 1;

   __hw_cli();

   if (!isbitset(devbmap, dd)) {
      __hw_sti();
      return E_ID;
   }
   devins = &devins_pool[dd];

   if (!devins) {
      //  SYSTEM ERROR  //
      // Device instance bit is active, but the pointer to the
      // instance is invalid. Something bad happened on the system.
      // ¡DO SOMETHING! //
      __hw_sti();
      return E_ID;
   }

   // Is current task the owner of this device instance?
   if (devins->tskid != current_task->tskid) {
      __hw_sti();
      return E_ID;
   }

   // Is this request a request for dd?
   if (request_pool[reqidx].desc != dd) {
      __hw_sti();
      return E_ID;
   }

   // Is there another task waiting for this request?
   if (request_pool[reqidx].flags & REQ_WAITING) {
      __hw_sti();
      return E_OBJ;
   }

   // Wait on request accepted
   if (request_pool[reqidx].flags & REQ_COMPLETED) {
      ret = request_pool[reqidx].stat;
      if (asize) {
         *asize = request_pool[reqidx].size;
      }
      free_request(reqidx);
      __hw_sti();
      return ret;
   } else {
      request_pool[reqidx].status |= REQ_WAITING;
      request_pool[reqidx].tskid = current_task->tskid;

      // Change task state to WAITING
      current_task->tskstat = TTS_WAI;
      current_task->wobjid = reqid;
      rq_remove(current_task);
      tq_insert(current_task);
   }

   __hw_sti();

   scheduler();

   __hw_cli();

   // Timeout expired
   if (current_task->wobjid == E_TMOUT) {
      __hw_sti();
      return E_TMOUT;
   }

   // Remove from timeout queue
   tq_remove(current_task);

   // Release request and finish
   if (request_pool[reqidx].flags & REQ_COMPLETED) {
      ret = request_pool[reqidx].stat;
      if (asize) {
         *asize = request_pool[reqidx].size;
      }
      free_request(reqidx);
      __hw_sti();
      return ret;
   }

   // Should never get here
   free_request(reqidx);
   __hw_sti();
   return -1;
}

/*
 * dev_reqdone()
 *
 *    This call must be executed on a callback
 */
void dev_reqdone(int32_t reqid, int32_t size, int16_t stat)
{
   int32_t reqidx = reqid - 1;
   // If there's a task waiting, wake it up
   if (request_pool[reqidx].flags & REQ_WAITING) {
      task_t *tsk;

      // Get task
      tsk = get_task_by_id(request_pool[reqidx].tskid);

      if (!tsk) {
         // System error. Request is not associated with a valid task
         // Maybe the task died while the request was active
      }

      tsk->tskstat = TTS_RDY;

      // Remove task from timeout queue
      if (request_pool[reqid].tmout > 0) {
         tq_remove(tsk);
      }
      rq_insert(tsk);

      // Call the scheduler. It will return with no processing, but will
      // mark that a scheduling is pending
      scheduler();
   }

   request_pool[reqidx].size = size;
   request_pool[reqidx].status = stat;
   request_pool[reqidx].flags = REQ_COMPLETED;
}

