/*
 * Copyright (C) 2007 Ingenic Semiconductor Inc.
 * Author: Peter <jlwei@ingenic.cn>
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
#include <asm/jz4750.h>

/*
 * NAND flash definitions
 */

#define NAND_DATAPORT	0xb8000000
unsigned int NAND_ADDRPORT;
unsigned int NAND_COMMPORT;

#define ECC_BLOCK	512
#define ECC_POS		3
#define PAR_SIZE	13

#define __nand_cmd(n)		(REG8(NAND_COMMPORT) = (n))
#define __nand_addr(n)		(REG8(NAND_ADDRPORT) = (n))
#define __nand_data8()		REG8(NAND_DATAPORT)
#define __nand_data16()		REG16(NAND_DATAPORT)

#define __nand_enable()		(REG_EMC_NFCSR |= EMC_NFCSR_NFE1 | EMC_NFCSR_NFCE1)
#define __nand_disable()	(REG_EMC_NFCSR &= ~(EMC_NFCSR_NFCE1))

static inline void nand_wait_ready(void)
{
	unsigned int timeout = 1000;
	while ((REG_GPIO_PXPIN(2) & 0x08000000) && timeout--);
	while (!(REG_GPIO_PXPIN(2) & 0x08000000));
}

/*
 * NAND flash parameters
 */
static int bus_width = 8;
static int page_size = 2048;
static int oob_size = 64;
static int ecc_count = 4;
static int row_cycle = 3;
static int page_per_block = 64;
static int bad_block_pos = 0;
static int block_size = 131072;

static unsigned char oob_buf[128] = {0};

/*
 * External routines
 */
extern void flush_cache_all(void);
extern int serial_init(void);
extern void serial_puts(const char *s);
extern void sdram_init(void);
extern void pll_init(void);

/*
 * NAND flash routines
 */

static inline void nand_read_buf16(void *buf, int count)
{
	int i;
	u16 *p = (u16 *)buf;

	for (i = 0; i < count; i += 2)
		*p++ = __nand_data16();
}

static inline void nand_read_buf8(void *buf, int count)
{
	int i;
	u8 *p = (u8 *)buf;

	for (i = 0; i < count; i++)
		*p++ = __nand_data8();
}

static inline void nand_read_buf(void *buf, int count, int bw)
{
	if (bw == 8)
		nand_read_buf8(buf, count);
	else
		nand_read_buf16(buf, count);
}

/*
 * Correct the error bit in 512-bytes data
 */
static void bch_correct(unsigned char *dat, int idx)
{
	int i, bit;  // the 'bit' of i byte is error 
	i = (idx - 1) >> 3;
	bit = (idx - 1) & 0x7;
	dat[i] ^= (1 << bit);
}

/*
 * Read oob
 */
static int nand_read_oob(int page_addr, u8 *buf, int size)
{
	int col_addr;

	if (page_size != 512)
		col_addr = page_size;
	else
		col_addr = 0;

	if (page_size != 512)
		/* Send READ0 command */
		__nand_cmd(NAND_CMD_READ0);
	else
		/* Send READOOB command */
		__nand_cmd(NAND_CMD_READOOB);

	/* Send column address */
	__nand_addr(col_addr & 0xff);
	if (page_size != 512)
		__nand_addr((col_addr >> 8) & 0xff);

	/* Send page address */
	__nand_addr(page_addr & 0xff);
	__nand_addr((page_addr >> 8) & 0xff);
	if (row_cycle == 3)
		__nand_addr((page_addr >> 16) & 0xff);

	/* Send READSTART command for 2048 or 4096 ps NAND */
	if (page_size != 512)
		__nand_cmd(NAND_CMD_READSTART);

	/* Wait for device ready */
	nand_wait_ready();

	/* Read oob data */
	nand_read_buf(buf, size, bus_width);

	if (page_size == 512)
		nand_wait_ready();

	return 0;
}

