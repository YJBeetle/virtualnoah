/*
 * (C) Copyright 2006
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
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
#include <asm/jz5730.h>


/*
 * HW ECC info
 */
#define NAND_ECC_ENABLE		1

#define ECC_BLOCK		256 /* 3-bytes HW ECC per 256-bytes data */
#define ECC_POS			4   /* ECC offset to spare area */

/*
 * NAND flash routines
 */
#define __nand_enable()		(REG_EMC_NFCSR |= EMC_NFCSR_NFE | EMC_NFCSR_FCE)
#define __nand_disable()	(REG_EMC_NFCSR &= ~(EMC_NFCSR_NFE | EMC_NFCSR_FCE))
#define __nand_ecc_enable()	(REG_EMC_NFCSR |= EMC_NFCSR_ECCE | EMC_NFCSR_ERST)
#define __nand_ecc_disable()	(REG_EMC_NFCSR &= ~EMC_NFCSR_ECCE)
#define __nand_dev_ready()	while (!(REG_EMC_NFCSR & EMC_NFCSR_RB))
#define __nand_ecc()		(REG_EMC_NFECC & 0x00ffffff)
#define __nand_cmd(n)		(REG8(NAND_COMMPORT) = (n))
#define __nand_addr(n)		(REG8(NAND_ADDRPORT) = (n))
#define __nand_data8()		REG8(NAND_DATAPORT)
#define __nand_data16()		REG16(NAND_DATAPORT)

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

/*
 * External routines
 */
extern void flush_cache_all(void);
extern int serial_init(void);
extern void serial_puts(const char *s);

/*
 * Init SDRAM memory.
 */
#define CPU_CLK (CFG_CPU_SPEED)
#define MEM_CLK (CFG_CPU_SPEED/3)

static void sdram_init(void)
{
	register unsigned int dmcr, sdmode, tmp, ns;

	unsigned int cas_latency_sdmr[2] = {
		EMC_SDMR_CAS_2,
		EMC_SDMR_CAS_3,
	};

	unsigned int cas_latency_dmcr[2] = {
		1 << EMC_DMCR_TCL_BIT,	/* CAS latency is 2 */
		2 << EMC_DMCR_TCL_BIT	/* CAS latency is 3 */
	};

	REG_EMC_BCR = EMC_BCR_BRE;	/* Enable SPLIT */
	REG_EMC_RTCSR = EMC_RTCSR_CKS_DISABLE;
	REG_EMC_RTCOR = 0;
	REG_EMC_RTCNT = 0;

	/* Basic DMCR register value. */
	dmcr = ((SDRAM_ROW-11)<<EMC_DMCR_RA_BIT) |
		((SDRAM_COL-8)<<EMC_DMCR_CA_BIT) |
		(SDRAM_BANK4<<EMC_DMCR_BA_BIT) |
		(SDRAM_BW16<<EMC_DMCR_BW_BIT) |
		EMC_DMCR_EPIN |
		cas_latency_dmcr[((SDRAM_CASL == 3) ? 1 : 0)];

	/* SDRAM timimg parameters */
	ns = 1000000000 / MEM_CLK;

	tmp = SDRAM_TRAS/ns;
	if (tmp < 4)
		tmp = 4;
	if (tmp > 11)
		tmp = 11;
	dmcr |= ((tmp-4) << EMC_DMCR_TRAS_BIT);
	tmp = SDRAM_RCD/ns;
	if (tmp > 3)
		tmp = 3;
	dmcr |= (tmp << EMC_DMCR_RCD_BIT);
	tmp = SDRAM_TPC/ns;
	if (tmp > 7)
		tmp = 7;
	dmcr |= (tmp << EMC_DMCR_TPC_BIT);
	tmp = SDRAM_TRWL/ns;
	if (tmp > 3)
		tmp = 3;
	dmcr |= (tmp << EMC_DMCR_TRWL_BIT);
	tmp = (SDRAM_TRAS + SDRAM_TPC)/ns;
	if (tmp > 14)
		tmp = 14;
	dmcr |= (((tmp + 1) >> 1) << EMC_DMCR_TRC_BIT);

	/* SDRAM mode values */
	sdmode = EMC_SDMR_BT_SEQ | 
		 EMC_SDMR_OM_NORMAL |
		 EMC_SDMR_BL_4 | 
		 cas_latency_sdmr[((SDRAM_CASL == 3) ? 1 : 0)];

	if (SDRAM_BW16)
		sdmode <<= 1;
	else
		sdmode <<= 2;

	/* First, precharge phase */
	REG_EMC_DMCR = dmcr;

	/* Set refresh registers */
	tmp = SDRAM_TREF/ns;
	tmp = tmp/64 + 1;
	if (tmp > 0xff)
		tmp = 0xff;

	REG_EMC_RTCOR = tmp;
	REG_EMC_RTCSR = EMC_RTCSR_CKS_64;	/* Divisor is 64, CKO/64 */

	/* precharge all chip-selects */
	REG8(EMC_SDMR0|sdmode) = 0;
	REG8(EMC_SDMR1|sdmode) = 0;

	/* wait for precharge, > 200us */
	tmp = (CPU_CLK / 1000000) * 200;
	while (tmp--);

	/* enable refresh and set SDRAM mode */
	REG_EMC_DMCR = dmcr | EMC_DMCR_RFSH | EMC_DMCR_MRSET;

	/* write sdram mode register for each chip-select */
	REG8(EMC_SDMR0|sdmode) = 0;
	REG8(EMC_SDMR1|sdmode) = 0;

	/* everything is ok now */
}

