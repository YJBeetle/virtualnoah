/* Jz4760 ethernet support
 *
 *  Copyright (c) 2005
 *  Ingenic Semiconductor, <xwang@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>

#include <common.h>
#include <malloc.h>
#include <net.h>
#include <command.h>
#include <asm/io.h>

#if defined(CONFIG_JZ4760)
#include <asm/jz4760.h>

#include "jz4760_eth.h"


#define MAX_WAIT	4000
//#define USE_RMII	1

/* Tx and Rx DMA descriptor */
typedef struct {
	u32	pkt_start_addr;
	u32	pkt_size;
	u32	next_descriptor;
	u32	for_align;
} eth_desc_t;

#define NUM_RX_DESCS	16
#define NUM_TX_DESCS	4

static eth_desc_t	tx_desc[NUM_TX_DESCS];
static eth_desc_t	rx_desc[NUM_RX_DESCS];

static int		next_tx;
static int		next_rx;

static u32		full_duplex, phy_mode;


/***************************************************************************
 * Test functions
 **************************************************************************/

/* havent been used */
static void fifo_test()
{
	int	i = 0;

	printf("==fifo_test==txfifo========================================\n");

	while (REG32(ETH_FIFO_RAR0) & RAR0_HT_W_ACK) {
		if (i++ > MAX_WAIT) {
			printf("fifo_test: time out ... 001\n");
			return ;
		}
		udelay(1);
	}

	REG32(ETH_FIFO_RAR1) = 0x1234;
	REG32(ETH_FIFO_RAR0) |= RAR0_HT_W_REQ;
	printf("Write tx FIFO: 0x1234 to [0x0000] ...\n");
	udelay(1);

	while (REG32(ETH_FIFO_RAR2) & RAR0_HT_W_ACK) {
		if (i++ > MAX_WAIT) {
			printf("fifo_test: time out ... 002\n");
			return ;
		}
		udelay(1);
	}
	REG32(ETH_FIFO_RAR2) |= RAR2_HT_R_REQ;
	
	printf("Read tx FIFO: from [0x0000]: 0x%x\n", REG32(ETH_FIFO_RAR3));
	printf("===========================================================\n");
}

static void desc_dump(void)
{
	int i;
	printf("==================================================\n");

	for (i = 0; i < NUM_TX_DESCS; i++) {
		printf("tx desc %2d :[0x%08x],  addr = 0x%08x,  pkt_size = 0x%08x,  next = 0x%08x\n",
		       i, &(tx_desc[i]), tx_desc[i].pkt_start_addr, tx_desc[i].pkt_size, tx_desc[i].next_descriptor);
	}

	for (i = 0; i < NUM_RX_DESCS; i++) {
		printf("rx desc %2d :[0x%08x],  addr = 0x%08x,  pkt_size = 0x%08x,  next = 0x%08x\n",
		       i, &(rx_desc[i]), rx_desc[i].pkt_start_addr, rx_desc[i].pkt_size, rx_desc[i].next_descriptor);
	}
	printf("==================================================\n");
}

static void dma_regs_dump(void)
{
	printf("==================================================\n");
	printf("DMA_TCR = 0x%08x    DMA_TDR = 0x%08x\n", REG32(ETH_DMA_TCR), REG32(ETH_DMA_TDR));
	printf("DMA_TSR = 0x%08x    DMA_RCR = 0x%08x\n", REG32(ETH_DMA_TSR), REG32(ETH_DMA_RCR));
	printf("DMA_RDR = 0x%08x    DMA_RSR = 0x%08x\n", REG32(ETH_DMA_RDR), REG32(ETH_DMA_RSR));
	printf("DMA_IMR = 0x%08x    DMA_IR  = 0x%08x\n", REG32(ETH_DMA_IMR), REG32(ETH_DMA_IR));
	printf("==================================================\n");
}

