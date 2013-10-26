/*
 *  Startup Code for MIPS32 CPU-core
 *
 *  Copyright (c) 2003	Wolfgang Denk <wd@denx.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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
#include <version.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <asm/cacheops.h>

#ifdef CONFIG_JZ4730
#include <asm/jz4730.h>
#endif
#ifdef CONFIG_JZ4740
#include <asm/jz4740.h>
#endif
#ifdef CONFIG_JZ4750
#include <asm/jz4750.h>
#endif
#ifdef CONFIG_JZ4750D
#include <asm/jz4750d.h>
#endif
#ifdef CONFIG_JZ4750L
#include <asm/jz4750l.h>
#endif
#ifdef CONFIG_JZ4760
#include <asm/jz4760.h>
#endif	
#ifdef CONFIG_JZ4810
#include <asm/jz4810.h>
#endif		
#if defined(CONFIG_JZ4750) || defined(CONFIG_JZ4750D) || defined(CONFIG_JZ4750L)
#define JZ4750_NANDBOOT_CFG0 (0x55555500 | (CFG_NAND_BW8*0xff))
#define JZ4750_NANDBOOT_CFG1 0x55555555	
#define JZ4750_NANDBOOT_CFG2 ((CFG_NAND_PAGE_SIZE==2048)&0xff0000) | ((CFG_NAND_PAGE_SIZE!=512)&0xff00) | ((CFG_NAND_ROW_CYCLE==3)&0xff)
#endif

#if defined(CONFIG_JZ4760) || defined(CONFIG_JZ4810)
#define JZ4760_NANDBOOT_CFG0 (0x55555500 | (CFG_NAND_BW8*0xff))
#define JZ4760_NANDBOOT_CFG1 0x55555555	
#define JZ4760_NANDBOOT_CFG2 ((CFG_NAND_PAGE_SIZE==2048)&0xff0000) | ((CFG_NAND_PAGE_SIZE!=512)&0xff00) | ((CFG_NAND_ROW_CYCLE==3)&0xff)
#endif

#define JZ_MSCBOOT_CFG       0x4d53504c
#define JZ_SPIBOOT_CFG0      0x55020304
#define	CFG_SSI_GR           ((6000000 + (CFG_SPI_MAX_FREQ - 1)) / CFG_SPI_MAX_FREQ - 1)
#define JZ_SPIBOOT_CFG1      (0x00aa55aa | (CFG_SSI_GR << 24))

#define RVECENT(f,n) \
   b f; nop
#define XVECENT(f,bev) \
   b f     ;           \
   li k0,bev

	.set noreorder

	.globl _start
	.text
_start:
#if defined(CONFIG_JZ4740)
#if !defined(CONFIG_NAND_SPL) && !defined(CONFIG_NAND_U_BOOT)
	.word JZ4740_NORBOOT_CFG	/* fetched during NOR Boot */
#else
#if defined(CONFIG_NAND_SPL)
	.word JZ4740_NANDBOOT_CFG	/* fetched during NAND Boot */
#endif
#endif
#endif /* CONFIG_JZ4740 */
#if defined(CONFIG_JZ4750) || defined(CONFIG_JZ4750D) || defined(CONFIG_JZ4750L)
#if defined(CONFIG_NAND_SPL)
	/* First three words fetched by CPU during NAND Boot */
	.word JZ4750_NANDBOOT_CFG0
	.word JZ4750_NANDBOOT_CFG1
	.word JZ4750_NANDBOOT_CFG2
#elif defined(CONFIG_JZ4750L) && defined(CONFIG_MSC_SPL)
	.word JZ_MSCBOOT_CFG
#elif !defined(CONFIG_JZ4750L) && defined(CONFIG_SPI_SPL)
	.word JZ_SPIBOOT_CFG0
	.word JZ_SPIBOOT_CFG1
#elif !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_MSC_U_BOOT) && !defined(CONFIG_SPI_U_BOOT)
	.word JZ4750_NORBOOT_CFG	/* fetched during NOR Boot */
