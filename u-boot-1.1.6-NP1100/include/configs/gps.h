/*
 * (C) Copyright 2005
 * Ingenic Semiconductor, <seeger.chin@gmail.com>
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

/*
 * This file contains the configuration parameters for the GPS board.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_MIPS32		1  /* MIPS32 CPU core */
#define CONFIG_JzRISC		1  /* JzRISC core */
#define CONFIG_JZSOC		1  /* Jz SoC */
#define CONFIG_JZ4730		1  /* Jz4730 SoC */
#define CONFIG_GPS		1  /* gps board */

#define CFG_CPU_SPEED		336000000	/* CPU speed */

#define CFG_EXTAL		3686400		/* EXTAL freq: 3.6864 MHz */
#define	CFG_HZ			(CFG_CPU_SPEED/(3*256)) /* incrementer freq */

#define CFG_UART_BASE  		UART0_BASE	/* Base of the UART channel */
#define CONFIG_BAUDRATE		115200
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

#define CONFIG_COMMANDS		(CONFIG_CMD_DFL | \
				 CFG_CMD_ASKENV | \
				 CFG_CMD_DHCP	| \
				 CFG_CMD_PING   )

#define CONFIG_BOOTP_MASK	( CONFIG_BOOTP_DEFAUL )

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#define CONFIG_BOOTDELAY	3
#define CONFIG_BOOTFILE	        "uImage"	/* file to load */
#define CONFIG_BOOTARGS		"mem=16M console=ttyS0,115200 ip=bootp nfsroot=192.168.1.20:/nfsroot/miniroot rw ethaddr=00:19:11:20:00:01"
#define CONFIG_BOOTCOMMAND	"bootp;setenv serverip 192.168.1.20;tftp;bootm"
#define CFG_AUTOLOAD		"n"		/* No autoload */

#define CONFIG_NET_MULTI
#define CONFIG_ETHADDR		00:19:11:20:00:01    /* Ethernet address */

/*
 * Serial download configuration
 *
 */
#define CONFIG_LOADS_ECHO	1	/* echo on for serial download	*/
#define CFG_LOADS_BAUD_CHANGE	1	/* allow baudrate change	*/

/*
 * Miscellaneous configurable options
 */
#define	CFG_LONGHELP				/* undef to save memory      */
#define	CFG_PROMPT		"GPS # "	/* Monitor Command Prompt    */
#define	CFG_CBSIZE		256		/* Console I/O Buffer Size   */
#define	CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16)  /* Print Buffer Size */
#define	CFG_MAXARGS		16		/* max number of command args*/

#define CFG_MALLOC_LEN		128*1024
#define CFG_BOOTPARAMS_LEN	128*1024

#define CFG_SDRAM_BASE		0x80000000     /* Cached addr */

#define CFG_INIT_SP_OFFSET	0x400000

#define	CFG_LOAD_ADDR		0x80600000     /* default load address	*/

#define CFG_MEMTEST_START	0x80100000
#define CFG_MEMTEST_END		0x80800000

#define CFG_RX_ETH_BUFFER	16	/* use 16 rx buffers on jz47xx eth */

/*-----------------------------------------------------------------------
 * Flash configuration
 * (SST 39VF3201 16-bit Flash): 2KWord(4KB) * 1024 = 4MB
 */
#define CFG_MAX_FLASH_BANKS	1	/* max # of memory banks	*/
#define CFG_MAX_FLASH_SECT	1024	/* max # of sectors on one chip	*/

#define PHYS_FLASH_1		0xbfc00000   /* Flash Bank #1 */
#define CFG_FLASH_BASE		PHYS_FLASH_1 /* Flash at 0xbfc00000 - 0xbfffffff */

#define	CFG_MONITOR_BASE	0xbfc00000
#define	CFG_MONITOR_LEN		(180*1024)  /* Reserve 256 kB for Monitor */

/* Environment settings */
#define CFG_ENV_IS_IN_FLASH	1
#define CFG_ENV_SECT_SIZE	(20*1024) /* Total Size of Environment Sector */
#define CFG_ENV_SIZE		CFG_ENV_SECT_SIZE
#define CFG_ENV_ADDR		(CFG_MONITOR_BASE + CFG_MONITOR_LEN) /* Environment after Monitor */

#define CFG_DIRECT_FLASH_TFTP	1	/* allow direct tftp to flash */
#define CONFIG_ENV_OVERWRITE	1	/* allow overwrite MAC address */

/*
 *  SDRAM Info.
 */
#define CONFIG_NR_DRAM_BANKS	1

// SDRAM paramters
#define SDRAM_BW16		0	/* Data bus width: 0-32bit, 1-16bit */
#define SDRAM_BANK4		1	/* Banks each chip: 0-2bank, 1-4bank */
#define SDRAM_ROW		12	/* Row address: 11 to 13 */
#define SDRAM_COL		8	/* Column address: 8 to 12 */
#define SDRAM_CASL		2	/* CAS latency: 2 or 3 */

// SDRAM Timings, unit: ns
#define SDRAM_TRAS		35	/* RAS# Active Time */
#define SDRAM_RCD		15	/* RAS# to CAS# Delay */
#define SDRAM_TPC		20	/* RAS# Precharge Time */
#define SDRAM_TRWL		7	/* Write Latency Time */
#define SDRAM_TREF	        ((64*1000000)/(1<<SDRAM_ROW))	/* Refresh period: 8192 refresh cycles/64ms */

/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CFG_DCACHE_SIZE		16384
#define CFG_ICACHE_SIZE		16384
#define CFG_CACHELINE_SIZE	32

#endif	/* __CONFIG_H */
