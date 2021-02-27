/**
 * @file itronsys/mbox.h
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: mbox.h,v 1.8 2005/01/28 11:07:14 ralf Exp $
 */

#ifndef _ITRONSYS_MBOX_H
#define _ITRONSYS_MBOX_H

//#include "linked_list.h"

#ifdef __cplusplus
extern "C" {
#endif

/*typedef struct t_msg {
   linked_list_t msgque;
   void *msgdat;
} T_MSG;*/
typedef void *T_MSG;

/*
 *  Create Mailbox (cre_mbx) Structure
 */

typedef struct t_cmbx {
   ATR   mbxatr;     /* mailbox attributes */
   PRI   maxmpri;    /* extended information */
   VP    mprihd;     /* Start address of the area for message queue headers */
} T_CMBX;

/*
 *  mbxatr
 */

#define TA_TFIFO   0x00   /* waiting tasks are handled by FIFO */
#define TA_TPRI    0x01   /* waiting tasks are handled by priority */
#define TA_MFIFO   0x00   /* messages are handled by FIFO */
#define TA_MPRI    0x02   /* messages are handled by priority */

/*
 *  Reference Mailbox (ref_mbx) Structure
 */

typedef struct t_rmbx {
  ID   wtskid;        /* indicates whether or not there is a waiting task */
  T_MSG    *pk_msg;   /* message to be sent next */
} T_RMBX;

/*
 *  Mailbox Functions
 */

/*
 *  cre_mbx - Create Mailbox
 */

ER cre_mbx_sys(
  ID      mbxid,
  T_CMBX *pk_cmbx
);

/*
 *  del_mbx_sys - Delete Mailbox
 */

ER del_mbx_sys(
  ID mbxid
);

/*
 *  snd_msg - Send Message to Mailbox
 */

ER snd_msg_sys(
  ID     mbxid,
  T_MSG *pk_msg
);

/*
 *  rcv_msg_sys - Receive Message from Mailbox
 */

ER rcv_msg_sys(
  ID      mbxid,
  T_MSG **ppk_msg

);

/*
 *  prcv_msg_sys - Poll and Receive Message from Mailbox
 */

ER prcv_msg_sys(
  ID      mbxid,
  T_MSG **ppk_msg
);

/*
 *  trcv_msg_sys - Receive Message from Mailbox with Timeout
 */

ER trcv_msg_sys(
  ID      mbxid,
  T_MSG **ppk_msg,
  TMO     tmout
);

/*
 *  ref_mbx_sys - Reference Mailbox Status
 */

ER ref_mbx_sys(
  ID      mbxid,
  T_RMBX *pk_rmbx
);


#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