static void mac_regs_dump(void)
{
	printf("==================================================\n");
	printf("MAC_MCR1 = 0x%08x    MAC_MCR2 = 0x%08x\n", REG32(ETH_MAC_MCR1), REG32(ETH_MAC_MCR2));
	printf("MAC_IPGR = 0x%08x    MAC_NIPGR= 0x%08x\n", REG32(ETH_MAC_IPGR), REG32(ETH_MAC_NIPGR));
	printf("MAC_CWR  = 0x%08x    MAC_MFR  = 0x%08x\n", REG32(ETH_MAC_CWR), REG32(ETH_MAC_MFR));
	printf("MAC_PSR  = 0x%08x    MAC_TR   = 0x%08x\n", REG32(ETH_MAC_PSR), REG32(ETH_MAC_TR));
	printf("MAC_MCFGR= 0x%08x    MAC_MCMDR= 0x%08x\n", REG32(ETH_MAC_MCFGR), REG32(ETH_MAC_MCMDR));
	printf("MAC_MADRR= 0x%08x    MAC_MINDR= 0x%08x\n", REG32(ETH_MAC_MADRR), REG32(ETH_MAC_MINDR));
	printf("MAC_SA0  = 0x%08x    MAC_SA1  = 0x%08x    MAC_SA2  = 0x%08x\n",
	       REG32(ETH_MAC_SA0), REG32(ETH_MAC_SA1), REG32(ETH_MAC_SA2));
	printf("==================================================\n");
}

static void fifo_regs_dump(void)
{
	printf("==================================================\n");
	printf("FIFO_CR0 = 0x%08x\n", REG32(ETH_FIFO_CR0));
	printf("FIFO_CR1 = 0x%08x\n", REG32(ETH_FIFO_CR1));
	printf("FIFO_CR2 = 0x%08x\n", REG32(ETH_FIFO_CR2));
	printf("FIFO_CR3 = 0x%08x\n", REG32(ETH_FIFO_CR3));
	printf("FIFO_CR4 = 0x%08x\n", REG32(ETH_FIFO_CR4));
	printf("FIFO_CR5 = 0x%08x\n", REG32(ETH_FIFO_CR5));
#if 0
	printf("RAR0 = 0x%08x    RAR1 = 0x%08x\n", REG32(ETH_FIFO_RAR0), REG32(ETH_FIFO_RAR1));
	printf("RAR2 = 0x%08x    RAR3 = 0x%08x\n", REG32(ETH_FIFO_RAR2), REG32(ETH_FIFO_RAR3));
	printf("RAR4 = 0x%08x    RAR5 = 0x%08x\n", REG32(ETH_FIFO_RAR4), REG32(ETH_FIFO_RAR5));
	printf("RAR6 = 0x%08x    RAR7 = 0x%08x\n", REG32(ETH_FIFO_RAR6), REG32(ETH_FIFO_RAR7));
#endif
	printf("==================================================\n");
}

