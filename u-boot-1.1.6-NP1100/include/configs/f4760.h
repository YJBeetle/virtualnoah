/*
 * (C) Copyright 2008  Ingenic Semiconductor
 * 
 *  Author: <lhhuang@ingenic.cn>
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
 * This file contains the configuration parameters for the fuwa board.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_MIPS32		1  /* MIPS32 CPU core */
#define CONFIG_JzRISC		1  /* JzRISC core */
#define CONFIG_JZSOC		1  /* Jz SoC */
#define CONFIG_JZ4760		1  /* Jz4760 SoC */
#define CONFIG_FPGA		1  /* f4760 is an FPGA board */
#define CONFIG_F4760		1  /* f4760 validation board */

/* memory group */
// [MAY CHANGE] RAM
#ifdef CONFIG_SDRAM_MDDR
#include "asm/jz_mem_nand_configs/MDDR_H5MS5122DFR-J3M.h"
#elif defined CONFIG_SDRAM_DDR1
#include "asm/jz_mem_nand_configs/DDR1_H5DU516ETR-E3C.h"
#elif defined CONFIG_SDRAM_DDR2
#include "asm/jz_mem_nand_configs/DDR2_H5PS1G63EFR-G7C.h"
#elif defined CONFIG_MOBILE_SDRAM
#include "asm/jz_mem_nand_configs/MSDRAM_H55S5122DFR.h"
#else
#include "asm/jz_mem_nand_configs/SDRAM_.h"
#endif
// [MAY CHANGE] NAND
#include "asm/jz_mem_nand_configs/NAND_K9GAG08U0M.h"

#define JZ4760_NORBOOT_CFG	JZ4760_NORBOOT_8BIT	/* NOR Boot config code */

#define CFG_EXTAL		24000000	/* EXTAL freq: 24MHz */
#define CFG_CPU_SPEED		CFG_EXTAL*CFG_DIV	/* CPU clock */
#define CFG_DIV                 2               /* hclk=pclk=CFG_EXTAL,
						   mclk=CFG_EXTAL/CFG_DIV, just for FPGA board */
#define	CFG_HZ			(CFG_EXTAL/256) /* incrementer freq */

#define CFG_UART_BASE  		UART1_BASE	/* Base of the UART channel */
#define CONFIG_BAUDRATE		115200
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_COMMANDS		(CONFIG_CMD_DFL | \
				 CFG_CMD_ASKENV | \
				 CFG_CMD_NAND   | \
				 CFG_CMD_DHCP	| \
				 CFG_CMD_PING)
#define CONFIG_BOOTP_MASK	( CONFIG_BOOTP_DEFAUL )

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

// [MAY CHANGE] Boot Arguments
#define CONFIG_BOOTDELAY	1
#define CONFIG_BOOTFILE	        "uImage"	/* file to load */
#if defined(CONFIG_SDRAM_MDDR)
#define CONFIG_BOOTARGS		"mem=128M console=ttyS0,9600n8 ip=dhcp nfsroot=192.168.3.56:/nfsroot/root26 rw"
#else
#define CONFIG_BOOTARGS		"mem=256M console=ttyS0,9600n8 ip=dhcp nfsroot=192.168.3.56:/nfsroot/root26 rw"
#endif
#define CONFIG_BOOTCOMMAND	"bootp;setenv serverip 192.168.3.56;tftp;bootm"
#define CFG_AUTOLOAD		"n"		/* No autoload */

#define CONFIG_NET_MULTI
#define CONFIG_ETHADDR		00:2a:c6:2c:ad:fc    /* Ethernet address */

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
#define	CFG_PROMPT		"F4760 # "	/* Monitor Command Prompt    */
#define	CFG_CBSIZE		256		/* Console I/O Buffer Size   */
#define	CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16)  /* Print Buffer Size */
#define	CFG_MAXARGS		16		/* max number of command args*/

#define CFG_MALLOC_LEN		896*1024
#define CFG_BOOTPARAMS_LEN	128*1024

#define CFG_SDRAM_BASE		0x80000000     /* Cached addr */

#define CFG_INIT_SP_OFFSET	0x400000

#define	CFG_LOAD_ADDR		0x80600000     /* default load address	*/

#define CFG_MEMTEST_START	0x80100000
#define CFG_MEMTEST_END		0x80800000

#define CFG_RX_ETH_BUFFER	16	/* use 16 rx buffers on jz47xx eth */

/*
 * Configurable options for zImage if SPL is to load zImage instead of u-boot.
 */
#define CONFIG_LOAD_UBOOT       /* If it's defined, then spl load u-boot instead of zImage, and following options isn't used */
#define PARAM_BASE		0x80004000      /* The base of parameters which will be sent to kernel zImage */
#define CFG_ZIMAGE_SIZE	        (2 << 20)	/* Size of kernel zImage */
#define CFG_ZIMAGE_DST	        0x80100000	/* Load kernel zImage to this addr */
#define CFG_ZIMAGE_START	CFG_ZIMAGE_DST	/* Start kernel zImage from this addr	*/
#define CFG_CMDLINE		CONFIG_BOOTARGS
#define CFG_NAND_ZIMAGE_OFFS	(CFG_NAND_BLOCK_SIZE*4) /* NAND offset of zImage being loaded */
#define CFG_SPI_ZIMAGE_OFFS	(256 << 10) /* NAND offset of zImage being loaded */