#endif
#endif /* defined(CONFIG_JZ4750) || defined(CONFIG_JZ4750D) || defined(CONFIG_JZ4750L) */
#if defined(CONFIG_JZ4760) || defined(CONFIG_JZ4810)
#if defined(CONFIG_NAND_SPL)
	/* First three words fetched by CPU during NAND Boot */
	/* First 4 flags fetched by CPU during NAND Boot */
	//	Flag name		Length(in byte)		Location(in byte)
	//  	buswidth_flag		64			0
	//  	rowcycle_flag		32			64
	//  	pagesize_flag1		32			96
	//  	pagesize_flag0		32			128

	//	buswidth_flag	--	bus width(8/16bit)
	//  	0x55			8bit
	//  	0xaa			16bit
	//	rowcycle_flag	--	row cycle(2/3)
	//	0x55			2
	//	0xaa			3
	//   pagesize_flag1, pagesize_flag0	--	pagesize(byte)
	//  	0x55		0x55			512
	//	0x55		0xaa			2048
	//	0xaa		0x55			4096
	//	0xaa		0xaa			8192

	/* buswidth_flag */
#if (CFG_NAND_BW8 == 1)
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
#else
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa	
#endif
	/* rowcycle_flag */
#if (CFG_NAND_ROW_CYCLE == 3)
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa	
#else
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
#endif
	/* pagesize_flag1 */
#if (CFG_NAND_PAGE_SIZE < 4096)
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555	
#else
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa	
#endif
	/* pagesize_flag0 */
#if ((CFG_NAND_PAGE_SIZE == 512) || (CFG_NAND_PAGE_SIZE == 4096))
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555
	.word	0x55555555	
#else
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
	.word	0xaaaaaaaa
#endif
#elif defined(CONFIG_MSC_SPL)
	.word   JZ_MSCBOOT_CFG
#elif defined(CONFIG_SPI_SPL)
	.word   JZ_SPIBOOT_CFG0
	.word   JZ_SPIBOOT_CFG1
#else
	.word JZ4760_NORBOOT_CFG
#endif
#endif /* CONFIG_JZ4760 || CONFIG_JZ4810 */

#if !defined(CONFIG_JzRISC)
	RVECENT(reset,0)	/* U-boot entry point */
	RVECENT(reset,1)	/* software reboot */
#if defined(CONFIG_INCA_IP)
	.word INFINEON_EBU_BOOTCFG /* EBU init code, fetched during booting */
	.word 0x00000000           /* phase of the flash                    */
#elif defined(CONFIG_PURPLE)
	.word INFINEON_EBU_BOOTCFG /* EBU init code, fetched during booting */
	.word INFINEON_EBU_BOOTCFG /* EBU init code, fetched during booting */
#else
	RVECENT(romReserved,2)