static void stat_regs_dump(void)
{
	printf("==================================================\n");
	printf("ETH_STAT_TR64= 0x%08x, ETH_STAT_TR127= 0x%08x, ETH_STAT_TR255= 0x%08x, ETH_STAT_TR511= 0x%08x\n",
	       REG32(ETH_STAT_TR64), REG32(ETH_STAT_TR127), REG32(ETH_STAT_TR255), REG32(ETH_STAT_TR511));
	printf("ETH_STAT_TR1K= 0x%08x, ETH_STAT_TRMAX= 0x%08x, ETH_STAT_TRMGV= 0x%08x\n",
	       REG32(ETH_STAT_TR1K), REG32(ETH_STAT_TRMAX), REG32(ETH_STAT_TRMGV));
	printf("ETH_STAT_RBYT= 0x%08x, ETH_STAT_RPKT= 0x%08x, ETH_STAT_RFCS= 0x%08x, ETH_STAT_RMCA= 0x%08x\n",
	       REG32(ETH_STAT_RBYT), REG32(ETH_STAT_RPKT), REG32(ETH_STAT_RFCS), REG32(ETH_STAT_RMCA));
	printf("ETH_STAT_RBCA= 0x%08x, ETH_STAT_RXCF= 0x%08x, ETH_STAT_RXPF= 0x%08x, ETH_STAT_RXUO= 0x%08x\n",
	       REG32(ETH_STAT_RBCA), REG32(ETH_STAT_RXCF), REG32(ETH_STAT_RXPF), REG32(ETH_STAT_RXUO));
	printf("ETH_STAT_RALN= 0x%08x, ETH_STAT_RFLR= 0x%08x, ETH_STAT_RCDE= 0x%08x, ETH_STAT_RCSE= 0x%08x\n",
	       REG32(ETH_STAT_RALN), REG32(ETH_STAT_RFLR), REG32(ETH_STAT_RCDE), REG32(ETH_STAT_RCSE));
	printf("ETH_STAT_RUND= 0x%08x, ETH_STAT_ROVR= 0x%08x, ETH_STAT_RFRG= 0x%08x, ETH_STAT_RJBR= 0x%08x\n",
	       REG32(ETH_STAT_RUND), REG32(ETH_STAT_ROVR), REG32(ETH_STAT_RFRG), REG32(ETH_STAT_RJBR));
	printf("ETH_STAT_RDRP= 0x%08x\n", REG32(ETH_STAT_RDRP));
	printf("ETH_STAT_TBYT= 0x%08x, ETH_STAT_TPKT= 0x%08x, ETH_STAT_TMCA= 0x%08x, ETH_STAT_TBCA= 0x%08x\n",
	       REG32(ETH_STAT_TBYT), REG32(ETH_STAT_TPKT), REG32(ETH_STAT_TMCA), REG32(ETH_STAT_TBCA));
	printf("ETH_STAT_TXPF= 0x%08x, ETH_STAT_TDFR= 0x%08x, ETH_STAT_TEDF= 0x%08x, ETH_STAT_TSCL= 0x%08x\n",
	       REG32(ETH_STAT_TXPF), REG32(ETH_STAT_TDFR), REG32(ETH_STAT_TEDF), REG32(ETH_STAT_TSCL));
	printf("ETH_STAT_TMCL= 0x%08x, ETH_STAT_TLCL= 0x%08x, ETH_STAT_TXCL= 0x%08x, ETH_STAT_TNCL= 0x%08x\n",
	       REG32(ETH_STAT_TMCL), REG32(ETH_STAT_TLCL), REG32(ETH_STAT_TXCL), REG32(ETH_STAT_TNCL));
	printf("ETH_STAT_TPFH= 0x%08x, ETH_STAT_TDRP= 0x%08x, ETH_STAT_TJBR= 0x%08x, ETH_STAT_TFCS= 0x%08x\n",
	       REG32(ETH_STAT_TPFH), REG32(ETH_STAT_TDRP), REG32(ETH_STAT_TJBR), REG32(ETH_STAT_TFCS));
	printf("ETH_STAT_TXCF= 0x%08x, ETH_STAT_TOVR= 0x%08x, ETH_STAT_TUND= 0x%08x, ETH_STAT_TFRG= 0x%08x\n",
	       REG32(ETH_STAT_TXCF), REG32(ETH_STAT_TOVR), REG32(ETH_STAT_TUND), REG32(ETH_STAT_TFRG));
	printf("ETH_STAT_CAR1= 0x%08x, ETH_STAT_CAR2= 0x%08x, ETH_STAT_CARM1= 0x%08x, ETH_STAT_CARM2= 0x%08x\n",
	       REG32(ETH_STAT_CAR1), REG32(ETH_STAT_CAR2), REG32(ETH_STAT_CARM1), REG32(ETH_STAT_CARM2));
	printf("==================================================\n");
}

static void sal_regs_dump(void)
{
	printf("==================================================\n");
	printf("ETH_SAL_AFR = 0x%08x, ETH_SAL_HT1 = 0x%08x, ETH_SAL_HT2 = 0x%08x\n",
	       REG32(ETH_SAL_AFR), REG32(ETH_SAL_HT1), REG32(ETH_SAL_HT2));
	printf("==================================================\n");	
}

static void sal_regs_test(void)
{
	printf("before write\n");
	sal_regs_dump();
	REG32(ETH_SAL_AFR) = 0x3;
	REG32(ETH_SAL_HT1) = 0x3;
	REG32(ETH_SAL_HT2) = 0x3;

	udelay(2000);
	printf("after write\n");
	sal_regs_dump();
}

