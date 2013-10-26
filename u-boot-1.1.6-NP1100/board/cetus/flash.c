/*
 * (C) Copyright 2005 Ingenic Semiconductor
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
/*
 * This file supports SST 16-bit Flashes:
 * 39xF1601, 39xF1602, 39xF3201, 39xF3202, 39xF6401, 39xF6402,
 * 39xF6401B, 39xF6402B.
 */

#include <common.h>

#define SST_ID_xF6401B	0x236D236D	/* 39xF6401B ID (64M =	4M x 16 )	*/
#define SST_ID_xF6402B	0x236C236C	/* 39xF6402B ID (64M =	4M x 16 )	*/
#define AMD_ID_DL640	0x227E227E	/* 29DL640D ID (64 M, dual boot sectors)*/

#define AMD_SECT_SIZE     0x10000
#define SST_SECT_SIZE     0x1000

flash_info_t flash_info[CFG_MAX_FLASH_BANKS]; /* info for FLASH chips	*/
static ulong flash_base = 0xa8000000;    /* base address of CS4 */

/*-----------------------------------------------------------------------
 * Functions
 */
static ulong flash_get_size (vu_long *addr, flash_info_t *info);

/*-----------------------------------------------------------------------
 */
ulong flash_init(void)
{
	int i, j;
	ulong size = 0;

	/* Init: no FLASHes known */
	for (i = 0; i < CFG_MAX_FLASH_BANKS; ++i) {
		flash_info[i].flash_id = FLASH_UNKNOWN;
	}

	for (i = 0; i < CFG_MAX_FLASH_BANKS; i++) {
		if (i == 0) {
			size += flash_get_size((vu_long *)flash_base, &flash_info[0]);
		}
		else
			panic("configured too many flash banks!\n");
		memset (flash_info[i].protect, 0, flash_info[i].sector_count);

 		switch (flash_info[i].flash_id & FLASH_VENDMASK) {
 		case (AMD_MANUFACT & FLASH_VENDMASK):
			for (j = 0; j < flash_info[i].sector_count; j++) {
#if 1
				if (j < 8) {     // 8 x 8k boot sectors	
					flash_info[i].start[j] = flash_base + j * 0x2000;
				}
				else {
					flash_info[i].start[j] = flash_base + (j-7) * AMD_SECT_SIZE;
				}
#else
				flash_info[i].start[j] = flash_base + j * AMD_SECT_SIZE;
#endif
					
			}
			break;

 		case (SST_MANUFACT & FLASH_VENDMASK): 
			for (j = 0; j < flash_info[i].sector_count; j++) { 
				flash_info[i].start[j] = flash_base + j * SST_SECT_SIZE;
			}
			break;

		default:
			break;
  		}
 	} 

#ifdef CFG_ENV_IS_IN_FLASH
	/* Protect monitor and environment sectors
	 */
	flash_protect ( FLAG_PROTECT_SET,
			CFG_MONITOR_BASE,
			CFG_MONITOR_BASE + monitor_flash_len - 1,
			&flash_info[0]);

	flash_protect ( FLAG_PROTECT_SET,
			CFG_ENV_ADDR,
			CFG_ENV_ADDR + CFG_ENV_SIZE - 1, &flash_info[0]);
#endif

	return size;
}


/*-----------------------------------------------------------------------
 */