/* Init PLL
 *
 * PLL output = EXTAL * NF / (NR * NO)
 *
 * NF = FD + 2, NR = RD + 2
 * NO = 1 (if OD = 0), NO = 2 (if OD = 1 or 2), NO = 4 (if OD = 3)
 */
static void pll_init(void)
{
	unsigned int nf, plcr1;

	nf = CFG_CPU_SPEED * 2 / CFG_EXTAL;
	plcr1 = ((nf-2) << CPM_PLCR1_PLL1FD_BIT) |
		(0 << CPM_PLCR1_PLL1RD_BIT) |	/* RD=0, NR=2, 1.8432 = 3.6864/2 */
		(0 << CPM_PLCR1_PLL1OD_BIT) |   /* OD=0, NO=1 */
		(0x20 << CPM_PLCR1_PLL1ST_BIT) | /* PLL stable time */
		CPM_PLCR1_PLL1EN;                /* enable PLL */          

	/* Clock divisors.
	 * 
	 * CFCR values: when CPM_CFCR_UCS(bit 28) is set, select external USB clock.
	 *
	 * 0x10411110 -> 1:2:2:2:2
	 * 0x10422220 -> 1:3:3:3:3
	 * 0x10433330 -> 1:4:4:4:4
	 * 0x10444440 -> 1:6:6:6:6
	 * 0x10455550 -> 1:8:8:8:8
	 * 0x10466660 -> 1:12:12:12:12
	 */
	REG_CPM_CFCR = 0x00422220 | (((CFG_CPU_SPEED/48000000) - 1) << 25);

	/* PLL out frequency */
	REG_CPM_PLCR1 = plcr1;
}

static void gpio_init(void)
{
#if (CFG_UART_BASE == UART0_BASE)
	__gpio_as_uart0();
#endif
#if (CFG_UART_BASE == UART1_BASE)
	__gpio_as_uart1();
#endif
#if (CFG_UART_BASE == UART2_BASE)
	__gpio_as_uart2();
#endif
#if (CFG_UART_BASE == UART3_BASE)
	__gpio_as_uart3();
#endif
//	__gpio_as_emc();
}

/* NAND flash routines */

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

#ifdef NAND_ECC_ENABLE

/* Detect and correct a 1 bit error for 256 byte block */

