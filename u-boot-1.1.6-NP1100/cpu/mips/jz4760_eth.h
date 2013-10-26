/*
 *  Jz4760 On-Chip ethernet driver.
 *
 *  Copyright (C) 2005 - 2007  Ingenic Semiconductor Inc. Jason<xwang@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef __JZ4760_ETH_H__
#define __JZ4760_ETH_H__


#define REG16(addr)	*((volatile unsigned short *)(addr))
#define REG32(addr)	*((volatile unsigned int *)(addr))

/* Register Map:
   ---------------------------------------------------------------------------------------------------
   0x000h to 0x04bh	PE-MACMII	(include 4 byte (0x03ch to 0x03fh) for M-MIIFIF.ETH_FIFO_CR0
   ---------------------------------------------------------------------------------------------------
   0x04ch to 0x05fh	M-MIIFIF	(begin with ETH_FIFO_CR1)
   ---------------------------------------------------------------------------------------------------
*/


// ------------------------------------------------------------------------
// Register defination
// ------------------------------------------------------------------------

// M-AHBE DMA control and status registers (32-bits)
#define ETH_DMA_TCR			(ETHC_BASE + 0x180)	// Transmit control
#define ETH_DMA_TDR			(ETHC_BASE + 0x184)	// Pointer to Transmit Descriptor
#define ETH_DMA_TSR			(ETHC_BASE + 0x188)	// Transmit Status
#define ETH_DMA_RCR			(ETHC_BASE + 0x18c)	// Receive Control
#define ETH_DMA_RDR			(ETHC_BASE + 0x190)	// Pointer to Receive Descriptor
#define ETH_DMA_RSR			(ETHC_BASE + 0x194)	// Receive Status
#define ETH_DMA_IMR			(ETHC_BASE + 0x198)	// Interrupt Mask
#define ETH_DMA_IR			(ETHC_BASE + 0x19c)	// Interrupts

// M-MIIFIF (32-bits)
#define ETH_FIFO_CR0			(ETHC_BASE + (0x0f << 2))// Configuration Register 0
#define ETH_FIFO_CR1			(ETHC_BASE + (0x13 << 2))// Configuration Register 1
#define ETH_FIFO_CR2			(ETHC_BASE + (0x14 << 2))// Configuration Register 2
#define ETH_FIFO_CR3			(ETHC_BASE + (0x15 << 2))// Configuration Register 3
#define ETH_FIFO_CR4			(ETHC_BASE + (0x16 << 2))// Configuration Register 4
#define ETH_FIFO_CR5			(ETHC_BASE + (0x17 << 2))// Configuration Register 5

#define ETH_FIFO_RAR0			(ETHC_BASE + (0x18 << 2))// RAM Access Register 0
#define ETH_FIFO_RAR1			(ETHC_BASE + (0x19 << 2))// RAM Access Register 1
#define ETH_FIFO_RAR2			(ETHC_BASE + (0x1a << 2))// RAM Access Register 2
#define ETH_FIFO_RAR3			(ETHC_BASE + (0x1b << 2))// RAM Access Register 3
#define ETH_FIFO_RAR4			(ETHC_BASE + (0x1c << 2))// RAM Access Register 4
#define ETH_FIFO_RAR5			(ETHC_BASE + (0x1d << 2))// RAM Access Register 5
#define ETH_FIFO_RAR6			(ETHC_BASE + (0x1e << 2))// RAM Access Register 6
#define ETH_FIFO_RAR7			(ETHC_BASE + (0x1f << 2))// RAM Access Register 7

// PE-MACMII registers (16-bits)
#define ETH_MAC_MCR1			(ETHC_BASE + (0x00 << 2))// MAC configuration register #1
#define ETH_MAC_MCR2			(ETHC_BASE + (0x01 << 2))// MAC configuration register #2
#define ETH_MAC_IPGR			(ETHC_BASE + (0x02 << 2))// Back-to-Back Inter-Packet-Gap register
#define ETH_MAC_NIPGR			(ETHC_BASE + (0x03 << 2))// Non-Back-to-Back Inter-Packet-Gap register
#define ETH_MAC_CWR			(ETHC_BASE + (0x04 << 2))// Collision Window / Retry register
#define ETH_MAC_MFR			(ETHC_BASE + (0x05 << 2))// Maximum Frame register
#define ETH_MAC_PSR			(ETHC_BASE + (0x06 << 2))// PHY Support register (SMII / RMII / PMD / ENDEC)
#define ETH_MAC_TR			(ETHC_BASE + (0x07 << 2))// Test register
#define ETH_MAC_MCFGR			(ETHC_BASE + (0x08 << 2))// MII Mgmt Configuration
#define ETH_MAC_MCMDR			(ETHC_BASE + (0x09 << 2))// MII Mgmt Command
#define ETH_MAC_MADRR			(ETHC_BASE + (0x0a << 2))// MII Mgmt Address
#define ETH_MAC_MWTDR			(ETHC_BASE + (0x0b << 2))// MII Mgmt Write Data
#define ETH_MAC_MRDDR			(ETHC_BASE + (0x0c << 2))// MII Mgmt Read Data
#define ETH_MAC_MINDR			(ETHC_BASE + (0x0d << 2))// MII Mgmt Indicators
#define ETH_MAC_SA0			(ETHC_BASE + (0x10 << 2))// Station Address
#define ETH_MAC_SA1			(ETHC_BASE + (0x11 << 2))// Station Address
#define ETH_MAC_SA2			(ETHC_BASE + (0x12 << 2))// Station Address

