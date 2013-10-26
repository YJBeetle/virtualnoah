/*
 * Cirrus Logic CS8900A Ethernet
 *
 * (C) 2003 Wolfgang Denk, wd@denx.de
 *     Extension to synchronize ethaddr environment variable
 *     against value in EEPROM
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Copyright (C) 1999 Ben Williamson <benw@pobox.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is loaded into SRAM in bootstrap mode, where it waits
 * for commands on UART1 to read and write memory, jump to code etc.
 * A design goal for this program is to be entirely independent of the
 * target board.  Anything with a CL-PS7111 or EP7211 should be able to run
 * this code in bootstrap mode.  All the board specifics can be handled on
 * the host.
 *
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <config.h>
#include <common.h>
#include <malloc.h>

#ifndef CONFIG_FPGA
#if defined(CONFIG_JZ4740) || defined(CONFIG_JZ4750) || defined(CONFIG_JZ4760)
#if defined(CONFIG_JZ4740) 
#include <asm/jz4740.h>
#elif defined(CONFIG_JZ4750) 
#include <asm/jz4750.h>
#elif defined(CONFIG_JZ4760) 
#include <asm/jz4760.h>
#elif defined(CONFIG_JZ4810) 
#include <asm/jz4810.h>
#endif

#include <net.h>
struct eth_device *dev;
#define CS8900_BASE ((u32)(dev->iobase))
#define CONFIG_DRIVER_CS8900
#define CS8900_BUS16
#include <command.h>
#include <asm/io.h>

#include "jz_cs8900.h"

#ifdef CONFIG_DRIVER_CS8900

#if (CONFIG_COMMANDS & CFG_CMD_NET)

#undef DEBUG

/* packet page register access functions */

#ifdef CS8900_BUS32
/* we don't need 16 bit initialisation on 32 bit bus */
#define get_reg_init_bus(x) get_reg((x))
#else
static unsigned short get_reg_init_bus (int regno)
{
	/* force 16 bit busmode */
	volatile unsigned char c;
    //c = CS8900_BUS16_0;
	//c = CS8900_BUS16_1;
	//c = CS8900_BUS16_0;
	//c = CS8900_BUS16_1;
	//c = CS8900_BUS16_0;

	CS8900_PPTR = regno;
    //udelay(100);
	
	return (unsigned short) CS8900_PDATA;
}
#endif

static unsigned short get_reg (int regno)
{
	CS8900_PPTR = regno;
	return (unsigned short) CS8900_PDATA;
}


static void put_reg (int regno, unsigned short val)
{
	CS8900_PPTR = regno;
	CS8900_PDATA = val;
}

static void eth_reset (void)
{
	int tmo;
	unsigned short us;

	/* reset NIC */
	put_reg (PP_SelfCTL, get_reg (PP_SelfCTL) | PP_SelfCTL_Reset);

	/* wait for 200ms */
	udelay (200000);
	/* Wait until the chip is reset */

	tmo = get_timer (0) + 1 * CFG_HZ;
	while ((((us = get_reg_init_bus (PP_SelfSTAT)) & PP_SelfSTAT_InitD) == 0)
		   && tmo < get_timer (0))
		/*NOP*/;
}

static void eth_reginit (void)
{
	/* receive only error free packets addressed to this card */
	put_reg (PP_RxCTL, PP_RxCTL_IA | PP_RxCTL_Broadcast | PP_RxCTL_RxOK);
	/* do not generate any interrupts on receive operations */
	put_reg (PP_RxCFG, 0);
	/* do not generate any interrupts on transmit operations */
	put_reg (PP_TxCFG, 0);
	/* do not generate any interrupts on buffer operations */
	put_reg (PP_BufCFG, 0);
	/* enable transmitter/receiver mode */
	put_reg (PP_LineCTL, PP_LineCTL_Rx | PP_LineCTL_Tx);
}