void flash_print_info (flash_info_t * info)
{
	int i;

	switch (info->flash_id & FLASH_VENDMASK) {
	case (SST_MANUFACT & FLASH_VENDMASK):
		printf ("SST ");
		break;
	case (AMD_MANUFACT & FLASH_VENDMASK):
		printf ("AMD ");
		break;
	default:
		printf ("Unknown Vendor ");
		break;
	}

	switch (info->flash_id & FLASH_TYPEMASK) {
	case (AMD_ID_DL640 & FLASH_TYPEMASK):
		printf ("29DL640D (64Mbit)\n");
		break;
	case (AMD_ID_DL640G_2 & FLASH_TYPEMASK):
		printf ("AM29DL640G2 (16Mbit)\n");
		break;
	case (SST_ID_xF1601 & FLASH_TYPEMASK):
		printf ("39xF1601 (16Mbit)\n");
		break;
	case (SST_ID_xF1602 & FLASH_TYPEMASK):
		printf ("39xF1602 (16Mbit)\n");
		break;
	case (SST_ID_xF3201 & FLASH_TYPEMASK):
		printf ("39xF3201 (32Mbit)\n");
		break;
	case (SST_ID_xF3202 & FLASH_TYPEMASK):
		printf ("39xF3202 (32Mbit)\n");
		break;
	case (SST_ID_xF6401 & FLASH_TYPEMASK):
		printf ("39xF6401 (64Mbit)\n");
		break;
	case (SST_ID_xF6402 & FLASH_TYPEMASK):
		printf ("39xF6402 (64Mbit)\n");
		break;
	case (SST_ID_xF6401B & FLASH_TYPEMASK):
		printf ("39xF6401B (64Mbit)\n");
		break;
	case (SST_ID_xF6402B & FLASH_TYPEMASK):
		printf ("39xF6402B (64Mbit)\n");
		break;
	default:
		printf ("Unknown Chip Type\n");
		break;
	}

	printf ("  Size: %ld MB in %d Sectors\n",
			info->size >> 20, info->sector_count);

	printf ("  Sector Start Addresses:");
	for (i = 0; i < info->sector_count; i++) {
		if ((i % 5) == 0) {
			printf ("\n   ");
		}
		printf (" %08lX%s", info->start[i],
				info->protect[i] ? " (RO)" : "     ");
	}
	printf ("\n");
}

/*-----------------------------------------------------------------------
 */

static ulong flash_get_size (vu_long *addr, flash_info_t *info)
{
	ushort vid, did;
	vu_short *saddr = (vu_short *)addr;

	/* Read Manufacturer ID and Device ID */

	saddr[0x5555] = 0xAA;
	saddr[0x2AAA] = 0x55;
	saddr[0x5555] = 0x90;

	vid = saddr[0];
	did = saddr[1];

	/* Exit Read IDs */
	saddr[0x5555] = 0xAA;
	saddr[0x2AAA] = 0x55;
	saddr[0x5555] = 0xF0;

	if (vid != (SST_MANUFACT & 0xFFFF)) {
		/* 39xF640xB stuff */
		saddr[0x555] = 0xAA;
		saddr[0x2AA] = 0x55;
		saddr[0x555] = 0x90;

		vid = saddr[0];
		did = saddr[1];

		/* Exit Read IDs */
		saddr[0x555] = 0xAA;
		saddr[0x2AA] = 0x55;
		saddr[0x555] = 0xF0;
	}

	switch (vid) {
	case (AMD_MANUFACT & 0xFFFF):
		info->flash_id = (AMD_MANUFACT & 0xFFFF0000);
       		break;
	case (SST_MANUFACT & 0xFFFF):
		info->flash_id = (SST_MANUFACT & 0xFFFF0000);
		break;
	default:
		info->flash_id = FLASH_UNKNOWN;
		info->sector_count = 0;
		info->size = 0;
		return (0);			/* no or unknown flash	*/
	}

	switch (did) {
	case (AMD_ID_DL640 & 0xFFFF):
		info->flash_id += (AMD_ID_DL640 & 0xFFFF); 
		info->sector_count = 135;
		info->size = 0x00800000;
		break;
	case (AMD_ID_DL640G_2 & 0xFFFF):
		info->flash_id += (AMD_ID_DL640G_2 & 0xFFFF);
		info->sector_count = 128;
		info->size = 0x00800000;
		break;
	case (SST_ID_xF1601 & 0xFFFF):
		info->flash_id += (SST_ID_xF1601 & 0xFFFF);
		info->sector_count = 512;
		info->size = 0x00200000;
		break;
	case (SST_ID_xF1602 & 0xFFFF):
		info->flash_id += (SST_ID_xF1602 & 0xFFFF);
		info->sector_count = 512;
		info->size = 0x00200000;
		break;
	case (SST_ID_xF3201 & 0xFFFF):
		info->flash_id += (SST_ID_xF3201 & 0xFFFF);
		info->sector_count = 1024;
		info->size = 0x00400000;
		break;
	case (SST_ID_xF3202 & 0xFFFF):
		info->flash_id += (SST_ID_xF3202 & 0xFFFF);
		info->sector_count = 1024;
		info->size = 0x00400000;
		break;
	case (SST_ID_xF6401 & 0xFFFF):
		info->flash_id += (SST_ID_xF6401 & 0xFFFF);
		info->sector_count = 2048;
		info->size = 0x00800000;
		break;
	case (SST_ID_xF6402 & 0xFFFF):
		info->flash_id += (SST_ID_xF6402 & 0xFFFF);
		info->sector_count = 2048;
		info->size = 0x00800000;
		break;
	case (SST_ID_xF6401B & 0xFFFF):
		info->flash_id += (SST_ID_xF6401B & 0xFFFF);
		info->sector_count = 2048;
		info->size = 0x00800000;
		break;
	case (SST_ID_xF6402B & 0xFFFF):
		info->flash_id += (SST_ID_xF6402B & 0xFFFF);
		info->sector_count = 2048;
		info->size = 0x00800000;
		break;
	default:
		info->flash_id = FLASH_UNKNOWN;
		info->sector_count = 0;
		info->size = 0;
		return (0);			/* no or unknown flash	*/
	}

	return (info->size);
}