/*-----------------------------------------------------------------------
 * Environment
 *----------------------------------------------------------------------*/
#if !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_NAND_SPL)
#define CFG_ENV_IS_IN_FLASH     1	/* use FLASH for environment vars	*/
#else
#define CFG_ENV_IS_IN_NAND	1	/* use NAND for environment vars	*/
#endif

#define CFG_NAND_BCH_BIT        8               /* Specify the hardware BCH algorithm for 4760 (4|8) */
#define CFG_NAND_ECC_POS        3               /* Ecc offset position in oob area, its default value is 3 if it isn't defined. */
#define CFG_NAND_SMCR1          0x0d444400      /* 0x0fff7700 is slowest */
#define CFG_NAND_USE_PN         0               /* Use PN in jz4760 for TLC NAND */
#define CFG_NAND_BACKUP_NUM     1               /* TODO */

#define CFG_MAX_NAND_DEVICE     1
#define NAND_MAX_CHIPS          1
#define CFG_NAND_BASE           0xBA000000
#define NAND_ADDR_OFFSET        0x00800000
#define NAND_CMD_OFFSET         0x00400000
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
#define CFG_NAND_U_BOOT_OFFS	(CFG_NAND_BLOCK_SIZE * (CFG_NAND_BACKUP_NUM+1))	/* Offset to U-Boot image */

/* Size of U-Boot image */
#if CFG_NAND_BLOCK_SIZE > (512 << 10)
#define CFG_NAND_U_BOOT_SIZE    CFG_NAND_BLOCK_SIZE
#else
#define CFG_NAND_U_BOOT_SIZE	(512 << 10)
#endif

#ifdef CFG_ENV_IS_IN_NAND
#define CFG_ENV_SIZE		0x10000
#define CFG_ENV_OFFSET		(CFG_NAND_U_BOOT_OFFS + CFG_NAND_U_BOOT_SIZE)	/* environment starts here  */
#define CFG_ENV_OFFSET_REDUND	(CFG_ENV_OFFSET + CFG_ENV_SIZE)
#endif

/*-----------------------------------------------------------------------
 * SPI NOR FLASH configuration
 */
#define CFG_SPI_MAX_FREQ        1000000
#define CFG_SPI_U_BOOT_DST	0x80100000	/* Load NUB to this addr	*/
#define CFG_SPI_U_BOOT_START	CFG_SPI_U_BOOT_DST
#define CFG_SPI_U_BOOT_OFFS     (8 << 10)
#define CFG_SPI_U_BOOT_SIZE	(256 << 10)

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CFG_MAX_FLASH_SECT	(128)	/* max number of sectors on one chip */

#define PHYS_FLASH_1		0xB8000000 /* Flash Bank #1 */

/* The following #defines are needed to get flash environment right */
#define	CFG_MONITOR_BASE	TEXT_BASE
#define	CFG_MONITOR_LEN		(256*1024)  /* Reserve 256 kB for Monitor */

#define CFG_FLASH_BASE		PHYS_FLASH_1
/* Environment settings */
#ifdef CFG_ENV_IS_IN_FLASH

#define CFG_ENV_SECT_SIZE	0x20000 /* Total Size of Environment Sector */
#define CFG_ENV_SIZE		CFG_ENV_SECT_SIZE
#endif
#define CFG_ENV_ADDR		0xB8040000

#define CFG_DIRECT_FLASH_TFTP	1	/* allow direct tftp to flash */

#if 0
/*--------------------------------------------------------------------------------
 * DDR2 info
 */
/* Chip Select */
#define DDR_CS1EN 1 // CSEN : whether a ddr chip exists 0 - un-used, 1 - used
#define DDR_CS0EN 1
#define DDR_DW32 1 /* 0 - 16-bit data width, 1 - 32-bit data width */

/* SDRAM paramters */
#if defined(CONFIG_SDRAM_DDR2) // ddr2
#define DDR_ROW 13 /* ROW : 12 to 14 row address */
#define DDR_COL 10 /* COL :  8 to 10 column address */
#define DDR_BANK8 0 /* Banks each chip: 0-4bank, 1-8bank */
#define DDR_CL 5 /* CAS latency: 1 to 7 */

/*
 * ddr2 controller timing1 register
 */
#define DDR_tRAS 45 /*tRAS: ACTIVE to PRECHARGE command period to the same bank. */
#define DDR_tRTP 8 /* 7.5ns READ to PRECHARGE command period. */
#define DDR_tRP 15 /* tRP: PRECHARGE command period to the same bank */
#define DDR_tRCD 15 /* ACTIVE to READ or WRITE command period to the same bank. */
#define DDR_tRC 60 /* ACTIVE to ACTIVE command period to the same bank.*/
#define DDR_tRRD 10   /* ACTIVE bank A to ACTIVE bank B command period. */
#define DDR_tWR 15 /* WRITE Recovery Time defined by register MR of DDR2 memory */
#define DDR_tWTR 2 /* unit: tCK. WRITE to READ command delay. */

