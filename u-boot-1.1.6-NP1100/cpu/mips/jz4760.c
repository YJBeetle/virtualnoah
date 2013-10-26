/*
 * Jz4760 common routines
 *
 *  Copyright (c) 2006
 *  Ingenic Semiconductor, <ljia@ingenic.cn>
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
#include <asm/mipsregs.h>

#include <common.h>
#include <command.h>
#include <asm/jz4760.h>

extern void board_early_init(void);
extern void serial_put_hex(unsigned int  d);
extern void sdram_init(void);

#ifndef CONFIG_FPGA

void pll_init(void)
{
	register unsigned int cfcr, plcr1;
	int n2FR[9] = {
		0, 0, 1, 2, 3, 0, 4, 0, 5
	};

        /** divisors,
	 *  for jz4760 ,I:H:H2:P:M:S.
	 *  DIV should be one of [1, 2, 3, 4, 6, 8]
         */
	int div[6] = {1, 2, 4, 4, 4, 4};
	//int div[6] = {1, 2, 2, 2, 2, 2};
	int pllout2;
	int gpuclk = 240000000;

	cfcr = 	CPM_CPCCR_PCS |
		(n2FR[div[0]] << CPM_CPCCR_CDIV_BIT) |
		(n2FR[div[1]] << CPM_CPCCR_HDIV_BIT) |
		(n2FR[div[2]] << CPM_CPCCR_H2DIV_BIT) |
		(n2FR[div[3]] << CPM_CPCCR_PDIV_BIT) |
		(n2FR[div[4]] << CPM_CPCCR_MDIV_BIT) |
		(n2FR[div[5]] << CPM_CPCCR_SDIV_BIT);

	if (CFG_EXTAL > 16000000)
		cfcr |= CPM_CPCCR_ECS;
	else
		cfcr &= ~CPM_CPCCR_ECS;

	/* set CPM_CPCCR_MEM only for ddr1 or ddr2 */
#if (defined(CONFIG_SDRAM_DDR1) || defined(CONFIG_SDRAM_DDR2))
	cfcr |= CPM_CPCCR_MEM;
#else	/* mddr or sdram */
	cfcr &= ~CPM_CPCCR_MEM;
#endif
	cfcr |= CPM_CPCCR_CE;

	pllout2 = (cfcr & CPM_CPCCR_PCS) ? CFG_CPU_SPEED : (CFG_CPU_SPEED / 2);

	plcr1 = CPCCR_M_N_OD;
	plcr1 |= (0x20 << CPM_CPPCR_PLLST_BIT)	/* PLL stable time */
		 | CPM_CPPCR_PLLEN;             /* enable PLL */

	/*
	 * Init USB Host clock, pllout2 must be n*48MHz
	 * For JZ4760 UHC - River.
	 */
	REG_CPM_UHCCDR = pllout2 / 48000000 - 1;

	/* init PLL */
	REG_CPM_CPCCR = cfcr;
	REG_CPM_CPPCR = plcr1;


	while (!(REG_CPM_CPPCR & CPM_CPPCR_PLLS));
/*
	serial_puts("REG_CPM_CPCCR = ");
	serial_put_hex(REG_CPM_CPCCR);
	serial_puts("REG_CPM_CPPCR = ");
	serial_put_hex(REG_CPM_CPPCR);
*/
}
/*
 * M = PLLM * 2, N = PLLN
 * NO = 2 ^ OD
 *
 */
void pll_1_init(void)
{
	register unsigned int plcr1;

	/* set CPM_CPCCR_MEM only for ddr1 or ddr2 */
	plcr1 = CPCCR1_M_N_OD | CPM_CPPCR1_PLL1EN;

	/* init PLL_1 , source clock is extal clock */
	REG_CPM_CPPCR1 = plcr1;
	__cpm_enable_pll_change();
	while (!(REG_CPM_CPPCR1 & CPM_CPPCR1_PLL1S));
}
#endif /* ifndef CONFIG_FPGA */

