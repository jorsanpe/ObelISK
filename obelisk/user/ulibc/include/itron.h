/*
 * $FILE: itron.h
 *
 * ITRON definitions
 *
 * Author: Jordi Sánchez <jorsanp3@upvnet.upv.es>
 *
 * $LICENSE:
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _ITRON_H_
#define _ITRON_H_

#include <itron_defs.h>
#include <kernel_inc/itronsys/fncodes.h>
#include <list.h>

// ITRON common defines
#define TA_TFIFO   0x00   /* waiting tasks are handled by FIFO */
#define TA_TPRI    0x01   /* waiting tasks are handled by priority */


/*
 *  Eventflags
 */
typedef struct t_cflg {
  VP     exinf;     /* extended information */
  ATR    flgatr;    /* eventflag attribute */
  FLGPTN iflgptn;   /* initial eventflag */
  /* additional information may be included depending on the implementation */
} T_CFLG;

typedef struct t_rflg {
  VP        exinf;  /* extended information */
  BOOL_ID   wtsk;   /* indicates whether or not there is a waiting task */
  FLGPTN    flgptn; /* eventflag bit pattern */
  /* additional information may be included depending on the implementation */
} T_RFLG;

#define TA_WSGL   0x00     /* Wait Single Task */
#define TA_TPRI   0x01     /* Wait queue with priority order */
#define TA_WMUL   0x02     /* Wait Multiple Task */
#define TA_CLR    0x04     /* clear specification */
#define TWF_ANDW   0x00   /* AND wait */
#define TWF_ORW    0x01   /* OR wait */

extern ER cre_flg(ID flgid, T_CFLG *pk_cflg);
extern ER del_flg(ID flgid);
extern ER set_flg(ID flgid, FLGPTN setptn);
extern ER clr_flg(ID flgid, FLGPTN clrptn);
extern ER wai_flg(ID flgid, FLGPTN waiptn, UINT wfmode, FLGPTN *p_flgptn);
extern ER pol_flg(ID flgid, FLGPTN waiptn, UINT wfmode, FLGPTN *p_flgptn);
extern ER twai_flg(ID flgid, FLGPTN waiptn, UINT wfmode, FLGPTN *p_flgptn, TMO tmout);
extern ER ref_flg(ID flgid, T_RFLG *pk_rflg);



/*
 *  Fixed Memory Pool
 */
typedef struct t_cmpf {
  ATR   mpfatr;   /* memorypool attributes */
  UINT  blkcnt;   /* block count for entire memorypool */
  UINT  blksz;    /* fixed-size memory block size */
  VP    mpf;      /* start address of the fixed-sized memory pool area */
  /* additional information may be included depending on the implementation */
} T_CMPF;

typedef struct t_rmpf {
  VP    exinf;    /* extended information */
  ID    wtskid;   /* indicates whether or not there are waiting tasks */
  INT   fblkcnt;  /* free block count */
  /* additional information may be included depending on the implementation */
} T_RMPF;

#define TA_TFIFO   0x00   /* waiting tasks are handled by FIFO */
#define TA_TPRI    0x01   /* waiting tasks are handled by priority */

extern ER cre_mpf(ID mpfid, T_CMPF *pk_cmpf);
extern ER del_mpf(ID mpfid);
extern ER get_blf(VP *p_blf, ID mpfid);
extern ER pget_blf(VP *p_blf, ID mpfid);
extern ER tget_blf(VP *p_blf, ID mpfid, TMO tmout);
extern ER rel_blf(ID mpfid, VP blf);
extern ER ref_mpf(T_RMPF *pk_rmpf, ID mpfid);



/*
 * Variable Memory Pool
 */
typedef struct t_cmpl {
   ATR   mplatr;   /* Variable-sized memory pool attributes */
   UINT  mplsz;    /* Variable-sized memory pool size  */
   VP    mpl;      /* Start address of the variable-sized memory pool area */
} T_CMPL;

typedef struct t_rmpl {
  VP        exinf;    /* extended information */
  BOOL_ID   wtsk;     /* indicates whether or not there are waiting tasks */
  INT       frsz;     /* total size of free memory */
  INT       maxsz;    /* size of largest contiguous memory */
  /* additional information may be included depending on the implementation */
} T_RMPL;

extern ER cre_mpl(ID mplid, T_CMPL *pk_cmpl);
extern ER del_mpl(ID mplid);
extern ER get_mpl(ID mplid, UINT blksz, VP *p_blk);
extern ER pget_mpl(VP *p_blk, ID mplid, INT blksz);
extern ER tget_mpl(VP *p_blk, ID mplid, INT blksz, TMO tmout);
extern ER rel_mpl(ID mplid, VP blk);
extern ER ref_mpl(T_RMPL *pk_rmpl, ID mplid);

