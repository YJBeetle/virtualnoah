/*
 * (C) Copyright 2004
 * Jian Zhang, Texas Instruments, jzhang@ti.com.

 * (C) Copyright 2000-2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>

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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* #define DEBUG */

#include <common.h>

#if defined(CFG_ENV_IS_IN_MSC) /* Environment is in Nand Flash */

#include <command.h>
#include <environment.h>
#include <linux/stddef.h>
#include <malloc.h>

#if ((CONFIG_COMMANDS&(CFG_CMD_ENV|CFG_CMD_MSC)) == (CFG_CMD_ENV|CFG_CMD_MSC))
#define CMD_SAVEENV
#elif defined(CFG_ENV_OFFSET_REDUND)
#error Cannot use CFG_ENV_OFFSET_REDUND without CFG_CMD_ENV & CFG_CMD_NAND
#endif

#if defined(CFG_ENV_SIZE_REDUND) && (CFG_ENV_SIZE_REDUND != CFG_ENV_SIZE)
#error CFG_ENV_SIZE_REDUND should be the same as CFG_ENV_SIZE
#endif

#ifdef CONFIG_INFERNO
#error CONFIG_INFERNO not supported yet
#endif

/* references to names in env_common.c */
extern uchar default_environment[];
extern int default_environment_size;

char * env_name_spec = "MSC";

#undef ENV_IS_EMBEDDED
#ifdef ENV_IS_EMBEDDED
extern uchar environment[];
env_t *env_ptr = (env_t *)(&environment[0]);
#else /* ! ENV_IS_EMBEDDED */
env_t *env_ptr = 0;
#endif /* ENV_IS_EMBEDDED */

extern int msc_read(ulong start_byte, u8 *dst, size_t len);
extern int msc_write(ulong start_byte, u8 *dst, size_t len);

/* local functions */
#if !defined(ENV_IS_EMBEDDED)
static void use_default(void);
#endif

DECLARE_GLOBAL_DATA_PTR;

uchar env_get_char_spec (int index)
{
	return ( *((uchar *)(gd->env_addr + index)) );
}

/* this is called before u-boot starts
 * so we can't read MMC/SD to validate env data.
 * Mark it OK for now. env_relocate() in env_common.c
 * will call our relocate function which will does
 * the real validation.
 *
 * When using a MMC/SD boot image , the environment
 * can be embedded or attached to the U-Boot image in MMC/SD card. This way
 * the SPL loads not only the U-Boot image from MMC/SD card but also the
 * environment.
 */
int env_init(void)
{
#if defined(ENV_IS_EMBEDDED)
	ulong total;
	int crc1_ok = 0;
	env_t *tmp_env1;

	total = CFG_ENV_SIZE;

	tmp_env1 = env_ptr;
	crc1_ok = (crc32(0, tmp_env1->data, ENV_SIZE) == tmp_env1->crc);

	if (!crc1_ok)
		gd->env_valid = 0;
	else 
		gd->env_valid = 1;

	if (gd->env_valid == 1)
		env_ptr = tmp_env1;
#else /* ENV_IS_EMBEDDED */
	gd->env_addr  = (ulong)&default_environment[0];
	gd->env_valid = 1;
#endif /* ENV_IS_EMBEDDED */
	return (0);
}

#ifdef CMD_SAVEENV
/*
 * The legacy NAND code saved the environment in the first NAND device i.e.,
 * nand_dev_desc + 0. This is also the behaviour using the new NAND code.
 */
int saveenv(void)
{
	ulong total;
	int ret = 0;

	total = CFG_ENV_SIZE;

	ret = msc_write(CFG_ENV_OFFSET, (u_char*)env_ptr, total);
	return ret;
}
#endif /* CMD_SAVEENV */

void env_relocate_spec (void)
{
#if !defined(ENV_IS_EMBEDDED)
	ulong total;
	int crc1_ok = 0;
	env_t *tmp_env1;

	total = CFG_ENV_SIZE;
	tmp_env1 = (env_t *) malloc(CFG_ENV_SIZE);
	msc_read(CFG_ENV_OFFSET, (u_char*) tmp_env1, total);

	crc1_ok = (crc32(0, tmp_env1->data, ENV_SIZE) == tmp_env1->crc);

	if(!crc1_ok)
		return use_default();

	else if(crc1_ok)
		gd->env_valid = 1;

	if(gd->env_valid == 1) {
		env_ptr = tmp_env1;
	}
#endif /* ! ENV_IS_EMBEDDED */
}

#if !defined(ENV_IS_EMBEDDED)
static void use_default()
{
	puts ("*** Warning - MMC/SD first load, using default environment\n\n");

	if (default_environment_size > CFG_ENV_SIZE){
		puts ("*** Error - default environment is too large\n\n");
		return;
	}

	memset (env_ptr, 0, sizeof(env_t));
	memcpy (env_ptr->data,
			default_environment,
			default_environment_size);
	env_ptr->crc = crc32(0, env_ptr->data, ENV_SIZE);
	gd->env_valid = 1;
}
#endif
#endif /* CFG_ENV_IS_IN_MSC */
