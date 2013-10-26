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

#define CH_RTC 3
#define CH_OST 15
#define CH_EXT 4
#if 0
int do_check_rtc ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i = 0, number;
	unsigned short result[100];

	REG_TCU_TCSR(CH_RTC) = TCU_TCSR_RTC_EN;
	REG_TCU_TDHR(CH_RTC) = 0xffff;
	REG_TCU_TDFR(CH_RTC) = 0xffff;

	REG_TCU_TSCR = (1 << CH_RTC); /* enable timer clock */

	REG_TCU_TCSR(CH_EXT) = TCU_TCSR_EXT_EN;
	REG_TCU_TDHR(CH_EXT) = 0xffff;
	REG_TCU_TDFR(CH_EXT) = 0xffff;

	REG_TCU_TSCR = (1 << CH_EXT); /* enable timer clock */
	
	REG_TCU_TESR = ((1 << CH_RTC) | (1 << CH_EXT)); /* start counting up */

	
	if(argc < 2) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return -1;
	}

	number = (unsigned int)simple_strtoul(argv[1], NULL, 10);

	while(0) {
		REG_TCU_TCNT(CH_RTC) = 0;
		for(i = 0; i < 100; i++) {
			REG_TCU_TCNT(CH_EXT) = 0;
			while(REG_TCU_TCNT(CH_RTC) < ((i + 1) * number));
			result[i] = REG_TCU_TCNT(CH_EXT);
		}

		for(i = 0; i < 100; i++)
			printf("result[%d] = %d\n", i, result[i]);
	
		printf("press 'c' to continue, or exit\n");
		if(getc() == 'c')
			continue;
		else
			return 0;
	}

	REG_TCU_TCNT(CH_RTC) = 0;
	REG_TCU_TCNT(CH_EXT) = 0;
		
	while(REG_TCU_TCNT(CH_RTC) < (number * 60 * 60 * (64 << 10))) {
		
		while(REG_TCU_TCNT(CH_EXT) == (64 << 10));		

		full_counter++;
	}

	last_time = REG_TCU_TCNT(CH_EXT);
	rtc_result = REG_TCU_TCNT(CH_RTC);

	printf("tcu_result = %d\n, rtc_result = %d\n", full_counter * (64 << 10) + tcu_last_time, rtc_result);

	return 0;
}
#endif
int do_check_rtc ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i = 0, number;
	unsigned int tcu_last_time, rtc_result, full_counter = 0;

	REG_OST_CSR = OST_CSR_RTC_EN;
	REG_OST_DR = 0xffffffff;

	REG_TCU_TCSR(CH_EXT) = TCU_TCSR_EXT_EN;
	REG_TCU_TDHR(CH_EXT) = 0xffff;
	REG_TCU_TDFR(CH_EXT) = 0xffff;

	REG_TCU_TSCR = ((1 << CH_OST) | (1 << CH_EXT)); /* enable timer clock */
	REG_TCU_TESR = ((1 << CH_OST) | (1 << CH_EXT)); /* start counting up */

	if(argc < 2) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return -1;
	}

	number = (unsigned int)simple_strtoul(argv[1], NULL, 10);

	REG_OST_CNT = 0;
	REG_TCU_TCNT(CH_EXT) = 0;
		
	// 32.768KHz
	while(REG_OST_CNT < (number * 60 * 60 * ((32 << 10) - 1))) {
		
		while(REG_TCU_TCNT(CH_EXT) == ((64 << 10) - 1));   // 16-bit		

		full_counter++;
	}

	REG_TCU_TECR = ((1 << CH_OST) | (1 << CH_EXT)); /* stop counting up */
	REG_TCU_TSSR = ((1 << CH_OST) | (1 << CH_EXT)); /* disable timer clock */

	tcu_last_time = REG_TCU_TCNT(CH_EXT);
	rtc_result = REG_OST_CNT;

	printf("tcu_result = %d(S)\n, rtc_result = %d(S)\n", full_counter * (64 << 10) + tcu_last_time / (12 << 20), rtc_result / (32 << 10));

	return 0;
}


/***************************************************/

U_BOOT_CMD(
 	check_rtc,	3,	1,	do_check_rtc,
	"check_rtc     - check precision of rtc\n",
	"..."
);

