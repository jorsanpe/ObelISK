/*
 * $FILE: smc9218.h
 *
 * SMSC LAN9218 driver header
 *
 * Author: Jordi Sánchez, <jorsanp3@upvnet.upv.es>
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
#ifndef _SMC9218_H_
#define _SMC9218_H_

#include <arch/arch_types.h>
#include <processor.h>
#include <lwip/pbuf.h>

// The base address for the Blackfin BF548 processor
#define CONFIG_DRIVER_SMC911X_BASE              0x24000000

#define RX_DATA_FIFO             (CONFIG_DRIVER_SMC911X_BASE + 0x00)

#define TX_DATA_FIFO             (CONFIG_DRIVER_SMC911X_BASE + 0x20)
#define TX_CMD_A_INT_ON_COMP                    0x80000000
#define TX_CMD_A_INT_BUF_END_ALGN               0x03000000
#define TX_CMD_A_INT_4_BYTE_ALGN                0x00000000
#define TX_CMD_A_INT_16_BYTE_ALGN               0x01000000
#define TX_CMD_A_INT_32_BYTE_ALGN               0x02000000
#define TX_CMD_A_INT_DATA_OFFSET                0x001F0000
#define TX_CMD_A_INT_FIRST_SEG                  0x00002000
#define TX_CMD_A_INT_LAST_SEG                   0x00001000
#define TX_CMD_A_BUF_SIZE                       0x000007FF
#define TX_CMD_B_PKT_TAG                        0xFFFF0000
#define TX_CMD_B_ADD_CRC_DISABLE                0x00002000
#define TX_CMD_B_DISABLE_PADDING                0x00001000
#define TX_CMD_B_PKT_BYTE_LENGTH                0x000007FF

#define RX_STATUS_FIFO          (CONFIG_DRIVER_SMC911X_BASE + 0x40)
#define RX_STS_PKT_LEN                          0x3FFF0000
#define RX_STS_ES                               0x00008000
#define RX_STS_BCST                             0x00002000
#define RX_STS_LEN_ERR                          0x00001000
#define RX_STS_RUNT_ERR                         0x00000800
#define RX_STS_MCAST                            0x00000400
#define RX_STS_TOO_LONG                         0x00000080
#define RX_STS_COLL                             0x00000040
#define RX_STS_ETH_TYPE                         0x00000020
#define RX_STS_WDOG_TMT                         0x00000010
#define RX_STS_MII_ERR                          0x00000008
#define RX_STS_DRIBBLING                        0x00000004
#define RX_STS_CRC_ERR                          0x00000002

#define RX_STATUS_FIFO_PEEK     (CONFIG_DRIVER_SMC911X_BASE + 0x44)

#define TX_STATUS_FIFO          (CONFIG_DRIVER_SMC911X_BASE + 0x48)
#define TX_STS_TAG                              0xFFFF0000
#define TX_STS_ES                               0x00008000
#define TX_STS_LOC                              0x00000800
#define TX_STS_NO_CARR                          0x00000400
#define TX_STS_LATE_COLL                        0x00000200
#define TX_STS_MANY_COLL                        0x00000100
#define TX_STS_COLL_CNT                         0x00000078
#define TX_STS_MANY_DEFER                       0x00000004
#define TX_STS_UNDERRUN                         0x00000002
#define TX_STS_DEFERRED                         0x00000001

#define TX_STATUS_FIFO_PEEK     (CONFIG_DRIVER_SMC911X_BASE + 0x4C)

#define ID_REV                  (CONFIG_DRIVER_SMC911X_BASE + 0x50)
#define ID_REV_CHIP_ID                          0xFFFF0000  /* RO */
#define ID_REV_REV_ID                           0x0000FFFF  /* RO */

#define INT_CFG                 (CONFIG_DRIVER_SMC911X_BASE + 0x54)
#define INT_CFG_INT_DEAS                        0xFF000000  /* R/W */
#define INT_CFG_INT_DEAS_CLR                    0x00004000
#define INT_CFG_INT_DEAS_STS                    0x00002000
#define INT_CFG_IRQ_INT                         0x00001000  /* RO */
#define INT_CFG_IRQ_EN                          0x00000100  /* R/W */
#define INT_CFG_IRQ_POL                         0x00000010  /* R/W Not Affected by SW Reset */
#define INT_CFG_IRQ_TYPE                        0x00000001  /* R/W Not Affected by SW Reset */

