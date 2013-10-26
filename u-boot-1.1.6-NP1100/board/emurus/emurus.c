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
	 * Initialize SDRAM pins
	 */

	/* PORT A: D0 ~ D31 */
	REG_GPIO_PXFUNS(0) = 0xffffffff;
	REG_GPIO_PXSELC(0) = 0xffffffff;

	/* PORT B: A0 ~ A16, DCS#, RAS#, CAS#, CKE#, RDWE#, CKO#, WE0# */
	REG_GPIO_PXFUNS(1) = 0x81f9ffff;
	REG_GPIO_PXSELC(1) = 0x81f9ffff;

	/* PORT C: WE1#, WE2#, WE3# */
	REG_GPIO_PXFUNS(2) = 0x07000000;
	REG_GPIO_PXSELC(2) = 0x07000000;


	/*
	 * Initialize Static Memory Pins
	 */

	/* CS4# */
	REG_GPIO_PXFUNS(1) = 0x10000000;
	REG_GPIO_PXSELC(1) = 0x10000000;


	/*
	 * Initialize UART0 pins
	 */

	/* PORT D: TXD/RXD */
	REG_GPIO_PXFUNS(3) = 0x06000000;
	REG_GPIO_PXSELS(3) = 0x06000000;

	/*
	 * Initialize LED pins
	 */

	/* PORT B: CS2# */
	REG_GPIO_PXFUNS(1) = 0x04000000;
	REG_GPIO_PXSELC(1) = 0x04000000;
}

//----------------------------------------------------------------------
// board early init routine

void board_early_init(void)
{
	gpio_init();
}

//#define LED_ADDR 0xb6000058
#define LED_ADDR 0xb6000016

static const unsigned char digits[16] = "0123456789abcdef";

void led_puts(const unsigned char *s)
{
	volatile unsigned char* leds = (volatile unsigned char *)LED_ADDR;
	unsigned int i = 0;

	while ( s[i] && (i<8)) {
		leds[i] = s[i];
		i++;
	}
	while (i<8)
		leds[i++] = ' ';
}

void led_putb(unsigned char val)
{
	volatile unsigned char* leds = (unsigned char *)LED_ADDR+8;
	unsigned int i;
    
	for (i = 0; i < 2; i++) {
		*--leds = digits[(unsigned char)(val &0x0f)];
		val >>= 4;
	}
	while (i<8)
		*--leds = ' ';
}

void led_putl(unsigned int val)
{
	volatile unsigned char* leds = (unsigned char *)LED_ADDR+8;
	unsigned int i;

	for (i = 0; i < 8; i++) {
		*--leds = digits[(unsigned char)(val &0x0f)];
		val >>= 4;
	}
}

//----------------------------------------------------------------------
// U-Boot common routines

int checkboard (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	printf("Board: Ingenic EMURUS (CPU Speed %d MHz)\n",
	       gd->cpu_clk/1000000);

	return 0; /* success */
}
