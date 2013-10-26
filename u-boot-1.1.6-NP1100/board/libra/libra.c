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
	/* board led pins */
	__gpio_as_output(96);
	__gpio_as_output(97);
	__gpio_as_output(98);
	__gpio_as_output(99);

	__gpio_as_uart0();
	__gpio_as_uart1();
	__gpio_as_uart2();
	__gpio_as_uart3();
	__gpio_as_emc();
	__gpio_as_ssi();
	__gpio_as_scc();
	__gpio_as_msc();
	__gpio_as_lcd_master();
	__gpio_as_usb();
	__gpio_as_ac97();
	__gpio_as_eth();
	__gpio_as_ps2();
}

//----------------------------------------------------------------------
// board early init routine

void board_early_init(void)
{
	gpio_init();
}

void board_led(char ch)
{
	__gpio_port_data(3) = 
		((ch ^ 0x0f)&0x0f) | (__gpio_port_data(3) & 0xfffffff0);
}

//----------------------------------------------------------------------
// U-Boot common routines

int checkboard(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	printf("Board: Ingenic LIBRA (CPU Speed %d MHz)\n", gd->cpu_clk/1000000);

	return 0; /* success */
}