#define INT_STS                 (CONFIG_DRIVER_SMC911X_BASE + 0x58)
#define INT_STS_SW_INT                          0x80000000  /* R/WC */
#define INT_STS_TXSTOP_INT                      0x02000000  /* R/WC */
#define INT_STS_RXSTOP_INT                      0x01000000  /* R/WC */
#define INT_STS_RXDFH_INT                       0x00800000  /* R/WC */
#define INT_STS_RXDF_INT                        0x00400000  /* R/WC */
#define INT_STS_TX_IOC                          0x00200000  /* R/WC */
#define INT_STS_RXD_INT                         0x00100000  /* R/WC */
#define INT_STS_GPT_INT                         0x00080000  /* R/WC */
#define INT_STS_PHY_INT                         0x00040000  /* RO */
#define INT_STS_PME_INT                         0x00020000  /* R/WC */
#define INT_STS_TXSO                            0x00010000  /* R/WC */
#define INT_STS_RWT                             0x00008000  /* R/WC */
#define INT_STS_RXE                             0x00004000  /* R/WC */
#define INT_STS_TXE                             0x00002000  /* R/WC */
/*#define       INT_STS_ERX             0x00001000*//* R/WC */
#define INT_STS_TDFU                            0x00000800  /* R/WC */
#define INT_STS_TDFO                            0x00000400  /* R/WC */
#define INT_STS_TDFA                            0x00000200  /* R/WC */
#define INT_STS_TSFF                            0x00000100  /* R/WC */
#define INT_STS_TSFL                            0x00000080  /* R/WC */
/*#define       INT_STS_RXDF            0x00000040*//* R/WC */
#define INT_STS_RDFO                            0x00000040  /* R/WC */
#define INT_STS_RDFL                            0x00000020  /* R/WC */
#define INT_STS_RSFF                            0x00000010  /* R/WC */
#define INT_STS_RSFL                            0x00000008  /* R/WC */
#define INT_STS_GPIO2_INT                       0x00000004  /* R/WC */
#define INT_STS_GPIO1_INT                       0x00000002  /* R/WC */
#define INT_STS_GPIO0_INT                       0x00000001  /* R/WC */

#define INT_EN                  (CONFIG_DRIVER_SMC911X_BASE + 0x5C)
#define INT_EN_SW_INT_EN                        0x80000000  /* R/W */
#define INT_EN_TXSTOP_INT_EN                    0x02000000  /* R/W */
#define INT_EN_RXSTOP_INT_EN                    0x01000000  /* R/W */
#define INT_EN_RXDFH_INT_EN                     0x00800000  /* R/W */
/*#define       INT_EN_RXDF_INT_EN              0x00400000*//* R/W */
#define INT_EN_TIOC_INT_EN                      0x00200000  /* R/W */
#define INT_EN_RXD_INT_EN                       0x00100000  /* R/W */
#define INT_EN_GPT_INT_EN                       0x00080000  /* R/W */
#define INT_EN_PHY_INT_EN                       0x00040000  /* R/W */
#define INT_EN_PME_INT_EN                       0x00020000  /* R/W */
#define INT_EN_TXSO_EN                          0x00010000  /* R/W */
#define INT_EN_RWT_EN                           0x00008000  /* R/W */
#define INT_EN_RXE_EN                           0x00004000  /* R/W */
#define INT_EN_TXE_EN                           0x00002000  /* R/W */
/*#define       INT_EN_ERX_EN                   0x00001000*//* R/W */
#define INT_EN_TDFU_EN                          0x00000800  /* R/W */
#define INT_EN_TDFO_EN                          0x00000400  /* R/W */
#define INT_EN_TDFA_EN                          0x00000200  /* R/W */
#define INT_EN_TSFF_EN                          0x00000100  /* R/W */
#define INT_EN_TSFL_EN                          0x00000080  /* R/W */
/*#define       INT_EN_RXDF_EN                  0x00000040*//* R/W */
#define INT_EN_RDFO_EN                          0x00000040  /* R/W */
#define INT_EN_RDFL_EN                          0x00000020  /* R/W */
#define INT_EN_RSFF_EN                          0x00000010  /* R/W */
#define INT_EN_RSFL_EN                          0x00000008  /* R/W */
#define INT_EN_GPIO2_INT                        0x00000004  /* R/W */
#define INT_EN_GPIO1_INT                        0x00000002  /* R/W */
#define INT_EN_GPIO0_INT                        0x00000001  /* R/W */