/*
 * ddr2 controller timing2 register
 */
#define DDR_tRFC 128 /* ns,  AUTO-REFRESH command period. */
#define DDR_tMINSR 6 /* Minimum Self-Refresh / Deep-Power-Down */
#define DDR_tXP 2 /* EXIT-POWER-DOWN to next valid command period: 1 to 8 tCK. */
#define DDR_tMRD 2 /* unit: tCK. Load-Mode-Register to next valid command period: 1 to 4 tCK */

/*
 * ddr2 controller refcnt register
 */
#define DDR_tREFI	        7800	/* Refresh period: ns */

#elif defined(CONFIG_SDRAM_MDDR) // ddr1 and mddr
#define DDR_ROW 13 /* ROW : 12 to 14 row address */
#define DDR_COL 9 /* COL :  8 to 10 column address */
#define DDR_BANK8 0 /* Banks each chip: 0-4bank, 1-8bank */
#define DDR_CL 3 /* CAS latency: 1 to 7 */
/*
 * ddr2 controller timing1 register
 */
#define DDR_tRAS 45 /*tRAS: ACTIVE to PRECHARGE command period to the same bank. */
#define DDR_tRTP 12 /* 7.5ns READ to PRECHARGE command period. */
#define DDR_tRP 18 /* tRP: PRECHARGE command period to the same bank */
#define DDR_tRCD 18 /* ACTIVE to READ or WRITE command period to the same bank. */
#define DDR_tRC 60 /* ACTIVE to ACTIVE command period to the same bank.*/
#define DDR_tRRD 12   /* ACTIVE bank A to ACTIVE bank B command period. */
#define DDR_tWR 15 /* WRITE Recovery Time defined by register MR of DDR2 memory */
#define DDR_tWTR 1 /* WRITE to READ command delay. */
/*
 * ddr2 controller timing2 register
 */
#define DDR_tRFC 72 /* ns,  AUTO-REFRESH command period. */
#define DDR_tMINSR 6 /* Minimum Self-Refresh / Deep-Power-Down */
#define DDR_tXP 3 /* EXIT-POWER-DOWN to next valid command period: 1 to 8 tCK. */
#define DDR_tMRD 2 /* unit: tCK Load-Mode-Register to next valid command period: 1 to 4 tCK */
/*
 * ddr2 controller refcnt register
 */
#define DDR_tREFI	        7800	/* Refresh period: 4096 refresh cycles/64ms */

#elif defined(CONFIG_SDRAM_DDR1) // ddr1 and mddr
#define DDR_ROW 13 /* ROW : 12 to 14 row address */
#define DDR_COL 10  /* COL :  8 to 10 column address */
#define DDR_BANK8 0 /* Banks each chip: 0-4bank, 1-8bank */
#define DDR_CL 1 /* CAS latency: 1 to 7 */
#define DDR_CL_HALF 0 /*Only for DDR1, Half CAS latency: 0 or 1 */
/*
 * ddr2 controller timing1 register
 */
#define DDR_tRAS 40 /*tRAS: ACTIVE to PRECHARGE command period to the same bank. */
#define DDR_tRTP 12 /* 7.5ns READ to PRECHARGE command period. */
#define DDR_tRP 15 /* tRP: PRECHARGE command period to the same bank */
#define DDR_tRCD 15 /* ACTIVE to READ or WRITE command period to the same bank. */
#define DDR_tRC 55 /* ACTIVE to ACTIVE command period to the same bank.*/
#define DDR_tRRD 10   /* ACTIVE bank A to ACTIVE bank B command period. */
#define DDR_tWR 15 /* WRITE Recovery Time defined by register MR of DDR2 memory */
#define DDR_tWTR 2 /* WRITE to READ command delay 2*tCK */
/*
 * ddr2 controller timing2 register
 */
#define DDR_tRFC 70 /* ns,  AUTO-REFRESH command period. */
#define DDR_tMINSR 6 /* Minimum Self-Refresh / Deep-Power-Down */
#define DDR_tXP 2 /* EXIT-POWER-DOWN to next valid command period: 1 to 8 tCK. */
#define DDR_tMRD 2 /* unit: tCK. Load-Mode-Register to next valid command period: 1 to 4 tCK */
/*
 * ddr2 controller refcnt register
 */
#define DDR_tREFI	        7800	/* Refresh period: 4096 refresh cycles/64ms */

#endif

#define DDR_CLK_DIV 1    /* Clock Divider. auto refresh
						  *	cnt_clk = memclk/(16*(2^DDR_CLK_DIV))
						  */
#endif
/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CFG_DCACHE_SIZE		16384
#define CFG_ICACHE_SIZE		16384
#define CFG_CACHELINE_SIZE	32

/*====================================================================== 
 * GPIO
 */
#define GPIO_LCD_PWM   		(32*2+14) /* GPE14 PWM4 */

#endif