static int nand_correct_data(uchar *dat, uchar *read_ecc, uchar *calc_ecc)
{
	uchar d1, d2, d3;
	ulong d, bit, i, j;

	/* Do error detection */ 
	d1 = calc_ecc[0] ^ read_ecc[0];
	d2 = calc_ecc[1] ^ read_ecc[1];
	d3 = calc_ecc[2] ^ read_ecc[2];

	d = (d3 << 16) | (d2 << 8) | d1;

	if (d == 0) {
		/* No errors */
		return 0;
	}

	/* Found and will correct single bit error in the data */
	bit = 0;
	for (i = 0; i < 24; i++) {
		if ((d >> i) & 0x1)
			bit++;
	}

	if (bit != 11) {
		/* Uncorrectable Error */
		return -2;
	} else {
		/* ECC Code Error Correction */
		bit = 0;
		for (i = 12; i >= 1; i--) {
			bit <<= 1;
			bit |= ((d>>(i*2-1)) & 0x1);
		}
		j = bit & 0x07;
		dat[(bit >> 3)] ^= (1 << j);
		return 1;
	}
}
#endif /* NAND_ECC_ENABLE */

static int nand_read_page(int block, int page, uchar *dst, uchar *oob_buf, ulong *calc_ecc)
{
	int page_addr = page + block * page_per_block;
	uchar *databuf = dst;
	ulong *eccbuf = calc_ecc;
	int i;

	/* Send READ0 command */
	__nand_cmd(NAND_CMD_READ0);

	/* Send column address */
	__nand_addr(0);
	if (page_size == 2048)
		__nand_addr(0);

	/* Send page address */
	__nand_addr(page_addr & 0xff);
	__nand_addr((page_addr >> 8) & 0xff);
	if (row_cycle == 3)
		__nand_addr((page_addr >> 16) & 0xff);

	/* Send READSTART command for 2048 ps NAND */
	if (page_size == 2048)
		__nand_cmd(NAND_CMD_READSTART);

	/* Wait for device ready */
	__nand_dev_ready();

	/* Read page data */
	for (i = 0; i < ecc_count; i++) {
		__nand_ecc_enable();
		nand_read_buf((void *)databuf, ECC_BLOCK, bus_width);
		__nand_ecc_disable();
		eccbuf[i] = __nand_ecc();
		databuf += ECC_BLOCK;
	}

	/* Read oob data */
	nand_read_buf((void *)oob_buf, oob_size, bus_width);


	return 0;
}

static int nand_load(int offs, int uboot_size, uchar *dst)
{
	int block;
	int blockcopy_count;
	int page;
	int i;
	uchar oob_buf[64];
	ulong calc_ecc[16];
	ulong *read_ecc;

	__nand_enable();

	/*
	 * offs has to be aligned to a block address!
	 */
	block = offs / block_size;
	blockcopy_count = 0;

	while (blockcopy_count < (uboot_size / block_size)) {
		for (page = 0; page < page_per_block; page++) {
			nand_read_page(block, page, dst, oob_buf, calc_ecc);

			if (page == 0) {
				if (oob_buf[bad_block_pos] != 0xff) {
					block++;

					/*
					 * Skip bad block
					 */
					continue;
				}
			}
#ifdef NAND_ECC_ENABLE
			read_ecc = (ulong *)(((u32)oob_buf) + ECC_POS);
			for (i = 0; i < ecc_count; i++) {
				nand_correct_data(dst + i * ECC_BLOCK, (uchar *)(read_ecc + i), (uchar *)(calc_ecc + i));
			}
#endif

			dst += page_size;

		}

		block++;
		blockcopy_count++;
	}

	__nand_disable();

	return 0;
}

void spl_boot(void)
{
	int boot_sel, ret;
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
	 * Decode the CPU boot select
	 */
	boot_sel = (REG_EMC_NFCSR & 0x70) >> 4;
	bus_width = (boot_sel & 0x1) ? 16 : 8;
	page_size = (boot_sel & 0x2) ? 2048 : 512;
	row_cycle = (boot_sel & 0x4) ? 3 : 2;
	page_per_block = (page_size == 2048) ? 64 : 32;
	bad_block_pos = (page_size == 2048) ? 0 : 5;
	ecc_count = page_size / ECC_BLOCK;
	oob_size = page_size / 32;
	block_size = page_size * page_per_block;

	/*
	 * Load U-Boot image from NAND into RAM
	 */
	ret = nand_load(CFG_NAND_U_BOOT_OFFS, CFG_NAND_U_BOOT_SIZE,
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
