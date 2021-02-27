/**
 * @file itronsys/time.h
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: time.h,v 1.6 2005/01/28 11:07:14 ralf Exp $
 */

#ifndef _ITRONSYS_TIME_H
#define _ITRONSYS_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  System Time Data Type
 *
 */
typedef int DLYTIME;
typedef long long SYSTIM, CYCTIM, ALMTIM;

/*
 *  XXX (def_cyc) Structure
 */

typedef struct t_dcyc {
  VP        exinf;    /* extended information */
  ATR       cycatr;   /* cyclic handler attributes */
  FP        cychdr;   /* cyclic handler address */
  UINT      cycact;   /* cyclic handler activation */
  CYCTIM   cyctim;   /* cyclic startup period */
} T_DCYC;

/*
 *  cycact
 */

#define TCY_OFF   0x00   /* do not invoke cyclic handler */
#define TCY_ON    0x01   /* invoke cyclic handler */
#define TCY_INT   0x02   /* initialize cycle count */

/*
 *  Reference Cyclic Handler (ref_cyc) Structure
 */

typedef struct t_rcyc {
  VP        exinf;    /* extended information */
  CYCTIM   lfttim;   /* time left before next handler startup */
  UINT      cycact;   /* cyclic handler activation */
  /* additional information may be included depending on the implementation */
} T_RCYC;

/*
 *  Define Alarm (def_alm) Structure
 */

typedef struct t_dalm {
  VP        exinf;    /* extended information */
  ATR       almatr;   /* alarm handler attributes */
  FP        almhdr;   /* alarm handler address */
  UINT      tmmode;   /* start time specification mode */
  ALMTIM   almtim;   /* handler startup time */
} T_DALM;

/*
 *  tmmode
 */

#define TTM_ABS   0x00   /* specified as an absolute time */
#define TTM_REL   0x01   /* specified as a relative time */

/*
 *  Reference Alarm (ref_alm) Structure
 */

typedef struct t_ralm {
  VP        exinf;    /* extended information */
  ALMTIM   lfttim;   /* time left before next handler startup */
  /* additional information may be included depending on the implementation */
} T_RALM;

/*
 *  Time Management Functions
 */

/*
 *  set_tim_sys - Set System Clock
 */

ER set_tim_sys(
  SYSTIM *pk_tim
);

/*
 *  get_tim_sys - Get System Clock
 */

ER get_tim_sys(
  SYSTIM *pk_tim
);

/*
 *  dly_tsk_sys - Delay Task
 */

ER dly_tsk_sys(
  DLYTIME dlytim
);

/*
 *  def_cyc_sys - Define Cyclic Handler
 */

ER def_cyc_sys(
  HNO cycno,
  T_DCYC *pk_dcyc
);

/*
 *  act_cyc_sys - Activate Cyclic Handler
 */

ER act_cyc_sys(
  HNO cycno,
  UINT cycact
);

/*
 *  ref_cyc_sys - Reference Cyclic Handler Status
 */

ER ref_cyc_sys(
  T_RCYC *pk_rcyc,
  HNO cycno
);

/*
 *  def_alm_sys - Define Alarm Handler
 */

ER def_alm_sys(
  HNO almno,
  T_DALM *pk_dalm
);

/*
 *  ref_alm_sys - Reference Alarm Handler Status
 */

ER ref_alm_sys(
  T_RALM *pk_ralm,
  HNO almno
);

/*
 *  ret_tmr_sys - Return from Timer Handler
 */

void ret_tmr_sys( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