// PE-MSTAT registers (32-bits)
/* Combined transmit and receive counters. [17 : 0] available */
#define ETH_STAT_TR64			(ETHC_BASE + 0x80 )// Tx & Rx 64          bytes frame counter
#define ETH_STAT_TR127			(ETHC_BASE + 0x84 )// Tx & Rx 65 ~ 127    bytes frame counter
#define ETH_STAT_TR255			(ETHC_BASE + 0x88 )// Tx & Rx 128 ~ 255   bytes frame counter
#define ETH_STAT_TR511			(ETHC_BASE + 0x8c )// Tx & Rx 256 ~ 511   bytes frame counter
#define ETH_STAT_TR1K			(ETHC_BASE + 0x90 )// Tx & Rx 512 ~ 1023  bytes frame counter
#define ETH_STAT_TRMAX			(ETHC_BASE + 0x94 )// Tx & Rx 1024 ~ 1518 bytes frame counter
#define ETH_STAT_TRMGV			(ETHC_BASE + 0x98 )// Tx & Rx 1519 ~ 1522 bytes good VLAN frame counter

/* Receive counters */ 
#define ETH_STAT_RBYT			(ETHC_BASE + 0x9c )// Rx Byte counter				[23 : 0]
#define ETH_STAT_RPKT			(ETHC_BASE + 0xa0 )// Rx Packet counter				[17 : 0]
#define ETH_STAT_RFCS			(ETHC_BASE + 0xa4 )// Rx FCS Error counter			[11 : 0]
#define ETH_STAT_RMCA			(ETHC_BASE + 0xa8 )// Rx Multicast packet counter		[17 : 0]

#define ETH_STAT_RBCA			(ETHC_BASE + 0xac )// Rx Broadcast packet counter		[21 : 0]
#define ETH_STAT_RXCF			(ETHC_BASE + 0xb0 )// Rx Control frame packet counter		[17 : 0]
#define ETH_STAT_RXPF			(ETHC_BASE + 0xb4 )// Rx PAUSE frame packet counter		[11 : 0]
#define ETH_STAT_RXUO			(ETHC_BASE + 0xb8 )// Rx Unkown OP code counter			[11 : 0]

#define ETH_STAT_RALN			(ETHC_BASE + 0xbc )// Rx Alignment Error counter			[11 : 0]
#define ETH_STAT_RFLR			(ETHC_BASE + 0xc0 )// Rx Frame Length Error counter		[15 : 0]
#define ETH_STAT_RCDE			(ETHC_BASE + 0xc4 )// Rx Code Error counter			[11 : 0]
#define ETH_STAT_RCSE			(ETHC_BASE + 0xc8 )// Rx Carrier Sense Error counter		[11 : 0]

#define ETH_STAT_RUND			(ETHC_BASE + 0xcc )// Rx Undersize packet counter		[11 : 0]
#define ETH_STAT_ROVR			(ETHC_BASE + 0xd0 )// Rx Oversize packet counter			[11 : 0]
#define ETH_STAT_RFRG			(ETHC_BASE + 0xd4 )// Rx Fragments counter			[11 : 0]
#define ETH_STAT_RJBR			(ETHC_BASE + 0xd8 )// Rx Jabber counter				[11 : 0]

#define ETH_STAT_RDRP			(ETHC_BASE + 0xdc )// Rx Drop					[11 : 0]

/* Transmit counters */ 
#define ETH_STAT_TBYT			(ETHC_BASE + 0xe0 )// Tx Byte counter				[23 : 0]
#define ETH_STAT_TPKT			(ETHC_BASE + 0xe4 )// Tx Packet counter				[17 : 0]
#define ETH_STAT_TMCA			(ETHC_BASE + 0xe8 )// Tx Multicast packet counter		[17 : 0]
#define ETH_STAT_TBCA			(ETHC_BASE + 0xec )// Tx Broadcast packet counter		[17 : 0]

#define ETH_STAT_TXPF			(ETHC_BASE + 0xf0 )// Tx PAUSE frame packet counter		[11 : 0]
#define ETH_STAT_TDFR			(ETHC_BASE + 0xf4 )// Tx Deferral packet counter			[11 : 0]
#define ETH_STAT_TEDF			(ETHC_BASE + 0xf8 )// Tx Excessive Deferral packet counter	[11 : 0]
#define ETH_STAT_TSCL			(ETHC_BASE + 0xfc )// Tx Single Collision packet counter		[11 : 0]

#define ETH_STAT_TMCL			(ETHC_BASE + 0x100 )// Tx Multiple Collision packet counte	[11 : 0]
#define ETH_STAT_TLCL			(ETHC_BASE + 0x104 )// Tx Late Collision packet counter		[11 : 0]
#define ETH_STAT_TXCL			(ETHC_BASE + 0x108 )// Tx Excessive Collision packet counter	[11 : 0]
#define ETH_STAT_TNCL			(ETHC_BASE + 0x10c )// Tx Total Collision packet counter		[12 : 0]

#define ETH_STAT_TPFH			(ETHC_BASE + 0x110 )// Tx PAUSE frames Honored counter		[11 : 0]
#define ETH_STAT_TDRP			(ETHC_BASE + 0x114 )// Tx Drop frame counter			[11 : 0]
#define ETH_STAT_TJBR			(ETHC_BASE + 0x118 )// Tx Jabber frame counter			[11 : 0]
#define ETH_STAT_TFCS			(ETHC_BASE + 0x11c )// Tx FCS Error counter			[11 : 0]

#define ETH_STAT_TXCF			(ETHC_BASE + 0x120 )// Tx Control frame counter			[11 : 0]
#define ETH_STAT_TOVR			(ETHC_BASE + 0x124 )// Tx Oversize frame counter			[11 : 0]
#define ETH_STAT_TUND			(ETHC_BASE + 0x128 )// Tx Undersize frame counter		[11 : 0]
#define ETH_STAT_TFRG			(ETHC_BASE + 0x12c )// Tx Fragments frame counter		[11 : 0]