#endif
	RVECENT(romReserved,3)
	RVECENT(romReserved,4)
	RVECENT(romReserved,5)
	RVECENT(romReserved,6)
	RVECENT(romReserved,7)
	RVECENT(romReserved,8)
	RVECENT(romReserved,9)
	RVECENT(romReserved,10)
	RVECENT(romReserved,11)
	RVECENT(romReserved,12)
	RVECENT(romReserved,13)
	RVECENT(romReserved,14)
	RVECENT(romReserved,15)
	RVECENT(romReserved,16)
	RVECENT(romReserved,17)
	RVECENT(romReserved,18)
	RVECENT(romReserved,19)
	RVECENT(romReserved,20)
	RVECENT(romReserved,21)
	RVECENT(romReserved,22)
	RVECENT(romReserved,23)
	RVECENT(romReserved,24)
	RVECENT(romReserved,25)
	RVECENT(romReserved,26)
	RVECENT(romReserved,27)
	RVECENT(romReserved,28)
	RVECENT(romReserved,29)
	RVECENT(romReserved,30)
	RVECENT(romReserved,31)
	RVECENT(romReserved,32)
	RVECENT(romReserved,33)
	RVECENT(romReserved,34)
	RVECENT(romReserved,35)
	RVECENT(romReserved,36)
	RVECENT(romReserved,37)
	RVECENT(romReserved,38)
	RVECENT(romReserved,39)
	RVECENT(romReserved,40)
	RVECENT(romReserved,41)
	RVECENT(romReserved,42)
	RVECENT(romReserved,43)
	RVECENT(romReserved,44)
	RVECENT(romReserved,45)
	RVECENT(romReserved,46)
	RVECENT(romReserved,47)
	RVECENT(romReserved,48)
	RVECENT(romReserved,49)
	RVECENT(romReserved,50)
	RVECENT(romReserved,51)
	RVECENT(romReserved,52)
	RVECENT(romReserved,53)
	RVECENT(romReserved,54)
	RVECENT(romReserved,55)
	RVECENT(romReserved,56)
	RVECENT(romReserved,57)
	RVECENT(romReserved,58)
	RVECENT(romReserved,59)
	RVECENT(romReserved,60)
	RVECENT(romReserved,61)
	RVECENT(romReserved,62)
	RVECENT(romReserved,63)
	XVECENT(romExcHandle,0x200)	/* bfc00200: R4000 tlbmiss vector */
	RVECENT(romReserved,65)
	RVECENT(romReserved,66)
	RVECENT(romReserved,67)
	RVECENT(romReserved,68)
	RVECENT(romReserved,69)
	RVECENT(romReserved,70)
	RVECENT(romReserved,71)
	RVECENT(romReserved,72)
	RVECENT(romReserved,73)
	RVECENT(romReserved,74)
	RVECENT(romReserved,75)
	RVECENT(romReserved,76)
	RVECENT(romReserved,77)
	RVECENT(romReserved,78)
	RVECENT(romReserved,79)
	XVECENT(romExcHandle,0x280)	/* bfc00280: R4000 xtlbmiss vector */
	RVECENT(romReserved,81)
	RVECENT(romReserved,82)
	RVECENT(romReserved,83)
	RVECENT(romReserved,84)
	RVECENT(romReserved,85)
	RVECENT(romReserved,86)
	RVECENT(romReserved,87)
	RVECENT(romReserved,88)
	RVECENT(romReserved,89)
	RVECENT(romReserved,90)
	RVECENT(romReserved,91)
	RVECENT(romReserved,92)
	RVECENT(romReserved,93)
	RVECENT(romReserved,94)
	RVECENT(romReserved,95)
	XVECENT(romExcHandle,0x300)	/* bfc00300: R4000 cache vector */
	RVECENT(romReserved,97)
	RVECENT(romReserved,98)
	RVECENT(romReserved,99)
	RVECENT(romReserved,100)
	RVECENT(romReserved,101)
	RVECENT(romReserved,102)
	RVECENT(romReserved,103)
	RVECENT(romReserved,104)
	RVECENT(romReserved,105)
	RVECENT(romReserved,106)
	RVECENT(romReserved,107)
	RVECENT(romReserved,108)
	RVECENT(romReserved,109)
	RVECENT(romReserved,110)
	RVECENT(romReserved,111)
	XVECENT(romExcHandle,0x380)	/* bfc00380: R4000 general vector */
	RVECENT(romReserved,113)
	RVECENT(romReserved,114)
	RVECENT(romReserved,115)
	RVECENT(romReserved,116)
	RVECENT(romReserved,116)
	RVECENT(romReserved,118)
	RVECENT(romReserved,119)
	RVECENT(romReserved,120)
	RVECENT(romReserved,121)
	RVECENT(romReserved,122)
	RVECENT(romReserved,123)
	RVECENT(romReserved,124)
	RVECENT(romReserved,125)
	RVECENT(romReserved,126)
	RVECENT(romReserved,127)

	/* We hope there are no more reserved vectors!
	 * 128 * 8 == 1024 == 0x400
	 * so this is address R_VEC+0x400 == 0xbfc00400
	 */