static void cs8900_get_enetaddr (uchar * addr)
{
	int i;
	unsigned char env_enetaddr[6];
	char *tmp = getenv ("ethaddr");
	char *end;

	for (i=0; i<6; i++) {
		env_enetaddr[i] = tmp ? simple_strtoul(tmp, &end, 16) : 0;
		if (tmp)
			tmp = (*end) ? end+1 : end;
	}

	/* verify chip id */
	if (get_reg_init_bus (PP_ChipID) != 0x630e)
		return;
	eth_reset ();
	if ((get_reg (PP_SelfST) & (PP_SelfSTAT_EEPROM | PP_SelfSTAT_EEPROM_OK)) ==
			(PP_SelfSTAT_EEPROM | PP_SelfSTAT_EEPROM_OK)) {

		/* Load the MAC from EEPROM */
		for (i = 0; i < 6 / 2; i++) {
			unsigned int Addr;

			Addr = get_reg (PP_IA + i * 2);
			addr[i * 2] = Addr & 0xFF;
			addr[i * 2 + 1] = Addr >> 8;
		}

		if (memcmp(env_enetaddr, "\0\0\0\0\0\0", 6) != 0 &&
		    memcmp(env_enetaddr, addr, 6) != 0) {
			printf ("\nWarning: MAC addresses don't match:\n");
			printf ("\tHW MAC address:  "
				"%02X:%02X:%02X:%02X:%02X:%02X\n",
				addr[0], addr[1],
				addr[2], addr[3],
				addr[4], addr[5] );
			printf ("\t\"ethaddr\" value: "
				"%02X:%02X:%02X:%02X:%02X:%02X\n",
				env_enetaddr[0], env_enetaddr[1],
				env_enetaddr[2], env_enetaddr[3],
				env_enetaddr[4], env_enetaddr[5]) ;
			debug ("### Set MAC addr from environment\n");
			memcpy (addr, env_enetaddr, 6);
		}
		if (!tmp) {
			char ethaddr[20];
			sprintf (ethaddr, "%02X:%02X:%02X:%02X:%02X:%02X",
				 addr[0], addr[1],
				 addr[2], addr[3],
				 addr[4], addr[5]) ;
			debug ("### Set environment from HW MAC addr = \"%s\"\n",				ethaddr);
			setenv ("ethaddr", ethaddr);
		}

	}
}

static void jz_eth_halt (struct eth_device *dev)
{
	/* disable transmitter/receiver mode */
	put_reg (PP_LineCTL, 0);

	/* "shutdown" to show ChipID or kernel wouldn't find he cs8900 ... */
	get_reg_init_bus (PP_ChipID);
}

static int jz_eth_init (struct eth_device* dev, bd_t * bd)
{
    u16 id;
	
    dev = dev;
   
	
    /* verify chip id */
	id = get_reg_init_bus (PP_ChipID);
	if (id != 0x630e) {
		printf ("CS8900 jz_eth_init error!\n");
		return 0;
	}

	eth_reset ();
	/* set the ethernet address */
	//printf("MAC = %02x:%02x:%02x:%02x:%02x:%02x\r\n",
	//	   bd->bi_enetaddr[5],bd->bi_enetaddr[4],
	//	   bd->bi_enetaddr[3],bd->bi_enetaddr[2],
	//	   bd->bi_enetaddr[1],bd->bi_enetaddr[0]);
	
	put_reg (PP_IA + 0, bd->bi_enetaddr[0] | (bd->bi_enetaddr[1] << 8));
	put_reg (PP_IA + 2, bd->bi_enetaddr[2] | (bd->bi_enetaddr[3] << 8));
	put_reg (PP_IA + 4, bd->bi_enetaddr[4] | (bd->bi_enetaddr[5] << 8));

	eth_reginit ();
	return 0;
}

/* Get a data block via Ethernet */
static int jz_eth_rx (struct eth_device* dev)
{
	int i;
	unsigned short rxlen;
	unsigned short *addr;
	unsigned short status;
    dev = dev;
	status = get_reg (PP_RER);

	if ((status & PP_RER_RxOK) == 0)
		return 0;

	status = CS8900_RTDATA;		/* stat */
	rxlen = CS8900_RTDATA;		/* len */

#ifdef DEBUG
	if (rxlen > PKTSIZE_ALIGN + PKTALIGN)
		printf ("packet too big!\n");
#endif
	for (addr = (unsigned short *) NetRxPackets[0], i = rxlen >> 1; i > 0;
		i--)
		*addr++ = CS8900_RTDATA;
	if (rxlen & 1)
		*addr++ = CS8900_RTDATA;

	/* Pass the packet up to the protocol layers. */
	NetReceive (NetRxPackets[0], rxlen);

	return rxlen;
}

