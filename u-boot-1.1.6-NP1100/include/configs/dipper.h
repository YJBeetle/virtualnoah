/*
 * (C) Copyright 2006
 * Ingenic Semiconductor, <jlwei@ingenic.cn>
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
 * This file contains the configuration parameters for the jz4725 dipper board.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_MIPS32		1  /* MIPS32 CPU core */
#define CONFIG_JzRISC		1  /* JzRISC core */
#define CONFIG_JZSOC		1  /* Jz SoC */
#define CONFIG_JZ4725		1  /* Jz4725 SoC */
#define CONFIG_JZ4740		1  /* Jz4740 SoC */
#define CONFIG_DIPPER		1  /* DIPPER validation board */

//#define CONFIG_LCD                 /* LCD support */
#define CONFIG_JZLCD_SAMSUNG_LTP400WQF02_18BIT
#define LCD_BPP			5  /* 5: 18,24,32 bits per pixel */
#define CFG_WHITE_ON_BLACK
#define CONFIG_LCD_LOGO

#define JZ4740_NORBOOT_CFG	JZ4740_NORBOOT_16BIT	/* NOR Boot config code */
#define JZ4740_NANDBOOT_CFG	JZ4740_NANDBOOT_B8R3	/* NAND Boot config code */

#define CFG_CPU_SPEED		336000000	/* CPU clock: 336 MHz */
#define CFG_EXTAL		12000000	/* EXTAL freq: 12 MHz */
#define	CFG_HZ			(CFG_EXTAL/256) /* incrementer freq */

#define CFG_UART_BASE  		UART0_BASE	/* Base of the UART channel */

#define CONFIG_BAUDRATE		57600
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

#define CONFIG_MMC      	1
#define CONFIG_FAT      	1    
#define CONFIG_SUPPORT_VFAT 	1

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_COMMANDS		(CONFIG_CMD_DFL | \
				 CFG_CMD_ASKENV | \
                                 CFG_CMD_NAND   | \
				 CFG_CMD_MMC    | \
                                 CFG_CMD_FAT    | \
				 CFG_CMD_DHCP	| \
				 CFG_CMD_PING   )

#define CONFIG_BOOTP_MASK	( CONFIG_BOOTP_DEFAUL )

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#define CONFIG_BOOTDELAY	3
#define CONFIG_BOOTFILE	        "uImage"	/* file to load */
#define CONFIG_BOOTARGS		"mem=32M console=ttyS0,57600n8 ip=off root=/dev/mtdblock2 rw"
#define CONFIG_BOOTCOMMAND	"nand read 0x80600000 0x400000 0x300000;bootm"
#define CFG_AUTOLOAD		"n"		/* No autoload */

//#define CONFIG_NET_MULTI

#define CONFIG_DRIVER_CS8900      1
#define CS8900_BASE             (0xa8000000)
#define CS8900_BUS16

#define CONFIG_ETHADDR		00:2a:cc:2a:af:fe    /* Ethernet address */

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
#define	CFG_PROMPT		"DIPPER # "	/* Monitor Command Prompt    */
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
 * Environment
 *----------------------------------------------------------------------*/
#if !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_NAND_SPL)
#define CFG_ENV_IS_IN_FLASH     1	/* use FLASH for environment vars	*/
#else
#define CFG_ENV_IS_IN_NAND	1	/* use NAND for environment vars	*/
#endif

/*-----------------------------------------------------------------------
 * NAND FLASH configuration
 */
#define CFG_MAX_NAND_DEVICE     1
#define NAND_MAX_CHIPS          1
#define CFG_NAND_BASE           0xB8000000
#define CFG_NAND_SELECT_DEVICE  1       /* nand driver supports mutipl. chips   */

/*
 * IPL (Initial Program Loader, integrated inside CPU)
 * Will load first 8k from NAND (SPL) into cache and execute it from there.
 *
 * SPL (Secondary Program Loader)
 * Will load special U-Boot version (NUB) from NAND and execute it. This SPL
 * has to fit into 8kByte. It sets up the CPU and configures the SDRAM
 * controller and the NAND controller so that the special U-Boot image can be
 * loaded from NAND to SDRAM.
 *
 * NUB (NAND U-Boot)
 * This NAND U-Boot (NUB) is a special U-Boot version which can be started
 * from RAM. Therefore it mustn't (re-)configure the SDRAM controller.
 *
 */
