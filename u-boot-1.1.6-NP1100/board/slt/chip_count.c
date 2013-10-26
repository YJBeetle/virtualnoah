/*
 *  Jz4730 System Level Test
 *
 *  Copyright (c) 2007
 *  Ingenic Semiconductor, <lgwang@ingenic.cn>
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

#if defined(CONFIG_JZ4730)
#include <asm/jz4730.h>
#elif defined(CONFIG_JZ4740)
#include <asm/jz4740.h>
#else
#error "chip type not defined."
#endif

#if defined(CFG_CHIP_COUNT)


//#define CHIP_COUNT_DEBUG 1
#undef CHIP_COUNT_DEBUG

#define CFG_EEPROM_DATA_BACKUP_ENABLE 		1

#if defined(CFG_EEPROM_AT24C02A)		/* Old board: 256 bytes */

#define eeprom_size     256	/* AT24C02A: 256byte, AT24C16A: 2048byte */
#define section_size    64	/* 32, 64, 128, 256 bytes */
#define section_num     (eeprom_size/section_size) /* 4,8,16 */
#define cpu_clk_type    (section_num/2) 
#define page_size       8 //bytes
#define check_max       60000

#elif defined(CFG_EEPROM_AT24C16A)		/* Old board: 256 bytes */
#define eeprom_size     2048	/* AT24C02A: 256byte, AT24C16A: 2048byte */
#define section_size    64	/* 32, 64, 128, 256 bytes */
#define section_num     (eeprom_size/section_size) /* 4,8,16 */
#define cpu_clk_type    (section_num/2) 
#define page_size       16 //bytes
#define check_max       60000

#else
#error "unknown eeprom type"
#endif

#if defined(CHIP_COUNT_DEBUG)
#undef check_max
#define check_max       6
#endif

struct section_info {
	unsigned short cpuclk;	/* 2bytes */
	unsigned char sum_offset; /* 1byte */
	unsigned char pass_offset; /* 1byte */
	unsigned short summary;	/* 2bytes */
	unsigned short pass;	/* 2bytes */
};


extern void i2c_open(void);
extern void i2c_close(void);
extern void i2c_setclk(unsigned int i2cclk);
extern int i2c_read(unsigned char device, unsigned char *buf,
		    unsigned char address, int count);
extern int i2c_write(unsigned char device, unsigned char *buf,
		    unsigned char address, int count);
extern inline int get_cpu_speed(void);

/* used for stage: nandboot -- new pll_init , cpu speed is 100MHz. */
static inline void my_udelay(int n )
{
	int i;
	i=n*100;
	while ( i-- )
		;
}

static inline void mdelay ( int n )
{
	while ( n-- ) 
		my_udelay(1000);
}


static void led_dancing( void )
{
	int i;
	while (1) {
		for ( i=0; i<8; i++ ) 
			__gpio_set_pin(i);
		mdelay(500);
		for ( i=0; i<8; i++ ) 
			__gpio_clear_pin(i);
		mdelay(500);
	}
}


unsigned short encrypt(unsigned short org)
{
	unsigned short an_val, pos21, pos65, posa9, posed;

	an_val = ~org;
	pos21 = an_val & 0x6;
	pos65 = an_val & 0x60;
	posa9 = an_val & 0x600;
	posed = an_val & 0x6000;
	an_val = an_val & 0x9999;
	an_val = an_val | (pos21 << 12) | (pos65 << 4) | (posa9 >> 4) | (posed >> 12);

	return(an_val);
}

unsigned short decrypt(unsigned short an_val)
{
	unsigned short org, pos21, pos65, posa9, posed;

	pos21 = an_val & 0x6000;pos21 = pos21 >> 12;
	pos65 = an_val & 0x600;pos65 = pos65 >> 4;
	posa9 = an_val & 0x60;posa9 = posa9 << 4;
	posed = an_val & 0x6;posed = posed << 12;
	an_val = an_val & 0x9999;
	an_val = an_val | (pos21) | (pos65) | (posa9) | (posed);
	org = ~an_val;

	return(org);
}


static struct section_info section_info[1];
/* 
 * chip count increase 1
 * NOTES: calculate deviceid before i2c_read or i2c_write.
 */
