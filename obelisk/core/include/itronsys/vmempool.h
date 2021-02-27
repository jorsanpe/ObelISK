/**
 * @file itronsys/vmempool.h
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: vmempool.h,v 1.6 2005/01/28 11:07:14 ralf Exp $
 */

#ifndef _ITRONSYS_VMEMPOOL_H
#define _ITRONSYS_VMEMPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Create Variable Memory Pool (cre_mpl) Structure
 */

typedef struct t_cmpl {
   ATR   mplatr;   /* Variable-sized memory pool attributes */
   UINT  mplsz;    /* Variable-sized memory pool size  */
   VP    mpl;      /* Start address of the variable-sized memory pool area */
} T_CMPL;

/*
 *  mplatr
 */

#define TA_TFIFO   0x00   /* waiting tasks are handled by FIFO */
#define TA_TPRI    0x01   /* waiting tasks are handled by priority */

/*
 *  Reference Variable Memory Pool (ref_mpl) Structure
 */

typedef struct t_rmpl {
  VP        exinf;    /* extended information */
  BOOL_ID   wtsk;     /* indicates whether or not there are waiting tasks */
  INT       frsz;     /* total size of free memory */
  INT       maxsz;    /* size of largest contiguous memory */
  /* additional information may be included depending on the implementation */
} T_RMPL;

/*
 *  Variable Memory Pool Functions
 */

/*
 *  cre_mpl_sys - Create Variable-Size Memorypool
 */

ER cre_mpl_sys(
  ID mplid,
  T_CMPL *pk_cmpl
);

/*
 *  del_mpl_sys - Delete Variable-Size Memorypool
 */

ER del_mpl_sys(
  ID mplid
);

/*
 *  get_mpl_sys - Get Variable-Size Memory Block
 */

ER get_mpl_sys(
  ID mplid,
  UINT blksz,
  VP *p_blk
);

/*
 *  pget_mpl_sys - Poll and Get Variable-Size Memory Block
 */

ER pget_mpl_sys(
  VP *p_blk,
  ID mplid,
  INT blksz
);

/*
 *  tget_mpl_sys - Get Variable-Size Memory Block with Timeout
 */

ER tget_mpl_sys(
  VP *p_blk,
  ID mplid,
  INT blksz,
  TMO tmout
);

/*
 *  rel_mpl_sys - Release Variable-Size Memory Block
 */

ER rel_mpl_sys(
  ID mplid,
  VP blk
);

/*
 *  ref_mpl_sys - Reference Variable-Size Memorypool Status
 */

ER ref_mpl_sys(
  T_RMPL *pk_rmpl,
  ID mplid
);


#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
