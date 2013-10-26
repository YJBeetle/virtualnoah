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
#include <asm/jz4740.h>

static void gpio_init(void)
{
	/*
	 * Initialize NAND Flash Pins
	 */
	__gpio_as_nand();

	/*
	 * Initialize SDRAM pins
	 */
	__gpio_as_sdram_16bit_4720();
	/*
	 * Initialize UART0 pins
	 */
	__gpio_as_uart0();

	/*
	 * Initialize MSC pins
	 */
	__gpio_as_msc();

	/*
	 * Initialize LCD pins
	 */
	__gpio_as_lcd_18bit();

	/*
	 * Initialize SSI pins
	 */
//	__gpio_as_ssi();

	/*
	 * Initialize I2C pins
	 */
//	__gpio_as_i2c();

	/*
	 * Initialize Other pins
	 */
	__gpio_as_output(GPIO_SD_VCC_EN_N);
	__gpio_clear_pin(GPIO_SD_VCC_EN_N);

	__gpio_as_input(GPIO_SD_CD_N);
	__gpio_disable_pull(GPIO_SD_CD_N);

//	__gpio_as_input(GPIO_SD_WP);
//	__gpio_disable_pull(GPIO_SD_WP);

	__gpio_as_input(GPIO_DC_DETE_N);
//	__gpio_as_input(GPIO_CHARG_STAT_N);
	__gpio_as_input(GPIO_USB_DETE);
	__gpio_as_output(119);
	__gpio_clear_pin(119);
	__gpio_as_output(GPIO_DISP_OFF_N);
}

//----------------------------------------------------------------------
// board early init routine

void board_early_init(void)
{
	gpio_init();
}

//----------------------------------------------------------------------
// U-Boot common routines

int checkboard (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	printf("Board: Ingenic VIRGO (CPU Speed %d MHz)\n",
	       gd->cpu_clk/1000000);

	return 0; /* success */
}