#ifdef CONFIG_PURPLE
/* 0xbfc00400 */
	.word	0xdc870000
	.word	0xfca70000
	.word	0x20840008
	.word	0x20a50008
	.word	0x20c6ffff
	.word	0x14c0fffa
	.word	0x00000000
	.word	0x03e00008
	.word	0x00000000
	.word   0x00000000
/* 0xbfc00428 */
	.word	0xdc870000
	.word	0xfca70000
	.word	0x20840008
	.word	0x20a50008
	.word	0x20c6ffff
	.word	0x14c0fffa
	.word	0x00000000
	.word	0x03e00008
	.word	0x00000000
	.word   0x00000000
#endif /* CONFIG_PURPLE */
	.align 4
#endif /* CONFIG_JzRISC */

reset:

#if defined(CONFIG_NAND_SPL) || defined(CONFIG_MSC_SPL) || defined(CONFIG_SPI_SPL) || (!defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_MSC_U_BOOT) && !defined(CONFIG_SPI_U_BOOT))

#ifdef CONFIG_JZ4730

	/* Disable interrupts */
	la	t0, INTC_IMR
	li	t1, 0xffffffff
	sw	t1, 0(t0)

	/*
	 * Clear SCR.HGP
	 */
	la	t0, CPM_SCR
	lw	t1, 0(t0)
	ori	t1, 0x8
	xori	t1, 0x8
	sw	t1, 0(t0)

	/*
	 * Set usb port0 as host
	 */
	la	t0, HARB_HAPOR
	lw	t1, 0(t0)
	ori	t1, HARB_HAPOR_UCHSEL
	sw	t1, 0(t0)

	/*
	 * Check reset status
	 */
	la	t0, CPM_RSTR
	lw	t1, 0(t0)
	andi	t1, 0x4
	bnez	t1, resume_from_hibernate
	nop
#endif /* CONFIG_JZ4730 */

#ifdef CONFIG_JZ4760
	/*
	 * Check reset status
	 */
	la	t0, CPM_RSR
	lw	t1, 0(t0)
	andi	t1, 0x4
	bnez	t1, resume_from_sleep
	nop
#endif /* CONFIG_JZ4760 */
	
#if !defined(CONFIG_NAND_SPL) && !defined(CONFIG_MSC_SPL) && !defined(CONFIG_SPI_SPL)
	/* Clear watch registers.
	 */
	mtc0	zero, CP0_WATCHLO
	mtc0	zero, CP0_WATCHHI
#endif

	/* STATUS register */
#ifdef CONFIG_JzRISC
	/* 
	 * CU0=UM=EXL=IE=0, BEV=ERL=1, IP2~7=1
	 */
	li	t0, 0x0040FC04
	mtc0	t0, CP0_STATUS
#else
#ifdef  CONFIG_TB0229
	li	k0, ST0_CU0
#else
	mfc0	k0, CP0_STATUS
#endif
	li	k1, ~ST0_IE
	and	k0, k1
	mtc0	k0, CP0_STATUS
#endif

	/* CAUSE register */
#ifdef CONFIG_JzRISC
	/* IV=1, use the specical interrupt vector (0x200) */
	li	t1, 0x00800000
	mtc0	t1, CP0_CAUSE
#else
	mtc0	zero, CP0_CAUSE
#endif

#ifndef CONFIG_JzRISC
	/* Init Timer */
	mtc0	zero, CP0_COUNT
	mtc0	zero, CP0_COMPARE
#endif

#endif /* defined(CONFIG_NAND_SPL) || defined(CONFIG_MSC_SPL) || defined(CONFIG_SPI_SPL) || nor boot */

#if !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_MSC_U_BOOT) && !defined(CONFIG_SPI_U_BOOT)
	/* CONFIG0 register */
	li	t0, CONF_CM_UNCACHED
	mtc0	t0, CP0_CONFIG
