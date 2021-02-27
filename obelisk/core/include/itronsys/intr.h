/**
 * @file itronsys/intr.h
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: intr.h,v 1.6 2005/01/28 11:07:14 ralf Exp $
 */

#ifndef _ITRONSYS_INTR_H
#define _ITRONSYS_INTR_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Define Interrupt (def_int) Structure
 */

typedef struct t_dint {
  ATR   inhatr;   /* interrupt handler attributes */
  FP    inthdr;   /* interrupt handler address */
  /* additional information may be included depending on the implementation */
} T_DINH;

/*
 *  Interrupt Functions
 */

/*
 *  def_int_sys - Define Interrupt Handler
 */

ER def_inh_sys(
  UINT inhno,
  T_DINH *pk_dinh
);

/*
 *  ret_int_sys - Return from Interrupt Handler
 */

void ret_int_sys( void );

/*
 *  ret_wup_sys - Return and Wakeup Task
 */

void ret_wup_sys(
  ID tskid
);

/*
 *  loc_cpu_sys - Lock CPU
 */

ER loc_cpu_sys( void );

/*
 *  unl_cpu_sys - Unlock CPU
 */

ER unl_cpu_sys( void );

/*
 *  dis_int_sys - Disable Interrupt
 */

ER dis_int_sys(
  UINT eintno
);

/*
 *  ena_int_sys - Enable Interrupt
 */

ER ena_int_sys(
  UINT eintno
);

/*
 *  chg_iXX_sys - Change Interrupt Mask(Level or Priority)
 */

ER chg_iXX_sys(
  UINT iXXXX
);

/*
 *  ref_iXX - Reference Interrupt Mask(Level or Priority)
 */

ER ref_iXX_sys(
  UINT *p_iXXXX
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
