/*
 * Big page NAND flash memory emulation.  based on 256M/16 bit flash datasheet from micro(MT29F2G16ABC)
 *
 * Copyright (C) 2008 yajin(yajin@vm-kernel.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 or
 * (at your option) version 3 of the License.
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

#include "hw.h"
#include "flash.h"
#include "block.h"
#include "sysemu.h"


#define MAX_PAGE		0x800
#define MAX_OOB		0x40
#define PAGE_MASK		(0xffff)
#define BUS_WIDTH_16  2
#define BUS_WIDTH_8 1

//#define DEBUG

struct nand_flash_info_s
{
    uint8_t  manf_id,chip_id;
    uint32_t size;;
    int bus_width;
    int page_shift;
    int oob_shift;
    int block_shift;
};

struct nand_flash_info_s nand_flash_info[2] =
{
    {0x2c, 0xba, 256,2, 11, 6, 6},
    {0Xec, 0xd3, 1024,1, 11, 6, 6}
};


struct nand_bflash_s
{
	BlockDriverState *bdrv;
    uint8_t manf_id, chip_id;
    uint32_t size, pages;
    uint32_t page_size, page_shift;
    uint32_t oob_size, oob_shift;
    uint32_t page_oob_size;
    uint32_t page_sectors;      /*sector = 512 bytes */
    uint32_t block_shift, block_pages;  /*how many pages in a block */
    uint32_t bus_width;         /*bytes */

    //uint8_t *internal_buf;
    uint8_t io[MAX_PAGE + MAX_OOB + 0x400];
    uint8_t *ioaddr;
    int iolen;


    uint32 addr_low, addr_high;
    uint32 addr_cycle;

    uint32 cmd, status;
  #ifdef DEBUG  
    FILE *fp;
  #endif
};


#ifdef DEBUG
static void debug_init(struct nand_bflash_s *s)
{
	s->fp=fopen("nandflash_debug.txt","w+");
	if (s->fp==NULL)
	{
		fprintf(stderr,"can not open nandflash_debug.txt \n");
		exit(-1);
	}
		
}
static void debug_out(struct nand_bflash_s *s,const char* format, ...)
{
	va_list ap;
	if (s->fp)
	{
		 va_start(ap, format);
    	 vfprintf(s->fp, format, ap);
    	 fflush(s->fp);
    	va_end(ap);
	}
}

#else
static void debug_init(struct nand_bflash_s *s)
{
	
}
static void debug_out(struct nand_bflash_s *s,const char* format, ...)
{
	
}

#endif

static inline uint32_t get_page_number(struct nand_bflash_s *s,
                                       uint32_t addr_low, uint32 addr_high)
{
    return (addr_high << 16) + ((addr_low >> 16) & PAGE_MASK);
}



/* Program a single page */
static void nand_blk_write(struct nand_bflash_s *s)
{
    uint32_t page_number, off,  sector, soff;
    uint8_t *iobuf=NULL;

	if (!iobuf)
    	iobuf = qemu_mallocz((s->page_sectors + 2) * 0x200);
    if (!iobuf)
    {
        fprintf(stderr, "can not alloc io buffer size 0x%x \n",
                (s->page_sectors + 2) * 0x200);
        cpu_abort(cpu_single_env, "%s: can not alloc io buffer size 0x%x \n",
                  __FUNCTION__, (s->page_sectors + 2) * 0x200);
    }

    page_number = get_page_number(s, s->addr_low, s->addr_high);

    debug_out(s,"nand_blk_write page number %x s->addr_low %x s->addr_high %x\n",page_number,s->addr_low,s->addr_high);

    if (page_number >= s->pages)
        return;

    off = page_number * s->page_oob_size + (s->addr_low & PAGE_MASK);
    sector = off >> 9;
    soff = off & 0x1ff;
    if (bdrv_read(s->bdrv, sector, iobuf, s->page_sectors + 2) == -1)
    {
        printf("%s: read error in sector %i\n", __FUNCTION__, sector);
        return;
    }

    memcpy(iobuf + soff, s->io, s->iolen);

    if (bdrv_write(s->bdrv, sector, iobuf, s->page_sectors + 2) == -1)
        printf("%s: write error in sector %i\n", __FUNCTION__, sector);

    //qemu_free(iobuf);
}