#endif

	/* Initialize GOT pointer.
	*/
	bal     1f
	nop
	.word   _GLOBAL_OFFSET_TABLE_
	1:
	move    gp, ra
	lw      t1, 0(ra)
	move	gp, t1

#ifdef CONFIG_INCA_IP
	/* Disable INCA-IP Watchdog.
	 */
	la      t9, disable_incaip_wdt
	jalr    t9
	nop
#endif

/* JzRISC will init external memory in board_init_f,
   which uses cache as stack and calls into C code. */
#ifndef CONFIG_JzRISC
	/* Initialize any external memory.
	 */
	la      t9, lowlevel_init
	jalr    t9
	nop
#endif

#if !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_MSC_U_BOOT) && !defined(CONFIG_SPI_U_BOOT)
	/* Initialize caches...
	 */
#ifdef CONFIG_JzRISC
	.set	mips32
	mtc0	zero, CP0_TAGLO
	mtc0	zero, CP0_TAGHI

	li	t0, K0BASE
	ori	t1, t0, CFG_DCACHE_SIZE
1:
	cache	Index_Store_Tag_D, 0(t0)
	bne	t0, t1, 1b
	addiu	t0, t0, CFG_CACHELINE_SIZE

	li	t0, K0BASE
	ori	t1, t0, CFG_ICACHE_SIZE
2:
	cache	Index_Store_Tag_I, 0(t0)
	bne	t0, t1, 2b
	addiu	t0, t0, CFG_CACHELINE_SIZE

	/* Invalidate BTB */
	mfc0	t0, CP0_CONFIG, 7
	nop
	ori	t0, 2
	mtc0	t0, CP0_CONFIG, 7
	nop

	.set	mips2
#else
	la      t9, mips_cache_reset
	jalr    t9
	nop
#endif

	/* ... and enable them.
	 */
	li	t0, CONF_CM_CACHABLE_NONCOHERENT
	mtc0	t0, CP0_CONFIG
	nop

#endif /* !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_MSC_U_BOOT) && !defined(CONFIG_SPI_U_BOOT) */

	/* Set up temporary stack.
	 */
#ifndef CONFIG_JzRISC
	li	a0, CFG_INIT_SP_OFFSET
	la      t9, mips_cache_lock
	jalr    t9
	nop
#endif

#if defined(CONFIG_NAND_SPL) || defined(CONFIG_MSC_SPL) || defined(CONFIG_SPI_SPL)
	la	sp, 0x80004000
	la	t9, spl_boot
	j	t9
	nop
#else
#if defined(CONFIG_JZ4760) || defined(CONFIG_JZ4810) 
	/* Sdram will be access when "la sp, 0(t0)", so it should be
	 * configured before accessing,	but it needn't be configured
	 * again when nand boot for it has been initialized in spl
	 */
#if 0	/* test gpio */
	la	$29, GPIO_BASE
	li	$9, 0x00200000		// CS1  (32*0+21)
	sw	$9, 0x048($29)		// PCFUNC
	sw	$9, 0x058($29)		// PCSELC
	sw	$9, 0x064($29)		// PCDIRS

1:
	sw	$9, 0x014($29)		// PCDATS
	nop;nop;nop;nop;nop;nop;nop;nop
	sw	$9, 0x018($29)		// PCDATC
	nop;nop;nop;nop;nop;nop;nop;nop
	b 1b
        nop
#endif	
#if !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_MSC_U_BOOT) && !defined(CONFIG_SPI_U_BOOT)
#if defined(CONFIG_DDRC)
	/* config as ddr */
	la	t0, DDRC_BASE
	li	t1, 0x00000001
	sw	t1, 0x8(t0)
	li	t1, 0x00000040
	sw	t1, 0x4(t0)
	li	t1, 0x000020f8
	sw	t1, 0x24(t0)
#else /* sdram */
	/* config as sdr */	
	la	t0, DDRC_BASE
	li	t1, 0x10000000
	sw	t1, 0x4(t0)
