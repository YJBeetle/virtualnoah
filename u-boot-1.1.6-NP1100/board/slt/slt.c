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
#include <net.h>
#include <asm/mipsregs.h>
#include <asm/jz4730.h>

#if defined(CFG_CHIP_COUNT)
extern int chip_count( void );
#endif

inline int get_cpu_speed(void)
{
	unsigned int speed, cfg;

	/* set gpio as input??? */
	cfg = (REG_GPIO_GPDR(3) >> 12) & 0x7; /* read GP110,GP109,GP108 */

	switch ( cfg ) {
	case 0:
		speed = 200000000;
		break;
	case 2:
		speed = 334000000;
		break;
	case 4:
		speed = 366000000;
		break;
	default:
		speed = 300000000; /* default speed */
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
	int div[5] = {1, 3, 3, 3, 3}; /* divisors of I:S:P:L:M */
//	int div[5] = {1, 4, 4, 4, 4}; /* divisors of I:S:P:L:M */
	int nf;

	cfcr = CPM_CFCR_CKOEN1;
	cfcr |=  ((CFG_CPU_SPEED/48000000 - 1) << 25); /* USB clock divider */

	cfcr |= (n2FR[div[0]] << CPM_CFCR_IFR_BIT) | 
		(n2FR[div[1]] << CPM_CFCR_SFR_BIT) | 
		(n2FR[div[2]] << CPM_CFCR_PFR_BIT) |
		(n2FR[div[3]] << CPM_CFCR_LFR_BIT) |
		(n2FR[div[4]] << CPM_CFCR_MFR_BIT);

	nf = get_cpu_speed() * 2 / CFG_EXTAL + 1;

	plcr1 = ((nf - 2) << CPM_PLCR1_PLL1FD_BIT) | /* FD=NF-2 */
		(0 << CPM_PLCR1_PLL1RD_BIT) |	   /* RD=0, NR=2, 1.8432 = 3.6864/2 */
		(0 << CPM_PLCR1_PLL1OD_BIT) |      /* OD=0, NO=1 */
		(0x20 << CPM_PLCR1_PLL1ST_BIT) |   /* PLL stable time */
		CPM_PLCR1_PLL1EN;                  /* enable PLL */          

	/* init PLL */
	REG_CPM_CFCR = cfcr;
	REG_CPM_PLCR1 = plcr1;
}

/* see u-boot-1.1.6/nand_spl/board/slt/nand_boot_jz4730.c, gpio_init() */
static void gpio_init(void)
{
	int i;

	/* LED enable */
	for ( i=0; i< 8; ++i) {	/* LED light */
		__gpio_as_output(i);
		__gpio_clear_pin(i); /* 0: on, 1: off */
	}

	__gpio_as_eth();
	__gpio_as_dma();

}


//----------------------------------------------------------------------
// board early init routine

void board_early_init(void)
{

	gpio_init();

#if defined(CFG_CHIP_COUNT)
	{
		unsigned int cfg;
		cfg = (REG_GPIO_GPDR(3) >> 12) & 0x7; /* read GP110,GP109,GP108 */
		if ( cfg != 0 ) 		/* if cfg == 7, skip chip count */
			chip_count();		/* chip count increase */
	}
#endif
	pll_init();

}

void board_led(char ch)
{
}

//----------------------------------------------------------------------
// U-Boot common routines

int checkboard(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	printf("Board: Ingenic SLT Board, System Level Test Board\n");
	printf("For JZ4730 Chip-Sorting ");
	printf("CPU Speed [%d] MHz\n",
	       gd->cpu_clk/1000000);

	return 0; /* success */
}
