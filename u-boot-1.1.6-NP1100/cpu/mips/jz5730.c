/*
 * Jz5730 common routines
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

#ifdef CONFIG_JZ5730

#include <common.h>
#include <command.h>

#include <asm/jz5730.h>

extern void board_early_init(void);
extern void board_led(char ch);

static void sdram_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	register unsigned int dmcr, sdmode, tmp, ns;
	volatile unsigned int *p;

	unsigned int cas_latency_sdmr[2] = {
		EMC_SDMR_CAS_2,
		EMC_SDMR_CAS_3,
	};

	unsigned int cas_latency_dmcr[2] = {
		1 << EMC_DMCR_TCL_BIT,	/* CAS latency is 2 */
		2 << EMC_DMCR_TCL_BIT	/* CAS latency is 3 */
	};

	REG_EMC_BCR = EMC_BCR_BRE;	/* Enable SPLIT */
	REG_EMC_RTCSR = EMC_RTCSR_CKS_DISABLE;
	REG_EMC_RTCOR = 0;
	REG_EMC_RTCNT = 0;

	/* Basic DMCR register value. */
	dmcr = ((SDRAM_ROW-11)<<EMC_DMCR_RA_BIT) |
		((SDRAM_COL-8)<<EMC_DMCR_CA_BIT) |
		(SDRAM_BANK4<<EMC_DMCR_BA_BIT) |
		(SDRAM_BW16<<EMC_DMCR_BW_BIT) |
		EMC_DMCR_EPIN |
		cas_latency_dmcr[((SDRAM_CASL == 3) ? 1 : 0)];

	/* SDRAM timimg parameters */
	ns = 1000000000 / gd->mem_clk;

	tmp = SDRAM_TRAS/ns;
	if (tmp < 4)
		tmp = 4;
	if (tmp > 11)
		tmp = 11;
	dmcr |= ((tmp-4) << EMC_DMCR_TRAS_BIT);
	tmp = SDRAM_RCD/ns;
	if (tmp > 3)
		tmp = 3;
	dmcr |= (tmp << EMC_DMCR_RCD_BIT);
	tmp = SDRAM_TPC/ns;
	if (tmp > 7)
		tmp = 7;
	dmcr |= (tmp << EMC_DMCR_TPC_BIT);
	tmp = SDRAM_TRWL/ns;
	if (tmp > 3)
		tmp = 3;
	dmcr |= (tmp << EMC_DMCR_TRWL_BIT);
	tmp = (SDRAM_TRAS + SDRAM_TPC)/ns;
	if (tmp > 14)
		tmp = 14;
	dmcr |= (((tmp + 1) >> 1) << EMC_DMCR_TRC_BIT);

	/* SDRAM mode values */
	sdmode = EMC_SDMR_BT_SEQ | 
		 EMC_SDMR_OM_NORMAL |
		 EMC_SDMR_BL_4 | 
		 cas_latency_sdmr[((SDRAM_CASL == 3) ? 1 : 0)];

	if (SDRAM_BW16)
		sdmode <<= 1;
	else
		sdmode <<= 2;

	/* First, precharge phase */
	REG_EMC_DMCR = dmcr;

	/* Set refresh registers */
	tmp = SDRAM_TREF/ns;
	tmp = tmp/64 + 1;
	if (tmp > 0xff)
		tmp = 0xff;

	REG_EMC_RTCOR = tmp;
	REG_EMC_RTCSR = EMC_RTCSR_CKS_64;	/* Divisor is 64, CKO/64 */

	/* precharge all chip-selects */
	REG8(EMC_SDMR0|sdmode) = 0;
	REG8(EMC_SDMR1|sdmode) = 0;

	/* wait for precharge, > 200us */
	tmp = (gd->cpu_clk / 1000000) * 200;
	while (tmp--);

	/* enable refresh and set SDRAM mode */
	REG_EMC_DMCR = dmcr | EMC_DMCR_RFSH | EMC_DMCR_MRSET;

	/* write sdram mode register for each chip-select */
	REG8(EMC_SDMR0|sdmode) = 0;
	REG8(EMC_SDMR1|sdmode) = 0;

	/* do simple memory check */
	p = (volatile unsigned int *)(0xa0100000);
	*p = 0x12345678;
	p += 1;
	*p = 0x87654321;    
	p -= 1;

	if (*p != 0x12345678) {
		/* memory initialization failed */
		board_led(0xe);
		while (1);
	}

	/* everything is ok now */
	board_led(0x1);
}

