/*
 * QEMU pavo demo board emulation
 *
 * Copyright (c) 2009 yajin (yajin@vm-kernel.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


/*
 * The emulation target is pavo demo board.
 *  http://www.ingenic.cn/eng/productServ/kfyd/Hardware/pffaqQuestionContent.aspx?Category=2&Question=3
 *
 */

#include "hw.h"
#include "pc.h"
#include "fdc.h"
#include "net.h"
#include "boards.h"
#include "smbus.h"
#include "block.h"
#include "flash.h"
#include "mips.h"
#include "pci.h"
#include "qemu-char.h"
#include "sysemu.h"
#include "audio/audio.h"
#include "boards.h"
#include "qemu-log.h"
#include "mips_jz.h"



#define PAVO_RAM_SIZE       (0x4000000) /*64M */
#define PAVO_OSC_EXTAL     (12000000)   /*12MHZ */

/* pavo board support */
struct mips_pavo_s
{
    struct jz_state_s *soc;

    struct nand_bflash_s *nand;
};

static uint32_t pavo_nand_read8(void *opaque, target_phys_addr_t addr)
{
	struct mips_pavo_s *s = (struct mips_pavo_s *) opaque;

	switch (addr)
	{
		case 0x8000: /*NAND_COMMAND*/
		case 0x10000: /*NAND_ADDRESS*/
			jz4740_badwidth_read8(s,addr);
			break;
		case 0x0: /*NAND_DATA*/
			return nandb_read_data8(s->nand);
			break;
		default:
			jz4740_badwidth_read8(s,addr);
			break;
	}
    return 0;
}

static void pavo_nand_write8(void *opaque, target_phys_addr_t addr,
                uint32_t value)
{
	struct mips_pavo_s *s = (struct mips_pavo_s *) opaque;

	//printf("write addr %x value %x \n",addr,value);

    switch (addr)
	{
		case 0x8000: /*NAND_COMMAND*/
			nandb_write_command(s->nand,value);
			break;
		case 0x10000: /*NAND_ADDRESS*/
			nandb_write_address(s->nand,value);
			break;
		case 0x0: /*NAND_DATA*/
			nandb_write_data8(s->nand,value);
			break;
		default:
			jz4740_badwidth_write8(s,addr,value);
			break;
	}
}


CPUReadMemoryFunc *pavo_nand_readfn[] = {
        pavo_nand_read8,
        jz4740_badwidth_read16,
        jz4740_badwidth_read32,
};
CPUWriteMemoryFunc *pavo_nand_writefn[] = {
        pavo_nand_write8,
        jz4740_badwidth_write16,
        jz4740_badwidth_write32,
};
    
static void pavo_nand_setup(struct mips_pavo_s *s)
{
	int iomemtype;
	
	/*K9K8G08U0*/
	s->nand = nandb_init(NAND_MFR_SAMSUNG,0xd3);

	iomemtype = cpu_register_io_memory(0, pavo_nand_readfn,
                    pavo_nand_writefn, s);
    cpu_register_physical_memory(0x18000000, 0x20000, iomemtype);
}

static int pavo_nand_read_page(struct mips_pavo_s *s,uint8_t *buf, uint16_t page_addr)
{
	uint8_t *p;
	int i;

	p=(uint8_t *)buf;

	/*send command 0x0*/
	pavo_nand_write8(s,0x00008000,0);
	/*send page address */
	pavo_nand_write8(s,0x00010000,page_addr&0xff);
	pavo_nand_write8(s,0x00010000,(page_addr>>8)&0x7);
	pavo_nand_write8(s,0x00010000,(page_addr>>11)&0xff);
	pavo_nand_write8(s,0x00010000,(page_addr>>19)&0xff);
	pavo_nand_write8(s,0x00010000,(page_addr>>27)&0xff);
	/*send command 0x30*/
	pavo_nand_write8(s,0x00008000,0x30);

	for (i=0;i<0x800;i++)
	{
		*p++ = pavo_nand_read8(s,0x00000000);
	}
	return 1;
}

/*read the u-boot from NAND Flash into internal RAM*/
static int pavo_boot_from_nand(struct mips_pavo_s *s)
{
	uint32_t len;
	uint8_t nand_page[0x800],*load_dest;
	uint32_t nand_pages,i;

	//int fd;
	

	len = 0x2000; /*8K*/
	
	/*put the first page into internal ram*/
	load_dest = phys_ram_base;
	
	nand_pages = len/0x800;
	//fd = open("u-boot.bin", O_RDWR | O_CREAT);
	for (i=0;i<nand_pages;i++)
	{
		pavo_nand_read_page(s,nand_page,i*0x800);
		memcpy(load_dest,nand_page,0x800);
		//write(fd,nand_page,0x800);
		load_dest += 0x800;
	}
	s->soc->env->active_tc.PC = 0x80000004;

	//close(fd);
	return 0;

}


 static int pavo_rom_emu(struct mips_pavo_s *s)
{
	if (pavo_boot_from_nand(s)<0)
		return (-1); 
	return (0);
}

static void mips_pavo_init(ram_addr_t ram_size, int vga_ram_size,
                    const char *boot_device, DisplayState * ds,
                    const char *kernel_filename,
                    const char *kernel_cmdline,
                    const char *initrd_filename, const char *cpu_model)
{
    struct mips_pavo_s *s = (struct mips_pavo_s *) qemu_mallocz(sizeof(*s));

    if (ram_size < PAVO_RAM_SIZE + JZ4740_SRAM_SIZE)
    {
        fprintf(stderr, "This architecture uses %d bytes of memory\n",
                PAVO_RAM_SIZE + JZ4740_SRAM_SIZE);
        exit(1);
    }
    s->soc = jz4740_init(PAVO_RAM_SIZE, PAVO_OSC_EXTAL,ds);
    pavo_nand_setup(s);
    if (pavo_rom_emu(s)<0)
   	{
   		fprintf(stderr,"boot from nand failed \n");
   		exit(-1);
   	}

}




QEMUMachine mips_pavo_machine = {
    .name = "pavo",
    .desc = "JZ Pavo demo board",
    .init = mips_pavo_init,
    .ram_require = (JZ4740_SRAM_SIZE + PAVO_RAM_SIZE) | RAMSIZE_FIXED,
    .nodisk_ok = 1,
};