#define BYTE_TEST               (CONFIG_DRIVER_SMC911X_BASE + 0x64)

#define FIFO_INT                (CONFIG_DRIVER_SMC911X_BASE + 0x68)
#define FIFO_INT_TX_AVAIL_LEVEL                 0xFF000000  /* R/W */
#define FIFO_INT_TX_STS_LEVEL                   0x00FF0000  /* R/W */
#define FIFO_INT_RX_AVAIL_LEVEL                 0x0000FF00  /* R/W */
#define FIFO_INT_RX_STS_LEVEL                   0x000000FF  /* R/W */

#define RX_CFG                  (CONFIG_DRIVER_SMC911X_BASE + 0x6C)
#define RX_CFG_RX_END_ALGN                      0xC0000000  /* R/W */
#define         RX_CFG_RX_END_ALGN4             0x00000000  /* R/W */
#define         RX_CFG_RX_END_ALGN16            0x40000000  /* R/W */
#define         RX_CFG_RX_END_ALGN32            0x80000000  /* R/W */
#define RX_CFG_RX_DMA_CNT                       0x0FFF0000  /* R/W */
#define RX_CFG_RX_DUMP                          0x00008000  /* R/W */
#define RX_CFG_RXDOFF                           0x00001F00  /* R/W */
/*#define       RX_CFG_RXBAD                    0x00000001*//* R/W */

#define TX_CFG                  (CONFIG_DRIVER_SMC911X_BASE + 0x70)
/*#define       TX_CFG_TX_DMA_LVL               0xE0000000*//* R/W */
/*#define       TX_CFG_TX_DMA_CNT               0x0FFF0000*//* R/W Self Clearing */
#define TX_CFG_TXS_DUMP                         0x00008000  /* Self Clearing */
#define TX_CFG_TXD_DUMP                         0x00004000  /* Self Clearing */
#define TX_CFG_TXSAO                            0x00000004  /* R/W */
#define TX_CFG_TX_ON                            0x00000002  /* R/W */
#define TX_CFG_STOP_TX                          0x00000001  /* Self Clearing */

#define HW_CFG                  (CONFIG_DRIVER_SMC911X_BASE + 0x74)
#define HW_CFG_TTM                              0x00200000  /* R/W */
#define HW_CFG_SF                               0x00100000  /* R/W */
#define HW_CFG_TX_FIF_SZ                        0x000F0000  /* R/W */
#define HW_CFG_TR                               0x00003000  /* R/W */
#define HW_CFG_PHY_CLK_SEL                      0x00000060  /* R/W */
#define HW_CFG_PHY_CLK_SEL_INT_PHY              0x00000000 /* R/W */
#define HW_CFG_PHY_CLK_SEL_EXT_PHY              0x00000020 /* R/W */
#define HW_CFG_PHY_CLK_SEL_CLK_DIS              0x00000040 /* R/W */
#define HW_CFG_SMI_SEL                          0x00000010  /* R/W */
#define HW_CFG_EXT_PHY_DET                      0x00000008  /* RO */
#define HW_CFG_EXT_PHY_EN                       0x00000004  /* R/W */
#define HW_CFG_32_16_BIT_MODE                   0x00000004  /* RO */
#define HW_CFG_SRST_TO                          0x00000002  /* RO */
#define HW_CFG_SRST                             0x00000001  /* Self Clearing */

