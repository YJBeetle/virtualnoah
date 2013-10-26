/*
 * Platform independend driver for JZ4730.
 *
 * Copyright (c) 2007 Ingenic Semiconductor Inc.
 * Author: <jlwei@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */
#include <common.h>

#if (CONFIG_COMMANDS & CFG_CMD_NAND) && defined(CONFIG_JZ4740)

#include <nand.h>
#include <asm/jz4740.h>

static struct nand_oobinfo nand_oob_rs = {
	.useecc = MTD_NANDECC_AUTOPLACE,
	.eccbytes = 36,
	.eccpos = {
		6,  7,  8,  9,  10, 11, 12, 13,
		14, 15, 16, 17, 18, 19, 20, 21,
		22, 23, 24, 25, 26, 27, 28, 29,
		30, 31, 32, 33, 34, 35, 36, 37,
		38, 39, 40, 41},
	.oobfree = { {2, 4}, {42, 22} }
};

#define PAR_SIZE 9
#define __nand_ecc_enable()    (REG_EMC_NFECR = EMC_NFECR_ECCE | EMC_NFECR_ERST )
#define __nand_ecc_disable()   (REG_EMC_NFECR &= ~EMC_NFECR_ECCE)

#define __nand_select_rs_ecc() (REG_EMC_NFECR |= EMC_NFECR_RS)

#define __nand_rs_ecc_encoding()	(REG_EMC_NFECR |= EMC_NFECR_RS_ENCODING)
#define __nand_rs_ecc_decoding()	(REG_EMC_NFECR |= EMC_NFECR_RS_DECODING)
#define __nand_ecc_encode_sync() while (!(REG_EMC_NFINTS & EMC_NFINTS_ENCF))
#define __nand_ecc_decode_sync() while (!(REG_EMC_NFINTS & EMC_NFINTS_DECF))

static void jz_hwcontrol(struct mtd_info *mtd, int cmd)
{
	struct nand_chip *this = (struct nand_chip *)(mtd->priv);
	switch (cmd) {
		case NAND_CTL_SETNCE:
			REG_EMC_NFCSR |= EMC_NFCSR_NFCE1;
			break;

		case NAND_CTL_CLRNCE:
			REG_EMC_NFCSR &= ~EMC_NFCSR_NFCE1;
			break;

		case NAND_CTL_SETCLE:
			this->IO_ADDR_W = (void __iomem *)((unsigned long)(this->IO_ADDR_W) | 0x00008000);
			break;

		case NAND_CTL_CLRCLE:
			this->IO_ADDR_W = (void __iomem *)((unsigned long)(this->IO_ADDR_W) & ~0x00008000);
			break;

		case NAND_CTL_SETALE:
			this->IO_ADDR_W = (void __iomem *)((unsigned long)(this->IO_ADDR_W) | 0x00010000);
			break;

		case NAND_CTL_CLRALE:
			this->IO_ADDR_W = (void __iomem *)((unsigned long)(this->IO_ADDR_W) & ~0x00010000);
			break;
	}
}

static int jz_device_ready(struct mtd_info *mtd)
{
	int ready;
	udelay(20);	/* FIXME: add 20us delay */
	ready = (REG_GPIO_PXPIN(2) & 0x40000000) ? 1 : 0;
	return ready;
}

/*
 * EMC setup
 */
static void jz_device_setup(void)
{
	/* Set NFE bit */
	REG_EMC_NFCSR |= EMC_NFCSR_NFE1;
	REG_EMC_SMCR1 = 0x094c4400;
}

void board_nand_select_device(struct nand_chip *nand, int chip)
{
	/*
	 * Don't use "chip" to address the NAND device,
	 * generate the cs from the address where it is encoded.
	 */
}

static int jzsoc_nand_calculate_rs_ecc(struct mtd_info* mtd, const u_char* dat,
				u_char* ecc_code)
{
	volatile u8 *paraddr = (volatile u8 *)EMC_NFPAR0;
	short i;

	__nand_ecc_encode_sync() 
	__nand_ecc_disable();
	
	for(i = 0; i < PAR_SIZE; i++) 
		ecc_code[i] = *paraddr++;			
	
	return 0;
}

