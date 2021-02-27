/**
 * @file itronsys/semaphore.h
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: semaphore.h,v 1.6 2005/01/28 11:07:14 ralf Exp $
 */

#ifndef _ITRONSYS_SEMAPHORE_H
#define _ITRONSYS_SEMAPHORE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Create Semaphore (cre_sem) Structure
 */

typedef struct t_csem {
  ATR   sematr;   /* semaphore attributes */
  INT   isemcnt;  /* initial semaphore count */
  INT   maxsem;   /* maximum semaphore count */
} T_CSEM;

/*
 *  sematr - Semaphore Attribute Values
 */

#define TA_TFIFO   0x00   /* waiting tasks are handled by FIFO */
#define TA_TPRI    0x01   /* waiting tasks are handled by priority */

#define _ITRON_SEMAPHORE_UNUSED_ATTRIBUTES ~(TA_TPRI)

/*
 *  Reference Semaphore (ref_sem) Structure
 */

typedef struct t_rsem {
  VP      exinf;    /* extended information */
  BOOL_ID wtsk;     /* indicates whether or not there is a waiting task */
  INT     semcnt;   /* current semaphore count */
  /* additional information may be included depending on the implementation */
} T_RSEM;

/*
 *  Semaphore Functions
 */

/*
 *  cre_sem_sys - Create Semaphore
 */

ER cre_sem_sys(
  ID semid,
  T_CSEM *pk_csem
);

/*
 *  del_sem_sys - Delete Semaphore
 */

ER del_sem_sys(
  ID semid
);

/*
 *  sig_sem_sys - Release Semaphore
 */

ER sig_sem_sys(
  ID semid
);

/*
 *  wai_sem_sys - Acquire Semaphore
 */

ER wai_sem_sys(
  ID semid
);

/*
 *  preq_sem_sys - Acquire Semaphore Polling
 */

ER preq_sem_sys(
  ID semid
);

/*
 *  twai_sem_sys - Acquire Semaphore with Timeout
 */

ER twai_sem_sys(
  ID semid,
  TMO tmout
);

/*
 *  ref_sem_sys - Reference Semaphore Status
 */

ER ref_sem_sys(
  ID      semid,
  T_RSEM *pk_rsem
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