/*-----------------------------------------------------------------------
 */

int flash_erase (flash_info_t * info, int s_first, int s_last)
{
	volatile u16 *base = (volatile u16 *) flash_base;
	int flag, prot, sect;
	int rc = ERR_OK;
	unsigned int timeout;

	if (info->flash_id == FLASH_UNKNOWN)
		return ERR_UNKNOWN_FLASH_TYPE;

	if ((s_first < 0) || (s_first > s_last)) {
		return ERR_INVAL;
	}

	if ((info->flash_id & FLASH_VENDMASK) !=
	    (SST_MANUFACT & FLASH_VENDMASK) &&
	    (info->flash_id & FLASH_VENDMASK) !=
	    (AMD_MANUFACT & FLASH_VENDMASK)) {
		return ERR_UNKNOWN_FLASH_VENDOR;
	}

	prot = 0;
	for (sect = s_first; sect <= s_last; ++sect) {
		if (info->protect[sect]) {
			prot++;
		}
	}
	if (prot)
		return ERR_PROTECTED;

	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts ();

	/* Start erase on unprotected sectors */
	for (sect = s_first; sect <= s_last && !ctrlc (); sect++) {

		printf ("Erasing sector %3d ... ", sect);

		if (info->protect[sect] == 0) {	/* not protected */
			volatile u16 *addr = (volatile u16 *) (info->start[sect]);
			switch (info->flash_id & FLASH_VENDMASK) {
			case (SST_MANUFACT & FLASH_VENDMASK):
				if (((info->flash_id & FLASH_TYPEMASK) == 
				     (SST_ID_xF6401B & FLASH_TYPEMASK)) || 
				    ((info->flash_id & FLASH_TYPEMASK) == 
				     (SST_ID_xF6402B & FLASH_TYPEMASK)))
				{
					base[0x555] = 0x00AA;
					base[0x2AA] = 0x0055;
					base[0x555] = 0x0080;
					base[0x555] = 0x00AA;
					base[0x2AA] = 0x0055;
					addr[0x000] = 0x0030;
				}
				else {
					base[0x5555] = 0x00AA;
					base[0x2AAA] = 0x0055;
					base[0x5555] = 0x0080;
					base[0x5555] = 0x00AA;
					base[0x2AAA] = 0x0055;
					addr[0x0000] = 0x0030;
				}
				break;
			case (AMD_MANUFACT & FLASH_VENDMASK):
				if (((info->flash_id & FLASH_TYPEMASK) == 
				     (AMD_ID_DL640 & FLASH_TYPEMASK))  
					)
				{
					base[0x555] = 0x00AA;
					base[0x2AA] = 0x0055;
					base[0x555] = 0x0080;
					base[0x555] = 0x00AA;
					base[0x2AA] = 0x0055;
					addr[0x000] = 0x0030;
				}
				else {
					base[0x5555] = 0x00AA;
					base[0x2AAA] = 0x0055;
					base[0x5555] = 0x0080;
					base[0x5555] = 0x00AA;
					base[0x2AAA] = 0x0055;
					addr[0x0000] = 0x0030;
				}
				break;
			}

			timeout = 0x3ffffff;
			while ((*base & 0x40) != (*base & 0x40)) {
				if ((timeout--) == 0) {
					rc = ERR_TIMOUT;
					goto outahere;
				}
			}
		}
		printf ("ok.\n");
	}
	if (ctrlc ())
		printf ("User Interrupt!\n");

  outahere:

	/* allow flash to settle - wait 20 ms */
	udelay(20000);

	if (flag)
		enable_interrupts ();

	return rc;
}