#define RX_DP_CTRL              (CONFIG_DRIVER_SMC911X_BASE + 0x78)
#define RX_DP_CTRL_RX_FFWD                      0x80000000  /* R/W */
#define RX_DP_CTRL_FFWD_BUSY                    0x80000000  /* RO */

#define RX_FIFO_INF             (CONFIG_DRIVER_SMC911X_BASE + 0x7C)
#define RX_FIFO_INF_RXSUSED                     0x00FF0000  /* RO */
#define RX_FIFO_INF_RXDUSED                     0x0000FFFF  /* RO */

#define TX_FIFO_INF             (CONFIG_DRIVER_SMC911X_BASE + 0x80)
#define TX_FIFO_INF_TSUSED                      0x00FF0000  /* RO */
#define TX_FIFO_INF_TDFREE                      0x0000FFFF  /* RO */

#define PMT_CTRL                (CONFIG_DRIVER_SMC911X_BASE + 0x84)
#define PMT_CTRL_PM_MODE                        0x00003000  /* Self Clearing */
#define PMT_CTRL_PHY_RST                        0x00000400  /* Self Clearing */
#define PMT_CTRL_WOL_EN                         0x00000200  /* R/W */
#define PMT_CTRL_ED_EN                          0x00000100  /* R/W */
#define PMT_CTRL_PME_TYPE                       0x00000040  /* R/W Not Affected by SW Reset */
#define PMT_CTRL_WUPS                           0x00000030  /* R/WC */
#define PMT_CTRL_WUPS_NOWAKE                    0x00000000  /* R/WC */
#define PMT_CTRL_WUPS_ED                        0x00000010  /* R/WC */
#define PMT_CTRL_WUPS_WOL                       0x00000020  /* R/WC */
#define PMT_CTRL_WUPS_MULTI                     0x00000030  /* R/WC */
#define PMT_CTRL_PME_IND                        0x00000008  /* R/W */
#define PMT_CTRL_PME_POL                        0x00000004  /* R/W */
#define PMT_CTRL_PME_EN                         0x00000002  /* R/W Not Affected by SW Reset */
#define PMT_CTRL_READY                          0x00000001  /* RO */

#define GPIO_CFG                (CONFIG_DRIVER_SMC911X_BASE + 0x88)
#define GPIO_CFG_LED3_EN                        0x40000000  /* R/W */
#define GPIO_CFG_LED2_EN                        0x20000000  /* R/W */
#define GPIO_CFG_LED1_EN                        0x10000000  /* R/W */
#define GPIO_CFG_GPIO2_INT_POL                  0x04000000  /* R/W */
#define GPIO_CFG_GPIO1_INT_POL                  0x02000000  /* R/W */
#define GPIO_CFG_GPIO0_INT_POL                  0x01000000  /* R/W */
#define GPIO_CFG_EEPR_EN                        0x00700000  /* R/W */
#define GPIO_CFG_GPIOBUF2                       0x00040000  /* R/W */
#define GPIO_CFG_GPIOBUF1                       0x00020000  /* R/W */
#define GPIO_CFG_GPIOBUF0                       0x00010000  /* R/W */
#define GPIO_CFG_GPIODIR2                       0x00000400  /* R/W */
#define GPIO_CFG_GPIODIR1                       0x00000200  /* R/W */
#define GPIO_CFG_GPIODIR0                       0x00000100  /* R/W */
#define GPIO_CFG_GPIOD4                         0x00000010  /* R/W */
#define GPIO_CFG_GPIOD3                         0x00000008  /* R/W */
#define GPIO_CFG_GPIOD2                         0x00000004  /* R/W */
#define GPIO_CFG_GPIOD1                         0x00000002  /* R/W */
#define GPIO_CFG_GPIOD0                         0x00000001  /* R/W */

#define GPT_CFG                 (CONFIG_DRIVER_SMC911X_BASE + 0x8C)
#define GPT_CFG_TIMER_EN                        0x20000000  /* R/W */
#define GPT_CFG_GPT_LOAD                        0x0000FFFF  /* R/W */