static void nandb_blk_load(struct nand_bflash_s *s)
{
    uint32_t page_number, offset;
    offset = s->addr_low & PAGE_MASK;

    page_number = get_page_number(s, s->addr_low, s->addr_high);
	debug_out(s,"nandb_blk_load page number %x s->addr_low %x s->addr_high %x\n",page_number,s->addr_low,s->addr_high);
    if (page_number >= s->pages)
        return;
	
    if (bdrv_read(s->bdrv, (page_number * s->page_oob_size + offset) >> 9,
                  s->io, (s->page_sectors + 2)) == -1)
        printf("%s: read error in sector %i\n",
               __FUNCTION__, page_number * s->page_oob_size);
    s->ioaddr = s->io + ((page_number * s->page_oob_size + offset) & 0x1ff);
}


/* Erase a single block */
static void nandb_blk_erase(struct nand_bflash_s *s)
{
    uint32_t page_number,  sector, addr, i;

    uint8_t iobuf[0x200];

	 memset(iobuf,0xff,sizeof(iobuf));
	 s->addr_low = s->addr_low & ~((1 << (16 + s->block_shift)) - 1);
    page_number = get_page_number(s, s->addr_low, s->addr_high);
    debug_out(s,"nandb_blk_erase page number %x s->addr_low %x s->addr_high %x\n",page_number,s->addr_low,s->addr_high);
    if (page_number >= s->pages)
        return;

    addr = page_number * s->page_oob_size;
    
    sector = addr >> 9;
    if (bdrv_read(s->bdrv, sector, iobuf, 1) == -1)
        printf("%s: read error in sector %i\n", __FUNCTION__, sector);
    memset(iobuf + (addr & 0x1ff), 0xff, (~addr & 0x1ff) + 1);
    if (bdrv_write(s->bdrv, sector, iobuf, 1) == -1)
        printf("%s: write error in sector %i\n", __FUNCTION__, sector);

    memset(iobuf, 0xff, 0x200);
    i = (addr & ~0x1ff) + 0x200;
    for (addr += (s->page_oob_size*s->block_pages - 0x200); i < addr; i += 0x200)
        if (bdrv_write(s->bdrv, i >> 9, iobuf, 1) == -1)
            printf("%s: write error in sector %i\n", __FUNCTION__, i >> 9);

    sector = i >> 9;
    if (bdrv_read(s->bdrv, sector, iobuf, 1) == -1)
        printf("%s: read error in sector %i\n", __FUNCTION__, sector);
    memset(iobuf, 0xff, ((addr - 1) & 0x1ff) + 1);
    if (bdrv_write(s->bdrv, sector, iobuf, 1) == -1)
        printf("%s: write error in sector %i\n", __FUNCTION__, sector);
}

static void nandb_next_page(struct nand_bflash_s *s)
{
    if ((s->addr_low + 0x10000) < s->addr_low)
        s->addr_high++;
    s->addr_low += 0x10000;
}

