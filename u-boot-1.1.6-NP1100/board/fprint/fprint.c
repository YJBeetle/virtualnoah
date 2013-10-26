/*
 * (C) Copyright 2005
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
#include <asm/jz4730.h>

static void gpio_init(void)
{
	__gpio_as_output(96);	/* board led */
	__gpio_as_output(104);	/* cim */
	__gpio_as_output(105);	/* lcd module backlight */
	__gpio_as_output(106);	/* ac97 */
	__gpio_as_output(107);	/* uart */

	__gpio_clear_pin(104);	/* enable cim power */
	__gpio_clear_pin(105);	/* turn on lcd module backlight */
	__gpio_set_pin(106);	/* ac97 enable */
	__gpio_set_pin(107);	/* uart enable */

	__gpio_as_uart0();
	__gpio_as_uart1();
	__gpio_as_uart2();
	__gpio_as_uart3();
	__gpio_as_emc();
	__gpio_as_lcd_master();
	__gpio_as_usb();
	__gpio_as_ac97();
	__gpio_as_cim();
	__gpio_as_eth();
	__harb_usb0_uhc(); /* USB port 0 as host port */
}

//----------------------------------------------------------------------
// board early init routine

void board_early_init(void)
{
	gpio_init();
}

void board_led(unsigned char ch)
{
	if (ch)
		__gpio_clear_pin(96); /* led on */
	else
		__gpio_set_pin(96);   /* led off */
}

//----------------------------------------------------------------------
// U-Boot common routines

int checkboard(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	printf("Board: Ingenic FPRINT (CPU Speed %d MHz)\n", gd->cpu_clk/1000000);

	return 0; /* success */
}
