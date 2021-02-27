/*
 * $FILE: fncodes.h
 *
 * ITRON Function codes
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

#ifndef _ITRONSYS_FNCODES_H_
#define _ITRONSYS_FNCODES_H_


/*
 *  Non ITRON function codes (included here for coherency)
 */
#define TFN_SCHEDULER   -0x01
#define TFN_RQ_INSERT   -0x02
#define TFN_RQ_REMOVE   -0x03
#define TFN_RQ_RELOCATE -0x04
#define TFN_CALL_INT    -0xEE
#define TFN_INITCALL    -0xF0

/*
 * ITRON function codes
 */
#define TFN_CRE_TSK  -0x05
#define TFN_DEL_TSK  -0x06
#define TFN_ACT_TSK  -0x07
#define TFN_CAN_ACT  -0x08
#define TFN_STA_TSK  -0x09
#define TFN_EXT_TSK  -0x0a
#define TFN_EXD_TSK  -0x0b
#define TFN_TER_TSK  -0x0c
#define TFN_CHG_PRI  -0x0d
#define TFN_GET_PRI  -0x0e
#define TFN_REF_TSK  -0x0f
#define TFN_REF_TST  -0x10
#define TFN_SLP_TSK  -0x11
#define TFN_TSLP_TSK -0x12
#define TFN_WUP_TSK  -0x13
#define TFN_CAN_WUP  -0x14
#define TFN_REL_WAI  -0x15
#define TFN_SUS_TSK  -0x16
#define TFN_RSM_TSK  -0x17
#define TFN_TRSM_TSK -0x18
#define TFN_DLY_TSK  -0x19
#define TFN_DIS_TEX  -0x1d
#define TFN_CRE_SEM  -0x21
#define TFN_DEL_SEM  -0x22
#define TFN_SIG_SEM  -0x23
#define TFN_WAI_SEM  -0x25
#define TFN_POL_SEM  -0x26
#define TFN_TWAI_SEM -0x27
#define TFN_REF_SEM  -0x28
#define TFN_CRE_FLG  -0x29
#define TFN_DEL_FLG  -0x2a
#define TFN_SET_FLG  -0x2b
#define TFN_CLR_FLG  -0x2c
#define TFN_WAI_FLG  -0x2d
#define TFN_POL_FLG  -0x2e
#define TFN_TWAI_FLG -0x2f
#define TFN_REF_FLG  -0x30
#define TFN_WAI_FLG  -0x2d
#define TFN_CRE_DTQ  -0x31
#define TFN_SND_DTQ  -0x35
#define TFN_RCV_DTQ  -0x39
#define TFN_CRE_MBX  -0x3d
#define TFN_DEL_MBX  -0x3e
#define TFN_SND_MBX  -0x3f
#define TFN_RCV_MBX  -0x41
#define TFN_PRCV_MBX -0x42
#define TFN_TRCV_MBX -0x43
#define TFN_REF_MBX  -0x44
#define TFN_CRE_MPF  -0x45
#define TFN_DEL_MPF  -0x46
#define TFN_REL_MPF  -0x47
#define TFN_GET_MPF  -0x49
#define TFN_PGET_MPF -0x4a
#define TFN_TGET_MPF -0x4b
#define TFN_REF_MPF  -0x4c
#define TFN_SET_TIM  -0x4d
#define TFN_GET_TIM  -0x4e
#define TFN_STA_CYC  -0x51
#define TFN_ROT_RDQ  -0x55
#define TFN_LOC_CPU  -0x59
#define TFN_SNS_CTX  -0x5d
#define TFN_REF_SYS  -0x61
#define TFN_DEF_INH  -0x65
#define TFN_DIS_INT  -0x69
#define TFN_DEF_SVC  -0x6d
#define TFN_IACT_TSK -0x71
#define TFN_IWUP_TSK -0x72
#define TFN_ISIG_SEM -0x75
#define TFN_ISET_FLG -0x76
#define TFN_IROT_RDQ -0x79
#define TFN_ISIG_TIM -0x7d
#define TFN_CRE_MTX  -0x81
#define TFN_LOC_MTX  -0x85
#define TFN_CRE_MBF  -0x89
#define TFN_SND_MBF  -0x8d
#define TFN_RCV_MBF  -0x91
#define TFN_CRE_POR  -0x95
#define TFN_ACP_POR  -0x99
#define TFN_RPL_RDV  -0x9d
#define TFN_CRE_MPL  -0xa1
#define TFN_DEL_MPL  -0xa2
#define TFN_REL_MPL  -0xa3
#define TFN_GET_MPL  -0xa5
#define TFN_PGET_MPL -0xa6
#define TFN_TGET_MPL -0xa7
#define TFN_REF_MPL  -0xa8
#define TFN_GET_MPL  -0xa5
#define TFN_CRE_ALM  -0xa9
#define TFN_REF_ALM  -0xad
#define TFN_DEF_OVR  -0xb1
#define TFN_ACRE_TSK -0xc1
#define TFN_ACRE_SEM -0xc2
#define TFN_ACRE_FLG -0xc3
#define TFN_ACRE_MBX -0xc5
#define TFN_ACRE_MPF -0xc9
#define TFN_ACRE_MPL -0xca
#define TFN_ACRE_ISR -0xcd

// Extended function codes
#define TFN_REA8_MMR  -0xd0
#define TFN_WRI8_MMR  -0xd1
#define TFN_REA16_MMR -0xd2
#define TFN_WRI16_MMR -0xd3
#define TFN_REA32_MMR -0xd4
#define TFN_WRI32_MMR -0xd5

// Sockets
#define TFN_ACCEPT      -0xd8
#define TFN_BIND        -0xd9
#define TFN_SHUTDOWN    -0xda
#define TFN_GETPEERNAME -0xdb
#define TFN_GETSOCKNAME -0xdc
#define TFN_GETSOCKOPT  -0xdd
#define TFN_SETSOCKOPT  -0xde
#define TFN_CLOSE       -0xdf
#define TFN_CONNECT     -0xe0
#define TFN_LISTEN      -0xe1
#define TFN_RECV        -0xe2
#define TFN_READ        -0xe3
#define TFN_RECVFROM    -0xe4
#define TFN_SEND        -0xe5
#define TFN_SENDTO      -0xe6
#define TFN_SOCKET      -0xe7
#define TFN_WRITE       -0xe8
#define TFN_SELECT      -0xe9
#define TFN_IOCTL       -0xea

#endif   //_ITRONSYS_FNCODES_H_