void nandb_write_command(struct nand_bflash_s *s, uint16_t value)
{
    int id_index[5] = { 0, 1, 2, 3,4};

    debug_out(s,"nandb_write_command %x\n",value);

    switch (value)
    {
    case 0x00:
    case 0x05:
    	 s->iolen = 0;
        s->addr_cycle = 0;
        break;
    case 0x60:
    	/*earse only need 3 addrss cycle.Its address is block address*/
    	s->addr_low &= ~PAGE_MASK;
    	s->addr_high =0;
    	s->addr_cycle = 2;
    	break;
    case 0x30:
    case 0xe0:
        s->iolen = s->page_oob_size - (s->addr_low & PAGE_MASK);
        nandb_blk_load(s);
        break;
    case 0x31:
    case 0x3f:
        nandb_next_page(s);
        s->iolen = s->page_oob_size - (s->addr_low & PAGE_MASK);
        nandb_blk_load(s);
        break;
    case 0x90:
        s->iolen = 5 * s->bus_width;
        memset(s->io, 0x0, s->iolen);
        if (s->bus_width == BUS_WIDTH_16)
        {
            id_index[0] = 0;
            id_index[1] = 2;
            id_index[2] = 4;
            id_index[3] = 6;
            id_index[4] = 6;
        }
        s->io[id_index[0]] = s->manf_id;
        s->io[id_index[1]] = s->chip_id;
        s->io[id_index[2]] = 'Q';       /* Don't-care byte (often 0xa5) */
        if ((s->manf_id == NAND_MFR_MICRON) && (s->chip_id == 0xba))
            s->io[id_index[3]] = 0x55;
       if ((s->manf_id == NAND_MFR_SAMSUNG) && (s->chip_id == 0xd3))
       {
       	s->io[id_index[3]] = 0x95;
       	s->io[id_index[4]] = 0x48;
       }
        s->ioaddr = s->io;
        s->addr_cycle = 0;
        break;
    case 0x70:
        if ((s->manf_id == NAND_MFR_MICRON) && (s->chip_id == 0xba))
        {
            s->status |= 0x60;  /*flash is ready */
            s->status |= 0x80;  /*not protect */
        }
         if ((s->manf_id == NAND_MFR_SAMSUNG) && (s->chip_id == 0xd3))
        {
            s->status |= 0x40;  /*flash is ready */
            s->status |= 0x80;  /*not protect */
        }
        s->io[0] = s->status;
        s->ioaddr = s->io;
        s->iolen = 1;
        break;
    case 0xd0:
        nandb_blk_erase(s);
        break;
    case 0x80:
    case 0x85:
    	s->addr_cycle = 0;
    	s->ioaddr = s->io;
       s->iolen = 0;
        break;
    case 0x10:
        nand_blk_write(s);
        break;
    case 0xff:
    	s->addr_cycle =0;
    	s->iolen=0;
    	s->addr_low =0;
    	s->addr_high =0;
    	s->ioaddr = NULL;
    	break;
    default:
        fprintf(stderr, "unknown nand command 0x%x \n", value);
        exit(-1);
    }
    s->cmd = value;
}

void nandb_write_address(struct nand_bflash_s *s, uint16_t value)
{
    uint32_t mask;
    uint32_t colum_addr;
    //if (s->cmd==0x60)
    debug_out(s,"value %x addr_cycle %x \n",value,s->addr_cycle);
    if (s->addr_cycle < 5)
    {
        if (s->addr_cycle < 4)
        {
            mask = ~(0xff << (s->addr_cycle * 8));
            s->addr_low &= mask;
            s->addr_low |= value << (s->addr_cycle * 8);
        }
        else
        {
			  mask = ~(0xff << ((s->addr_cycle-4) * 8));
            s->addr_high &= mask;
            s->addr_high |= value << ((s->addr_cycle-4) * 8);
        }
    }
    else
    {
    	fprintf(stderr,"%s wrong addr cycle\n",__FUNCTION__);
    	exit(-1);
    }
    if ((s->addr_cycle==1)&&(s->bus_width!=1))
    {
    	colum_addr = s->addr_low & PAGE_MASK;
    	colum_addr *= s->bus_width;
    	s->addr_low &= ~PAGE_MASK;
    	s->addr_low += colum_addr;
    }
    s->addr_cycle++;
    
}

