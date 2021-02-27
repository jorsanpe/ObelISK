/**
 * @file itronsys/status.h
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: status.h,v 1.6 2005/01/28 11:07:14 ralf Exp $
 */

#ifndef _ITRONSYS_STATUS_H_
#define _ITRONSYS_STATUS_H_

#define E_OK       0        /* Normal completion */
#define E_SYS      (-5)     /* System error */
#define E_NOSPT    (-09)    /* Feature not supported */
#define E_RSFN     (-10)    /* Reserved function code */
#define E_RSATR    (-11)    /* Reserved attribute */
#define E_PAR      (-17)    /* Parameter error */
#define E_ID       (-18)    /* Invalid ID number */
#define E_CTX      (-25)    /* Context error */
#define E_MACV     (-26)    /* Memory access disabled or memory access */
                            /*   violation */
#define E_OACV     (-27)    /* Object access violation */
#define E_ILUSE    (-28)    /* Illegal service call use */
#define E_NOMEM    (-33)    /* Insufficient memory */
#define E_NOID     (-34)    /* No ID number available */
#define E_OBJ      (-41)    /* Invalid object state */
#define E_NOEXS    (-42)    /* Object does not exist */
#define E_QOVR     (-43)    /* Queuing or nesting overflow */
#define E_RLWAI    (-49)    /* WAIT state was forcibly released */
#define E_TMOUT    (-50)    /* Polling failure or timeout exceeded */
#define E_DLT      (-51)    /* Object being waited for was deleted */
#define E_CLS      (-52)    /* Waiting object state changed */
#define E_WBLK     (-57)    /* Non-blocking call accepted */
#define E_BOVR     (-58)    /* Buffer overflow */

#define E_BUSY     (-65)    /* Device busy */
#define E_ABORT    (-66)    /* Processing aborted */
#define E_RONLY    (-67)    /* Device read only */

#endif   //_ITRONSYS_STATUS_H_