static void stat_regs_test(void)
{
	printf("before write\n");
	stat_regs_dump();

	/* Combined transmit and receive counters  */
	REG32(ETH_STAT_TR64) = 0x3;
	REG32(ETH_STAT_TR127) = 0x3;
	REG32(ETH_STAT_TR255) = 0x3;
	REG32(ETH_STAT_TR511) = 0x3;
	REG32(ETH_STAT_TR1K) = 0x3;
	REG32(ETH_STAT_TRMAX) = 0x3;
	REG32(ETH_STAT_TRMGV) = 0x3;
	/* Receive counters */ 
	REG32(ETH_STAT_RBYT) = 0x3;
	REG32(ETH_STAT_RPKT) = 0x3;
	REG32(ETH_STAT_RFCS) = 0x3;
	REG32(ETH_STAT_RMCA) = 0x3;
	REG32(ETH_STAT_RBCA) = 0x3;
	REG32(ETH_STAT_RXCF) = 0x3;
	REG32(ETH_STAT_RXPF) = 0x3;
	REG32(ETH_STAT_RXUO) = 0x3;
	REG32(ETH_STAT_RALN) = 0x3;
	REG32(ETH_STAT_RFLR) = 0x3;
	REG32(ETH_STAT_RCDE) = 0x3;
	REG32(ETH_STAT_RCSE) = 0x3;
	REG32(ETH_STAT_RUND) = 0x3;
	REG32(ETH_STAT_ROVR) = 0x3;
	REG32(ETH_STAT_RFRG) = 0x3;
	REG32(ETH_STAT_RJBR) = 0x3;
	REG32(ETH_STAT_RDRP) = 0x3;
	/* Transmit counters */ 
	REG32(ETH_STAT_TBYT) = 0x3;
	REG32(ETH_STAT_TPKT) = 0x3;
	REG32(ETH_STAT_TMCA) = 0x3;
	REG32(ETH_STAT_TBCA) = 0x3;
	REG32(ETH_STAT_TXPF) = 0x3;
	REG32(ETH_STAT_TDFR) = 0x3;
	REG32(ETH_STAT_TEDF) = 0x3;
	REG32(ETH_STAT_TSCL) = 0x3;
	REG32(ETH_STAT_TMCL) = 0x3;
	REG32(ETH_STAT_TLCL) = 0x3;
	REG32(ETH_STAT_TXCL) = 0x3;
	REG32(ETH_STAT_TNCL) = 0x3;
	REG32(ETH_STAT_TPFH) = 0x3;
	REG32(ETH_STAT_TDRP) = 0x3;
	REG32(ETH_STAT_TJBR) = 0x3;
	REG32(ETH_STAT_TFCS) = 0x3;
	REG32(ETH_STAT_TXCF) = 0x3;
	REG32(ETH_STAT_TOVR) = 0x3;
	REG32(ETH_STAT_TUND) = 0x3;
	REG32(ETH_STAT_TFRG) = 0x3;
	/* Carry registers */ 
	REG32(ETH_STAT_CAR1) = 0x3;
	REG32(ETH_STAT_CAR2) = 0x3;
	REG32(ETH_STAT_CARM1) = 0x3;
	REG32(ETH_STAT_CARM2) = 0x3;

	udelay(2000);
	printf("after write\n");
	stat_regs_dump();
}



static inline void enable_eth(void)
{
	__eth_enable();
}

static inline void disable_eth(void)
{
	__eth_disable();
}

static u32 mii_read(int phy_id, int reg_offset)
{
	u32	i;

	__mac_send_mii_read_cmd(phy_id, reg_offset, MII_NO_SCAN);

	for (i = 0;
	     i < MAX_WAIT && __mac_mii_is_busy();
	     i++, udelay(1)) {
//		printf("*");
	}

//	printf("\nMAC_MCMDR= 0x%04x    MAC_MADRR= 0x%04x    MAC_MINDR = 0x%04x\n",
//	       REG16(ETH_MAC_MCMDR),  REG16(ETH_MAC_MADRR), REG16(ETH_MAC_MINDR));

	if (i == MAX_WAIT) {
		printf("MII wait timeout\n");
		return 0;
	}

	return __mac_mii_read_data();
}

static int autonet_complete(int phy_id)
{
	int	i;

	for (i = 0;
	     i < MAX_WAIT && !(mii_read(phy_id, MII_SR) & 0x0020);
	     i++, udelay(10)) {
		;
	}

	if (i == MAX_WAIT) {
		printf("autonet_comlete: autonet time out!\n");
		return -1;
	}

	return 0;
}

static int search_phy(int phy_id)
{
	u32	r;

	r = mii_read(phy_id, MII_SR);
	//printf("mii read data MII_SR: 0x%04x\n", r);
	if (r != 0 && r != 0xffff) {
//		printf("search_phy: Found phy 0x%x\n", r);
		return 1;
	}

	return 0;
}