/* Carry registers */ 
#define ETH_STAT_CAR1			(ETHC_BASE + 0x130 )// Carry Register 1
#define ETH_STAT_CAR2			(ETHC_BASE + 0x134 )// Carry Register 2
#define ETH_STAT_CARM1			(ETHC_BASE + 0x138 )// Carry Mask Register 1
#define ETH_STAT_CARM2			(ETHC_BASE + 0x13c )// Carry Mask Register 2

// PE-SAL registers (32-bits)
#define ETH_SAL_AFR			(ETHC_BASE + 0x1a0)
#define ETH_SAL_HT1			(ETHC_BASE + 0x1a4)
#define ETH_SAL_HT2			(ETHC_BASE + 0x1a8)

// Constants for ETH_DMA_TCR register
#define TCR_ENABLE			(0x1 << 0)	// Enable DMA tx packet transfers

// Constants for ETH_DMA_TSR register
#define TSR_PKTSENT			(0x1 << 0)	// Indicates packet(s) have(has) been successfully txed
#define TSR_UNDERRUN			(0x1 << 1)	// Set whenever DMA controller reads a Empty Flag
#define TSR_BUSERR			(0x1 << 3)	// Indicates that DMA controller found tx bus error
#define TSR_PKTCNT_MASK			(0xFF << 16)	// Bit mask of tx packet counter

// Constants for ETH_DMA_RCR register
#define RCR_ENABLE			(0x1 << 0)	// Enable DMA rx packet transfers

// Constants for ETH_DMA_RSR register
#define RSR_PKTRECV			(0x1 << 0)	// Indicates packet(s) have(has) been successfully rxed
#define RSR_OVERFLOW			(0x1 << 1)	// Set whenever DMA controller reads a Non-Empty Flag
#define RSR_BUSERR			(0x1 << 3)	// Indicates that DMA controller found rx bus error
#define RSR_PKTCNT_MASK			(0xFF << 16)	// Bit mask of rx packet counter

// Constants for ETH_DMA_IMR register
#define IMR_PKTSENT			(0x1 << 0)	// Packet sent interrupt mask
#define IMR_UNDERRUN			(0x1 << 1)	// Underrun interrupt mask
#define IMR_TBUSERR			(0x1 << 3)	// Tx bus error interrupt mask
#define IMR_PKTRECV			(0x1 << 4)	// Packet received interrupt mask
#define IMR_OVERFLOW			(0x1 << 6)	// Overflow interrupt mask
#define IMR_RBUSERR			(0x1 << 7)	// Rx bus error interrupt mask
#define IMR_ALL_IRQ			0x000000db	// All above interrupt mask

// Constants for ETH_MAC_MCR1 register
#define MCR1_SOFTRST			(0x1 << 15)	// Soft reset
#define MCR1_SMLTRST			(0x1 << 14)	// Simulation reset
#define MCR1_MCSRRST			(0x1 << 11)	// MAC Control Sublayer / Rx domain logic reset
#define MCR1_RFUNRST			(0x1 << 10)	// Rx Function logic reset
#define MCR1_MCSTRST			(0x1 << 9)	// MAC Control Sublayer / Tx domain logic reset
#define MCR1_TFUNRST			(0x1 << 8)	// Tx Function logic reset
#define MCR1_LB				(0x1 << 4)	// Tx interface loop back to Rx interface
#define MCR1_TFC			(0x1 << 3)	// Enable Tx flow control
#define MCR1_RFC			(0x1 << 2)	// Enable Rx flow control
#define MCR1_PARF			(0x1 << 1)	// Pass all receive frames
#define MCR1_RE				(0x1 << 0)	// Enable receive

// Constants for ETH_MAC_MCR2 register
#define MCR2_ED				(0x1 << 14)	// Defer to carrier indefinitely as per the Standard
#define MCR2_BPNB			(0x1 << 13)	// Back Pressure / No Backoff
#define MCR2_NB				(0x1 << 12)	// No Backoff
#define MCR2_LPE			(0x1 << 9)	// Long Preamble Enforcement
#define MCR2_PPE			(0x1 << 8)	// Pure Preamble Enforcement
#define MCR2_ADPE			(0x1 << 7)	// Auto-Detect Pad Enable
#define MCR2_VPE			(0x1 << 6)	// VLAN Pad Enable
#define MCR2_PCE			(0x1 << 5)	// Pad / CRC Enable
#define MCR2_CE				(0x1 << 4)	// CRC Enable
#define MCR2_DC				(0x1 << 3)	// Delayed CRC
#define MCR2_HFE			(0x1 << 2)	// Huge Frame Enable
#define MCR2_FLC			(0x1 << 1)	// Frame Length Checking
#define MCR2_FD				(0x1 << 0)	// Full-Duplex

// Contants for ETH_MAC_IPGR register
#define IPGR_MASK			0x007f		// In Full-Duplex the recommended value is 0x15(21d)

// Contants for ETH_MAC_NIPGR register
#define NIPGR_P1_MASK			0x7f00		// The recommended value is 0xC(12d)
#define NIPGR_P2_MASK			0x007f		// The recommended value is 0x12(18d)

// Contants for ETH_MAC_CWR register
#define CWR_CW_MASK			0x3f00		// Collision window, Default value is 0x37(55d)
#define CWR_RM_MASK			0x000f		// Retry time, default & standard is 0xF(15d)