/*
 *  Interrupts
 */
typedef struct t_dint {
  ATR   inhatr;   /* interrupt handler attributes */
  FP    inthdr;   /* interrupt handler address */
  /* additional information may be included depending on the implementation */
} T_DINH;

extern ER def_inh(UINT inhno, T_DINH *pk_dinh);
extern void ret_int(void);
extern void ret_wup(ID tskid);
extern ER loc_cpu(void);
extern ER unl_cpu(void);
extern ER dis_int(UINT eintno);
extern ER ena_int(UINT eintno);
extern ER chg_iXX(UINT iXXXX);
extern ER ref_iXX(UINT *p_iXXXX);



/*
 *  Mailboxes
 */
typedef struct t_cmbx {
   ATR   mbxatr;     /* mailbox attributes */
   PRI   maxmpri;    /* extended information */
   VP    mprihd;     /* Start address of the area for message queue headers */
} T_CMBX;

typedef struct t_msg {
   list_t msgque;
   void *msgdat;
} T_MSG;

#define TA_MFIFO   0x00   /* messages are handled by FIFO */
#define TA_MPRI    0x02   /* messages are handled by priority */

typedef struct t_rmbx {
  ID   wtskid;        /* indicates whether or not there is a waiting task */
  T_MSG    *pk_msg;   /* message to be sent next */
} T_RMBX;

extern ER cre_mbx(ID mbxid, T_CMBX *pk_cmbx);
extern ER del_mbx(ID mbxid);
extern ER snd_msg(ID mbxid, T_MSG *pk_msg);
extern ER rcv_msg(ID mbxid, T_MSG **ppk_msg);
extern ER prcv_msg(ID mbxid, T_MSG **ppk_msg);
extern ER trcv_msg(ID mbxid, T_MSG **ppk_msg, TMO tmout);
extern ER ref_mbx(ID mbxid, T_RMBX *pk_rmbx);



/*
 *  Semaphores
 */
typedef struct t_csem {
  ATR   sematr;   /* semaphore attributes */
  INT   isemcnt;  /* initial semaphore count */
  INT   maxsem;   /* maximum semaphore count */
} T_CSEM;

#define _ITRON_SEMAPHORE_UNUSED_ATTRIBUTES ~(TA_TPRI)

typedef struct t_rsem {
  VP      exinf;    /* extended information */
  BOOL_ID wtsk;     /* indicates whether or not there is a waiting task */
  INT     semcnt;   /* current semaphore count */
  /* additional information may be included depending on the implementation */
} T_RSEM;

extern ER cre_sem(ID semid, T_CSEM *pk_csem);
extern ER del_sem(ID semid);
extern ER sig_sem(ID semid);
extern ER wai_sem(ID semid);
extern ER preq_sem(ID semid);
extern ER twai_sem(ID semid, TMO tmout);
extern ER ref_sem(ID semid, T_RSEM *pk_rsem);



/*
 * System Management
 */
typedef struct t_ver {
  UH   maker;     /* vendor */
  UH   id;        /* format number */
  UH   spver;     /* specification version */
  UH   prver;     /* product version */
  UH   prno[4];   /* product control information */
  UH   cpu;       /* CPU information */
  UH   var;       /* variation descriptor */
} T_VER;

typedef struct t_rsys {
  INT   sysstat;   /* system state */
  /* additional information may be included depending on the implementation */
} T_RSYS;

#define TSS_TSK    0   /* normal state in which dispatching is enabled during
                          task portion execution */
#define TSS_DDSP   1   /* state after dis_dsp has been executed during task
                          portion execution (dispatch disabled) */
#define TSS_LOC    3   /* state after loc_cpu has been executed during task
                          portion execution (interrupt and dispatch disabled)
                          */
#define TSS_INDP   4   /* state during execution of task-independent portions
                          (interrupt and timer handlers) */
typedef struct t_rcfg {
  /* details concerning members are implementation dependent */
} T_RCFG;

typedef struct t_dsvc {
  ATR   svcatr;   /* extended SVC handler attributes */
  FP    svchdr;   /* extended SVC handler address */
  /* additional information may be included depending on the implementation */
} T_DSVC;

typedef struct t_dexc {
  ATR   excatr;   /* exception handler attributes */
  FP    exchdr;   /* exception handler address */
  /* additional information may be included depending on the implementation */
} T_DEXC;

extern ER get_ver(T_VER *pk_ver);
extern ER ref_sys(T_RSYS *pk_rsys);
extern ER ref_cfg(T_RCFG *pk_rcfg);
extern ER def_svc(FN s_fncd, T_DSVC *pk_dsvc);
extern ER def_exc(UINT exckind, T_DEXC *pk_dexc);
extern ER dis_dsp();
extern ER ena_dsp();