#define GPT_CNT                 (CONFIG_DRIVER_SMC911X_BASE + 0x90)
#define GPT_CNT_GPT_CNT                         0x0000FFFF  /* RO */

#define ENDIAN                  (CONFIG_DRIVER_SMC911X_BASE + 0x98)
#define FREE_RUN                (CONFIG_DRIVER_SMC911X_BASE + 0x9C)
#define RX_DROP                 (CONFIG_DRIVER_SMC911X_BASE + 0xA0)
#define MAC_CSR_CMD             (CONFIG_DRIVER_SMC911X_BASE + 0xA4)
#define  MAC_CSR_CMD_CSR_BUSY                   0x80000000  /* Self Clearing */
#define  MAC_CSR_CMD_R_NOT_W                    0x40000000  /* R/W */
#define  MAC_CSR_CMD_CSR_ADDR                   0x000000FF  /* R/W */

#define MAC_CSR_DATA            (CONFIG_DRIVER_SMC911X_BASE + 0xA8)
#define AFC_CFG                 (CONFIG_DRIVER_SMC911X_BASE + 0xAC)
#define         AFC_CFG_AFC_HI                  0x00FF0000  /* R/W */
#define         AFC_CFG_AFC_LO                  0x0000FF00  /* R/W */
#define         AFC_CFG_BACK_DUR                0x000000F0  /* R/W */
#define         AFC_CFG_FCMULT                  0x00000008  /* R/W */
#define         AFC_CFG_FCBRD                   0x00000004  /* R/W */
#define         AFC_CFG_FCADD                   0x00000002  /* R/W */
#define         AFC_CFG_FCANY                   0x00000001  /* R/W */

#define E2P_CMD                 (CONFIG_DRIVER_SMC911X_BASE + 0xB0)
#define         E2P_CMD_EPC_BUSY                0x80000000  /* Self Clearing */
#define         E2P_CMD_EPC_CMD                 0x70000000  /* R/W */
#define         E2P_CMD_EPC_CMD_READ            0x00000000  /* R/W */
#define         E2P_CMD_EPC_CMD_EWDS            0x10000000  /* R/W */
#define         E2P_CMD_EPC_CMD_EWEN            0x20000000  /* R/W */
#define         E2P_CMD_EPC_CMD_WRITE           0x30000000  /* R/W */
#define         E2P_CMD_EPC_CMD_WRAL            0x40000000  /* R/W */
#define         E2P_CMD_EPC_CMD_ERASE           0x50000000  /* R/W */
#define         E2P_CMD_EPC_CMD_ERAL            0x60000000  /* R/W */
#define         E2P_CMD_EPC_CMD_RELOAD          0x70000000  /* R/W */
#define         E2P_CMD_EPC_TIMEOUT             0x00000200  /* RO */
#define         E2P_CMD_MAC_ADDR_LOADED         0x00000100  /* RO */
#define         E2P_CMD_EPC_ADDR                0x000000FF  /* R/W */

#define E2P_DATA                (CONFIG_DRIVER_SMC911X_BASE + 0xB4)
#define E2P_DATA_EEPROM_DATA                    0x000000FF  /* R/W */
/* end of LAN register offsets and bit definitions */

/* MAC Control and Status registers */
#define MAC_CR                  0x01  /* R/W */

/* MAC_CR - MAC Control Register */
#define MAC_CR_RXALL                    0x80000000
/* TODO: delete this bit? It is not described in the data sheet. */
#define MAC_CR_HBDIS                    0x10000000
#define MAC_CR_RCVOWN                   0x00800000
#define MAC_CR_LOOPBK                   0x00200000
#define MAC_CR_FDPX                     0x00100000
#define MAC_CR_MCPAS                    0x00080000
#define MAC_CR_PRMS                     0x00040000
#define MAC_CR_INVFILT                  0x00020000
#define MAC_CR_PASSBAD                  0x00010000
#define MAC_CR_HFILT                    0x00008000
#define MAC_CR_HPFILT                   0x00002000
#define MAC_CR_LCOLL                    0x00001000
#define MAC_CR_BCAST                    0x00000800
#define MAC_CR_DISRTY                   0x00000400
#define MAC_CR_PADSTR                   0x00000100
#define MAC_CR_BOLMT_MASK               0x000000C0
#define MAC_CR_DFCHK                    0x00000020
#define MAC_CR_TXEN                     0x00000008
#define MAC_CR_RXEN                     0x00000004