/* Send a data block via Ethernet. */
static int jz_eth_send (struct eth_device* dev,volatile void *packet, int length)
{
	volatile unsigned short *addr;
	int tmo;
	unsigned short s;
    dev = dev;
	
retry:
	/* initiate a transmit sequence */
	CS8900_TxCMD = PP_TxCmd_TxStart_Full;
	CS8900_TxLEN = length;

	/* Test to see if the chip has allocated memory for the packet */
	if ((get_reg (PP_BusSTAT) & PP_BusSTAT_TxRDY) == 0) {
		/* Oops... this should not happen! */
#ifdef DEBUG
		printf ("cs: unable to send packet; retrying...\n");
#endif
		for (tmo = get_timer (0) + 5 * CFG_HZ; get_timer (0) < tmo;)
			/*NOP*/;
		eth_reset ();
		eth_reginit ();
		goto retry;
	}

	/* Write the contents of the packet */
	/* assume even number of bytes */
	for (addr = packet; length > 0; length -= 2)
		CS8900_RTDATA = *addr++;

	/* wait for transfer to succeed */
	tmo = get_timer (0) + 5 * CFG_HZ;
	while ((s = get_reg (PP_TER) & ~0x1F) == 0) {
		if (get_timer (0) >= tmo)
			break;
	}

	/* nothing */ ;
	if ((s & (PP_TER_CRS | PP_TER_TxOK)) != PP_TER_TxOK) {
#ifdef DEBUG
		printf ("\ntransmission error %#x\n", s);
#endif
	}

	return 0;
}

static void cs8900_e2prom_ready(void)
{
	while(get_reg(PP_SelfST) & SI_BUSY);
}

/***********************************************************/
/* read a 16-bit word out of the EEPROM                    */
/***********************************************************/

static int cs8900_e2prom_read(unsigned char addr, unsigned short *value)
{
	cs8900_e2prom_ready();
	put_reg(PP_EECMD, EEPROM_READ_CMD | addr);
	cs8900_e2prom_ready();
	*value = get_reg(PP_EEData);

	return 0;
}


/***********************************************************/
/* write a 16-bit word into the EEPROM                     */
/***********************************************************/

