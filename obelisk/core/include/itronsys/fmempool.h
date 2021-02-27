/**
 * @file itronsys/fmempool.h
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: fmempool.h,v 1.6 2005/01/28 11:07:14 ralf Exp $
 */

#ifndef _ITRONSYS_FMEMPOOL_H
#define _ITRONSYS_FMEMPOOL_H

#include <mempool.h>

/*
 *  Create Fixed Memory Pool (cre_mpf) Structure
 */

    /* cre_mpf */
typedef struct t_cmpf {
  ATR   mpfatr;   /* memorypool attributes */
  UINT   blkcnt;  /* block count for entire memorypool */
  UINT   blksz;   /* fixed-size memory block size */
  VP       mpf;   /* start address of the fixed-sized memory pool area */
  /* additional information may be included depending on the implementation */
} T_CMPF;

/*
 *  mpfatr
 */

#define TA_TFIFO   0x00   /* waiting tasks are handled by FIFO */
#define TA_TPRI    0x01   /* waiting tasks are handled by priority */

#include <mpool.h>
static inline UINT TSZ_MPF(UINT blkcnt, UINT blksz)
{
   return POOL_SIZE(ALIGN32(blksz), blkcnt);
}

/*
 *  Reference Fixed Memory Pool (ref_mpf) Structure
 */

    /* ref_mpf */
typedef struct t_rmpf {
  VP      exinf;  /* extended information */
  ID     wtskid;  /* indicates whether or not there are waiting tasks */
  INT   fblkcnt;  /* free block count */
  /* additional information may be included depending on the implementation */
} T_RMPF;

/*
 *  Fixed Memory Pool Functions
 */

/*
 *  cre_mpf_sys - Create Fixed-Size Memorypool
 */

ER cre_mpf_sys(
  ID mpfid,
  T_CMPF *pk_cmpf
);

/*
 *  del_mpf_sys - Delete Fixed-Size Memorypool
 */

ER del_mpf_sys(
  ID mpfid
);

/*
 *  get_blf - Get Fixed-Size Memory Block
 */

ER get_blf(
  VP *p_blf,
  ID mpfid
);

/*
 *  pget_blf - Poll and Get Fixed-Size Memory Block
 */

ER pget_blf(
  VP *p_blf,
  ID mpfid
);

/*
 *  tget_blf - Get Fixed-Size Memory Block with Timeout
 */

ER tget_blf(
  VP *p_blf,
  ID mpfid,
  TMO tmout
);

/*
 *  rel_blf - Release Fixed-Size Memory Block
 */

ER rel_blf(
  ID mpfid,
  VP blf
);

/*
 *  ref_mpf_sys - Reference Fixed-Size Memorypool Status
 */

ER ref_mpf_sys(
  T_RMPF *pk_rmpf,
  ID mpfid
);

#endif
/* end of include file */
