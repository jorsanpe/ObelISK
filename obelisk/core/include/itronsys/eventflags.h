/**
 * @file itronsys/eventflags.h
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: eventflags.h,v 1.6 2005/01/28 11:07:14 ralf Exp $
 */

#ifndef _ITRONSYS_EVENTFLAGS_H
#define _ITRONSYS_EVENTFLAGS_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Create Eventflags (cre_flg) Structure
 */

typedef struct t_cflg {
  VP     exinf;     /* extended information */
  ATR    flgatr;    /* eventflag attribute */
  FLGPTN iflgptn;   /* initial eventflag */
  /* additional information may be included depending on the implementation */
} T_CFLG;


/*
 *  flgatr
 */
#define TA_WSGL   0x00     /* Wait Single Task */
#define TA_TPRI   0x01     /* Wait queue with priority order */
#define TA_WMUL   0x02     /* Wait Multiple Task */
#define TA_CLR    0x04     /* clear specification */

/*
 *  wfmode
 */
#define TWF_ANDW   0x00   /* AND wait */
#define TWF_ORW    0x01   /* OR wait */


/*
 *  Reference Eventflags (ref_flg) Structure
 */

typedef struct t_rflg {
  VP        exinf;      /* extended information */
  BOOL_ID   wtsk;       /* indicates whether or not there is a waiting task */
  FLGPTN    flgptn;     /* eventflag bit pattern */
  /* additional information may be included depending on the implementation */
} T_RFLG;

/*
 *  Eventflag Functions
 */

/*
 *  cre_flg_sys - Create Eventflag
 */

ER cre_flg_sys(
  ID flgid,
  T_CFLG *pk_cflg
);

/*
 *  del_flg_sys - Delete Eventflag
 */

ER del_flg_sys(
  ID flgid
);

/*
 *  set_flg_sys - Set Eventflag
 */

ER set_flg_sys(
  ID flgid,
  FLGPTN setptn
);

/*
 *  clr_flg_sys - Clear Eventflag
 */

ER clr_flg_sys(
  ID flgid,
  FLGPTN clrptn
);

/*
 *  wai_flg_sys - Wait on Eventflag
 */

ER wai_flg_sys(
  ID flgid,
  FLGPTN waiptn,
  UINT wfmode,
  FLGPTN *p_flgptn
);

/*
 *  pol_flg_sys - Wait for Eventflag(Polling)
 */

ER pol_flg_sys(
  ID flgid,
  FLGPTN waiptn,
  UINT wfmode,
  FLGPTN *p_flgptn
);

/*
 *  twai_flg_sys - Wait on Eventflag with Timeout
 */

ER twai_flg_sys(
  ID flgid,
  FLGPTN waiptn,
  UINT wfmode,
  FLGPTN *p_flgptn,
  TMO tmout
);

/*
 *  ref_flg_sys - Reference Eventflag Status
 */

ER ref_flg_sys(
  ID flgid,
  T_RFLG *pk_rflg
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