static int cs8900_e2prom_write(unsigned char addr, unsigned short value)
{
	cs8900_e2prom_ready();
	put_reg(PP_EECMD, EEPROM_WRITE_EN);
	cs8900_e2prom_ready();
	put_reg(PP_EEData, value);
	put_reg(PP_EECMD, EEPROM_WRITE_CMD | addr);
	cs8900_e2prom_ready();
	put_reg(PP_EECMD, EEPROM_WRITE_DIS);
	cs8900_e2prom_ready();

	return 0;
}
int jz_enet_initialize(bd_t *bis)
{
	
    u32 reg;
	
	dev = (struct eth_device *) malloc(sizeof *dev);
	memset(dev, 0, sizeof *dev);

#if defined(CONFIG_JZ4740)
#define RD_N_PIN (32 + 29)
#define WE_N_PIN (32 + 30)
#define CS4_PIN (32 + 28)
	__gpio_as_func0(CS4_PIN);
	__gpio_as_func0(RD_N_PIN);
	__gpio_as_func0(WE_N_PIN);

	reg = REG_EMC_SMCR4;
	reg = (reg & (~EMC_SMCR_BW_MASK)) | EMC_SMCR_BW_16BIT;
	REG_EMC_SMCR4 = reg;
	dev->iobase = 0xa8000000;

#elif defined(CONFIG_JZ4750)
//#if !defined(CONFIG_AQUILA)
#ifndef CONFIG_AQUILA
#define RD_N_PIN (32*2 +25)
#define WE_N_PIN (32*2 +26)
#define CS3_PIN (32*2 +23)
	__gpio_as_func0(CS3_PIN);
	__gpio_as_func0(RD_N_PIN);
	__gpio_as_func0(WE_N_PIN);

	reg = REG_EMC_SMCR3;
	reg = (reg & (~EMC_SMCR_BW_MASK)) | EMC_SMCR_BW_16BIT;
	REG_EMC_SMCR3 = reg;
	dev->iobase = 0xac000000;
#endif // !defined(CONFIG_AQUILA)

#elif defined(CONFIG_JZ4760)
#define RD_N_PIN (32*0 +16)  //gpa16
#define WE_N_PIN (32*0 +17)  //gpa17

#ifdef CONFIG_LEPUS
#define WAIT_N (32*0 + 27)   //WAIT_N--->gpa27
#define CS_PIN (32*0 + 26)   //CS6--->gpa26

#elif defined(CONFIG_CYGNUS)
#define CS_PIN (32*0 + 25)   //CS5--->gpa25
#define WAIT_N (32*0 + 27)   //WAIT_N--->gpa27
#define CS8900_RESET_PIN (32 * 1 +23)  //gpb23
#endif
	__gpio_as_func0(CS_PIN);
	__gpio_as_func0(RD_N_PIN);
	__gpio_as_func0(WE_N_PIN);

	__gpio_as_func0(32 * 1 + 2);
	__gpio_as_func0(32 * 1 + 3);


#ifdef CONFIG_LEPUS
	REG_GPIO_PXFUNS(0) = 0x0000ffff;
	REG_GPIO_PXTRGC(0) = 0x0000ffff;
	REG_GPIO_PXSELC(0) = 0x0000ffff;

	__gpio_as_func0(WAIT_N);

	reg = REG_NEMC_SMCR6;
	reg = (reg & (~NEMC_SMCR_BW_MASK)) | NEMC_SMCR_BW_16BIT;
	REG_NEMC_SMCR6 = reg;
	dev->iobase = 0xb4000000;

#elif defined(CONFIG_CYGNUS)
	__gpio_as_output(CS8900_RESET_PIN);
	__gpio_set_pin(CS8900_RESET_PIN);
	udelay(10000);
	__gpio_clear_pin(CS8900_RESET_PIN);

	__gpio_as_func0(WAIT_N);

	reg = REG_NEMC_SMCR5;
	reg = (reg & (~NEMC_SMCR_BW_MASK)) | NEMC_SMCR_BW_16BIT;
	REG_NEMC_SMCR5 = reg;
	dev->iobase = 0xb5000000;
#endif
#elif defined(CONFIG_JZ4810)
#define RD_N_PIN (32*0 +16)
#define WE_N_PIN (32*0 +17)
#define CS5_PIN (32*0 +25)
#define CS8900_RESET_PIN (32 * 1 + 23)
	__gpio_as_func0(CS5_PIN);
	__gpio_as_func0(RD_N_PIN);
	__gpio_as_func0(WE_N_PIN);

	__gpio_as_func0(32 * 1 + 2);
	__gpio_as_func0(32 * 1 + 3);

	__gpio_as_output(CS8900_RESET_PIN);
	__gpio_set_pin(CS8900_RESET_PIN);
	udelay(10000);
	__gpio_clear_pin(CS8900_RESET_PIN);

	reg = REG_NEMC_SMCR5;
	reg = (reg & (~NEMC_SMCR_BW_MASK)) | NEMC_SMCR_BW_16BIT;
	REG_NEMC_SMCR5 = reg;
	dev->iobase = 0xb5000000;
#endif

	sprintf(dev->name, "JZ ETHERNET");
	dev->priv   = 0;
	dev->init   = jz_eth_init;
	dev->halt   = jz_eth_halt;
	dev->send   = jz_eth_send;
	dev->recv   = jz_eth_rx;

	eth_register(dev);
	
	return 1;
}

#endif	/* COMMANDS & CFG_NET */

#endif	/* CONFIG_DRIVER_CS8900 */
#endif  /* defined(CONFIG_JZ4740) || defined(CONFIG_JZ4750) */

#if defined(CONFIG_JZ4750D) || defined(CONFIG_JZ4750L)
int jz_enet_initialize(bd_t *bis)
{
}
#endif

#endif  /* !CONFIG_FPGA */