/*
 *  Tasks
 */
typedef struct t_ctsk {
   VP exinf; /* extended information */
   ATR tskatr; /* task attributes */
   FP task; /* task start address */
   PRI itskpri; /* initial task priority */
   INT stksz; /* stack size */
   VP stk; /* base address of the stack space */

   /* additional information */
   VP extdat;
} T_CTSK;

#define TA_ASM    0x00     /* program written in assembly language */
#define TA_HLNG   0x01     /* program written in high-level language */
#define TA_COP0   0x8000   /* uses coprocessor having ID = 0 */
#define TA_COP1   0x4000   /* uses coprocessor having ID = 1 */
#define TA_COP2   0x2000   /* uses coprocessor having ID = 2 */
#define TA_COP3   0x1000   /* uses coprocessor having ID = 3 */
#define TA_COP4   0x0800   /* uses coprocessor having ID = 4 */
#define TA_COP5   0x0400   /* uses coprocessor having ID = 5 */
#define TA_COP6   0x0200   /* uses coprocessor having ID = 6 */
#define TA_COP7   0x0100   /* uses coprocessor having ID = 7 */

#define TSK_SELF   0   /* task specifies itself */
#define TSK_NONE   0   /* no task */

#define TPRI_INI   0   /* specifies the initial priority on */
                       /*   task startup (chg_pri) */
#define TPRI_RUN   0   /* specifies the highest priority during */
                       /*   execution (rot_rdq) */

#define TSK_NOARG    0  /* No parameter is passed to the task function */
#define TSK_VPARG    1  /* A void pointer parameter is passed as an argument to */
                        /* the task function                                    */

typedef struct t_rtsk {
  VP     exinf;     /* extended information */
  PRI    tskpri;    /* current priority */
  UINT   tskstat;   /* task state */
  UINT   tskwait;   /* cause of wait */
  ID     wid;       /* ID of object being waited for */
  INT    wupcnt;    /* wakeup request count */
  INT    suscnt;    /* SUSPEND request count */
  ATR    tskatr;    /* task attributes */
  FP     task;      /* task start address */
  PRI    itskpri;   /* initial task priority */
  INT    stksz;     /* stack size */
} T_RTSK;

#define TTS_RUN   0x01   /* RUN */
#define TTS_RDY   0x02   /* READY */
#define TTS_WAI   0x04   /* WAIT */
#define TTS_SUS   0x08   /* SUSPEND */
#define TTS_WAS   0x0C   /* WAIT-SUSPEND */
#define TTS_DMT   0x10   /* DORMANT */

#define TTW_SLP   0x0001   /* wait due to slp_tsk or tslp_tsk */
#define TTW_DLY   0x0002   /* wait due to dly_tsk */
#define TTW_NOD   0x0008   /* connection function response wait */
#define TTW_FLG   0x0010   /* wait due to wai_flg or twai_flg */
#define TTW_SEM   0x0020   /* wait due to wai_sem or twai_sem */
#define TTW_MBX   0x0040   /* wait due to rcv_msg or trcv_msg */
#define TTW_SMBF  0x0080   /* wait due to snd_mbf or tsnd_mbf */
#define TTW_MBF   0x0100   /* wait due to rcv_mbf or trcv_mbf */
#define TTW_CAL   0x0200   /* wait for rendezvous call */
#define TTW_ACP   0x0400   /* wait for rendezvous accept */
#define TTW_RDV   0x0800   /* wait for rendezvous completion */
#define TTW_MPL   0x1000   /* wait due to get_mpl or tget_mpl */
#define TTW_MPF   0x2000   /* wait due to get_mpf or tget_mpf */

extern ER act_tsk(ID tskid);
extern ER acre_tsk(T_CTSK *pk_ctsk);
extern ER cre_tsk(ID tskid, T_CTSK *pk_ctsk);
extern ER del_tsk(ID tskid);
extern ER sta_tsk(ID tskid, VP stacd);
extern void ext_tsk(void);
extern void exd_tsk(void);
extern ER ter_tsk(ID tskid);
extern ER dis_dsp(void);
extern ER ena_dsp(void);
extern ER chg_pri(ID tskid, PRI tskpri);
extern ER rot_rdq(PRI tskpri);
extern ER rel_wai(ID tskid);
extern ER get_tid(ID *p_tskid);
extern ER ref_tsk(T_RTSK *pk_rtsk, ID tskid);
extern ER sus_tsk(ID tskid);
extern ER rsm_tsk(ID tskid);
extern ER frsm_tsk(ID tskid);
extern ER slp_tsk(void);
extern ER tslp_tsk(TMO tmout);
extern ER wup_tsk(ID tskid);
extern ER can_wup(INT *p_wupcnt, ID tskid);

#endif //_ITRON_H_

