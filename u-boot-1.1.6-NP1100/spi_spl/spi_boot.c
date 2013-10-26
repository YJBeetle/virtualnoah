/*
 * Copyright (C) 2007 Ingenic Semiconductor Inc.
 * Author: Regen Huang <lhhuang@ingenic.cn>
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
#include <nand.h>
#include <asm/io.h>

#if defined(CONFIG_JZ4750)
#include <asm/jz4750.h>
#elif defined(CONFIG_JZ4760)
#include <asm/jz4760.h>
#endif

#define CMD_READ        	0x03	/* Read Data */
#define SPI_FLASH_CMD_SIZE 	1	/* Command length: 1 byte */

#define IDX  0

extern void sdram_init(void);
extern void flush_cache_all(void);

#if 0
static char chars[16] = {'0', '1', '2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
static void serial_puts_long(unsigned int d)
{
	int i;
	unsigned char c;
	for (i = 7; i >= 0; i--) {
		c = chars[(d >> (4 * i)) & 0xf];
		serial_putc(c);
	}
	serial_putc('\n');
}
#endif

static inline void spi_disable(void)
{
#if defined(CONFIG_JZ4750)
	REG_GPIO_PXPEC(1) = 0x3c000000;
#else /* CONFIG_JZ4760 */
	REG_GPIO_PXPEC(0) = 0x003c0000;
#endif
	REG_SSI_CR0(IDX) &= ~SSI_CR0_SSIE;
}

static inline void spi_enable(void)
{
	REG_SSI_CR0(IDX) |= SSI_CR0_SSIE;
#if defined(CONFIG_JZ4750)
	REG_GPIO_PXPES(1)  = 0x3c000000;
#else /* CONFIG_JZ4760 */
	REG_GPIO_PXPES(0) = 0x003c0000;
#endif
}

static inline int jz_spi_write_dummy_and_read( unsigned char * read_buf, int count)
{
	int i, j, q, r;

	q = count / SSI_MAX_FIFO_ENTRIES;
	r = count - q * SSI_MAX_FIFO_ENTRIES;

	for(i = 0; i< q; i++) {
		for(j = 0; j< SSI_MAX_FIFO_ENTRIES; j++)
			REG_SSI_DR(IDX) = 0;
		while (__ssi_get_rxfifo_count(IDX) != SSI_MAX_FIFO_ENTRIES);
		for(j = 0; j < SSI_MAX_FIFO_ENTRIES; j++) {
			*read_buf = REG_SSI_DR(IDX);
			//serial_puts_long(*read_buf);
			read_buf++;
		}
	}

	for(j = 0; j< r; j++)
		REG_SSI_DR(IDX) = 0;
	while (__ssi_get_rxfifo_count(IDX) != r);
	for(j = 0; j< r; j++) {
		*read_buf++ = REG_SSI_DR(IDX);
	}
	return 0;
}

static unsigned char spi_flash_addr_size(void)
{
	unsigned char addr_size;

	REG_SSI_DR(IDX) = CMD_READ;
	REG_SSI_DR(IDX) = 0;
	REG_SSI_DR(IDX) = 0;
	REG_SSI_DR(IDX) = 0;
	REG_SSI_DR(IDX) = 0;

	while (__ssi_get_rxfifo_count(IDX) != 5);

	/* flush Rx-FIFO and Tx-FIFO */
	__ssi_flush_fifo(IDX);

	while (!__ssi_rxfifo_empty(IDX));

	jz_spi_write_dummy_and_read(&addr_size, 1);

	return addr_size;
}

/*
 * read data start at any address
 */
static int spi_flash_read(unsigned char * buf, unsigned int addr, int count)
{
	unsigned char i, addr_size;
	unsigned char *ptr;

	addr_size = spi_flash_addr_size();

	spi_disable();
	spi_enable();

        /*
	 * translate address as spi flash wish MSB transporting firstly
	 */
	if ( addr_size == 4) {
		addr = ((addr&0xFF)<<24) | (addr&0x0000FF00)<<8 | (addr&0x00FF0000)>>8 | ((addr>>24)&0xFF);
	}
	else if ( addr_size == 3) {
		addr = ((addr&0xFF)<<16) | (addr&0x0000FF00) | ((addr>>16)&0xFF);
	}
	else if ( addr_size == 2) {
		addr = ((addr&0xFF)<<8) | ((addr>>8)&0xFF);
	}

	ptr = (unsigned char *)&addr;

	REG_SSI_DR(IDX) = CMD_READ;

	for(i = 0; i < addr_size; i++) {
		REG_SSI_DR(IDX) = *ptr++;
	}

	while (__ssi_get_rxfifo_count(IDX) != SPI_FLASH_CMD_SIZE + addr_size);

	/* flush Rx-FIFO and Tx-FIFO */
	__ssi_flush_fifo(IDX);

	while (!__ssi_rxfifo_empty(IDX));

	jz_spi_write_dummy_and_read(buf, count);

	return 0;
}


/* PLL output frequency */
static inline unsigned int cpm_get_pllout(void)
{
	unsigned long m, n, no, pllout;
	unsigned long cppcr = REG_CPM_CPPCR;
	unsigned long od[4] = {1, 2, 2, 4};
	if ((cppcr & CPM_CPPCR_PLLEN) && !(cppcr & CPM_CPPCR_PLLBP)) {
		m = __cpm_get_pllm() + 2;
		n = __cpm_get_plln() + 2;
		no = od[__cpm_get_pllod()];
		pllout = ((JZ_EXTAL) / (n * no)) * m;
	} else
		pllout = JZ_EXTAL;
	return pllout;
}

/* PLL output frequency for MSC/I2S/LCD/USB/SSI */
static inline unsigned int cpm_get_pllout2(void)
{
	if (REG_CPM_CPCCR & CPM_CPCCR_PCS)
		return __cpm_get_pllout();
	else
		return __cpm_get_pllout()/2;
}

static void change_ssi_clk(void)
{
        /* 
	 *  SSI_CLK = pllout/(2 * (SSI_GR + 1))
         *
         *  SSI_GR = pllout/(2 * SSI_CLK) - 1       
         */

	REG_SSI_GR(IDX) = (cpm_get_pllout2() + 2 * CFG_SPI_MAX_FREQ - 1) / (2 * CFG_SPI_MAX_FREQ) - 1;
}

/*
 * Load kernel image from SPI into RAM
 */
static int spi_load(int offs, int kernel_size, u8 *dst)
{
#if !defined(CONFIG_FPGA)
	change_ssi_clk();
#endif
	spi_enable();
	spi_flash_read(dst, offs, kernel_size);
	spi_disable();

	return 0;
}

static void gpio_init(void)
{
	/*
	 * Initialize UART3 pins
	 */
	switch (CFG_UART_BASE) {
	case UART0_BASE:
		__gpio_as_uart0();
		break;
	case UART1_BASE:
		__gpio_as_uart1();
		break;
	case UART2_BASE:
		__gpio_as_uart2();
		break;
	case UART3_BASE:
		__gpio_as_uart3();
		break;
	}
#ifdef CONFIG_FPGA
	/* Network needs it.*/
	__gpio_as_nor();

        /* if the delay isn't added on FPGA, the first line that uart 
	 * to print will not be normal. 
	 */
	{
		volatile int i=1000;
		while(i--);
	}
#endif
}


void spl_boot(void)
{
#ifdef CONFIG_LOAD_UBOOT
	void (*uboot)(void);
#else
	int i;
	static u32 *param_addr = 0;
	static u8 *tmpbuf = 0;
	static u8 cmdline[256] = CFG_CMDLINE;
	void (*kernel)(int, char **, char *);
#endif
	/*
	 * Init hardware
	 */
	__cpm_start_uart1();
	__cpm_start_mdma();
	__cpm_start_emc();
	__cpm_start_ddr();

	/* enable mdmac's clock */
	REG_MDMAC_DMACKE = 0x3;
	gpio_init();
	serial_init();

	serial_puts("\n\n SPI Secondary Program Loader\n\n");

#ifndef CONFIG_FPGA
	pll_init();
#endif
	sdram_init();

#ifdef CONFIG_LOAD_UBOOT
	/*
	 * Load U-Boot image from SPI NOR into RAM
	 */
	spi_load(CFG_SPI_U_BOOT_OFFS, CFG_SPI_U_BOOT_SIZE,
		  (unsigned char *)CFG_SPI_U_BOOT_DST);

	uboot = (void (*)(void))CFG_SPI_U_BOOT_START;
	serial_puts("Starting U-Boot ...\n");
#else
	/*
	 * Load kernel image from SPI NOR into RAM
	 */
	spi_load(CFG_SPI_ZIMAGE_OFFS, CFG_ZIMAGE_SIZE, (unsigned char *)CFG_ZIMAGE_DST);

	/*
	 * Prepare kernel parameters and environment
	 */
	param_addr = (u32 *)PARAM_BASE;
	param_addr[0] = 0;	/* might be address of ascii-z string: "memsize" */
	param_addr[1] = 0;	/* might be address of ascii-z string: "0x01000000" */
	param_addr[2] = 0;
	param_addr[3] = 0;
	param_addr[4] = 0;
	param_addr[5] = PARAM_BASE + 32;
	param_addr[6] = CFG_ZIMAGE_START;
	tmpbuf = (u8 *)(PARAM_BASE + 32);

	for (i = 0; i < 256; i++)
		tmpbuf[i] = cmdline[i];  /* linux command line */

	kernel = (void (*)(int, char **, char *))CFG_ZIMAGE_START;
	serial_puts("Starting kernel ...\n");
#endif
	/*
	 * Flush caches
	 */
	flush_cache_all();

#ifndef CONFIG_LOAD_UBOOT
	/*
	 * Jump to kernel image
	 */
	(*kernel)(2, (char **)(PARAM_BASE + 16), (char *)PARAM_BASE);
#else
	/*
	 * Jump to U-Boot image
	 */
	(*uboot)();
#endif
}