int chip_count( void )
{
	struct section_info *p_section_info = &section_info[0];
	int section_cnt = 0;
	int section_base_addr = 0;
	int buf_size = section_size;
	unsigned char buf[buf_size];
	int i, ret;
	unsigned short cpu_speed_test, cpu_speed_now;
	unsigned short summary_backup;
	
	memset(buf, 0, buf_size);
#if defined(CFG_EEPROM_AT24C02A)
	int device_id = (0xA<<3) | 7;
#elif defined(CFG_EEPROM_AT24C16A)
	int device_id = (0xA<<3) | 0;
#else
#error ""
#endif

	i2c_open();

//	i2c_setclk(100000);	/* 100 k */
	/* check cpu speed is right */
	cpu_speed_now = get_cpu_speed()/100000;

	cpu_speed_test = 0;
	/* section 1's firt 2bypte, cpu_speed is 3340 *100kHz */
	ret = i2c_read(device_id, (unsigned char *)&cpu_speed_test, 1*section_size, 2); 
	if ( ret < 0 ) 
		goto i2c_read_failed;

	
	if ( cpu_speed_now != cpu_speed_test ) {
		printf("cpu_speed(%d) is not correct.\n", cpu_speed_now);
		printf("cpu_speed should be %d.\n", cpu_speed_test);
		i2c_close();
		led_dancing();
	}
	
	
	/* find the correct section */
	for (i=0; i< section_num; i+=2 ) {
		section_base_addr = i*section_size;
#if defined(CFG_EEPROM_AT24C16A)
		device_id = (0xA<<3) | (((section_base_addr) >> 8) &0x7);
#endif
		ret = i2c_read(device_id, buf, section_base_addr, 4); /*  */
		if ( ret < 0 ) {
			goto i2c_read_failed;
		}
		p_section_info->cpuclk = *((unsigned short *) &buf[0]);
		
		if ( p_section_info->cpuclk == cpu_speed_test ) {
			section_cnt = i;
			p_section_info->sum_offset = *((unsigned char *) &buf[2]);
			p_section_info->pass_offset = *((unsigned char *) &buf[3]);
			break;
		}
		
	}
#if defined(CHIP_COUNT_DEBUG)
	printf("cpuspeed=%d, cpuclk=%d\n", cpu_speed_test, p_section_info->cpuclk);
	printf("sum_offset=%d\n", p_section_info->sum_offset);
	printf("pass_offset=%d\n", p_section_info->pass_offset);
#endif
	if ( i >= section_num ) {
		printf("no cpu speed %d, please added it first.\n", cpu_speed_test);
		i2c_close();
		led_dancing();
	}
	if ( p_section_info->sum_offset >= section_size - 2 ) {
		printf("EEPROM's sector[%d], sector_size=%d.\n", i, section_size);
		printf("sum_offset=%d, should be less than sector_size\n", 
		       p_section_info->sum_offset);
	}
	if ( p_section_info->sum_offset >= p_section_info->pass_offset-1 ) {
		printf("*****************************\n");
		printf("EEPROM's speed %d sector is full.\n", cpu_speed_now);
		printf("no space to save speed %d count.\n", cpu_speed_now);
		printf("*****************************\n");
		led_dancing();
	}

	
	/* read data summary */
#if defined(CFG_EEPROM_AT24C16A)
	device_id = (0xA<<3) | (((section_base_addr) >> 8) &0x7);
#endif
	ret = i2c_read(device_id, (unsigned char *)&p_section_info->summary,
		       section_base_addr+p_section_info->sum_offset, 2); /*  */

	if ( ret < 0 ) 
		goto i2c_read_failed;

#if defined(CFG_EEPROM_DATA_BACKUP_ENABLE)
	/* read backup data */
	ret = i2c_read(device_id, (unsigned char *)&summary_backup,
		       section_base_addr+p_section_info->sum_offset+section_size, 2); /*  */
	if ( ret < 0 ) 
		goto i2c_read_failed;

	/* decrypt summary_backup format??? */
	summary_backup = decrypt(summary_backup);

	/* check data */
	if ( !(p_section_info->sum_offset == 4 && /* skip the first data */
	       p_section_info->summary == 0)  ) {
		if ( p_section_info->summary != summary_backup ) {
			printf("data check error...\n");
			led_dancing();
		}
	}
#endif
	
	p_section_info->summary++;

	if ( p_section_info->summary > check_max ) {
		p_section_info->summary = 1;
		p_section_info->sum_offset +=2;
		if ( p_section_info->sum_offset >= p_section_info->pass_offset-1 ) {
			printf("*****************************\n");
			printf("EEPROM's speed %d sector is full.\n", cpu_speed_now);
			printf("no space to save speed %d count.\n", cpu_speed_now);
			printf("*****************************\n");
			led_dancing();
		}
		if ( p_section_info->sum_offset >= section_size - 2 ) {
			printf("EEPROM's sector[%d], sector_size=%d.\n", i, section_size);
			printf("sum_offset=%d, should be less than sector_size\n", 
			       p_section_info->sum_offset);
		}

		ret = i2c_write(device_id, 
				(unsigned char *)&p_section_info->sum_offset,
				section_base_addr+2, 1); /*  */
		ret = i2c_write(device_id, 
				(unsigned char *)&p_section_info->sum_offset,
				section_base_addr+2+section_size, 1); /*  */
		if ( ret < 0 ) 
			goto i2c_write_failed;
		
	}

	/* save data summary  */
	ret = i2c_write(device_id, (unsigned char *)&p_section_info->summary,
			section_base_addr+p_section_info->sum_offset, 2); /*  */
	if ( ret < 0 ) 
		goto i2c_write_failed;
	
#if defined(CFG_EEPROM_DATA_BACKUP_ENABLE)	
	/* convert summary_backup format??? */
	summary_backup = p_section_info->summary;
	summary_backup = encrypt(summary_backup);

	/* save backup data */
	ret = i2c_write(device_id, (unsigned char *)&summary_backup,
			section_base_addr+p_section_info->sum_offset+section_size, 2);
	if ( ret < 0 ) 
		goto i2c_write_failed;
#endif	

	i2c_close();
	return 0;
	
 i2c_read_failed:
 i2c_write_failed:
	printf("i2c read or write error.\n");
	i2c_close();
	return -1;
	
}

#endif /* #if defined(CFG_CHIP_COUNT) */
