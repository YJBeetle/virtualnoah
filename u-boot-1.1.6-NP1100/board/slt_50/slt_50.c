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

#include <common.h>
#include <command.h>
#include <asm/mipsregs.h>
#include <asm/jz4750.h>

#if defined(CFG_CHIP_COUNT)
extern int chip_count( void );

static int hs = 6;
inline int get_cpu_speed(void)
{
	unsigned int speed, cfg;

	/* set gpio as input??? */
	cfg = (REG_GPIO_PXPIN(2) >> 11) & 0x7; /* read GPC11,GPC12,GPC13 */
	switch (cfg) {
	case 0:
		speed = 336000000;
		break;
	case 1:
		speed = 392000000;
		break;
	case 2:
		speed = 400000000;
		break;
	case 3:
		speed = 180000000;
		break;
	case 4:
		speed = 410000000;
		break;
	default:
		speed = 420000000; /* default speed */
		break;
	}

	return speed;
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
	int div[5] = {1, hs, hs, hs, hs}; /* divisors of I:S:P:L:M */
	int nf, pllout2;

	cfcr = 	CPM_CPCCR_PCS |
		(n2FR[div[0]] << CPM_CPCCR_CDIV_BIT) | 
		(n2FR[div[1]] << CPM_CPCCR_HDIV_BIT) | 
		(n2FR[div[2]] << CPM_CPCCR_PDIV_BIT) |
		(n2FR[div[3]] << CPM_CPCCR_MDIV_BIT) |
		(n2FR[div[4]] << CPM_CPCCR_LDIV_BIT);

	if (CFG_EXTAL > 16000000)
		cfcr |= CPM_CPCCR_ECS;
	else
		cfcr &= ~CPM_CPCCR_ECS;

	pllout2 = (cfcr & CPM_CPCCR_PCS) ? get_cpu_speed() : (get_cpu_speed() / 2);

	/* Init USB Host clock, pllout2 must be n*48MHz */
	REG_CPM_UHCCDR = pllout2 / 48000000 - 1;

	nf = get_cpu_speed()  / 1000000;

//	nf = get_cpu_speed() * 2 / CFG_EXTAL;
	plcr1 = ((nf - 2) << CPM_CPPCR_PLLM_BIT) | /* FD */
		(22 << CPM_CPPCR_PLLN_BIT) |	/* RD=0, NR=2 */
		(0 << CPM_CPPCR_PLLOD_BIT) |    /* OD=0, NO=1 */
		(0x20 << CPM_CPPCR_PLLST_BIT) | /* PLL stable time */
		CPM_CPPCR_PLLEN;                /* enable PLL */          

	/* init PLL */
	REG_CPM_CPCCR = cfcr;
	REG_CPM_CPPCR = plcr1;
}

#endif
static void gpio_init(void)
{
	/*
	 * Initialize SDRAM pins
	 */
#if CONFIG_NR_DRAM_BANKS == 2   /*Use Two Banks SDRAM*/
	__gpio_as_sdram_x2_32bit();
#else
	__gpio_as_sdram_32bit();
#endif

	/*
	 * Initialize UART1 pins
	 */
	__gpio_as_uart1();
	__gpio_as_i2c();
	__cpm_start_i2c();
}

//----------------------------------------------------------------------
// board early init routine

void board_early_init(void)
{
	gpio_init();

#if defined(CFG_CHIP_COUNT)
	{
		unsigned int cfg;
		cfg = (REG_GPIO_PXPIN(2) >> 11) & 0x7; /* read GP110,GP109,GP108 */
		if ( cfg != 0 ) 		/* if cfg == 7, skip chip count */
			chip_count();		/* chip count increase */
	}
#endif
	pll_init();
}

//----------------------------------------------------------------------
// U-Boot common routines

int checkboard (void)
{
	DECLARE_GLOBAL_DATA_PTR;
	printf("Board: Ingenic SLT Board, System Level Test Board\n");
	printf("For JZ4750 Chip-Sorting ");

	printf("CPU Speed [%d] MHz\n",
	       gd->cpu_clk/1000000);

	return 0; /* success */
}
