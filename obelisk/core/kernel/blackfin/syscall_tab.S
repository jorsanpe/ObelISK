/*
 * $FILE: syscall_tab.S
 *
 * System Call Table
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
 *
 */

#include <linkage.h>

.section .syscalltab, "wa"
ENTRY(_syscall_table)
   .long 0
   .long 0
   .long 0
   .long 0
   .long 0
   .long _cre_tsk_sys
   .long _del_tsk_sys
   .long _act_tsk_sys
   .long _can_act_sys
   .long 0//_sta_tsk_sys
   .long _ext_tsk_sys
   .long _exd_tsk_sys
   .long _ter_tsk_sys
   .long _chg_pri_sys
   .long _get_pri_sys
   .long _ref_tsk_sys
   .long _ref_tst_sys
   .long _slp_tsk_sys
   .long _tslp_tsk_sys
   .long _wup_tsk_sys
   .long _can_wup_sys
   .long _rel_wai_sys
   .long 0//_sus_tsk_sys
   .long 0//_rsm_tsk_sys
   .long 0//_trsm_tsk_sys
   .long 0//_dly_tsk_sys
   .long 0
   .long 0
   .long 0
   .long 0//_dis_tex_sys
   .long 0
   .long 0
   .long 0
   .long _cre_sem_sys
   .long _del_sem_sys
   .long _sig_sem_sys
   .long 0
   .long _wai_sem_sys
   .long 0//_pol_sem_sys
   .long _twai_sem_sys
   .long 0//_ref_sem_sys
   .long _cre_flg_sys
   .long _del_flg_sys
   .long _set_flg_sys
   .long _clr_flg_sys
   .long _wai_flg_sys
   .long 0//_pol_flg_sys
   .long 0//_twai_flg_sys
   .long 0//_ref_flg_sys
   .long 0//_cre_dtq_sys
   .long 0
   .long 0
   .long 0
   .long 0//_snd_dtq_sys
   .long 0
   .long 0
   .long 0
   .long 0//_rcv_dtq_sys
   .long 0
   .long 0
   .long 0
   .long _cre_mbx_sys
   .long _del_mbx_sys
   .long _snd_mbx_sys
   .long 0
   .long _rcv_mbx_sys
   .long 0//_prcv_mbx_sys
   .long _trcv_mbx_sys
   .long _ref_mbx_sys
   .long _cre_mpf_sys
   .long _del_mpf_sys
   .long _rel_mpf_sys
   .long 0
   .long _get_mpf_sys
   .long 0//_pget_mpf_sys
   .long 0//_tget_mpf_sys
   .long 0//_ref_mpf_sys
   .long 0//_set_tim_sys
   .long _get_tim_sys
   .long 0
   .long 0
   .long 0//_sta_cyc_sys
   .long 0
   .long 0
   .long 0
   .long 0//_rot_rdq_sys
   .long 0
   .long 0
   .long 0
   .long 0//_loc_cpu_sys
   .long 0
   .long 0
   .long 0
   .long 0//_sns_ctx_sys
   .long 0
   .long 0
   .long 0
   .long 0//_ref_sys_sys
   .long 0
   .long 0
   .long 0
   .long _def_inh_sys
   .long 0
   .long 0
   .long 0
   .long 0//_dis_int_sys
   .long 0
   .long 0
   .long 0
   .long 0//_def_svc_sys
   .long 0
   .long 0
   .long 0//_iact_tsk_sys
   .long 0//_iwup_tsk_sys
   .long 0
   .long 0
   .long 0//_isig_sem_sys
   .long 0//_iset_flg_sys
   .long 0
   .long 0
   .long 0//_irot_rdq_sys
   .long 0
   .long 0
   .long 0
   .long 0//_isig_tim_sys
   .long 0
   .long 0
   .long 0
   .long 0//_cre_mtx_sys
   .long 0
   .long 0
   .long 0
   .long 0//_loc_mtx_sys
   .long 0
   .long 0
   .long 0
   .long 0//_cre_mbf_sys
   .long 0
   .long 0
   .long 0
   .long 0//_snd_mbf_sys
   .long 0
   .long 0
   .long 0
   .long 0//_rcv_mbf_sys
   .long 0
   .long 0
   .long 0
   .long 0//_cre_por_sys
   .long 0
   .long 0
   .long 0
   .long 0//_acp_por_sys
   .long 0
   .long 0
   .long 0
   .long 0//_rpl_rdv_sys
   .long 0
   .long 0
   .long 0
   .long _cre_mpl_sys	// 0xa1
   .long 0//_del_mpl_sys
   .long _rel_mpl_sys
   .long 0
   .long 0
   .long _get_mpl_sys
   .long 0//_pget_mpl_sys
   .long 0//_tget_mpl_sys
   .long 0//_ref_mpl_sys
   .long 0//_cre_alm_sys
   .long 0
   .long 0
   .long 0//_ref_alm_sys
   .long 0
   .long 0
   .long 0
   .long 0//_def_ovr_sys
   .long 0
   .long 0
   .long 0
   .long 0
   .long 0
   .long 0
   .long 0
   .long 0
   .long 0
   .long 0
   .long 0
   .long 0
   .long 0
   .long 0
   .long 0
   .long 0
   .long _acre_tsk_sys
   .long _acre_sem_sys
   .long 0//_acre_flg_sys
   .long 0
   .long _acre_mbx_sys
   .long 0
   .long 0
   .long 0
   .long 0//_acre_mpf_sys
   .long 0//_acre_mpl_sys
   .long 0
   .long 0
   .long 0//_acre_isr_sys //0xcd
   .long 0
   .long 0
   .long _rea8_mmr_sys    //0xd0
   .long _wri8_mmr_sys
   .long _rea16_mmr_sys
   .long _wri16_mmr_sys
   .long _rea32_mmr_sys
   .long _wri32_mmr_sys
   .long 0
   .long 0
   .long _lwip_accept     //0xd8
   .long _lwip_bind
   .long _lwip_shutdown
   .long _lwip_getpeername
   .long _lwip_getsockname
   .long _lwip_getsockopt
   .long _lwip_setsockopt
   .long _lwip_close
   .long _lwip_connect
   .long _lwip_listen
   .long _lwip_recv
   .long _lwip_read
   .long _lwip_recvfrom
   .long _lwip_send
   .long _lwip_sendto
   .long _lwip_socket
   .long _lwip_write
   .long _lwip_select
   .long _lwip_ioctl

END(syscall_table)
.previous
