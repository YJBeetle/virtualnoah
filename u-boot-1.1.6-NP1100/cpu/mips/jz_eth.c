/* Jz ethernet support
 *
 *  Copyright (c) 2005
 *  Ingenic Semiconductor, <jlwei@ingenic.cn>
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

#if defined(CONFIG_JZ4730) || defined(CONFIG_JZ5730) || defined(CONFIG_JZ4750) || defined(CONFIG_JZ4750L) || defined(CONFIG_JZ4750D)

#if defined(CONFIG_JZ4730) || defined(CONFIG_JZ5730) || defined(CONFIG_FPGA) 

#include <common.h>
#include <malloc.h>
#include <net.h>
#include <command.h>
#include <asm/io.h>
#if defined(CONFIG_JZ4730)
#include <asm/jz4730.h>
#endif
#if defined(CONFIG_JZ4740)
#include <asm/jz4740.h>
#endif
#if defined(CONFIG_JZ4750)
#include <asm/jz4750.h>
#endif
#if defined(CONFIG_JZ4750D)
#include <asm/jz4750d.h>
#endif
#if defined(CONFIG_JZ4750L)
#include <asm/jz4750l.h>
#endif
#if defined(CONFIG_JZ5730)
#include <asm/jz5730.h>
#endif

#include "jz_eth.h"

#define MAX_WAIT 1000

/* Tx and Rx Descriptor */
typedef struct {
	u32 status;
	u32 ctrl;
	u32 addr;
	u32 next;
} eth_desc_t;

#define NUM_RX_DESCS 16
#define NUM_TX_DESCS 4

static eth_desc_t rx_desc[NUM_RX_DESCS];
static eth_desc_t tx_desc[NUM_TX_DESCS];

static int next_rx;
static int next_tx;

static u32 full_duplex, phy_mode;

static inline void reset_eth(void)
{
    int i;

    jz_writel(ETH_BMR, jz_readl(ETH_BMR) | BMR_SWR);

    for (i = 0; i < MAX_WAIT; i++) {
         if(!(jz_readl(ETH_BMR) & BMR_SWR))
	     break;
         udelay(1);
    }

    if (i == MAX_WAIT)
	printf("Reset eth timeout\n");
}

static inline void enable_eth(void)
{
	jz_writel(ETH_OMR, jz_readl(ETH_OMR) | OMR_ST | OMR_SR);
}
	
static inline void disable_eth(void)
{
	jz_writel(ETH_OMR, jz_readl(ETH_OMR) & ~(OMR_ST | OMR_SR));
}

#define MII_CMD_ADDR(id, offset) (((id) << 11) | ((offset) << 6))
#define MII_CMD_READ(id, offset) (MII_CMD_ADDR(id, offset))
#define MII_CMD_WRITE(id, offset) (MII_CMD_ADDR(id, offset) | 0x2)

static u32 mii_read(int phy_id, int offset)
{
	int i;
	u32 mii_cmd = MII_CMD_READ(phy_id, offset);

	jz_writel(ETH_MIAR, mii_cmd);

	/* wait for completion */
	for (i = 0; i < MAX_WAIT; i++) {
		if (!(jz_readl(ETH_MIAR) & 0x1))
			break;
		udelay(1);
	}

	if (i == MAX_WAIT) {
		printf("MII wait timeout\n");
		return 0;
	}

	return jz_readl(ETH_MIDR) & 0x0000ffff;
}

static int autonet_complete(int phy_id)
{
	int i;

	for (i = 0; i < MAX_WAIT; i++) {
		if (mii_read(phy_id, MII_SR) & 0x0020) 
			break;  //auto negotiation completed
		udelay(500);
	}

	if (i == MAX_WAIT)
		return -1;     //auto negotiation  error
	else
		return 0;
}

static int search_phy(int phy_id)
{
	unsigned int r;
	r = mii_read(phy_id, 1);
	if (r!=0 && r!=0xffff)
		return 1;
	return 0;
}

static void config_phy(int phy_id)
{
	u32 mii_reg5;

	full_duplex = 0;

	mii_reg5 = mii_read(phy_id, MII_ANLPA);

	if (mii_reg5 != 0xffff) {
		mii_reg5 = mii_read(phy_id, MII_ANLPA);
		if ((mii_reg5 & 0x0100) || (mii_reg5 & 0x01C0) == 0x0040)
			full_duplex = 1;

		phy_mode = mii_reg5 >> 5;

		printf("ETH: setting %s %s-duplex based on MII tranceiver #%d\n",
		       (phy_mode & MII_ANLPA_100M) ? "100Mbps" : "10Mbps",
		       full_duplex ? "full" : "half", phy_id);
	}
}

static void config_mac(void)
{
	u32 omr, mcr;

	/* Set MAC address */
#define ea eth_get_dev()->enetaddr
	jz_writel(ETH_MALR, (ea[3] << 24) | (ea[2] << 16) | (ea[1] <<  8) | ea[0]);
	jz_writel(ETH_MAHR, (ea[5] <<  8) | ea[4]);

	jz_writel(ETH_HTLR, 0);
	jz_writel(ETH_HTHR, 0);

	/* Assert the MCR_PS bit in CSR */
	if (phy_mode & MII_ANLPA_100M)
		omr = OMR_SF;
	else
		omr = OMR_TTM | OMR_SF;

	mcr = MCR_TE | MCR_RE | MCR_DBF | MCR_LCC; 
    
	if (full_duplex)
		mcr |= MCR_FDX;

	/* Set the Operation Mode (OMR) and Mac Control (MCR) registers */
	jz_writel(ETH_OMR, omr);
	jz_writel(ETH_MCR, mcr);

	/* Set the Programmable Burst Length (BMR.PBL, value 1 or 4 is validate) */
	jz_writel(ETH_BMR, DMA_BURST << 8);

	/* Reset csr8 */
	jz_readl(ETH_MFCR); // missed frams counter
}

