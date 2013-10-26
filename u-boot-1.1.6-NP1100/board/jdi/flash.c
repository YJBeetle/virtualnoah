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
 * This file supports SST 39xF3201 and AMD S29GL032M-R4
 */

#define AMD_ID_GL032	0x227E227E	/* S29GL032 ID (32Mb, bottom boot sectors) */

#include <common.h>

flash_info_t	flash_info[CFG_MAX_FLASH_BANKS]; /* info for FLASH chips	*/

/*-----------------------------------------------------------------------
 * Functions
 */
static ulong flash_get_size (vu_long *addr, flash_info_t *info);

/*-----------------------------------------------------------------------
 */

ulong flash_init (void)
{
	int i, j;
	ulong size = 0;

	/* Init: no FLASHes known */
	for (i = 0; i < CFG_MAX_FLASH_BANKS; ++i) {
		flash_info[i].flash_id = FLASH_UNKNOWN;
	}

	for (i = 0; i < CFG_MAX_FLASH_BANKS; i++) {
		ulong flashbase = 0;

		if (i == 0) {
			flashbase = CFG_FLASH_BASE;
			size += flash_get_size((vu_long *)flashbase, &flash_info[0]);
		}
		else
			panic ("configured too many flash banks!\n");

		memset (flash_info[i].protect, 0, flash_info[i].sector_count);

		switch (flash_info[i].flash_id & FLASH_TYPEMASK) {
		case (AMD_ID_GL032 & FLASH_TYPEMASK):
			for (j = 0; j < flash_info[i].sector_count; j++) {
				if (j < 8) { /* 8 x 8KB bottom boot sectors */
					flash_info[i].start[j] = flashbase;
					flashbase += 0x2000;
				}
				else { /* 63 x 64KB */
					flash_info[i].start[j] = flashbase + (j-8) * 0x10000;
				}
			}
			break;
		case (SST_ID_xF3201 & FLASH_TYPEMASK):
		default:
			for (j = 0; j < flash_info[i].sector_count; j++) {
				/* 1024 * 4KB */
				flash_info[i].start[j] = flashbase + j * 0x1000;
			}
			break;
		}
	}

	/* Protect monitor and environment sectors
	 */
	flash_protect ( FLAG_PROTECT_SET,
			CFG_FLASH_BASE,
			CFG_FLASH_BASE + monitor_flash_len - 1,
			&flash_info[0]);

	flash_protect ( FLAG_PROTECT_SET,
			CFG_ENV_ADDR,
			CFG_ENV_ADDR + CFG_ENV_SIZE - 1, &flash_info[0]);

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
	case (SST_ID_xF3201 & FLASH_TYPEMASK):
		printf ("39xF3201 (32Mbit)\n");
		break;
	case (AMD_ID_GL032 & FLASH_TYPEMASK):
		printf ("29GL032 (32Mbit)\n");
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

	/* AMD Flash stuff */
	saddr[0x555] = 0xAA;
	saddr[0x2AA] = 0x55;
	saddr[0x555] = 0x90;

	vid = saddr[0];
	did = saddr[1];

	/* Exit Read IDs */
	saddr[0x555] = 0xAA;
	saddr[0x2AA] = 0x55;
	saddr[0x555] = 0xF0;

	if (vid != (AMD_MANUFACT & 0xFFFF)) {
		saddr[0x5555] = 0xAA;
		saddr[0x2AAA] = 0x55;
		saddr[0x5555] = 0x90;

		vid = saddr[0];
		did = saddr[1];

		/* Exit Read IDs */
		saddr[0x5555] = 0xAA;
		saddr[0x2AAA] = 0x55;
		saddr[0x5555] = 0xF0;

		/* SST Flash bug */
		vid = SST_MANUFACT & 0xFFFF;
		did = SST_ID_xF3201 & 0xFFFF;
	}

	switch (vid) {
	case (SST_MANUFACT & 0xFFFF):
		info->flash_id = (SST_MANUFACT & 0xFFFF0000);
		break;
	case (AMD_MANUFACT & 0xFFFF):
		info->flash_id = (AMD_MANUFACT & 0xFFFF0000);
		break;
	default:
		info->flash_id = FLASH_UNKNOWN;
		info->sector_count = 0;
		info->size = 0;
		return (0);			/* no or unknown flash	*/
	}

	switch (did) {
	case (SST_ID_xF3201 & 0xFFFF):
		info->flash_id += (SST_ID_xF3201 & 0xFFFF);
		info->sector_count = 1024;
		info->size = 0x00400000;
		break;
	case (AMD_ID_GL032 & 0xFFFF):
		info->flash_id += (AMD_ID_GL032 & 0xFFFF);
		info->sector_count = 71; /* 8 x 8KB + 63 x 64KB */
		info->size = 0x00400000;
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
	volatile u16 *base = (volatile u16 *) CFG_FLASH_BASE;
	int flag, prot, sect;
	int rc = ERR_OK;
	unsigned int timeout;

	if (info->flash_id == FLASH_UNKNOWN)
		return ERR_UNKNOWN_FLASH_TYPE;

	if ((s_first < 0) || (s_first > s_last)) {
		return ERR_INVAL;
	}
#if 0
	if ((info->flash_id & FLASH_VENDMASK) !=
		(SST_MANUFACT & FLASH_VENDMASK)) {
		return ERR_UNKNOWN_FLASH_VENDOR;
	}
#endif

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

			switch (info->flash_id & FLASH_TYPEMASK) {
			case (SST_ID_xF3201 & FLASH_TYPEMASK):
				base[0x5555] = 0x00AA;
				base[0x2AAA] = 0x0055;
				base[0x5555] = 0x0080;
				base[0x5555] = 0x00AA;
				base[0x2AAA] = 0x0055;
				addr[0x0000] = 0x0030;
				break;
			case (AMD_ID_GL032 & FLASH_TYPEMASK):
				base[0x555] = 0x00AA;
				base[0x2AA] = 0x0055;
				base[0x555] = 0x0080;
				base[0x555] = 0x00AA;
				base[0x2AA] = 0x0055;
				addr[0x000] = 0x0030;
				break;
			default:
				printf ("Unknown Chip Type\n");
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
	volatile u16 *base = (volatile u16 *) CFG_FLASH_BASE;
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
	switch (info->flash_id & FLASH_TYPEMASK) {
	case (SST_ID_xF3201 & FLASH_TYPEMASK):
		base[0x5555] = 0x00AA;
		base[0x2AAA] = 0x0055;
		base[0x5555] = 0x00A0;
		break;
	case (AMD_ID_GL032 & FLASH_TYPEMASK):
		base[0x555] = 0x00AA;
		base[0x2AA] = 0x0055;
		base[0x555] = 0x00A0;
		break;
	default:
		printf ("Unknown Chip Type\n");
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