#endif /*config_ddrc*/	
#endif
#endif /* Jz4760 || jz4810 */
	li	t0, CFG_SDRAM_BASE + CFG_INIT_SP_OFFSET
	la	sp, 0(t0)

	la	t9, board_init_f
	j	t9
	nop

/*
 * void relocate_code (addr_sp, gd, addr_moni)
 *
 * This "function" does not return, instead it continues in RAM
 * after relocating the monitor code.
 *
 * a0 = addr_sp
 * a1 = gd
 * a2 = destination address
 */
	.globl	relocate_code
	.ent	relocate_code
relocate_code:
	move	sp, a0		/* Set new stack pointer		*/

	li	t0, TEXT_BASE
	la	t3, in_ram
	lw	t2, -12(t3)	/* t2 <-- uboot_end_data	*/
	move	t1, a2
	subu	t2, t2, t0	/* total count in bytes */
	srl	t2, t2, 2	/* total count in word */
	li	t4, 0xa0000000
	or	t0, t0, t4	/* uncached address */

	/*
	 * Fix GOT pointer:
	 *
	 * New GOT-PTR = (old GOT-PTR - TEXT_BASE) + Destination Address
	 */
	move	t6, gp
	subu	gp, TEXT_BASE
	add	gp, a2			/* gp now adjusted		*/
	subu	t6, gp, t6		/* t6 <-- relocation offset	*/

	/*
	 * t0 = source address (in flash)
	 * t1 = target address (in memory)
	 * t2 = total count in word
	 */
	/* On the purple board we copy the code earlier in a special way
	 * in order to solve flash problems
	 */
#ifndef CONFIG_PURPLE
1:
	lw	t3, 0(t0)
	addiu	t0, 4
	addiu	t2, t2, -1
	sw	t3, 0(t1)
	bgtz	t2, 1b
	addiu	t1, 4			/* delay slot			*/
#endif

	/* If caches were enabled, we would have to flush them here.
	 */
#ifdef CONFIG_JzRISC
	/* flush d-cache */
	.set	mips32
	li	t0, KSEG0
	addi	t1, t0, CFG_DCACHE_SIZE
2:
	cache	Index_Writeback_Inv_D, 0(t0)
	bne	t0, t1, 2b
	addi	t0, CFG_CACHELINE_SIZE

	sync

	/* flush i-cache */
	li	t0, KSEG0
	addi	t1, t0, CFG_ICACHE_SIZE
3:
	cache	Index_Invalidate_I, 0(t0)
	bne	t0, t1, 3b
	addi	t0, CFG_CACHELINE_SIZE

	/* Invalidate BTB */
	mfc0	t0, CP0_CONFIG, 7
	nop
	ori	t0, 2
	mtc0	t0, CP0_CONFIG, 7
	nop

	.set	mips0
#endif

	/* Jump to where we've relocated ourselves.
	 */
	addi	t0, a2, in_ram - _start
	j	t0
	nop

	.word	uboot_end_data
	.word	uboot_end
	.word	num_got_entries

in_ram:
	/* Now we want to update GOT.
	 */
	lw	t3, -4(t0)	/* t3 <-- num_got_entries	*/
	addiu	t4, gp, 8	/* Skipping first two entries.	*/
	li	t2, 2
1:
	lw	t1, 0(t4)
	beqz	t1, 2f
	addu	t1, t6
	sw	t1, 0(t4)
2:
	addiu	t2, 1
	blt	t2, t3, 1b
	addiu	t4, 4		/* delay slot			*/

	/* Clear BSS.
	 */
	lw	t1, -12(t0)	/* t1 <-- uboot_end_data	*/
	lw	t2, -8(t0)	/* t2 <-- uboot_end		*/
	add	t1, t6		/* adjust pointers		*/
	add	t2, t6

	sub	t1, 4
1:	addi	t1, 4
	bltl	t1, t2, 1b
	sw	zero, 0(t1)	/* delay slot			*/

	move	a0, a1
	la	t9, board_init_r
	j	t9
	move	a1, a2		/* delay slot			*/

	.end	relocate_code