static void calc_clocks(void)
{
	DECLARE_GLOBAL_DATA_PTR;
  
#ifndef CONFIG_FPGA
	unsigned int pllout;
	unsigned int od[4] = {1, 2, 2, 4};
	unsigned int div[10] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};
	unsigned int nf, nr, nd;

	nf = __cpm_plcr1_fd() + 2;
	nr = __cpm_plcr1_rd() + 2;
	nd = od[__cpm_plcr1_od()];

	pllout = (CFG_EXTAL / (nr * nd)) * nf;

	gd->cpu_clk = pllout / div[__cpm_cfcr_ifr()];
	gd->sys_clk = pllout / div[__cpm_cfcr_sfr()];
	gd->per_clk = pllout / div[__cpm_cfcr_pfr()];
	gd->mem_clk = pllout / div[__cpm_cfcr_mfr()];
	gd->dev_clk = CFG_EXTAL;
#else
	gd->cpu_clk = gd->sys_clk = gd->per_clk = 
		gd->mem_clk = gd->dev_clk = CFG_EXTAL;
#endif
}

static void check_reset(void)
{
	if (REG_CPM_RSTR & 0x4) {
		/* CPU was reset by hibernate wakeup */
		void (*resume) (void);

		/* Clear HGP */
		REG_CPM_SCR &= ~CPM_SCR_HGP;

		/* Jump to sleep address directly */
		resume = (void (*)(void))(REG_CPM_SPR | 0x80000000);
		resume();
	}
}

/* PLL output clock = EXTAL * NF / (NR * NO)
 *
 * NF = FD + 2, NR = RD + 2
 * NO = 1 (if OD = 0), NO = 2 (if OD = 1 or 2), NO = 4 (if OD = 3)
 */
static void pll_init(void)
{
	register unsigned int cfcr, plcr1;
	int n2FR[33] = {
		0, 0, 1, 2, 3, 0, 4, 0, 5, 0, 0, 0, 6, 0, 0, 0,
		7, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0,
		9
	};
	int div[5] = {1, 3, 3, 3, 10}; /* divisors of I:S:P:M:SSI */
	int nf;

	cfcr = CPM_CFCR_CKOEN;
	cfcr |=  ((CFG_CPU_SPEED/48000000 - 1) << 25); /* USB clock divider */

	cfcr |= (n2FR[div[0]] << CPM_CFCR_IFR_BIT) | 
		(n2FR[div[1]] << CPM_CFCR_SFR_BIT) | 
		(n2FR[div[2]] << CPM_CFCR_PFR_BIT) |
		(n2FR[div[3]] << CPM_CFCR_MFR_BIT) |
		((div[4]-1) << CPM_CFCR_SSIFR_BIT);

	nf = CFG_CPU_SPEED * 2 / CFG_EXTAL;

	plcr1 = ((nf - 2) << CPM_PLCR1_PLL1FD_BIT) | /* FD=NF-2 */
		(0 << CPM_PLCR1_PLL1RD_BIT) |	   /* RD=0, NR=2, 1.8432 = 3.6864/2 */
		(0 << CPM_PLCR1_PLL1OD_BIT) |      /* OD=0, NO=1 */
		(0x20 << CPM_PLCR1_PLL1ST_BIT) |   /* PLL stable time */
		CPM_PLCR1_PLL1EN;                  /* enable PLL */          

	/* init PLL */
	REG_CPM_CFCR = cfcr;
	REG_CPM_PLCR1 = plcr1;
}

//----------------------------------------------------------------------
// jz5730 board init routine