#define CFG_NAND_U_BOOT_DST	0x80100000	/* Load NUB to this addr	*/
#define CFG_NAND_U_BOOT_START	CFG_NAND_U_BOOT_DST /* Start NUB from this addr	*/

/*
 * Define the partitioning of the NAND chip (only RAM U-Boot is needed here)
 */
#define CFG_NAND_U_BOOT_OFFS	(256 << 10)	/* Offset to RAM U-Boot image	*/
#define CFG_NAND_U_BOOT_SIZE	(512 << 10)	/* Size of RAM U-Boot image	*/

#define CFG_NAND_BLOCK_SIZE	(16 << 10)	/* NAND chip block size		*/
#define CFG_NAND_BADBLOCK_PAGE	0		/* NAND bad block was marked at this page in a block, starting from 0 */

#ifdef CFG_ENV_IS_IN_NAND
#define CFG_ENV_SIZE		CFG_NAND_BLOCK_SIZE
#define CFG_ENV_OFFSET		(CFG_NAND_BLOCK_SIZE + CFG_NAND_U_BOOT_SIZE + CFG_NAND_BLOCK_SIZE)	/* environment starts here  */
#define CFG_ENV_OFFSET_REDUND	(CFG_ENV_OFFSET + CFG_ENV_SIZE)
#endif


/*-----------------------------------------------------------------------
 * NOR FLASH and environment organization
 */
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CFG_MAX_FLASH_SECT	(128)	/* max number of sectors on one chip */

#define PHYS_FLASH_1		0xa8000000 /* Flash Bank #1 */

/* The following #defines are needed to get flash environment right */
#define	CFG_MONITOR_BASE	TEXT_BASE   /* in dipper/config.mk TEXT_BASE=0x88000000*/ 
#define	CFG_MONITOR_LEN		(256*1024)  /* Reserve 256 kB for Monitor */

#define CFG_FLASH_BASE		PHYS_FLASH_1

/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT	(2 * CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT	(2 * CFG_HZ) /* Timeout for Flash Write */

#ifdef CFG_ENV_IS_IN_FLASH
#define	CFG_ENV_IS_NOWHERE	1
#define CFG_ENV_ADDR		0xa8040000
#define CFG_ENV_SIZE		0x20000
#endif

/*-----------------------------------------------------------------------
 * SDRAM Info.
 */
#define CONFIG_NR_DRAM_BANKS	1

// SDRAM paramters
#define SDRAM_BW16		1	/* Data bus width: 0-32bit, 1-16bit */
#define SDRAM_BANK4		1	/* Banks each chip: 0-2bank, 1-4bank */
#define SDRAM_ROW		13	/* Row address: 11 to 13 */
#define SDRAM_COL		9	/* Column address: 8 to 12 */
#define SDRAM_CASL		3	/* CAS latency: 2 or 3 */

// SDRAM Timings, unit: ns
#define SDRAM_TRAS		45	/* RAS# Active Time */
#define SDRAM_RCD		20	/* RAS# to CAS# Delay */
#define SDRAM_TPC		20	/* RAS# Precharge Time */
#define SDRAM_TRWL		7	/* Write Latency Time */
#define SDRAM_TREF	        15625	/* Refresh period: 4096 refresh cycles/64ms */

/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CFG_DCACHE_SIZE		16384
#define CFG_ICACHE_SIZE		16384
#define CFG_CACHELINE_SIZE	32

/*-----------------------------------------------------------------------
 * GPIO definition
 */

#define GPIO_SD_VCC_EN_N	85  /* GPC21 */
#define GPIO_SD_CD_N		91  /* GPC27 */
#define GPIO_SD_WP		112 /* GPD16 */
#define GPIO_USB_DETE		124 /* GPD28 */
#define GPIO_DC_DETE_N		103 /* GPD7 */
#define GPIO_CHARG_STAT_N	86 /* GPC22 */
#define GPIO_DISP_OFF_N		118 /* GPD22 */

#endif	/* __CONFIG_H */
