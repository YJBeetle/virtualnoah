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

static void gpio_init(void)
{
	/* LED enable */
	__gpio_as_output(GPIO_LED_EN);
	__gpio_set_pin(GPIO_LED_EN);

	__harb_usb0_uhc();
	__gpio_as_uart0();
	__gpio_as_uart1();
	__gpio_as_uart2();
	__gpio_as_uart3();
	__gpio_as_emc();
	__gpio_as_dma();
	__gpio_as_msc();
	__gpio_as_lcd_master();
	__gpio_as_usb();
	__gpio_as_ac97();
	__gpio_as_cim();
	__gpio_as_eth();

	/* First PW_I output high */
	__gpio_as_output(GPIO_PW_I);
	__gpio_set_pin(GPIO_PW_I);

	/* Then PW_O output high */
	__gpio_as_output(GPIO_PW_O);
	__gpio_set_pin(GPIO_PW_O);

	/* Last PW_I output low and as input */
	__gpio_clear_pin(GPIO_PW_I);
	__gpio_as_input(GPIO_PW_I);

	/* make PW_I work properly */
	__gpio_disable_pull(GPIO_PW_I);

	/* USB clock enable */
	__gpio_as_output(GPIO_USB_CLK_EN);
	__gpio_set_pin(GPIO_USB_CLK_EN);

	/* LCD display off */
	__gpio_as_output(GPIO_DISP_OFF_N);
	__gpio_clear_pin(GPIO_DISP_OFF_N);

	/* No backlight */
	__gpio_as_output(94); /* PWM0 */
	__gpio_clear_pin(94);

	/* RTC IRQ input */
	__gpio_as_input(GPIO_RTC_IRQ);

	/* CHARG_STAT input */
	__gpio_as_input(GPIO_CHARG_STAT);
	__gpio_disable_pull(GPIO_CHARG_STAT);
}

//----------------------------------------------------------------------
// board early init routine

void board_early_init(void)
{
	gpio_init();
}

void board_led(char ch)
{
}

//----------------------------------------------------------------------
// U-Boot common routines

int checkboard(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	printf("Board: Ingenic PMP Ver 1.x (CPU Speed %d MHz)\n",
	       gd->cpu_clk/1000000);

	return 0; /* success */
}