static void config_phy(int phy_id)
{
	u32	mii_anlpar;

	full_duplex = 0;

	mii_anlpar = mii_read(phy_id, MII_ANLPA);

	if (mii_anlpar != 0xffff) {
		mii_anlpar = mii_read(phy_id, MII_ANLPA); // read twice to make data stable
		if ((mii_anlpar & 0x0100) || (mii_anlpar & 0x01C0) == 0x0040)
			full_duplex = 1;

		phy_mode = mii_anlpar >> 5;

#ifdef USE_RMII

		if (phy_mode & MII_ANLPA_100M)
			REG32(ETH_MAC_PSR) |= PSR_OS;
		else
			REG32(ETH_MAC_PSR) &= ~PSR_OS;
#endif

		printf("ETH: setting %s %s-duplex based on MII tranceiver #%d\n",
		       (phy_mode & MII_ANLPA_100M) ? "100Mbps" : "10Mbps",
		       full_duplex ? "full" : "half", phy_id);

	}
	else
		printf("config_phy: mii_anlpar is 0xffff, may be error ???\n");

}

static void config_mac(void)
{
	u32	mac_cfg_1 = 0, mac_cfg_2 = 0;

	// Set MAC address
	__eth_set_mac_address(eth_get_dev()->enetaddr[0],
			      eth_get_dev()->enetaddr[1],
			      eth_get_dev()->enetaddr[2],
			      eth_get_dev()->enetaddr[3],
			      eth_get_dev()->enetaddr[4],
			      eth_get_dev()->enetaddr[5]);
	
	// Enable tx & rx flow control, enable receive
	mac_cfg_1 = MCR1_TFC | MCR1_RFC | MCR1_RE;

#ifdef USE_RMII
	// Enable RMII
	mac_cfg_1 |= 1 << 13;
#endif

	// Enable loopback mode
	//mac_cfg_1 |= MCR1_LB;

	/* bit 7	bit 6		bit 5
	 * MCR2_ADPE	MCR2_VPE	MCR2_PCE
	 * x		x		0		No pad, check CRC
	 > 0		0		1		Pad to 60B, append CRC
	 * x		1		1		Pad to 64B, append CRC
	 * 1		0		1		if un-tagged, Pad to 60B, append CRC
	 * 						if VLAN tagged, Pad to 64B, append CRC
	 *
	 * if set MCR2_PCE(bit 5)
	 * 	MCR2_CE(bit 4) must be set.
	 *
	 * We need to pad frame to 60B and append 4-byte CRC.
	 */
	mac_cfg_2 = MCR2_PCE | MCR2_CE;

	// Pure preamble enforcement
	//mac_cfg_2 |= MCR2_PPE;

	// Frame length checking
	mac_cfg_2 |= MCR2_FLC;

	if (full_duplex) {
		mac_cfg_2 |= MCR2_FD;
		__mac_set_IPGR(0x15);
		
	} else {
		__mac_set_IPGR(0x12);
	}

	REG16(ETH_MAC_MCR1) = mac_cfg_1;
	REG16(ETH_MAC_MCR2) = mac_cfg_2;

	__mac_set_NIPGR1(0x0c);
	__mac_set_NIPGR2(0x12);

//	mac_regs_dump();

}

static void config_fifo(void)
{
	int	i;

	__fifo_reset_all();

	/* 4k tx fifo */
/*	__fifo_set_ft_threshold(0x0200);
	__fifi_set_ft_high_wm(0x0300);
*/
	/* 2k tx fifo */
	__fifo_set_ft_threshold(0x180);
	__fifi_set_ft_high_wm(0x01b0);

	__fifo_set_fr_threshold(0x0200);
	__fifo_set_XOFF_RTX(4);
	__fifo_set_high_wm(0x3bf);
	__fifo_set_low_wm(0x40);

	__fifo_set_drop_cond(RSV_CRCE);
	__fifo_set_dropdc_cond(RSV_CRCE);

	__fifo_set_drop_cond(RSV_BP);
	__fifo_set_dropdc_cond(RSV_BP);

	__fifo_set_drop_cond(RSV_LCE);
	__fifo_set_dropdc_cond(RSV_LCE);

	__fifo_set_drop_cond(RSV_MP);
	__fifo_set_dropdc_cond(RSV_MP);

	__fifo_set_pause_control();

	__fifo_enable_all_modules();

	for (i = 0;
	     i < MAX_WAIT && !__fifo_all_enabled();
	     i++, udelay(10)) {
		;
	}

	if (i == MAX_WAIT) {
		printf("config_fifo: Wait time out !\n");
//		return;
	}
//	fifo_regs_dump();
}