static void jzsoc_nand_enable_rs_hwecc(struct mtd_info* mtd, int mode)
{
 	__nand_ecc_enable();
	__nand_select_rs_ecc();

	REG_EMC_NFINTS = 0x0;
	if (NAND_ECC_READ == mode){
		__nand_rs_ecc_decoding();
	}
	if (NAND_ECC_WRITE == mode){
		__nand_rs_ecc_encoding();
	}
}	

/* Correct 1~9-bit errors in 512-bytes data */
static void jzsoc_rs_correct(unsigned char *dat, int idx, int mask)
{
	int i;

	idx--;

	i = idx + (idx >> 3);
	if (i >= 512)
		return;

	mask <<= (idx & 0x7);

	dat[i] ^= mask & 0xff;
	if (i < 511)
		dat[i+1] ^= (mask >> 8) & 0xff;
}

static int jzsoc_nand_rs_correct_data(struct mtd_info *mtd, u_char *dat,
				 u_char *read_ecc, u_char *calc_ecc)					
{
	volatile u8 *paraddr = (volatile u8 *)EMC_NFPAR0;
	short k;
	u32 stat;
	/* Set PAR values */
	
	for (k = 0; k < PAR_SIZE; k++) {
		*paraddr++ = read_ecc[k];
	}

	/* Set PRDY */
	REG_EMC_NFECR |= EMC_NFECR_PRDY;

	/* Wait for completion */
	__nand_ecc_decode_sync();
	__nand_ecc_disable();

	/* Check decoding */
	stat = REG_EMC_NFINTS;
	if (stat & EMC_NFINTS_ERR) {
		if (stat & EMC_NFINTS_UNCOR) {
			printk("Uncorrectable error occurred\n");
			return -1;
		}
		else {
			u32 errcnt = (stat & EMC_NFINTS_ERRCNT_MASK) >> EMC_NFINTS_ERRCNT_BIT;
			switch (errcnt) {
			case 4:
				jzsoc_rs_correct(dat, (REG_EMC_NFERR3 & EMC_NFERR_INDEX_MASK) >> EMC_NFERR_INDEX_BIT, (REG_EMC_NFERR3 & EMC_NFERR_MASK_MASK) >> EMC_NFERR_MASK_BIT);
			case 3:
				jzsoc_rs_correct(dat, (REG_EMC_NFERR2 & EMC_NFERR_INDEX_MASK) >> EMC_NFERR_INDEX_BIT, (REG_EMC_NFERR2 & EMC_NFERR_MASK_MASK) >> EMC_NFERR_MASK_BIT);
			case 2:
				jzsoc_rs_correct(dat, (REG_EMC_NFERR1 & EMC_NFERR_INDEX_MASK) >> EMC_NFERR_INDEX_BIT, (REG_EMC_NFERR1 & EMC_NFERR_MASK_MASK) >> EMC_NFERR_MASK_BIT);
			case 1:
				jzsoc_rs_correct(dat, (REG_EMC_NFERR0 & EMC_NFERR_INDEX_MASK) >> EMC_NFERR_INDEX_BIT, (REG_EMC_NFERR0 & EMC_NFERR_MASK_MASK) >> EMC_NFERR_MASK_BIT);
				return 0;
			default:
				break;
	   		}
		}
	}
	//no error need to be correct 
	return 0;
}

/*
 * Main initialization routine
 */
void board_nand_init(struct nand_chip *nand)
{
	jz_device_setup();

	nand->eccmode = NAND_ECC_HW9_512;	/* FIXME: should use NAND_ECC_SOFT */
        nand->hwcontrol = jz_hwcontrol;
        nand->dev_ready = jz_device_ready;
	
	nand->correct_data  = jzsoc_nand_rs_correct_data;
	nand->enable_hwecc  = jzsoc_nand_enable_rs_hwecc;
	nand->calculate_ecc = jzsoc_nand_calculate_rs_ecc;

        /* Set address of NAND IO lines */
        nand->IO_ADDR_R = (void __iomem *) CFG_NAND_BASE;
        nand->IO_ADDR_W = (void __iomem *) CFG_NAND_BASE;

        /* 20 us command delay time */
        nand->chip_delay = 20;
//	nand->autooob    = &nand_oob_rs; // init in nand_base.c
}
#endif /* (CONFIG_COMMANDS & CFG_CMD_NAND) */
