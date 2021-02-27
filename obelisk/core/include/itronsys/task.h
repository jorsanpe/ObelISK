/*
 * $FILE: task.h
 *
 * ITRON Tasks management
 *
 * Author: Jordi Sánchez <jorsanpe@gmail.com>
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

#ifndef _ITRONSYS_TASK_H
#define _ITRONSYS_TASK_H

/*
 *  Create Task (cre_tsk) Structure
 */

typedef struct t_ctsk {
   VP exinf;   /* extended information */
   ATR tskatr; /* task attributes */
   FP task;    /* task start address */
   PRI itskpri;/* initial task priority */
   INT stksz;  /* stack size */
   VP stk;     /* base address of the stack space */

   /* additional information */
   VP extdat;
} T_CTSK;

/*
 *  Values for the tskatr field
 */

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

/*
 *  Values for the tskid field
 */
#define TSK_SELF   0   /* task specifies itself */
#define TSK_NONE   0   /* no task */

/*
 *  Values for the tskpri field
 */

#define TPRI_INI   0   /* specifies the initial priority on */
/*   task startup (chg_pri) */
#define TPRI_RUN   0   /* specifies the highest priority during */
/*   execution (rot_rdq) */


/*
 *  Reference Task (ref_tsk) Structure
 */

typedef struct t_rtsk {
   STAT tskstat; /* task state */
   PRI tskpri; /* current priority */
   PRI tskbpri; /* base priority */
   STAT tskwait; /* cause of wait */
   ID wobjid; /* ID of object being waited for */
   TMO lefttmo; /* Timeout left */
   UINT actcnt; /* wakeup request count */
   UINT wupcnt; /* wakeup request count */
   UINT suscnt; /* SUSPEND request count */
} T_RTSK;

typedef struct t_rtst {
   STAT tskstat; /* task state */
   STAT tskwait; /* cause of wait */
} T_RTST;

/*
 *  Values for the tskstat field
 */
#define TTS_RUN   0x01   /* RUN */
#define TTS_RDY   0x02   /* READY */
#define TTS_WAI   0x04   /* WAIT */
#define TTS_SUS   0x08   /* SUSPEND */
#define TTS_WAS   0x0C   /* WAIT-SUSPEND */
#define TTS_DMT   0x10   /* DORMANT */

/*
 *  Values for the tskwait field
 */
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

/*
 *  Since the task states given by tskstat and tskwait are expressed
 *  by bit correspondences, they are convenient when looking for OR
 *  conditions (such as whether a task is in WAIT or READY state).
 */

/*
 *  Task Management Function Prototypes
 */
extern ER act_tsk_sys(ID tskid);
extern ER acre_tsk_sys(T_CTSK *pk_ctsk);
extern ER cre_tsk_sys(ID tskid, T_CTSK *pk_ctsk);
extern ER del_tsk_sys(ID tskid);
extern ER sta_tsk_sys(ID tskid, VP stacd);
extern void ext_tsk_sys(void);
extern void exd_tsk_sys(void);
extern ER ter_tsk_sys(ID tskid);
extern ER dis_dsp_sys(void);
extern ER ena_dsp_sys(void);
extern ER chg_pri_sys(ID tskid, PRI tskpri);
extern ER rot_rdq_sys(PRI tskpri);
extern ER rel_wai_sys(ID tskid);
extern ER get_tid_sys(ID *p_tskid);
extern ER ref_tsk_sys(ID tskid, T_RTSK *pk_rtsk);
extern ER ref_tst_sys(ID tskid, T_RTST *pk_rtst);

/*
 *  Task-Dependent Synchronization Functions
 */
extern ER sus_tsk_sys(ID tskid);
extern ER rsm_tsk_sys(ID tskid);
extern ER frsm_tsk_sys(ID tskid);
extern ER slp_tsk_sys(void);
extern ER tslp_tsk_sys(TMO tmout);
extern ER wup_tsk_sys(ID tskid);
extern ER_UINT can_act_sys(ID tskid);
extern ER_UINT can_wup_sys(ID tskid);

#endif
/* end of include file */