/***************************************************************************
 * ETH interface routines
 **************************************************************************/

static int jz_send(struct eth_device* dev, volatile void *packet, int length)
{
	volatile eth_desc_t *desc = 
		(volatile eth_desc_t *)((unsigned int)(tx_desc + next_tx) | 0xa0000000);
	int i, ret = 1;
	char c;

	if (length & ~PKT_SIZE_MASK)
		while(1) printf("Error, packet is too long !!!\n");

	if (!packet) {
		printf("jz_send: packet is NULL !\n");
		return -1;
	}

	desc->pkt_start_addr = virt_to_phys(packet);
	desc->pkt_size = length & ~EMPTY_FLAG_MASK;

	jz_flush_dcache();

/*
	printf(">>>>1 desc :[0x%08x],  addr = 0x%08x,  pkt_size = 0x%08x,  next = 0x%08x\n",
	       desc, desc->pkt_start_addr,
	       desc->pkt_size, desc->next_descriptor);

	for (i = 0; i < length && i < 128; i++) {
		printf("%2x ", *( (unsigned char *)packet + i));
		if ((i+1) % 16 == 0)
			printf("\n");
	}
	printf("\n");

	dma_regs_dump();
*/
	/* Clear underrun bit*/
	if ( __eth_get_flag_underrun() ) {
		__eth_clear_flag_underrun();
	}

	/* Clear bus error bit*/
	if ( __eth_get_flag_tx_bus_err() ) {
		__eth_clear_flag_tx_bus_err();
	}

	__eth_dma_tx_enable();

	i = 0;
	while (!__desc_get_empty_flag(desc->pkt_size)) {
		if (i > MAX_WAIT) {
			printf("ETH TX timeout0 !\n");
			ret = 0;
			break;
		}

		if (__eth_get_flag_underrun()) {
			//printf("ETH TX underrun !\n");
			ret = 0;
			break;
		}

		if (__eth_get_flag_tx_bus_err()) {
			printf("ETH TX tx bus error !\n");
			ret = 0;
			break;
		}

		i++;
	}
/*
	printf(">>>>2 desc :[0x%08x],  addr = 0x%08x,  pkt_size = 0x%08x,  next = 0x%08x\n",
	       desc, desc->pkt_start_addr, desc->pkt_size, desc->next_descriptor);

	dma_regs_dump();
*/
	/* Clear done bit */
	__eth_clear_flag_pkt_sent();

	/* Clear underrun bit*/
	//__eth_clear_flag_underrun();

	/* Clear bus error bit */
	//__eth_clear_flag_tx_bus_err();

	next_tx++;

	if (next_tx >= NUM_TX_DESCS)
		next_tx = 0;

	return ret;
}

static int jz_recv(struct eth_device* dev)
{
	volatile eth_desc_t *desc;
	u32	length;
	int	i;
	char	c;

	for (; ; ) {

		desc = (volatile eth_desc_t *)((unsigned int)(rx_desc + next_rx) | 0xa0000000);
/*
		printf("<<<<1 desc :[0x%08x],  addr = 0x%08x,  pkt_size = 0x%08x,  next = 0x%08x\n",
		       desc, desc->pkt_start_addr, desc->pkt_size, desc->next_descriptor);
*/

		if ( __desc_get_empty_flag(desc->pkt_size) ) {
			/* Nothing has been received */

			if (__desc_get_pkt_size(desc->pkt_size)) {
				//dma_regs_dump();
//				printf("\n<<<<1 desc :[0x%08x],  addr = 0x%08x,  pkt_size = 0x%08x,  next = 0x%08x\n",
//				       desc, desc->pkt_start_addr, desc->pkt_size, desc->next_descriptor);
				if ( __desc_get_empty_flag(desc->pkt_size) ) {
					printf("+++++");
					goto _continue;
				}
			}

			return -1;
		}

_continue:

/*
		printf("<<<<1 desc :[0x%08x],  addr = 0x%08x,  pkt_size = 0x%08x,  next = 0x%08x\n",
		       desc, desc->pkt_start_addr, desc->pkt_size, desc->next_descriptor);

		for (i = 0; i < __desc_get_pkt_size(desc->pkt_size) && i < 128; i++) {
			printf("%2x ", *( (unsigned char *)(desc->pkt_start_addr | 0xa0000000) + i ));
			if ((i+1) % 16 == 0)
				printf("\n");
		}
		printf("\n");

		dma_regs_dump();
*/
		if (__eth_get_flag_overflow()) {
			printf("ETH RX overflow !\n");
			__eth_clear_flag_overflow();
			__eth_set_rx_desc_addr(virt_to_phys(desc));
			__eth_dma_rx_enable();
		} else if (__eth_get_flag_rx_bus_err()) {
			printf("ETH RX bus error !\n");
			__eth_clear_flag_rx_bus_err();
			__eth_dma_rx_enable();
		} else {
			length = __desc_get_pkt_size(desc->pkt_size);
			/* Pass the packet up to the protocol layers */
			NetReceive(NetRxPackets[next_rx], length - 4);
			//printf("<<<< desc.size = 0x%08x\n", desc->pkt_size);
		}

		/* Clear bits */
		__eth_clear_flag_pkt_recv();

		/* Set empty flag */
		desc->pkt_size = 0x1 << 31;

		jz_flush_dcache();

		next_rx++;
		if (next_rx >= NUM_RX_DESCS)
			next_rx = 0;
	}/*for*/

}