#if 0
static void pll_add_test(int new_freq)
{
	register unsigned int cfcr, plcr1;
	int n2FR[9] = {
		0, 0, 1, 2, 3, 0, 4, 0, 5
	};
	int div[6] = {1, 6, 6, 6, 6, 6}; /* divisors of I:S:P:M:L */
	int nf, pllout2;

	cfcr = 	CPM_CPCCR_PCS |
		(n2FR[div[0]] << CPM_CPCCR_CDIV_BIT) |
		(n2FR[div[1]] << CPM_CPCCR_HDIV_BIT) |
		(n2FR[div[2]] << CPM_CPCCR_H2DIV_BIT) |
		(n2FR[div[3]] << CPM_CPCCR_PDIV_BIT) |
		(n2FR[div[4]] << CPM_CPCCR_MDIV_BIT) |
		(n2FR[div[5]] << CPM_CPCCR_SDIV_BIT);

	/* set CPM_CPCCR_MEM only for ddr1 or ddr2 */
#if defined(CONFIG_DDRC) && (defined(CONFIG_SDRAM_DDR1) || defined(CONFIG_SDRAM_DDR2))
	cfcr |= CPM_CPCCR_MEM;
#else
	cfcr &= ~CPM_CPCCR_MEM;
#endif

	if (CFG_EXTAL > 16000000)
		cfcr |= CPM_CPCCR_ECS;
	else
		cfcr &= ~CPM_CPCCR_ECS;

	pllout2 = (cfcr & CPM_CPCCR_PCS) ? new_freq : (new_freq / 2);

	//nf = new_freq * 2 / CFG_EXTAL;
	nf = new_freq / 1000000; //step length is 1M
	nf /= 2;
	if(nf > 0x7f)
		nf = 0x7f;
	plcr1 = (nf << CPM_CPPCR_PLLM_BIT) | /* FD */
		(12 << CPM_CPPCR_PLLN_BIT) |	/* RD=0, NR=2 */
		(0 << CPM_CPPCR_PLLOD_BIT) |    /* OD=0, NO=1 */
		(0x20 << CPM_CPPCR_PLLST_BIT) | /* PLL stable time */
		CPM_CPPCR_PLLEN;                /* enable PLL */

	/* init PLL */
	REG_CPM_CPCCR = cfcr;
	REG_CPM_CPPCR = plcr1;
}

static void calc_clocks_add_test(void)
{
	DECLARE_GLOBAL_DATA_PTR;

#ifndef CONFIG_FPGA
	unsigned int pllout;
	unsigned int div[10] = {1, 2, 3, 4, 6, 8};
	pllout = __cpm_get_pllout();

	gd->cpu_clk = pllout / div[__cpm_get_cdiv()];
	gd->sys_clk = pllout / div[__cpm_get_hdiv()];
	gd->per_clk = pllout / div[__cpm_get_pdiv()];
	gd->mem_clk = pllout / div[__cpm_get_mdiv()];
	gd->dev_clk = CFG_EXTAL;
#else
	gd->cpu_clk = gd->sys_clk = gd->per_clk =
		gd->mem_clk = gd->dev_clk = CFG_EXTAL;
#endif
}
#endif

//----------------------------------------------------------------------
// U-Boot common routines
#if !defined(CONFIG_NAND_SPL) && !defined(CONFIG_SPI_SPL) && !defined(CONFIG_MSC_SPL)
static void calc_clocks(void)
{
	DECLARE_GLOBAL_DATA_PTR;

#ifndef CONFIG_FPGA
	unsigned int pllout;
	unsigned int div[10] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};

	pllout = __cpm_get_pllout();

	gd->cpu_clk = pllout / div[__cpm_get_cdiv()];
	gd->sys_clk = pllout / div[__cpm_get_hdiv()];
	gd->per_clk = pllout / div[__cpm_get_pdiv()];
	gd->mem_clk = pllout / div[__cpm_get_mdiv()];
	gd->dev_clk = CFG_EXTAL;
