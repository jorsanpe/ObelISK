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

#ifndef _ITRONSYS_TYPES_H
#define _ITRONSYS_TYPES_H

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
typedef unsigned int TMO;     /* Timeout value.  A signed integer. */
                              /*   TMO_POL = 0 indicates polling, */
                              /*   while TMO_FEVR = -1 indicates wait forever. */
typedef unsigned int FLGPTN;  /* Flag pattern type */
typedef unsigned int STAT;    /* Object State */
typedef int          ER_UINT; /* Error code or unsigned integer */

/*
 *   6.6 Common Constants and Data Structure Packet Formats
 */

#define NADR   (-1)   /* invalid address or pointer value */

#ifndef TRUE
#define TRUE     1    /* true */
#endif

#ifndef FALSE
#define FALSE    0    /* false */
#endif

/*
 *  ATR tskatr, intatr, cycatr, almatr, svcatr, excatr:
 *
 *  TA_ASM indicates that the task or handler is directly started
 *         at the assembly language level.  The TA_ASM attribute has the
 *         opposite meaning of TA_HLNG.
 *
 *  NOTE: Really in <itronsys/task.h>
 */

#if 0
#define TA_ASM    0x00   /* program written in assembly language */
#define TA_HLNG   0x01   /* program written in high-level language */
#endif

/*
 *  TMO tmout:
 */
#define TMO_POL    (-1)   /* polling */
#define TMO_FEVR      0   /* wait forever */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
