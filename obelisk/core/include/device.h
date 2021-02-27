/*
 * $FILE: device.h
 *
 * ObelISK device management header
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

#ifndef _DEVICE_H_
#define _DEVICE_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <types.h>

#define L_DEVNM 8

/* Device Standard Attributes (LO) */
#define TD_PROTECT     0x8000 /* P: write protection             */
#define TD_REMOVABLE   0x4000 /* R: removable media              */
#define TD_DEVKIND     0x00ff /* K: device/media kind            */
#define TD_DEVTYPE     0x00f0 /* device type                     */
#define TD_RDONLY      0x0100 /* Device read only                */

/* Device Specific Attributes (HI) */
#define DEV_BUSY        0x0001 /* Device busy                     */

/* Device Type */
#define TDK_UNDEF      0x0000 /* undefined/unknown               */
#define TDK_DISK       0x0010 /* disk device                     */

/* Disk Kind */
#define TDK_DISK_UNDEF 0x0010 /* miscellaneous disk              */
#define TDK_DISK_RAM   0x0011 /* RAM disk (used as main memory)  */
#define TDK_DISK_ROM   0x0012 /* ROM disk (used as main memory)  */
#define TDK_DISK_FLA   0x0013 /* Flash ROM or other silicon disk */
#define TDK_DISK_FD    0x0014 /* floppy disk                     */
#define TDK_DISK_HD    0x0015 /* hard disk                       */
#define TDK_DISK_CDROM 0x0016 /* CD-ROM                          */

/* Open Mode */
#define TD_READ   0x0001 /* read only       */
#define TD_WRITE  0x0002 /* write only      */
#define TD_UPDATE 0x0003 /* read/write      */
#define TD_EXCL   0x0100 /* exclusive       */
#define TD_WEXCL  0x0200 /* exclusive write */
#define TD_REXCL  0x0400 /* exclusive read  */

/* Request Flags */
#define REQ_READY      0x0000
#define REQ_PROCESSING 0x0001
#define REQ_COMPLETED  0x0002
#define REQ_ERROR      0x0004
#define REQ_WAITING    0x0008
#define REQ_READ       0x0010
#define REQ_WRITE      0x0020

/* Declare Logical Device */
//#define DECLARE_LOGDEV(name)  __attribute__((__section__(".logical_devices"))) static logdev_t name;

/* Declare Physical Device */
//#define DECLARE_PHYSDEV(name) __attribute__((__section__(".physical_devices"))) static device_t name;

/* Declare Device */
#define DECLARE_DEVICE(name)  __attribute__((__section__(".devices"))) static device_t name;

/* Device functions for logical layer */
struct dev_ops {
   int32_t (*open)(void);
   void    (*close)(void);
   int32_t (*add_request)(device_t *, request_t *);
   void    (*callback)(device_t *, request_t *);
   //int     (*ioctl) (int fd, unsigned, unsigned long);
};

/* Logical device driver structure */
typedef struct dev_struct {
   /* Device name */
   int8_t name[L_DEVNM];

   /* Device ID */
   int32_t devid;

   /* Device Attribute */
   uin32_t devatr;

   /* Device open count */
   int32_t ocnt;

   /* Device specific data */
   void *devdat;

   /* Device status */
   int32_t stat;

   /* Request count, used for request index */
   uint16_t reqcnt;

   /* Device operations */
   struct dev_ops *ops;

   /* List of all opened instances */
   void *insts;

   /* Device request queue */
   queue_t request_queue;
} device_t;

typedef struct device_request {
   /* Request buffer */
   char *buf;

   /* Bytes requested */
   uint32_t size;

   /* Request info */
   int16_t flags;
   int16_t status;

   /* Request timeout */
   uint32_t tmout;

   /* Task waiting for this request */
   uint32_t tskid;

   /* Request is associated with a device descriptor */
   uint32_t desc;

   /* List for the request queue */
   linked_list_t list;
} request_t;

typedef struct device_instance_struct {
   /* List to keep all the instances of the same device */
   struct device_instance_struct *next;

   /* Pointer to device structure */
   device_t *dev;

   /* Task which opened the device */
   uint32_t tskid;

   /* Device open mode */
   int32_t omode;

} device_instance_t;

extern void dev_reqdone(int32_t reqid, int32_t size, int16_t stat);

#endif //_DEVICE_H_