uint8_t nandb_read_data8(struct nand_bflash_s *s)
{
	uint8_t ret;
	if ((s->iolen==0)&&(s->cmd==0x31))
	{
		nandb_next_page(s);
        s->iolen = s->page_oob_size - (s->addr_low & PAGE_MASK);
        nandb_blk_load(s);
	}
	if (s->iolen <= 0)
	{
		fprintf(stderr,"iolen <0 \n");
		exit(-1);
	}
  	if (s->cmd!=0x70)  	
    	s->iolen -=1 ;
    ret = *((uint8_t *)s->ioaddr);
    if (s->cmd!=0x70)  	
    	s->ioaddr += 1;   

    //debug_out(s," %x ",ret);
    return ret;
}

void nandb_write_data8(struct nand_bflash_s *s, uint8_t value)
{
	 if ((s->cmd == 0x80) )
	 {
        if (s->iolen < s->page_oob_size)
        {
        	s->io[s->iolen ++] = value&0xff;
        }
    }
}

uint16_t nandb_read_data16(struct nand_bflash_s *s)
{
	uint16_t ret;
	if ((s->iolen==0)&&(s->cmd==0x31))
	{
		nandb_next_page(s);
        s->iolen = s->page_oob_size - (s->addr_low & PAGE_MASK);
        nandb_blk_load(s);
	}
	if (s->iolen <= 0)
	{
		fprintf(stderr,"iolen <0 \n");
		exit(-1);
	}
  	if (s->cmd!=0x70)  	
    	s->iolen -=2 ;
    ret = *((uint16_t *)s->ioaddr);
    if (s->cmd!=0x70)  	
    	s->ioaddr += 2;    	
    return ret;
}

void nandb_write_data16(struct nand_bflash_s *s, uint16_t value)
{
	 if ((s->cmd == 0x80) )
	 {
        if (s->iolen < s->page_oob_size)
        {
        	s->io[s->iolen ++] = value&0xff;
        	s->io[s->iolen ++] = (value>>8)&0xff;
        }
    }
}

struct nand_bflash_s *nandb_init(int manf_id, int chip_id)
{
    //int pagesize;
    struct nand_bflash_s *s;
    int index;
    int i;

    s = (struct nand_bflash_s *) qemu_mallocz(sizeof(struct nand_bflash_s));
    for (i = 0; i < sizeof(nand_flash_info); i++)
    {
        if ((nand_flash_info[i].manf_id == manf_id)
            && (nand_flash_info[i].chip_id == chip_id))
        {
            s->manf_id = manf_id;
            s->chip_id = chip_id;
            s->page_shift = nand_flash_info[i].page_shift;
            s->oob_shift = nand_flash_info[i].oob_shift;
            s->bus_width = nand_flash_info[i].bus_width;
            s->page_size = 1 << s->page_shift;
            s->oob_size = 1 << s->oob_shift;
            s->block_shift = nand_flash_info[i].block_shift;
            s->block_pages = 1 << s->block_shift;
            s->page_oob_size = s->page_size + s->oob_size;
            s->page_sectors = 1 << (s->page_shift - 9);
            /*TODO: size overflow */
            s->size = nand_flash_info[i].size << 20;
            s->pages = (s->size / s->page_size);

            break;
        }

    }
    if (i >= sizeof(nand_flash_info))
    {
        fprintf(stderr, "%s: Unsupported NAND chip ID.\n",
                  __FUNCTION__);
        exit(-1);
    }

    
    index = drive_get_index(IF_MTD, 0, 0);
    if (index != -1)
        s->bdrv = drives_table[index].bdrv;
    else
    {
    	fprintf(stderr, "%s: Please use -mtdblock to specify flash image.\n",
                  __FUNCTION__);
        exit(-1);
    }

    if (bdrv_getlength(s->bdrv) != (s->pages*s->page_oob_size))
    {
    	fprintf(stderr,  "%s: Invalid flash image size.\n",
                  __FUNCTION__);
        exit(-1);

    }

	debug_init(s);
    return s;

}