#define ADDRH                   0x02      /* R/W mask 0x0000FFFFUL */
#define ADDRL                   0x03      /* R/W mask 0xFFFFFFFFUL */
#define HASHH                   0x04      /* R/W */
#define HASHL                   0x05      /* R/W */

#define MII_ACC                 0x06      /* R/W */
#define MII_ACC_PHY_ADDR                0x0000F800
#define MII_ACC_MIIRINDA                0x000007C0
#define MII_ACC_MII_WRITE               0x00000002
#define MII_ACC_MII_BUSY                0x00000001

#define MII_DATA                0x07      /* R/W mask 0x0000FFFFUL */

#define FLOW                    0x08      /* R/W */
#define FLOW_FCPT                       0xFFFF0000
#define FLOW_FCPASS                     0x00000004
#define FLOW_FCEN                       0x00000002
#define FLOW_FCBSY                      0x00000001

#define VLAN1                   0x09      /* R/W mask 0x0000FFFFUL */
#define VLAN1_VTI1                      0x0000ffff

#define VLAN2                   0x0A      /* R/W mask 0x0000FFFFUL */
#define VLAN2_VTI2                      0x0000ffff

#define WUFF                    0x0B      /* WO */

#define WUCSR                   0x0C      /* R/W */
#define WUCSR_GUE                       0x00000200
#define WUCSR_WUFR                      0x00000040
#define WUCSR_MPR                       0x00000020
#define WUCSR_WAKE_EN                   0x00000004
#define WUCSR_MPEN                      0x00000002


/* phy seed setup */
#define AUTO         99
#define _1000BASET      1000
#define _100BASET    100
#define _10BASET     10
#define HALF         22
#define FULL         44

/* phy register offsets */
#define PHY_BMCR     0x00
#define PHY_BMSR     0x01
#define PHY_PHYIDR1     0x02
#define PHY_PHYIDR2     0x03
#define PHY_ANAR     0x04
#define PHY_ANLPAR      0x05
#define PHY_ANER     0x06
#define PHY_ANNPTR      0x07
#define PHY_ANLPNP      0x08
#define PHY_1000BTCR    0x09
#define PHY_1000BTSR    0x0A
#define PHY_EXSR     0x0F
#define PHY_PHYSTS      0x10
#define PHY_MIPSCR      0x11
#define PHY_MIPGSR      0x12
#define PHY_DCR         0x13
#define PHY_FCSCR    0x14
#define PHY_RECR     0x15
#define PHY_PCSR     0x16
#define PHY_LBR         0x17
#define PHY_10BTSCR     0x18
#define PHY_PHYCTRL     0x19

/* PHY BMCR */
#define PHY_BMCR_RESET     0x8000
#define PHY_BMCR_LOOP      0x4000
#define PHY_BMCR_100MB     0x2000
#define PHY_BMCR_AUTON     0x1000
#define PHY_BMCR_POWD      0x0800
#define PHY_BMCR_ISO    0x0400
#define PHY_BMCR_RST_NEG   0x0200
#define PHY_BMCR_DPLX      0x0100
#define PHY_BMCR_COL_TST   0x0080

#define PHY_BMCR_SPEED_MASK   0x2040
#define PHY_BMCR_1000_MBPS 0x0040
#define PHY_BMCR_100_MBPS  0x2000
#define PHY_BMCR_10_MBPS   0x0000

/* phy BMSR */
#define PHY_BMSR_100T4     0x8000
#define PHY_BMSR_100TXF    0x4000
#define PHY_BMSR_100TXH    0x2000
#define PHY_BMSR_10TF      0x1000
#define PHY_BMSR_10TH      0x0800
#define PHY_BMSR_EXT_STAT  0x0100
#define PHY_BMSR_PRE_SUP   0x0040
#define PHY_BMSR_AUTN_COMP 0x0020
#define PHY_BMSR_RF     0x0010
#define PHY_BMSR_AUTN_ABLE 0x0008
#define PHY_BMSR_LS     0x0004
#define PHY_BMSR_JD     0x0002
#define PHY_BMSR_EXT    0x0001

