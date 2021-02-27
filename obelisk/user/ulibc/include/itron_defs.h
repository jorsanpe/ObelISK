/**
 * @file itronsys/types.h
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: types.h,v 1.11 2008/09/07 07:04:38 ralf Exp $
 */

#ifndef _ITRON_TYPES_H_
#define _ITRON_TYPES_H_

typedef char            B;      /* signed 8-bit integer */
typedef short           H;      /* signed 16-bit integer */
typedef int             W;      /* signed 32-bit integer */
typedef unsigned char  UB;      /* unsigned 8-bit integer */
typedef unsigned short UH;      /* unsigned 16-bit integer */
typedef unsigned int   UW;      /* unsigned 32-bit integer */

typedef unsigned int   VW;      /* unpredictable data type (32-bit size) */
typedef unsigned short VH;      /* unpredictable data type (16-bit size) */
typedef unsigned char  VB;      /* unpredictable data type (8-bit size) */

typedef void          *VP;      /* pointer to an unpredictable data type */

typedef void (*FP)(void);     /* program start address */

typedef int          INT;     /* Signed integer (bit width of processor) */
typedef unsigned int UINT;    /* Unsigned integer (bit width of processor) */
typedef int          BOOL;    /* Boolean value.  TRUE (1) or FALSE (0). */
typedef int          FN;      /* Function code. Signed integer. */
typedef int          ID;      /* Object ID number (???id) */
                              /*   Value range depends on the system.  Usually */
                              /*   a signed integer.  Certain ID values may */
                              /*   represent objects on other nodes when the */
                              /*   connection function is supported. */

typedef unsigned int BOOL_ID;/* Boolean value or ID number */

typedef int          HNO;     /* Handler number */
typedef int          RNO;     /* Rendezvous number */
typedef int          NODE;    /* Node Number.  Usually a signed integer. */
typedef int          ATR;     /* Object or handler attribute. */
                              /*   An unsigned integer. */
typedef int          ER;      /* Error code.  A signed integer. */
typedef unsigned int PRI;     /* Task priority.  A signed integer. */
typedef int          TMO;     /* Timeout value.  A signed integer. */
                              /*   TMO_POL = 0 indicates polling, */
                              /*   while TMO_FEVR = -1 indicates wait forever. */
typedef unsigned int FLGPTN;  /* Flag pattern type */
typedef int          DLYTIME;
typedef long long    SYSTIM, CYCTIM, ALMTIM;

#define NADR   (-1)   /* invalid address or pointer value */

#ifndef TRUE
#define TRUE     1    /* true */
#endif

#ifndef FALSE
#define FALSE    0    /* false */
#endif

#define TMO_POL    (-1)   /* polling */
#define TMO_FEVR      0   /* wait forever */

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

#endif //_ITRON_TYPES_H_