// Contants for ETH_MAC_PSR register
#define PSR_RIM				(0x1 << 15)	// Reset Interface Module
#define PSR_PM				(0x1 << 12)	// PHY Module
#define PSR_RPERMII			(0x1 << 11)	// Reset PERMII
#define PSR_OS				(0x1 << 8)	// Operating Speed
#define PSR_RPE100M			(0x1 << 7)	// Reset PE100M module
#define PSR_FQ				(0x1 << 6)	// Force Quiet
#define PSR_NC				(0x1 << 5)	// No Cipher
#define PSR_DLF				(0x1 << 4)	// Disable Link Fail
#define PSR_RPE10T			(0x1 << 3)	// Reset PE10T module
#define PSR_EJP				(0x1 << 1)	// Enable Jabber Protection
#define PSR_BM				(0x1 << 0)	// Bit Mode

// Contants for ETH_MAC_TR register
#define TR_TB				(0x1 << 2)	// Test Backpressure
#define TR_TP				(0x1 << 1)	// Test Pause
#define TR_SPQ				(0x1 << 0)	// Shortcut Pause Quanta

// Contants for ETH_MAC_MCFGR register
#define MCFGR_RST			(0x1 << 15)	// Reset MII Mgmt
#define MCFGR_CS_MASK			0x001c		// Clock Reset
#define MCFGR_SP			(0x1 << 1)	// Suppress Preamble
#define MCFGR_SI			(0x1 << 0)	// Scan Increment

// Contants for ETH_MAC_MCMDR register
#define MCMDR_SCAN			(0x1 << 1)	// Cause MII Mgmt module to perform Read cycles continuously
#define MCMDR_READ			(0x1 << 0)	// Cause MII Mgmt module to perform single Read cycles
#define MII_SCAN			(0x1 << 1)
#define MII_NO_SCAN			0x0

// Contants for ETH_MAC_MADRR register
#define MADRR_PA_MASK			0x1f00		// PHY Address
#define MADRR_RA_MASK			0x001f		// Register Address

// Contants for ETH_MAC_MINDR register
#define MINDR_LF			(0x1 << 3)	// Link Fail
#define MINDR_NV			(0x1 << 2)	// Not Valid
#define MINDR_S				(0x1 << 1)	// Scanning
#define MINDR_BUSY			(0x1 << 0)	// Busy

// Constants for ETH_FIFO_CR0 register
#define FTF_EN_RPLY			(0x1 << 20)	// Indicate whether mmiitfif_fab module is enabled
#define STF_EN_RPLY			(0x1 << 19)	// Indicate whether mmiitfif_sys module is enabled
#define FRF_EN_RPLY			(0x1 << 18)	// Indicate whether mmiirfif_fab module is enabled
#define SRF_EN_RPLY			(0x1 << 17)	// Indicate whether mmiirfif_sys module is enabled
#define WTM_EN_RPLY			(0x1 << 16)	// Indicate whether mmiitfif_wtm module is enabled
#define ALL_EN_RPLY			(FTF_EN_RPLY | STF_EN_RPLY | FRF_EN_RPLY | SRF_EN_RPLY | WTM_EN_RPLY)

#define FTF_EN_REQ			(0x1 << 12)	// Request enable/disable -ing the mmiitfif_fab module
#define STF_EN_REQ			(0x1 << 11)	// Request enable/disable -ing the mmiitfif_sys module
#define FRF_EN_REQ			(0x1 << 10)	// Request enable/disable -ing the mmiirfif_fab module
#define SRF_EN_REQ			(0x1 << 9)	// Request enable/disable -ing the mmiirfif_sys module
#define WTM_EN_REQ			(0x1 << 8)	// Request enable/disable -ing the mmiitfif_wtm module
#define ALL_EN_REQ			(FTF_EN_REQ | STF_EN_REQ | FRF_EN_REQ | SRF_EN_REQ | WTM_EN_REQ)

#define FTF_RST				(0x1 << 4)	// Reset mmiitfif_fab module
#define STF_RST				(0x1 << 3)	// Reset mmiitfif_sys module
#define FRT_RST				(0x1 << 2)	// Reset mmiirfif_fab module
#define SRF_RST				(0x1 << 1)	// Reset mmiirfif_sys module
#define WTM_RST				(0x1 << 0)	// Reset mmiitfif_wtm module
#define ALL_RST				(FTF_RST | STF_RST | FRT_RST | SRF_RST | WTM_RST)

// Constants for ETH_FIFO_CR1 register
#define CFG_FR_TH_MASK			(0x0fff << 16)	// Fabric Receive Threshold mask
#define CFG_XOFF_RTX_MASK		0xffff		// ??????????????

// Constants for ETH_FIFO_CR2 register
#define CFG_H_WM_MASK			(0x1fff << 16)	// Receive RAM high watermark mask
#define CFG_L_WM_MASK			0x1fff		// Receive RAM low watermark mask

// Constants for ETH_FIFO_CR3 register
#define CFG_H_WM_FT_MASK		(0x0fff << 16)	// ??????????????
#define CFG_FT_TH_MASK			0xffff		// Fabric Transmit Threshold mask

// Constants for ETH_FIFO_CR4 register
#define HST_FLT_RFRM_MASK		0xffff		// Indicate drop condition

// Constants for ETH_FIFO_CR5 register
#define CFG_H_DPLX			(0x1 << 22)	// Enable Half-duplex backpressure as flow control mchm
#define CFG_SR_FULL			(0x1 << 21)	// Indicate whether FIFO storage has been met or exceeded
#define CFG_SR_FULL_CLR			(0x1 << 20)	// Clear CFG_SR_FULL bit
#define CLK_EN_MODE			(0x1 << 19)	// 
#define HST_DR_LT_64			(0x1 << 18)	// Drop the frame which less than 16 bit length
#define HST_FLT_RFRMDC_MASK		(0xffff)	// Indicate drop condition ... which don't care

// Constants for ETH_FIFO_RAR0 register
#define RAR0_HT_W_REQ			(0x1 << 31)	// Host Tx RAM write request
#define RAR0_HT_W_ACK			(0x1 << 30)	// Host Tx RAM write acknowledge
#define RAR0_HT_W_CD_MASK		(0xff << 16)	// Host Tx RAM write control data
#define RAR0_HT_W_ADDR_MASK		(0x3ff << 0)	// Host Tx RAM write address, [9:0] 10 bit, (4k RAM / 4 = 1024)

