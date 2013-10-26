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

#include <common.h>

flash_info_t    flash_info[CFG_MAX_FLASH_BANKS];

/*-----------------------------------------------------------------------
 */

ulong flash_init (void)
{
	int i, j;
	ulong size = 0;

	for (i = 0; i < CFG_MAX_FLASH_BANKS; i++) {
		ulong flashbase = 0;

		if (i == 0)
			flashbase = PHYS_FLASH_1;
		else
			panic ("configured too many flash banks!\n");

		flash_info[i].flash_id =
			(AMD_MANUFACT & FLASH_VENDMASK) |
			(AMD_ID_LV320B & FLASH_TYPEMASK);
		flash_info[i].size = 0x00400000;
		flash_info[i].sector_count = CFG_MAX_FLASH_SECT;
		memset (flash_info[i].protect, 0, CFG_MAX_FLASH_SECT);
		for (j = 0; j < 8; j++) {
			flash_info[i].start[j] = flashbase + j * 0x2000;
		}
		flashbase = flashbase + 8 * 0x2000;
		for (j = 8; j < CFG_MAX_FLASH_SECT; j++) {
			flash_info[i].start[j] = flashbase + (j-8) * 0x10000;
		}
		size += flash_info[i].size;
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
	case (AMD_MANUFACT & FLASH_VENDMASK):
		printf ("AMD: ");
		break;
	default:
		printf ("Unknown Vendor ");
		break;
	}

	switch (info->flash_id & FLASH_TYPEMASK) {
	case (AMD_ID_LV320B & FLASH_TYPEMASK):
		printf ("AMLV320DB (32Mbit)\n");
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

	if ((info->flash_id & FLASH_VENDMASK) !=
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

			base[0x555] = 0x00AA;
			base[0x2AA] = 0x0055;
			base[0x555] = 0x0080;
			base[0x555] = 0x00AA;
			base[0x2AA] = 0x0055;
			addr[0x000] = 0x0030;

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
	base[0x555] = 0x00AA;
	base[0x2AA] = 0x0055;
	base[0x555] = 0x00A0;

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
