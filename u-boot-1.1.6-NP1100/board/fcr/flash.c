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

/*
 * SST39VF020: 4KB * 64 = 256KB
 */
#define FLASH_BANK_SIZE 0x40000
#define MAIN_SECT_SIZE  0x1000

#define SST_ID_xF020	0xBFD6BFD6	/* 39xF020 ID */

flash_info_t    flash_info[CFG_MAX_FLASH_BANKS];


/*-----------------------------------------------------------------------
 */

ulong flash_init (void)
{
	int i, j;
	ulong size = 0;

	for (i = 0; i < CFG_MAX_FLASH_BANKS; i++) {
		ulong flashbase = 0;

		flash_info[i].flash_id =
				(SST_MANUFACT & FLASH_VENDMASK) |
				(SST_ID_xF020 & FLASH_TYPEMASK);
		flash_info[i].size = FLASH_BANK_SIZE;
		flash_info[i].sector_count = CFG_MAX_FLASH_SECT;
		memset (flash_info[i].protect, 0, CFG_MAX_FLASH_SECT);
		if (i == 0)
			flashbase = PHYS_FLASH_1;
		else
			panic ("configured too many flash banks!\n");
		for (j = 0; j < flash_info[i].sector_count; j++) {
			flash_info[i].start[j] = flashbase + j * MAIN_SECT_SIZE;
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
	case (SST_MANUFACT & FLASH_VENDMASK):
		printf ("SST: ");
		break;
	default:
		printf ("Unknown Vendor ");
		break;
	}

	switch (info->flash_id & FLASH_TYPEMASK) {
	case (SST_ID_xF020 & FLASH_TYPEMASK):
		printf ("39VF020 (256KB)\n");
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
				info->protect[i] ? "(RO) " : "     ");
	}
	printf ("\n");
}

/*-----------------------------------------------------------------------
 */

int flash_erase (flash_info_t * info, int s_first, int s_last)
{
	volatile u8 *base = (volatile u8 *) CFG_FLASH_BASE;
	int flag, prot, sect;
	int rc = ERR_OK;
	unsigned int timeout;

	if (info->flash_id == FLASH_UNKNOWN)
		return ERR_UNKNOWN_FLASH_TYPE;

	if ((s_first < 0) || (s_first > s_last)) {
		return ERR_INVAL;
	}

	if ((info->flash_id & FLASH_VENDMASK) !=
		(SST_MANUFACT & FLASH_VENDMASK)) {
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
			volatile u8 *addr = (volatile u8 *) (info->start[sect]);

			*(base + 0x5555) = 0xAA;
			*(base + 0x2AAA) = 0x55;
			*(base + 0x5555) = 0x80;
			*(base + 0x5555) = 0xAA;
			*(base + 0x2AAA) = 0x55;

			*addr = 0x30;

			timeout = 1000000;
			while (((*addr & 0x80) != 0x80) && timeout--);
			if (!timeout) {
				rc = ERR_TIMOUT;
				goto outahere;
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

static int write_word (flash_info_t * info, ulong dest, unchar data)
{
	volatile u8 *base = (volatile u8 *) CFG_FLASH_BASE;
	volatile u8 *addr = (volatile u8 *) dest;
	int rc = ERR_OK;
	int flag;
	unsigned int timeout;

	/* Check if Flash is (sufficiently) erased
	 */
	if ((*addr & data) != data) {
		return ERR_NOT_ERASED;
	}

	/* Disable interrupts which might cause a timeout here. */
	flag = disable_interrupts ();

	/* program set-up command */
	*(base + 0x5555) = 0xAA;
	*(base + 0x2AAA) = 0x55;
	*(base + 0x5555) = 0xA0;

	/* load address/data */
	*addr = data;

	timeout = 500000;
	while (((*addr & 0x80) != 0x80) && timeout--);
	if (!timeout) {
		rc = ERR_TIMOUT;
		goto outahere;
	}

  outahere:

	/* allow flash to settle - wait 20 us */
	timeout = 10000;
	while (timeout--);

	if (flag)
		enable_interrupts ();

	return rc;
}

/*-----------------------------------------------------------------------
 * Copy memory to flash.
 */

int write_buff (flash_info_t * info, unchar * src, ulong addr, ulong cnt)
{
	unchar data;
	int rc;

	while (cnt > 0) {
		data = *((volatile u8 *) src);
		if ((rc = write_word (info, addr, data)) != 0) {
			return (rc);
		}
		src ++;
		addr++;
		cnt --;
	}

	return ERR_OK;
}