// Constants for ETH_FIFO_RAR2 register
#define RAR2_HT_R_REQ			(0x1 << 31)	// Host Tx RAM read request
#define RAR2_HT_R_ACK			(0x1 << 30)	// Host Tx RAM read acknowledge
#define RAR2_HT_R_CD_MASK		(0xff << 16)	// Host Tx RAM read control data
#define RAR2_HT_R_ADDR_MASK		(0x3ff << 0)	// Host Tx RAM read address, [9:0] 10 bit

// Constants for ETH_FIFO_RAR4 register
#define RAR4_HR_W_REQ			(0x1 << 31)	// Host Rx RAM write request
#define RAR4_HR_W_ACK			(0x1 << 30)	// Host Rx RAM write acknowledge
#define RAR4_HR_W_CD_MASK		(0xf << 16)	// Host Rx RAM write control data
#define RAR4_HR_W_ADDR_MASK		(0x3ff << 0)	// Host Rx RAM write address

// Constants for ETH_FIFO_RAR6 register
#define RAR4_HR_R_REQ			(0x1 << 31)	// Host Rx RAM read request
#define RAR4_HR_R_ACK			(0x1 << 30)	// Host Rx RAM read acknowledge
#define RAR4_HR_R_CD_MASK		(0xf << 16)	// Host Rx RAM read control data
#define RAR4_HR_R_ADDR_MASK		(0x3ff << 0)	// Host Rx RAM read address

// Receive Status Vector
#define RSV_RVTD			(0x1 << 30)	// Receive VLAN Type detected
#define RSV_RUO				(0x1 << 29)	// Receive Unsupported Op-code
#define RSV_RPCF			(0x1 << 28)	// Receive Pause Control Frame
#define RSV_RCF				(0x1 << 27)	// Receive Control Frame
#define RSV_DN				(0x1 << 26)	// Dribble Nibble
#define RSV_BP				(0x1 << 25)	// Broadcast Packet
#define RSV_MP				(0x1 << 24)	// Multicast Packet
#define RSV_OK				(0x1 << 23)	// Receive OK
#define RSV_LOR				(0x1 << 22)	// Length Out of Range
#define RSV_LCE				(0x1 << 21)	// Length Check Error
#define RSV_CRCE			(0x1 << 20)	// CRC Error
#define RSV_RCV				(0x1 << 19)	// Receive Code Violation
#define RSV_CEPS			(0x1 << 18)	// Carrier Event Previously Seen
#define RSV_REPS			(0x1 << 17)	// RXDV Event Previously Seen
#define RSV_PPI				(0x1 << 16)	// Packet Previously Ignored



// ------------------------------------------------------------------------
// Operation defination
// ------------------------------------------------------------------------

// Operations of ETH DMA
#define __eth_dma_tx_enable()					\
do {								\
	REG32(ETH_DMA_TCR) |= TCR_ENABLE;			\
} while(0)

#define __eth_dma_tx_disable()					\
do {								\
	REG32(ETH_DMA_TCR) &= ~TCR_ENABLE;			\
} while(0)


#define __eth_set_tx_desc_addr(desc_addr)			\
do {								\
	REG32(ETH_DMA_TDR) = desc_addr;				\
} while(0)

#define __eth_get_flag_pkt_sent()		(REG32(ETH_DMA_TSR) & TSR_PKTSENT)
#define __eth_get_flag_underrun()		(REG32(ETH_DMA_TSR) & TSR_UNDERRUN)
#define __eth_get_flag_tx_bus_err()		(REG32(ETH_DMA_TSR) & TSR_BUSERR)
#define __eth_get_tx_pkt_cnt()			((REG32(ETH_DMA_TSR) & TSR_PKTCNT_MASK) >> 0x10)

#define __eth_clear_flag_pkt_sent()		(REG32(ETH_DMA_TSR) &= ~TSR_PKTSENT)
#define __eth_clear_flag_underrun()		(REG32(ETH_DMA_TSR) |= TSR_UNDERRUN)
#define __eth_clear_flag_tx_bus_err()		(REG32(ETH_DMA_TSR) |= TSR_BUSERR)
//#define __eth_clear_tx_pkt_cnt()		(REG32(ETH_DMA_TSR) |= TSR_PKTCNT_MASK)

#define __eth_dma_rx_enable()					\
do {								\
	REG32(ETH_DMA_RCR) |= RCR_ENABLE;			\
} while(0)

#define __eth_dma_rx_disable()					\
do {								\
	REG32(ETH_DMA_RCR) &= ~RCR_ENABLE;			\
} while(0)

#define __eth_set_rx_desc_addr(desc_addr)			\
do {								\
	REG32(ETH_DMA_RDR) = desc_addr;				\
} while(0)

#define __eth_get_flag_pkt_recv()		(REG32(ETH_DMA_RSR) & RSR_PKTRECV)
#define __eth_get_flag_overflow()		(REG32(ETH_DMA_RSR) & RSR_OVERFLOW)
#define __eth_get_flag_rx_bus_err()		(REG32(ETH_DMA_RSR) & RSR_BUSERR)
#define __eth_get_rx_pkt_cnt()			((REG32(ETH_DMA_RSR) & RSR_PKTCNT_MASK) >> 0x10)

#define __eth_clear_flag_pkt_recv()		(REG32(ETH_DMA_RSR) &= ~RSR_PKTRECV)
#define __eth_clear_flag_overflow()		(REG32(ETH_DMA_RSR) |= RSR_OVERFLOW)
#define __eth_clear_flag_rx_bus_err()		(REG32(ETH_DMA_RSR) |= RSR_BUSERR)
//#define __eth_clear_rx_pkt_cnt()		(REG32(ETH_DMA_RSR) |= RSR_PKTCNT_MASK)