static int jz_init(struct eth_device* dev, bd_t * bd)
{
	int i, phyid = -1;

//	printf("in jz_init...............\n");

//	stat_regs_test();
//	sal_regs_test();

	/* Disable interrupts */
//	printf("_eth_disable_all_irq() ...\n");
	__eth_disable_all_irq();

//	printf("config mac-mii clock ...\n");
	__mac_set_mii_clk(0x7);

//	printf("reset mac ...\n");
	__mac_reset();

//	printf("search_phy ...\n");
	for (i = 0; i < 32; i++) {
		if (search_phy(i)) {
			phyid = i;
			break;
		}
	}

	if (phyid == -1) {
		printf("Can't locate any PHY\n");
		while(1) ;
	}

//	printf("autonet_complete(%d) ...\n", phyid);

	/* Start Auto Negotiation of PHY 0 and check it */
	if (autonet_complete(phyid))
		printf("ETH Auto-Negotiation failed\n");

//	printf("config_phy(%d) ...\n", phyid);
	/* Configure PHY */
	config_phy(phyid);

//	printf("config_mac() ...\n");
	/* Configure MAC */
	config_mac();

//	printf("config_fifo() ...\n");
	/* Configure FIFO */
	config_fifo();

//	printf("setup dma descriptors ...\n");
	/* Setup the Rx & Tx descriptors */
	for (i = 0; i < NUM_TX_DESCS; i++) {
		tx_desc[i].pkt_size = 1 << 31;
		tx_desc[i].pkt_start_addr = 0x0123;
		tx_desc[i].next_descriptor = virt_to_phys(tx_desc + i + 1);
	}
	tx_desc[i - 1].next_descriptor = virt_to_phys(tx_desc);

	for (i = 0; i < NUM_RX_DESCS; i++) {
		rx_desc[i].pkt_size = 1 << 31;
		rx_desc[i].pkt_start_addr = virt_to_phys(NetRxPackets[i]);
		rx_desc[i].next_descriptor = virt_to_phys(rx_desc + i + 1);
	}
	rx_desc[i - 1].next_descriptor = virt_to_phys(rx_desc);

//	desc_dump();

	next_tx = next_rx = 0;

//	printf("jz_flush_dcache() ...\n");

	jz_flush_dcache();

	//printf("enable_eth() ...\n");

	__eth_set_tx_desc_addr(virt_to_phys(tx_desc));
	__eth_set_rx_desc_addr(virt_to_phys(rx_desc));

/*
	dma_regs_dump();
	mac_regs_dump();
	fifo_regs_dump();
*/
	__eth_dma_rx_enable();

	return 1;
}

static void jz_halt(struct eth_device *dev)
{
	disable_eth();
}

int jz_enet_initialize(bd_t *bis)
{
	struct eth_device	*dev;

	dev = (struct eth_device *) malloc(sizeof *dev);
	memset(dev, 0, sizeof *dev);

	sprintf(dev->name, "JZ ETHERNET");

	dev->iobase	= 0;
	dev->priv	= 0;
	dev->init	= jz_init;
	dev->halt	= jz_halt;
	dev->send	= jz_send;
	dev->recv	= jz_recv;

	eth_register(dev);

	return 1;
}

#endif
