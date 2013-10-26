/*
 * (C) Copyright 2001
 * Gerald Van Baren, Custom IDEAS, vanbaren@cideas.com.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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
#include <asm/byteorder.h>
#include <asm/jz4760.h>

#define DDR_DMA_BASE  (0xa0400000)		/*un-cached*/
#define DESCRIPTOR_BASE (0xa0200000)		/*un-cached*/

typedef struct {
	volatile u32 dcmd;	/* DCMD value for the current transfer */
	volatile u32 dsadr;	/* DSAR value for the current transfer */
	volatile u32 dtadr;	/* DTAR value for the current transfer */
	volatile u32 dcnt;	/* transfer count */
	volatile u32 dstrd; /* DMA source and target stride address */
	volatile u32 dreqt; /* DMA request type for current transfer */
	volatile u32 dnt;	/* NAND detect timer enable(15) and value(0~5), and Tail counter(22~16)*/
	volatile u32 ddadr;	/* Next descriptor address(31~4)  */
} jz_dma_desc_8word;

int do_write ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned int addr;
	unsigned int value;
	
	if(argc < 3) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return -1;
	}

	addr = (unsigned int)simple_strtoul(argv[1], NULL, 16);
	value = (unsigned int)simple_strtoul(argv[2], NULL, 16);

	*(unsigned int *)addr = value;
	
	printf("write %x to address %x\n", value, addr);	

	return 0;
}

int do_read ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned int addr;
	unsigned int value;
	
	if(argc < 2) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return -1;
	}

	addr = (unsigned int)simple_strtoul(argv[1], NULL, 16);

	value = *(unsigned int *)addr;
	printf("read %x from address %x\n", value, addr);
	return 0;
}

int do_memcpy_loop ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	volatile jz_dma_desc_8word *desc;
	long int blocksize, memsize;
	unsigned int dma_src_phys_addr, blocks, addr;
	int i;
	
	desc = DESCRIPTOR_BASE;
	
	dma_src_phys_addr = (DESCRIPTOR_BASE & ~0xa0000000);

	if(argc < 2) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return -1;
	}	

	memsize = (unsigned int)simple_strtoul(argv[1], NULL, 0);
	//blocksize = (unsigned int)simple_strtoul(argv[2], NULL, 16);
		

	blocksize = (8 << 20);        // 8MB
	blocks = memsize / blocksize;

	addr = DDR_DMA_BASE;
	for (i = 0; i < blocksize; i += 4) {
		*(volatile unsigned int *)addr = ( (i/4*0x1111) | ( (i/4*0x1111) << 16 ) );
		//*(volatile unsigned int *)addr = addr;
		addr += 4;
	}
	
	// DMA loop
	desc->dcmd = DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32 | DMAC_DCMD_DS_32BYTE | DMAC_DCMD_LINK;
	desc->dsadr = (DDR_DMA_BASE & ~0xa0000000);
	desc->dtadr = ((DDR_DMA_BASE + blocksize)  & ~0xa0000000);
	desc->dcnt = blocksize / 32;
	desc->dreqt = DMAC_DRSR_RS_AUTO;
	desc->ddadr = dma_src_phys_addr;

	REG_BDMAC_DDA(1) = dma_src_phys_addr;

	/* Setup request source */
	REG_BDMAC_DRSR(1) = DMAC_DRSR_RS_AUTO;
	
	/* Enable DMA */
	REG_BDMAC_DMACR |= DMAC_DMACR_DMAE;

        /* DMA doorbell set -- start nand DMA now ... */
	REG_BDMAC_DMADBSR = (1 << (1));

	/* Setup DMA channel control/status register */
	REG_BDMAC_DCCSR(1) = DMAC_DCCSR_DES8 | DMAC_DCCSR_EN;

	printf("memory copy start\n");

	return 0;
}

/***************************************************/

U_BOOT_CMD(
 	addr_write,	3,	1,	do_write,
	"addr_write     - write a value to certain address\n",
	"...."
);

U_BOOT_CMD(
 	addr_read,	2,	1,	do_read,
	"addr_read     - read a value from certain address\n",
	"...."
);

U_BOOT_CMD(
 	mem_cpy_loop,	2,	1,	do_memcpy_loop,
	"mem_cpy_loop     - start a loop of memory copy\n",
	"...."
);