int jz_board_init(void)
{
	board_early_init();  /* init gpio etc. */
	pll_init();          /* init PLL */
	calc_clocks();       /* calc the clocks */
	sdram_init();        /* init sdram memory */
	check_reset();       /* check the reset status */
	return 0;
}

//----------------------------------------------------------------------
// U-Boot common routines

long int initdram(int board_type)
{
	u32 dmcr;
	u32 rows, cols, dw, banks;
	ulong size;

	dmcr = REG_EMC_DMCR;
	rows = 11 + ((dmcr & EMC_DMCR_RA_MASK) >> EMC_DMCR_RA_BIT);
	cols = 8 + ((dmcr & EMC_DMCR_CA_MASK) >> EMC_DMCR_CA_BIT);
	dw = (dmcr & EMC_DMCR_BW) ? 2 : 4;
	banks = (dmcr & EMC_DMCR_BA) ? 4 : 2;

	size = (1 << (rows + cols)) * dw * banks;

	return size;
}

//----------------------------------------------------------------------
// Timer routines

/* we always count down the max. */
#define TIMER_LOAD_VAL 0xffffffff

#define CHANNEL_ID  0
/* macro to read the 32 bit timer */
#define READ_TIMER __ost_get_count(CHANNEL_ID)

static ulong timestamp;
static ulong lastdec;

void	reset_timer_masked	(void);
ulong	get_timer_masked	(void);
void	udelay_masked		(unsigned long usec);

/*
 * timer without interrupts
 */

int timer_init(void)
{
	__ost_set_clock(CHANNEL_ID, OST_TCSR_CKS_PCLK_256);
	__ost_set_reload(CHANNEL_ID, TIMER_LOAD_VAL);
	__ost_set_count(CHANNEL_ID, TIMER_LOAD_VAL);
	__ost_enable_channel(CHANNEL_ID);

	lastdec = TIMER_LOAD_VAL;
	timestamp = 0;

	return 0;
}

void reset_timer(void)
{
	reset_timer_masked ();
}

ulong get_timer(ulong base)
{
	return get_timer_masked () - base;
}

void set_timer(ulong t)
{
	timestamp = t;
}

void udelay (unsigned long usec)
{
	ulong tmo,tmp;

	/* normalize */
	if (usec >= 1000) {
		tmo = usec / 1000;
		tmo *= CFG_HZ;
		tmo /= 1000;
	}
	else {
		if (usec >= 1) {
			tmo = usec * CFG_HZ;
			tmo /= (1000*1000);
		}
		else
			tmo = 1;
	}

	/* check for rollover during this delay */
	tmp = get_timer (0);
	if ((tmp + tmo) < tmp )
		reset_timer_masked();  /* timer would roll over */
	else
		tmo += tmp;

	while (get_timer_masked () < tmo);
}

void reset_timer_masked (void)
{
	/* reset time */
	lastdec = READ_TIMER;
	timestamp = 0;
}

ulong get_timer_masked (void)
{
	ulong now = READ_TIMER;

	if (lastdec >= now) {
		/* normal mode */
		timestamp += (lastdec - now);
	} else {
		/* we have an overflow ... */
		timestamp += ((lastdec + TIMER_LOAD_VAL) - now);
	}
	lastdec = now;

	return timestamp;
}

void udelay_masked (unsigned long usec)
{
	ulong tmo;
	ulong endtime;
	signed long diff;

	/* normalize */
	if (usec >= 1000) {
		tmo = usec / 1000;
		tmo *= CFG_HZ;
		tmo /= 1000;
	} else {
		if (usec > 1) {
			tmo = usec * CFG_HZ;
			tmo /= (1000*1000);
		} else {
			tmo = 1;
		}
	}

	endtime = get_timer_masked () + tmo;

	do {
		ulong now = get_timer_masked ();
		diff = endtime - now;
	} while (diff >= 0);
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On MIPS it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On MIPS it returns the number of timer ticks per second.
 */
ulong get_tbclk (void)
{
	return CFG_HZ;
}

//---------------------------------------------------------------------
// End of timer routine.
//---------------------------------------------------------------------

#endif /* CONFIG_JZ5730 */