#endif /* defined(CONFIG_NAND_SPL) || defined(CONFIG_MSC_SPL) || defined(CONFIG_SPI_SPL) */

#if !defined(CONFIG_JzRISC)
	/* Exception handlers.
	 */
romReserved:
	b romReserved

romExcHandle:
	b romExcHandle
#endif

#ifdef CONFIG_JZ4730

/* These are the runtime values, modify them according to your platform. */
#define PLCR1_VAL     0x1b000520
#define CFCR_VAL      0x0c526220

#define DMCR_VAL0     0x042a3211
#define DMCR_VAL1     0x05aa3211 /*(DMCR_VAL0|EMC_DMCR_RFSH|EMC_DMCR_MRSET)*/

#define RTCOR_VAL     0x10
#define RTCSR_VAL     0x83

	/*
	 * cpu was reset from hibernate mode
	 */
resume_from_hibernate:
	/*
	 * Init PLL
	 */
	la	t0, 0xB0000000	/* CFCR */
	li	t1, CFCR_VAL
	sw	t1, 0(t0)

	la	t0, 0xB0000010	/* PLCR1 */
	li	t1, PLCR1_VAL
	sw	t1, 0(t0)
	nop;nop;nop;nop

	/* Init caches */
	.set	mips32
	mtc0	zero, CP0_TAGLO
	mtc0	zero, CP0_TAGHI

	li	t0, K0BASE
	ori	t1, t0, CFG_DCACHE_SIZE
1:
	cache	Index_Store_Tag_D, 0(t0)
	cache	Index_Store_Tag_I, 0(t0)
	bne	t0, t1, 1b
	addiu	t0, t0, CFG_CACHELINE_SIZE

	/*
	 * Init SDRAM
	 */
	la	t0, 0xB0010070	/* GPALR2 */
	lw	t1, 0(t0)
	li	t2, 0x3FFFFFFF
	and	t1, t2
	li	t2, 0x40000000
	or	t1, t2
	sw	t1, 0(t0)

	la	t0, 0xB0010074	/* GPAUR2 */
	lw	t1, 0(t0)
	li	t2, 0xFFFF0000
	and	t1, t2
	li	t2, 0x00005555
	or	t1, t2
	sw	t1, 0(t0)

	la	t0, 0xB3010000	/* EMC base address */

	li	t1, DMCR_VAL0	/* DMCR */
	sw	t1, 0x80(t0)

	li	t1, RTCOR_VAL
	sh	t1, 0x8c(t0)	/* RTCOR */

	li	t1, RTCSR_VAL
	sh	t1, 0x84(t0)	/* RTCSR */

	/* precharge all chip-selects */
	ori	t1, t0, 0xa088
	sb	$0, 0(t1)
	ori	t1, t0, 0xb088
	sb	$0, 0(t1)

	/* delay about 200us */
	li	t1, 0x20000
1:
	bnez	t1, 1b
	sub	t1, 1

	la	t1, DMCR_VAL1	/* DMCR */
	sw	t1, 0x80(t0)

	/* write sdram mode register for each chip-select */
	ori	t1, t0, 0xa088
	sb	$0, 0(t1)
	ori	t1, t0, 0xb088
	sb	$0, 0(t1)

	/*
	 * jump to resume entry point
	 */
	la	t0, CPM_SPR
	lw	t1, 0(t0)
	li	t0, 0x80000000
	or	t0, t1

	j	t0
	nop

#endif /* CONFIG_JZ4730 */

#ifdef CONFIG_JZ4760

	/*
	 * cpu was reset from sleep  mode(when power down p0)
	 */
resume_from_sleep:

	/*
	 * jump to resume entry point
	 */
	la	t0, CPM_CPSPR
	lw	t1, 0(t0)
	li	t0, 0x80000000
	or	t0, t1

	j	t0
	nop

#endif /* CONFIG_JZ4760 */