/*-----------------------------------------------------------------------
 * Copy memory to flash
 */

static int write_word (flash_info_t * info, ulong dest, ushort data)
{
	volatile u16 *base = (volatile u16 *) flash_base;
	volatile u16 *addr = (volatile u16 *) dest;
	int rc = ERR_OK;
	int flag;
	unsigned int timeout;

	/* Check if Flash is (sufficiently) erased
	 */
	if ((*addr & data) != data)
		return ERR_NOT_ERASED;

	/* Disable interrupts which might cause a timeout here. */
	flag = disable_interrupts ();

	/* program set-up command */
	switch (info->flash_id & FLASH_VENDMASK) {
	case (SST_MANUFACT & FLASH_VENDMASK):
		if (((info->flash_id & FLASH_TYPEMASK) == 
		     (SST_ID_xF6401B & FLASH_TYPEMASK)) || 
		    ((info->flash_id & FLASH_TYPEMASK) == 
		     (SST_ID_xF6402B & FLASH_TYPEMASK)))
		{
			base[0x555] = 0x00AA;
			base[0x2AA] = 0x0055;
			base[0x555] = 0x00A0;
		}
		else {
			base[0x5555] = 0x00AA;
			base[0x2AAA] = 0x0055;
			base[0x5555] = 0x00A0;
		}
		break;
	case (AMD_MANUFACT & FLASH_VENDMASK):
		if ((info->flash_id & FLASH_TYPEMASK) == 
		    (AMD_ID_DL640 & FLASH_TYPEMASK)) 
		{
			base[0x555] = 0x00AA;
			base[0x2AA] = 0x0055;
			base[0x555] = 0x00A0; 
		}
		else {
			base[0x5555] = 0x00AA;
			base[0x2AAA] = 0x0055;
			base[0x5555] = 0x00A0;
		}
		break;
	}

	/* load address/data */
	*addr = data;

	/* wait while polling the status register */
	timeout = 0x3ffffff;
	while ((*base & 0x40) != (*base & 0x40)) {
		if ((timeout--) == 0) {
			rc = ERR_TIMOUT;
			goto outahere;
		}
	}

  outahere:

	/* allow flash to settle - wait 10 us */
	udelay(10);

	if (flag)
		enable_interrupts ();

	return rc;
}

/*-----------------------------------------------------------------------
 * Copy memory to flash.
 */

int write_buff (flash_info_t * info, uchar * src, ulong addr, ulong cnt)
{
	ulong cp, wp;
	ushort data;
	int l;
	int i, rc;

	wp = (addr & ~1);			/* get lower word aligned address */

	/*
	 * handle unaligned start bytes
	 */
	if ((l = addr - wp) != 0) {
		data = 0;
		for (i = 0, cp = wp; i < l; ++i, ++cp) {
			data = (data >> 8) | (*(uchar *) cp << 8);
		}
		for (; i < 2 && cnt > 0; ++i) {
			data = (data >> 8) | (*src++ << 8);
			--cnt;
			++cp;
		}
		for (; cnt == 0 && i < 2; ++i, ++cp) {
			data = (data >> 8) | (*(uchar *) cp << 8);
		}

		if ((rc = write_word (info, wp, data)) != 0) {
			return (rc);
		}
		wp += 2;
	}

	/*
	 * handle word aligned part
	 */
	while (cnt >= 2) {
		data = *((vu_short *) src);
		if ((rc = write_word (info, wp, data)) != 0) {
			return (rc);
		}
		src += 2;
		wp += 2;
		cnt -= 2;
	}

	if (cnt == 0) {
		return ERR_OK;
	}

	/*
	 * handle unaligned tail bytes
	 */
	data = 0;
	for (i = 0, cp = wp; i < 2 && cnt > 0; ++i, ++cp) {
		data = (data >> 8) | (*src++ << 8);
		--cnt;
	}
	for (; i < 2; ++i, ++cp) {
		data = (data >> 8) | (*(uchar *) cp << 8);
	}

	return write_word (info, wp, data);
}
