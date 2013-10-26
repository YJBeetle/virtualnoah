/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, <wd@denx.de>
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

#include <common.h>
#include <command.h>
#include <asm/inca-ip.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <asm/cacheops.h>

#ifdef CONFIG_JZ4730
#include <asm/jz4730.h>
#endif

#ifdef CONFIG_JZ4740
#include <asm/jz4740.h>
#endif

#ifdef CONFIG_JZ5730
#include <asm/jz5730.h>
#endif

#ifdef CONFIG_JZ4750
#include <asm/jz4750.h>
#endif

#ifdef CONFIG_JZ4750L
#include <asm/jz4750l.h>
#endif

#ifdef CONFIG_JZ4760
#include <asm/jz4760.h>
#endif

#if !defined (CONFIG_NAND_SPL) && !defined (CONFIG_MSC_SPL) 

int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#ifdef CONFIG_JZ4730
	__wdt_set_count(0xffffffff-32);
	__wdt_start();
	while(1);
#endif
#if defined(CONFIG_JZ4740) || defined(CONFIG_JZ4750) || defined(CONFIG_JZ4760) || defined(CONFIG_JZ4750L)
	__wdt_select_extalclk();
	__wdt_select_clk_div64();
	__wdt_set_data(100);
	__wdt_set_count(0);
	__tcu_start_wdt_clock();
	__wdt_start();
	while(1);
#endif
#if defined(CONFIG_INCA_IP)
	*INCA_IP_WDT_RST_REQ = 0x3f;
#elif defined(CONFIG_PURPLE) || defined(CONFIG_TB0229) || defined(CONFIG_JzRISC)
	void (*f)(void) = (void *) 0xbfc00000;

	f();
#endif
	fprintf(stderr, "*** reset failed ***\n");
	return 0;
}

#define cache16_unroll32(base,op)				\
	__asm__ __volatile__("					\
		.set noreorder;					\
		.set mips3;					\
		cache %1, 0x000(%0); cache %1, 0x010(%0);	\
		cache %1, 0x020(%0); cache %1, 0x030(%0);	\
		cache %1, 0x040(%0); cache %1, 0x050(%0);	\
		cache %1, 0x060(%0); cache %1, 0x070(%0);	\
		cache %1, 0x080(%0); cache %1, 0x090(%0);	\
		cache %1, 0x0a0(%0); cache %1, 0x0b0(%0);	\
		cache %1, 0x0c0(%0); cache %1, 0x0d0(%0);	\
		cache %1, 0x0e0(%0); cache %1, 0x0f0(%0);	\
		cache %1, 0x100(%0); cache %1, 0x110(%0);	\
		cache %1, 0x120(%0); cache %1, 0x130(%0);	\
		cache %1, 0x140(%0); cache %1, 0x150(%0);	\
		cache %1, 0x160(%0); cache %1, 0x170(%0);	\
		cache %1, 0x180(%0); cache %1, 0x190(%0);	\
		cache %1, 0x1a0(%0); cache %1, 0x1b0(%0);	\
		cache %1, 0x1c0(%0); cache %1, 0x1d0(%0);	\
		cache %1, 0x1e0(%0); cache %1, 0x1f0(%0);	\
		.set mips0;					\
		.set reorder"					\
		:						\
		: "r" (base),					\
		  "i" (op));

void flush_cache (ulong start_addr, ulong size)
{
#ifdef CONFIG_JzRISC
	unsigned long start = start_addr;
	unsigned long end = start + size;

	while (start < end) {
		cache16_unroll32(start,Hit_Writeback_Inv_D);
		start += 0x200;
	}
#endif
}

void write_one_tlb( int index, u32 pagemask, u32 hi, u32 low0, u32 low1 ){
	write_32bit_cp0_register(CP0_ENTRYLO0, low0);
	write_32bit_cp0_register(CP0_PAGEMASK, pagemask);
	write_32bit_cp0_register(CP0_ENTRYLO1, low1);
	write_32bit_cp0_register(CP0_ENTRYHI, hi);
	write_32bit_cp0_register(CP0_INDEX, index);
	tlb_write_indexed();
}

#endif /* !CONFIG_NAND_SPL */

#ifdef CONFIG_JzRISC

void flush_icache_all(void)
{
	u32 addr, t = 0;

	asm volatile ("mtc0 $0, $28"); /* Clear Taglo */
	asm volatile ("mtc0 $0, $29"); /* Clear TagHi */

	for (addr = K0BASE; addr < K0BASE + CFG_ICACHE_SIZE;
	     addr += CFG_CACHELINE_SIZE) {
		asm volatile (
			".set mips3\n\t"
			" cache %0, 0(%1)\n\t"
			".set mips2\n\t"
			:
			: "I" (Index_Store_Tag_I), "r"(addr));
	}

	/* invalicate btb */
	asm volatile (
		".set mips32\n\t"
		"mfc0 %0, $16, 7\n\t"
		"nop\n\t"
		"ori %0,2\n\t"
		"mtc0 %0, $16, 7\n\t"
		".set mips2\n\t"
		:
		: "r" (t));
}

void flush_dcache_all(void)
{
	u32 addr;

	for (addr = K0BASE; addr < K0BASE + CFG_DCACHE_SIZE; 
	     addr += CFG_CACHELINE_SIZE) {
		asm volatile (
			".set mips3\n\t"
			" cache %0, 0(%1)\n\t"
			".set mips2\n\t"
			:
			: "I" (Index_Writeback_Inv_D), "r"(addr));
	}

	asm volatile ("sync");
}

void flush_cache_all(void)
{
	flush_dcache_all();
	flush_icache_all();
}

#endif
