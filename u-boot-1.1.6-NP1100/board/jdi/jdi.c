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
#include <net.h>
#include <asm/mipsregs.h>
#include <asm/jz4730.h>

static void gpio_init(void)
{
	/* GPIO led */
	__gpio_as_output(62);
	__gpio_as_output(63);

	__gpio_set_pin(0);   /* low active */
	__gpio_clear_pin(5); /* high active */

	/* JTAG port */
	__gpio_as_output(0); /* TRSTN */
	__gpio_as_output(1); /* TDI */
	__gpio_as_input(2);  /* TDO */
	__gpio_as_output(3); /* TMS */
	__gpio_as_output(4); /* TCK */
	__gpio_as_output(5); /* RSTN */
	__gpio_as_input(6);  /* DINT */
	__gpio_as_output(7);  /* Buffer Enable */

	__gpio_set_pin(7);    /* Disable Buffer */

	__gpio_as_uart0();
	__gpio_as_uart1();
	__gpio_as_uart2();
	__gpio_as_uart3();
	__gpio_as_emc();
	__gpio_as_eth();
	__gpio_as_usb();
	__harb_usb0_udc(); /* USB port 0 as device port */
}

//----------------------------------------------------------------------
// board early init routine

void board_early_init(void)
{
	gpio_init();
}

void board_led(char ch)
{
	if (ch)
		__gpio_set_pin(62);
	else
		__gpio_clear_pin(62);
}

//----------------------------------------------------------------------
// U-Boot common routines

int checkboard(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	printf("Board: Ingenic JDI (CPU Speed %d MHz)\n", gd->cpu_clk/1000000);

	return 0; /* success */
}

extern void i2c_open(void);
extern void i2c_close(void);
extern void i2c_setclk(unsigned int i2cclk);
extern int i2c_read(unsigned char device, unsigned char *buf,
		    unsigned char address, int count);

#define EEPROM_MAC_START  0
#define EEPROM_IP_START   6
#define NETMASK_IP_START  14
#define GATEWAY_IP_START  18

U_BOOT_CMD(
	0,	0,	0,	0,
	"\n",
	NULL
	);
/*
 * Get ip-address...
 */
int do_get_ip(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	DECLARE_GLOBAL_DATA_PTR;

	bd_t *bd = gd->bd;
	unsigned char device, address, data[256], str[32];
	IPaddr_t ipaddr;

	device = (0xA << 3) | 7; /* EEPROM 7 */
	address = 0;

	i2c_open();
	i2c_setclk(100000);
	i2c_read(device, data, address, 256);
	i2c_close();

	ipaddr = (data[6] << 24) | (data[7] << 16) | (data[8] << 8) | (data[9]);

	/*
	 * Update whole ip-addr
	 */
	bd->bi_ip_addr = ipaddr;
	sprintf(str, "%ld.%ld.%ld.%ld",
		(bd->bi_ip_addr & 0xff000000) >> 24,
		(bd->bi_ip_addr & 0x00ff0000) >> 16,
		(bd->bi_ip_addr & 0x0000ff00) >> 8,
		(bd->bi_ip_addr & 0x000000ff));
	setenv("ipaddr", str);
	printf("Updated ipaddr from eeprom to %s\n", str);

	return 0;
}

U_BOOT_CMD(
	getip,	2,	1,	do_get_ip,
	"getip   - Get IP-Address\n",
	NULL
	);

/*
 * Get mac-address...
 */
int do_get_mac(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	DECLARE_GLOBAL_DATA_PTR;

	unsigned char device, address, data[256], str[32];

	device = (0xA << 3) | 7; /* EEPROM 7 */
	address = 0;

	i2c_open();
	i2c_setclk(100000);
	i2c_read(device, data, address, 256);
	i2c_close();

	/* Now setup ethaddr */
	sprintf (str, "%02x:%02x:%02x:%02x:%02x:%02x",
		 data[0], data[1], data[2], data[3], data[4],
		 data[5]);
	setenv ("ethaddr", str);
	memcpy (gd->bd->bi_enetaddr, data, 6);
	printf("Updated ethaddr from eeprom to %s\n", str);

	return 0;
}

U_BOOT_CMD(
	getmac,	2,	1,	do_get_mac,
	"getmac  - Get MAC-Address\n",
	NULL
	);

/*
 * Get netmask...
 */
int do_get_netmask(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned char device, address, data[256], str[32];

	device = (0xA << 3) | 7; /* EEPROM 7 */
	address = 0;

	i2c_open();
	i2c_setclk(100000);
	i2c_read(device, data, address, 256);
	i2c_close();

	sprintf(str, "%ld.%ld.%ld.%ld",
		data[14], data[15], data[16], data[17]);
	setenv("netmask", str);
	printf("Updated netmask from eeprom to %s\n", str);

	return 0;
}

U_BOOT_CMD(
	getnetmask,	2,	1,	do_get_netmask,
	"getnetmask  - Get NETMASK\n",
	NULL
	);

/*
 * Get gateway...
 */
int do_get_gateway(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned char device, address, data[256], str[32];

	device = (0xA << 3) | 7; /* EEPROM 7 */
	address = 0;

	i2c_open();
	i2c_setclk(100000);
	i2c_read(device, data, address, 256);
	i2c_close();

	sprintf(str, "%ld.%ld.%ld.%ld",
		data[18], data[19], data[20], data[21]);
	setenv("gateway", str);
	printf("Updated gateway from eeprom to %s\n", str);

	return 0;
}

U_BOOT_CMD(
	getgateway,	2,	1,	do_get_gateway,
	"getgateway  - Get GATEWAY\n",
	NULL
	);