/*phy ANLPAR */
#define PHY_ANLPAR_NP      0x8000
#define PHY_ANLPAR_ACK     0x4000
#define PHY_ANLPAR_RF      0x2000
#define PHY_ANLPAR_ASYMP   0x0800
#define PHY_ANLPAR_PAUSE   0x0400
#define PHY_ANLPAR_T4      0x0200
#define PHY_ANLPAR_TXFD    0x0100
#define PHY_ANLPAR_TX      0x0080
#define PHY_ANLPAR_10FD    0x0040
#define PHY_ANLPAR_10      0x0020
#define PHY_ANLPAR_100     0x0380   /* we can run at 100 */
/* phy ANLPAR 1000BASE-X */
#define PHY_X_ANLPAR_NP    0x8000
#define PHY_X_ANLPAR_ACK   0x4000
#define PHY_X_ANLPAR_RF_MASK  0x3000
#define PHY_X_ANLPAR_PAUSE_MASK  0x0180
#define PHY_X_ANLPAR_HD    0x0040
#define PHY_X_ANLPAR_FD    0x0020

#define PHY_ANLPAR_PSB_MASK   0x001f
#define PHY_ANLPAR_PSB_802_3  0x0001
#define PHY_ANLPAR_PSB_802_9  0x0002

/* phy 1000BTCR */
#define PHY_1000BTCR_1000FD   0x0200
#define PHY_1000BTCR_1000HD   0x0100

/* phy 1000BTSR */
#define PHY_1000BTSR_MSCF  0x8000
#define PHY_1000BTSR_MSCR  0x4000
#define PHY_1000BTSR_LRS   0x2000
#define PHY_1000BTSR_RRS   0x1000
#define PHY_1000BTSR_1000FD   0x0800
#define PHY_1000BTSR_1000HD   0x0400

/* phy EXSR */
#define PHY_EXSR_1000XF    0x8000
#define PHY_EXSR_1000XH    0x4000
#define PHY_EXSR_1000TF    0x2000
#define PHY_EXSR_1000TH    0x1000


// Link speed
#define LINK_SPEED_100MBPS       (100000000)
#define LINK_SPEED_10MBPS         (10000000)

/* Chip ID values */
#define CHIP_9115       0x115
#define CHIP_9116       0x116
#define CHIP_9117       0x117
#define CHIP_9118       0x118
#define CHIP_9215       0x115a
#define CHIP_9216       0x116a
#define CHIP_9217       0x117a
#define CHIP_9218       0x118a

struct chip_id {
   uint16_t id;
   char *name;
};

static const struct chip_id chip_ids[] = { { CHIP_9115, "LAN9115" }, {
      CHIP_9116, "LAN9116" }, { CHIP_9117, "LAN9117" },
      { CHIP_9118, "LAN9118" }, { CHIP_9215, "LAN9215" }, { CHIP_9216,
            "LAN9216" }, { CHIP_9217, "LAN9217" }, { CHIP_9218, "LAN9218" }, {
            0, NULL }, };

/* Return codes */
#define ESMC_OK            (0)
#define ESMC_INIT          (-1)
#define ESMC_SEND          (-2)
#define ESMC_RCV           (-3)

/* Interrupt events */
#define ESMC_INT_ERROR     (-2)
#define ESMC_INT_UNKNOWN   (-1)
#define ESMC_INT_RX        (0)
#define ESMC_INT_TX        (1)

#define DRIVERNAME "smc911x"

/*extern void eth_halt(void);
extern int32_t eth_rx(struct pbuf **pbchain);
extern int32_t eth_init(uint8_t *mac);
extern int32_t eth_send(volatile void *packet, int32_t length);*/
extern void smc911x_handle_int(void);

#endif //_SMC9218_H_
