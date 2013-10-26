/*
 * linux/drivers/media/video/jz4750_cim.h -- Ingenic Jz4750 On-Chip CIM driver
 *
 * Copyright (C) 2005-2008, Ingenic Semiconductor Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifndef __JZ4750_CIM_H__
#define __JZ4750_CIM_H__

/* gpio init */
#if defined(CONFIG_JZ4750_APUS) || defined(CONFIG_JZ4750D_FUWA1) /* board pavo */
#define GPIO_CAMERA_RST 	(32*4+8) /* CIM_MCLK as reset */
#else
#error "driver/video/Jzlcd.h, please define SPI pins on your board."
#endif

#define CONFIG_OV9650    1

#if defined(CONFIG_OV9650) || defined(CONFIG_OV2640)
#if defined(CONFIG_JZ4750_APUS) /* board pavo */
#define __sensor_gpio_init()	\
do {\
	__gpio_as_output(GPIO_CAMERA_RST);	\
	__gpio_set_pin(GPIO_CAMERA_RST); \
	mdelay(50); \
	__gpio_clear_pin(GPIO_CAMERA_RST);\
} while(0)

#elif defined(CONFIG_JZ4750D_FUWA1) /* board pavo */
#define __sensor_gpio_init()	\
do {\
	__gpio_as_output(GPIO_CAMERA_RST);	\
	__gpio_set_pin(GPIO_CAMERA_RST); \
	mdelay(50); \
	__gpio_clear_pin(GPIO_CAMERA_RST);\
} while(0)
#endif
#endif

#ifndef __sensor_gpio_init
#define __sensor_gpio_init()
#endif
#endif /* __JZ4750_CIM_H__ */