#define __eth_pkt_sent_as_irq(bool)				\
do {								\
	if (bool)						\
		REG32(ETH_DMA_IMR) |= IMR_PKTSENT;		\
	else							\
		REG32(ETH_DMA_IMR) &= ~IMR_PKTSENT;		\
} while(0)

#define __eth_underrun_as_irq(bool)				\
do {								\
	if (bool)						\
		REG32(ETH_DMA_IMR) |= IMR_UNDERRUN;		\
	else							\
		REG32(ETH_DMA_IMR) &= ~IMR_UNDERRUN;		\
} while(0)

#define __eth_tx_bus_error_as_irq(bool)				\
do {								\
	if (bool)						\
		REG32(ETH_DMA_IMR) |= IMR_TBUSERR;		\
	else							\
		REG32(ETH_DMA_IMR) &= ~IMR_TBUSERR;		\
} while(0)

#define __eth_pkt_received_as_irq(bool)				\
do {								\
	if (bool)						\
		REG32(ETH_DMA_IMR) |= IMR_PKTRECV;		\
	else							\
		REG32(ETH_DMA_IMR) &= ~IMR_PKTRECV;		\
} while(0)

#define __eth_overflow_as_irq(bool)				\
do {								\
	if (bool)						\
		REG32(ETH_DMA_IMR) |= IMR_OVERFLOW;		\
	else							\
		REG32(ETH_DMA_IMR) &= ~IMR_OVERFLOW;		\
} while(0)

#define __eth_rx_bus_error_as_irq(bool)				\
do {								\
	if (bool)						\
		REG32(ETH_DMA_IMR) |= IMR_RBUSERR;		\
	else							\
		REG32(ETH_DMA_IMR) &= ~IMR_RBUSERR;		\
} while(0)

#define __eth_enable_all_irq()					\
do {								\
	REG32(ETH_DMA_IMR) |= IMR_ALL_IRQ;			\
} while(0)

#define __eth_disable_all_irq()					\
do {								\
	REG32(ETH_DMA_IMR) &= ~IMR_ALL_IRQ;			\
} while(0)

#define __eth_enable()						\
do {								\
	__eth_dma_tx_enable();					\
	__eth_dma_rx_enable();					\
} while(0)

#define __eth_disable()						\
do {								\
	__eth_dma_tx_disable();					\
	__eth_dma_rx_disable();					\
} while(0)

#define __eth_set_mac_address(b1, b2, b3, b4, b5, b6)		\
do {								\
	REG16(ETH_MAC_SA0) = (b1 << 8) | (b2 & 0xff);		\
	REG16(ETH_MAC_SA1) = (b3 << 8) | (b4 & 0xff);		\
	REG16(ETH_MAC_SA2) = (b5 << 8) | (b6 & 0xff);		\
} while(0)


// Operations of ETH MAC registers

#define __mac_reset()						\
do {								\
	REG16(ETH_MAC_MCR1) |= MCR1_SOFTRST;			\
	udelay(2000);						\
	REG16(ETH_MAC_MCR1) &= ~MCR1_SOFTRST;			\
} while(0)

#define __mac_get_IPGR()			(REG16(ETH_MAC_IPGR) & IPGR_MASK)

#define __mac_set_IPGR(ipgt)					\
do {								\
	REG16(ETH_MAC_IPGR) = ipgt;				\
} while(0)

#define __mac_get_NIPGR1()			((REG16(ETH_MAC_NIPGR) & NIPGR_P1_MASK) >> 8)

#define __mac_set_NIPGR1(v1)					\
do {								\
	REG16(ETH_MAC_NIPGR) |= (v1 << 8) & NIPGR_P1_MASK;	\
} while(0)

#define __mac_get_NIPGR2()			(REG16(ETH_MAC_NIPGR) & NIPGR_P2_MASK)

#define __mac_set_NIPGR2(v2)					\
do {								\
	REG16(ETH_MAC_NIPGR) |= v2 & NIPGR_P2_MASK;		\
} while(0)

#define __mac_get_collision_window()		((REG16(ETH_MAC_CWR) & CWR_CW_MASK) >> 8)

#define __mac_set_collision_window(cw)				\
do {								\
	REG16(ETH_MAC_CWR) |= (cw << 8) & CWR_CW_MASK;		\
} while(0)

#define __mac_get_retx_maximum()		(REG16(ETH_MAC_CWR) & CWR_RM_MASK)

#define __mac_set_retx_maximum(rm)				\
do {								\
	REG16(ETH_MAC_CWR) |= rm & CWR_RM_MASK;			\
} while(0)

#define __mac_get_max_frame_length()		(REG16(ETH_MAC_MFR))

#define __mac_set_max_frame_length(len)				\
do {								\
	REG16(ETH_MAC_MFR) = len;				\
} while(0)

#define __mac_set_mii_clk(_clkdiv)				\
do {								\
	REG16(ETH_MAC_MCFGR) |= (_clkdiv << 2) & MCFGR_CS_MASK;	\
} while(0)

#define __mac_set_mii_address(pa, ra)				\
do {								\
	REG16(ETH_MAC_MADRR) =	 				\
		((pa << 8)& MADRR_PA_MASK)|(ra & MADRR_RA_MASK);\
} while(0)


#define __mac_send_mii_read_cmd(pa, ra, scan)			\
do {								\
	__mac_set_mii_address(pa, ra);				\
	REG16(ETH_MAC_MCMDR) &= ~MCMDR_SCAN & ~MCMDR_READ;	\
	REG16(ETH_MAC_MCMDR) |=	MCMDR_READ | scan & MCMDR_SCAN;	\
} while(0)

