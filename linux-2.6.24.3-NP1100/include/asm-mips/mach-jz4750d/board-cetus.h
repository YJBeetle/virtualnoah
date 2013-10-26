/*
 *  linux/include/asm-mips/mach-jz4750d/board-cetus.h
 *
 *  JZ4750D-based CETUS board ver 1.x definition.
 *
 *  Copyright (C) 2008 Ingenic Semiconductor Inc.
 *
 *  Author: <cwjia@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_JZ4750D_CETUS_H__
#define __ASM_JZ4750D_CETUS_H__

/*====================================================================== 
 * Frequencies of on-board oscillators
 */
#define JZ_EXTAL		24000000
#define JZ_EXTAL2		32768     /* RTC extal freq: 32.768 KHz */
//#define CFG_DIV                 1         /* hclk=pclk=mclk=CFG_EXTAL/CFG_DIV, just for FPGA board */


/*====================================================================== 
 * GPIO
 */

#define GPIO_SD0_VCC_EN_N	(32*4+0) /* CIM_D0 */
#define GPIO_SD0_CD_N		(32*4+1) /* CIM_D1 */
#define GPIO_SD0_WP		(32*4+2) /* CIM_D2 */
#define GPIO_SD1_VCC_EN_N	(32*4+3) /* CIM_D3 */
#define GPIO_SD1_CD_N		(32*4+4) /* CIM_D4 */

#define GPIO_USB_DETE		(32*4+6) /* CIM_D6 */
#define GPIO_DC_DETE_N		(32*4+8) /* CIM_MCLK */
#define GPIO_CHARG_STAT_N	(32*4+10) /* CIM_VSYNC */
#define GPIO_DISP_OFF_N		(32*4+18) /* SDATO */
#define GPIO_LCD_VCC_EN_N	(32*4+19) /* SDATI */
//#define GPIO_LED_EN       	124 /* GPD28 */

#define GPIO_UDC_HOTPLUG	GPIO_USB_DETE

/*====================================================================== 
 * LCD backlight
 */
#define GPIO_LCD_PWM   		(32*4+22) /* GPE22 PWM2 */ 
#define LCD_PWM_CHN 2    /* pwm channel */
#define LCD_PWM_FULL 101
/* 100 level: 0,1,...,100 */
#define __lcd_set_backlight_level(n)	\
do {					\
	__gpio_as_output(GPIO_LCD_PWM);	\
	__gpio_set_pin(GPIO_LCD_PWM);	\
} while (0)

#define __lcd_close_backlight()		\
do {					\
	__gpio_as_output(GPIO_LCD_PWM);	\
	__gpio_clear_pin(GPIO_LCD_PWM);	\
} while (0)

/*====================================================================== 
 * MMC/SD
 */

#define MSC0_WP_PIN		GPIO_SD0_WP
#define MSC0_HOTPLUG_PIN	GPIO_SD0_CD_N
#define MSC0_HOTPLUG_IRQ	(IRQ_GPIO_0 + GPIO_SD0_CD_N)

#define MSC1_WP_PIN		GPIO_SD1_WP
#define MSC1_HOTPLUG_PIN	GPIO_SD1_CD_N
#define MSC1_HOTPLUG_IRQ	(IRQ_GPIO_0 + GPIO_SD1_CD_N)

#define __msc0_init_io()			\
do {						\
	__gpio_as_output(GPIO_SD0_VCC_EN_N);	\
	__gpio_as_input(GPIO_SD0_CD_N);		\
} while (0)

#define __msc0_enable_power()			\
do {						\
	__gpio_clear_pin(GPIO_SD0_VCC_EN_N);	\
} while (0)

#define __msc0_disable_power()			\
do {						\
	__gpio_set_pin(GPIO_SD0_VCC_EN_N);	\
} while (0)

#define __msc0_card_detected(s)			\
({						\
	int detected = 1;			\
	if (__gpio_get_pin(GPIO_SD0_CD_N))	\
		detected = 0;			\
	detected;				\
})

#define __msc1_init_io()			\
do {						\
	__gpio_as_output(GPIO_SD1_VCC_EN_N);	\
	/*	__gpio_as_input(GPIO_SD1_CD_N);*/	\
} while (0)

#define __msc1_enable_power()			\
do {						\
	__gpio_clear_pin(GPIO_SD1_VCC_EN_N);	\
} while (0)

#define __msc1_disable_power()			\
do {						\
	__gpio_set_pin(GPIO_SD1_VCC_EN_N);	\
} while (0)

#define __msc1_card_detected(s)			\
({						\
	int detected = 0;			\
	if (__gpio_get_pin(GPIO_SD1_CD_N))	\
		detected = 1;			\
	detected;				\
})

#endif /* __ASM_JZ4750d_CETUS_H__ */