static int nand_read_page(int block, int page, uchar *dst, uchar *oobbuf)
{
	int page_addr = page + block * page_per_block;
	uchar *data_buf = dst;
	int i, j;

	/*
	 * Read oob data
	 */
	nand_read_oob(page_addr, oobbuf, oob_size);

	/*
	 * Read page data
	 */

	/* Send READ0 command */
	__nand_cmd(NAND_CMD_READ0);

	/* Send column address */
	__nand_addr(0);
	if (page_size != 512)
		__nand_addr(0);

	/* Send page address */
	__nand_addr(page_addr & 0xff);
	__nand_addr((page_addr >> 8) & 0xff);
	if (row_cycle == 3)
		__nand_addr((page_addr >> 16) & 0xff);

	/* Send READSTART command for 2048 or 4096 ps NAND */
	if (page_size != 512)
		__nand_cmd(NAND_CMD_READSTART);

	/* Wait for device ready */
	nand_wait_ready();

	/* Read page data */
	data_buf = dst;

	ecc_count = page_size / ECC_BLOCK;

	for (i = 0; i < ecc_count; i++) {
		unsigned int stat;

		/* Read data */
		nand_read_buf((void *)data_buf, ECC_BLOCK, bus_width);
	        
                /* Enable 8bit BCH decoding */
		REG_BCH_INTS = 0xffffffff;
		__ecc_decoding_8bit();

                /* Write 512 bytes and PAR_SIZE parities to REG_BCH_DR */
		for (j = 0; j < ECC_BLOCK; j++) {
			REG_BCH_DR = data_buf[j];
		}

		for (j = 0; j < PAR_SIZE; j++) {
			REG_BCH_DR = oob_buf[ECC_POS + i*PAR_SIZE + j];
		}

		/* Wait for completion */
		__ecc_decode_sync();
		__ecc_disable();

		/* Check decoding */
		stat = REG_BCH_INTS;
		if (stat & BCH_INTS_ERR) {
			if (stat & BCH_INTS_UNCOR) {
				/* Uncorrectable error occurred */
			}
			else {
				unsigned int errcnt = (stat & BCH_INTS_ERRC_MASK) >> BCH_INTS_ERRC_BIT;
				switch (errcnt) {
				case 8:
					bch_correct(data_buf, (REG_BCH_ERR3 & BCH_ERR_INDEX_ODD_MASK) >> BCH_ERR_INDEX_ODD_BIT);
				case 7:
					bch_correct(data_buf, (REG_BCH_ERR3 & BCH_ERR_INDEX_EVEN_MASK) >> BCH_ERR_INDEX_EVEN_BIT);
				case 6:
					bch_correct(data_buf, (REG_BCH_ERR2 & BCH_ERR_INDEX_ODD_MASK) >> BCH_ERR_INDEX_ODD_BIT);
				case 5:
					bch_correct(data_buf, (REG_BCH_ERR2 & BCH_ERR_INDEX_EVEN_MASK) >> BCH_ERR_INDEX_EVEN_BIT);
				case 4:
					bch_correct(data_buf, (REG_BCH_ERR1 & BCH_ERR_INDEX_ODD_MASK) >> BCH_ERR_INDEX_ODD_BIT);
				case 3:
					bch_correct(data_buf, (REG_BCH_ERR1 & BCH_ERR_INDEX_EVEN_MASK) >> BCH_ERR_INDEX_EVEN_BIT);
				case 2:
					bch_correct(data_buf, (REG_BCH_ERR0 & BCH_ERR_INDEX_ODD_MASK) >> BCH_ERR_INDEX_ODD_BIT);
				case 1:
					bch_correct(data_buf, (REG_BCH_ERR0 & BCH_ERR_INDEX_EVEN_MASK) >> BCH_ERR_INDEX_EVEN_BIT);
					break;
				default:
					break;
				}
			}
		}
		/* increment pointer */
		data_buf += ECC_BLOCK;
	}

	return 0;
}

#ifndef CFG_NAND_BADBLOCK_PAGE
#define CFG_NAND_BADBLOCK_PAGE 0 /* NAND bad block was marked at this page in a block, starting from 0 */
#endif

static void nand_load(int offs, int uboot_size, uchar *dst)
{
	int block;
	int blockcopy_count;
	int page;

	__nand_enable();

	/*
	 * offs has to be aligned to a block address!
	 */
	block = offs / block_size;
	blockcopy_count = 0;

	while (blockcopy_count < (uboot_size / block_size)) {

		/* New block is required to check the bad block flag */
		nand_read_oob(block * page_per_block + CFG_NAND_BADBLOCK_PAGE, oob_buf, oob_size);

		if (oob_buf[bad_block_pos] != 0xff) {
			block++;
			/* Skip bad block */
			continue;
		}

		for (page = 0; page < page_per_block; page++) {

			/* Load this page to dst, do the ECC */
			nand_read_page(block, page, dst, oob_buf);

			dst += page_size;
		}

		block++;
		blockcopy_count++;
	}

	__nand_disable();
}

static void gpio_init(void)
{
	/*
	 * Initialize SDRAM pins
	 */
	__gpio_as_sdram_32bit();

	/*
	 * Initialize UART1 pins
	 */
	__gpio_as_uart1();
}

void nand_boot(void)
{
	int boot_sel;
	void (*uboot)(void);

	/*
	 * Init hardware
	 */
	gpio_init();
	serial_init();

	serial_puts("\n\nNAND Secondary Program Loader\n\n");

	pll_init();
	sdram_init();

	/*
	 * JZ4750 can get some NAND parameters from NAND infomation in fuwa.h
	 */
	bus_width = (CFG_NAND_BW8==1) ? 8 : 16;
	page_size = CFG_NAND_PAGE_SIZE;
	row_cycle = CFG_NAND_ROW_CYCLE;
	block_size = CFG_NAND_BLOCK_SIZE;
	page_per_block =  CFG_NAND_BLOCK_SIZE / CFG_NAND_PAGE_SIZE;
	bad_block_pos = (page_size == 512) ? 5 : 0;
	oob_size = page_size / 32;
	ecc_count = page_size / ECC_BLOCK;

	if (REG_EMC_BCR && EMC_BCR_BSR_MASK == EMC_BCR_BSR_SHARE) {
		NAND_ADDRPORT =	0xb8010000;
		NAND_COMMPORT =	0xb8008000;
	} else {
		NAND_ADDRPORT =	0xb8000010;
		NAND_COMMPORT =	0xb8000008;
	}

	/*
	 * Load U-Boot image from NAND into RAM
	 */
	nand_load(CFG_NAND_U_BOOT_OFFS, CFG_NAND_U_BOOT_SIZE,
		  (uchar *)CFG_NAND_U_BOOT_DST);

	uboot = (void (*)(void))CFG_NAND_U_BOOT_START;

	serial_puts("Starting U-Boot ...\n");

	/*
	 * Flush caches
	 */
	flush_cache_all();

	/*
	 * Jump to U-Boot image
	 */
	(*uboot)();
}