#else
	gd->cpu_clk = CFG_CPU_SPEED;
	gd->sys_clk = gd->per_clk = gd->mem_clk = gd->dev_clk
		= CFG_EXTAL / CFG_DIV;
#endif
}

#ifndef CONFIG_FPGA
static void rtc_init(void)
{
#define RTC_UNLOCK()			\
do {					\
	while ( !__rtc_write_ready());	\
	__rtc_write_enable();		\
	while (!__rtc_write_enabled()) ;\
} while (0)


	RTC_UNLOCK();

	__rtc_enable_alarm();	/* enable alarm */

	RTC_UNLOCK();

	REG_RTC_RGR   = 0x00007fff; /* type value */

	RTC_UNLOCK();

	REG_RTC_HWFCR = 0x0000ffe0; /* Power on delay 2s */

	RTC_UNLOCK();

	REG_RTC_HRCR  = 0x00000fe0; /* reset delay 125ms */

}
#endif /* ifndef CONFIG_FPGA */

//----------------------------------------------------------------------
// jz4760 board init routine
int jz_board_init(void)
{
	board_early_init();  /* init gpio, pll etc. */

#ifndef CONFIG_FPGA
	pll_init();          /* init PLL, do it when nor boot or defined(CONFIG_MSC_U_BOOT) */
#if 1 // #ifdef CFG_PLL1_FRQ
	pll_1_init();          /* init PLL1, used for controllers who carefor it. */
#endif
#endif /* ifndef CONFIG_FPGA */

#if !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_SPI_U_BOOT) && !defined(CONFIG_MSC_U_BOOT)
	serial_init();
	sdram_init();        /* init sdram memory */
#endif

	calc_clocks();       /* calc the clocks */

#ifndef CONFIG_FPGA
	rtc_init();		/* init rtc on any reset: */
#endif

	return 0;
}

//----------------------------------------------------------------------
// Timer routines

#define TIMER_CHAN  0
#define TIMER_FDATA 0xffff  /* Timer full data value */
#define TIMER_HZ    CFG_HZ

#define READ_TIMER  REG_TCU_TCNT(TIMER_CHAN)  /* macro to read the 16 bit timer */

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
	REG_TCU_TCSR(TIMER_CHAN) = TCU_TCSR_PRESCALE256 | TCU_TCSR_EXT_EN;
	REG_TCU_TCNT(TIMER_CHAN) = 0;
	REG_TCU_TDHR(TIMER_CHAN) = 0;
	REG_TCU_TDFR(TIMER_CHAN) = TIMER_FDATA;

	REG_TCU_TMSR = (1 << TIMER_CHAN) | (1 << (TIMER_CHAN + 16)); /* mask irqs */
	REG_TCU_TSCR = (1 << TIMER_CHAN); /* enable timer clock */
	REG_TCU_TESR = (1 << TIMER_CHAN); /* start counting up */

	lastdec = 0;
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
		tmo *= TIMER_HZ;
		tmo /= 1000;
	}
	else {
		if (usec >= 1) {
			tmo = usec * TIMER_HZ;
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

	if (lastdec <= now) {
		/* normal mode */
		timestamp += (now - lastdec);
	} else {
		/* we have an overflow ... */
		timestamp += TIMER_FDATA + now - lastdec;
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
		tmo *= TIMER_HZ;
		tmo /= 1000;
	} else {
		if (usec > 1) {
			tmo = usec * TIMER_HZ;
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
	return TIMER_HZ;
}

#endif /* !defined(CONFIG_NAND_SPL) && !defined(CONFIG_SPI_SPL) && !defined(CONFIG_MSC_SPL) */

//---------------------------------------------------------------------
// End of timer routine.
//---------------------------------------------------------------------