#define __mac_send_mii_write_cmd(pa, ra, wdata)			\
do {								\
	__mac_set_mii_address(pa, ra);				\
	REG16(ETH_MAC_MCMDR) &= ~MCMDR_SCAN & ~MCMDR_READ;	\
	__mac_mii_write_data(wdata);				\
} while(0)

#define __mac_mii_write_data(_2byte)		(REG16(ETH_MAC_MWTDR) = _2byte)

#define __mac_mii_read_data()			REG16(ETH_MAC_MRDDR)

#define __mac_mii_is_busy()			(REG16(ETH_MAC_MINDR) & MINDR_BUSY)

#define __fifo_enable_all_modules()				\
do {								\
	REG32(ETH_FIFO_CR0) |= ALL_EN_REQ;			\
} while(0)

#define __fifo_enable_all_modules_finish()			\
do {								\
	REG32(ETH_FIFO_CR0) &= ~ALL_EN_REQ;			\
} while(0)

// All enabled mean all reply bits were set except SRF_EN_RPLY...
#define __fifo_all_enabled()			((REG32(ETH_FIFO_CR0) & (ALL_EN_RPLY & ~SRF_EN_RPLY)) == (ALL_EN_RPLY & ~SRF_EN_RPLY))

#define __fifo_reset_all()					\
do {								\
	REG32(ETH_FIFO_CR0) |= ALL_RST;				\
	REG32(ETH_FIFO_CR0) &= ~ALL_RST;			\
} while(0)

#define __fifo_set_fr_threshold(_th)				\
do {								\
	REG32(ETH_FIFO_CR1) &= ~CFG_FR_TH_MASK;			\
	REG32(ETH_FIFO_CR1) |= (_th << 16) & CFG_FR_TH_MASK;	\
} while(0)

#define __fifo_set_XOFF_RTX(_th)				\
do {								\
	REG32(ETH_FIFO_CR1) &= ~CFG_XOFF_RTX_MASK;		\
	REG32(ETH_FIFO_CR1) |= _th & CFG_XOFF_RTX_MASK;		\
} while(0)

#define __fifo_set_high_wm(_th)					\
do {								\
	REG32(ETH_FIFO_CR2) &= ~CFG_H_WM_MASK;			\
	REG32(ETH_FIFO_CR2) |= (_th << 16) & CFG_H_WM_MASK;	\
} while(0)

#define __fifo_set_low_wm(_th)					\
do {								\
	REG32(ETH_FIFO_CR2) &= ~CFG_L_WM_MASK;			\
	REG32(ETH_FIFO_CR2) |= _th & CFG_L_WM_MASK;		\
} while(0)

#define __fifi_set_ft_high_wm(_th)				\
do {								\
	REG32(ETH_FIFO_CR3) &= ~CFG_H_WM_FT_MASK;		\
	REG32(ETH_FIFO_CR3) |= (_th << 16) & CFG_H_WM_FT_MASK;	\
} while(0)

#define __fifo_set_ft_threshold(_th)				\
do {								\
	REG32(ETH_FIFO_CR3) &= ~CFG_FT_TH_MASK;			\
	REG32(ETH_FIFO_CR3) |= _th & CFG_FT_TH_MASK;		\
} while(0)

#define __fifo_set_drop_cond(_cdt)				\
do {								\
	REG32(ETH_FIFO_CR4) |= (_cdt >> 16) & HST_FLT_RFRM_MASK;\
} while(0)

#define __fifo_set_dropdc_cond(_cdt)				\
do {								\
	REG32(ETH_FIFO_CR5) &= ~((_cdt>>16) & HST_FLT_RFRMDC_MASK);	\
} while(0)

#define __fifo_set_pause_control()				\
do {								\
	REG32(ETH_FIFO_CR5) &= ~CFG_H_DPLX;			\
} while(0)

#define __fifo_set_clk_enable_mode()				\
do {								\
	REG32(ETH_FIFO_CR5) |= CLK_EN_MODE;			\
} while(0)

#define __fifo_set_half_duplex()				\
do {								\
	REG32(ETH_FIFO_CR5) |= CFG_H_DPLX;			\
} while(0)

#define __fifo_drop_lt64_frame()				\
do {								\
	REG32(ETH_FIFO_CR5) |= HST_DR_LT_64;			\
} while(0)

// Constants for DMA descriptor
#define EMPTY_FLAG_MASK			(0x1 << 31)
#define FTPP_FLAGS_MASK			(0x1f << 16)
#define FTCFRM_MASK			(0x1 << 20)
#define FTPP_PADMODE_MASK		(0x3 << 18)
#define FTPP_GENFCS_MASK		(0x1 << 17)
#define FTPP_EN_MASK			(0x1 << 16)

#define PKT_SIZE_MASK			(0x0FFF)


// Operations of DMA descripter
#define __desc_get_empty_flag(pktsize)		(pktsize & EMPTY_FLAG_MASK)
#define __desc_get_FTPP_flags(pktsize)		((pktsize & FTPP_FLAGS_MASK) >> 16)
#define __desc_get_FTCFRM_flag(pktsize)		(pktsize & FTCFRM_MASK)
#define __desc_get_FTPP_PADMODE_flag(pktsize)	((pktsize & FTPP_PADMODE_MASK) >> 18)
#define __desc_get_FTPP_GENFCS_flag(pktsize)	(pktsize & FTPP_GENFCS_MASK)
#define __desc_get_FTPP_enable_flag(pktsize)	(pktsize & FTPP_EN_MASK)

#define __desc_get_pkt_size(pktsize)		(pktsize & PKT_SIZE_MASK)