/*---------------------------------------------------------------------------
 * ETH interface routines
 *--------------------------------------------------------------------------*/

static int jz_send(struct eth_device* dev, volatile void *packet, int length)
{
	volatile eth_desc_t *desc = 
		(volatile eth_desc_t *)((unsigned int)(tx_desc + next_tx) | 0xa0000000);
	int i;

	/* tx fifo should always be idle */
	desc->addr = virt_to_phys(packet);
	desc->ctrl |= TD_LS | TD_FS | length;
	desc->status = T_OWN;

	jz_flush_dcache();
	jz_sync();

	/* Start the tx */
	jz_writel(ETH_TPDR, 1);

	i = 0;
	while (desc->status & T_OWN) {
		if(i > MAX_WAIT) {
			printf("ETH TX timeout\n");
			break;
		}
		udelay(1);
		i++;
	}

	/* Clear done bits */
	jz_writel(ETH_SR,  DMA_TX_DEFAULT);

	desc->status = 0;
	desc->addr = 0;
	desc->ctrl &= ~(TD_LS | TD_FS);

	next_tx++;
	if (next_tx >= NUM_TX_DESCS){
		next_tx=0;
	}

	return (desc->status);
}

static int jz_recv(struct eth_device* dev)
{
	volatile eth_desc_t *desc;
	int length;
	u32 status;

	for(;;) {
		desc = (volatile eth_desc_t *)((unsigned int)(rx_desc + next_rx) | 0xa0000000);

		status = desc->status;

		if (status & R_OWN) {
			/* Nothing has been received */
			return(-1);
		}

		length = ((status & RD_FL) >> 16); /* with 4-byte CRC value */

		if (status & RD_ES) {
			printf("ETH RX error 0x%x\n", status);
		}
		else {
			/* Pass the packet up to the protocol layers. */
			NetReceive(NetRxPackets[next_rx], length - 4);
		}

		/* Clear done bits */
		jz_writel(ETH_SR,  DMA_RX_DEFAULT);

		desc->status = R_OWN;

		jz_flush_dcache();
		jz_sync();

		next_rx++;
		if (next_rx >= NUM_RX_DESCS) {
			next_rx = 0;
		}
	} /* for */

	return(0); /* Does anyone use this? */
}

static int jz_init(struct eth_device* dev, bd_t * bd)
{
	int i, phyid = -1;

	/* Reset ethernet unit */
	reset_eth();

	/* Disable interrupts: we don't use ethernet interrupts */
	jz_writel(ETH_IER, 0);

	for (i=0;i<32;i++)
		if (search_phy(i)) {
			phyid = i;
			break;
		}

	if (phyid == -1)
		printf("Can't locate any PHY\n");

	/* Start Auto Negotiation of PHY 0 and check it */
	if (autonet_complete(phyid))
		printf("ETH Auto-Negotiation failed\n");

	/* Configure PHY */
	config_phy(phyid);

	/* Configure MAC */
	config_mac();

	/* Setup the Rx&Tx descriptors */
	for (i = 0; i < NUM_RX_DESCS; i++) {
		rx_desc[i].status  = R_OWN;
		rx_desc[i].ctrl    = PKTSIZE_ALIGN | RD_RCH;
		rx_desc[i].addr    = virt_to_phys(NetRxPackets[i]);
		rx_desc[i].next    = virt_to_phys(rx_desc + i + 1);
	}
	rx_desc[NUM_RX_DESCS - 1].next = virt_to_phys(rx_desc); // The last links to the first	
	rx_desc[NUM_RX_DESCS - 1].ctrl |= RD_RER;  // Set the Receive End Of Ring flag

	for (i = 0; i < NUM_TX_DESCS; i++) {
		tx_desc[i].status  = 0;
		tx_desc[i].ctrl    = TD_TCH;
		tx_desc[i].addr    = 0;
		tx_desc[i].next    = virt_to_phys(tx_desc + i + 1);
	}
	tx_desc[NUM_TX_DESCS - 1].next = virt_to_phys(tx_desc); // The last links to the first
	tx_desc[NUM_TX_DESCS - 1].ctrl |= TD_TER;  // Set the Transmit End Of Ring flag

	jz_flush_dcache();

	jz_writel(ETH_RAR, virt_to_phys(rx_desc));
	jz_writel(ETH_TAR, virt_to_phys(tx_desc));

	next_rx = next_tx = 0;

	/* Enable ETH */
	enable_eth();

	return (1);
}

static void jz_halt(struct eth_device *dev)
{
	disable_eth();
}

int jz_enet_initialize(bd_t *bis)
{
	struct eth_device *dev;

	dev = (struct eth_device *) malloc(sizeof *dev);
	memset(dev, 0, sizeof *dev);

	sprintf(dev->name, "JZ ETHERNET");
	dev->iobase = 0;
	dev->priv   = 0;
	dev->init   = jz_init;
	dev->halt   = jz_halt;
	dev->send   = jz_send;
	dev->recv   = jz_recv;

	eth_register(dev);

	return 1;
}

#endif /* CONFIG_JZ4730 || CONFIG_JZ5730 */
#endif 