// ------------------------------------------------------------------------
// MII Registers and Definitions
// ------------------------------------------------------------------------
#define MII_CR		0x00           /* MII Management Control Register */
#define MII_SR		0x01           /* MII Management Status Register */
#define MII_ID0		0x02           /* PHY Identifier Register 0 */
#define MII_ID1		0x03           /* PHY Identifier Register 1 */
#define MII_ANA		0x04           /* Auto Negotiation Advertisement */
#define MII_ANLPA	0x05           /* Auto Negotiation Link Partner Ability */
#define MII_ANE		0x06           /* Auto Negotiation Expansion */
#define MII_ANP		0x07           /* Auto Negotiation Next Page TX */

// MII Management Control Register
#define MII_CR_RST	0x8000         /* RESET the PHY chip */
#define MII_CR_LPBK	0x4000         /* Loopback enable */
#define MII_CR_SPD	0x2000         /* 0: 10Mb/s; 1: 100Mb/s */
#define MII_CR_10	0x0000         /* Set 10Mb/s */
#define MII_CR_100	0x2000         /* Set 100Mb/s */
#define MII_CR_ASSE	0x1000         /* Auto Speed Select Enable */
#define MII_CR_PD	0x0800         /* Power Down */
#define MII_CR_ISOL	0x0400         /* Isolate Mode */
#define MII_CR_RAN	0x0200         /* Restart Auto Negotiation */
#define MII_CR_FDM	0x0100         /* Full Duplex Mode */
#define MII_CR_CTE	0x0080         /* Collision Test Enable */

// MII Management Status Register
#define MII_SR_T4C	0x8000         /* 100BASE-T4 capable */
#define MII_SR_TXFD	0x4000         /* 100BASE-TX Full Duplex capable */
#define MII_SR_TXHD	0x2000         /* 100BASE-TX Half Duplex capable */
#define MII_SR_TFD	0x1000         /* 10BASE-T Full Duplex capable */
#define MII_SR_THD	0x0800         /* 10BASE-T Half Duplex capable */
#define MII_SR_ASSC	0x0020         /* Auto Speed Selection Complete*/
#define MII_SR_RFD	0x0010         /* Remote Fault Detected */
#define MII_SR_ANC	0x0008         /* Auto Negotiation capable */
#define MII_SR_LKS	0x0004         /* Link Status */
#define MII_SR_JABD	0x0002         /* Jabber Detect */
#define MII_SR_XC	0x0001         /* Extended Capabilities */

// MII Management Auto Negotiation Advertisement Register
#define MII_ANA_TAF	0x03e0         /* Technology Ability Field */
#define MII_ANA_T4AM	0x0200         /* T4 Technology Ability Mask */
#define MII_ANA_TXAM	0x0180         /* TX Technology Ability Mask */
#define MII_ANA_FDAM	0x0140         /* Full Duplex Technology Ability Mask */
#define MII_ANA_HDAM	0x02a0         /* Half Duplex Technology Ability Mask */
#define MII_ANA_100M	0x0380         /* 100Mb Technology Ability Mask */
#define MII_ANA_10M	0x0060         /* 10Mb Technology Ability Mask */
#define MII_ANA_CSMA	0x0001         /* CSMA-CD Capable */

// MII Management Auto Negotiation Remote End Register
#define MII_ANLPA_NP	0x8000         /* Next Page (Enable) */
#define MII_ANLPA_ACK	0x4000         /* Remote Acknowledge */
#define MII_ANLPA_RF	0x2000         /* Remote Fault */
#define MII_ANLPA_TAF	0x03e0         /* Technology Ability Field */
#define MII_ANLPA_T4AM	0x0200         /* T4 Technology Ability Mask */
#define MII_ANLPA_TXAM	0x0180         /* TX Technology Ability Mask */
#define MII_ANLPA_FDAM	0x0140         /* Full Duplex Technology Ability Mask */
#define MII_ANLPA_HDAM	0x02a0         /* Half Duplex Technology Ability Mask */
#define MII_ANLPA_100M	0x0380         /* 100Mb Technology Ability Mask */
#define MII_ANLPA_10M	0x0060         /* 10Mb Technology Ability Mask */
#define MII_ANLPA_CSMA	0x0001         /* CSMA-CD Capable */

// Media / mode state machine definitions
// User selectable:
#define TP              0x0040	       /* 10Base-T (now equiv to _10Mb)        */
#define TP_NW           0x0002         /* 10Base-T with Nway                   */
#define BNC             0x0004         /* Thinwire                             */
#define AUI             0x0008         /* Thickwire                            */
#define BNC_AUI         0x0010         /* BNC/AUI on DC21040 indistinguishable */
#define _10Mb           0x0040         /* 10Mb/s Ethernet                      */
#define _100Mb          0x0080         /* 100Mb/s Ethernet                     */
#define AUTO            0x4000         /* Auto sense the media or speed        */

// Internal states
#define NC              0x0000         /* No Connection                        */
#define ANS             0x0020         /* Intermediate AutoNegotiation State   */
#define SPD_DET         0x0100         /* Parallel speed detection             */
#define INIT            0x0200         /* Initial state                        */
#define EXT_SIA         0x0400         /* External SIA for motherboard chip    */
#define ANS_SUSPECT     0x0802         /* Suspect the ANS (TP) port is down    */
#define TP_SUSPECT      0x0803         /* Suspect the TP port is down          */
#define BNC_AUI_SUSPECT 0x0804         /* Suspect the BNC or AUI port is down  */
#define EXT_SIA_SUSPECT 0x0805         /* Suspect the EXT SIA port is down     */
#define BNC_SUSPECT     0x0806         /* Suspect the BNC port is down         */
#define AUI_SUSPECT     0x0807         /* Suspect the AUI port is down         */
#define MII             0x1000         /* MII on the 21143                     */



#endif
