/*
 *  linux/include/asm-mips/jzsoc.h
 *
 *  Ingenic's JZXXXX SoC common include.
 *
 *  Copyright (C) 2006 - 2008 Ingenic Semiconductor Inc.
 *
 *  Author: <jlwei@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_JZSOC_H__
#define __ASM_JZSOC_H__

/*
 * SoC include
 */

#ifdef CONFIG_SOC_JZ4730
#include <asm/mach-jz4730/jz4730.h>
#endif

#ifdef CONFIG_SOC_JZ4740
#include <asm/mach-jz4740/jz4740.h>
#endif

#ifdef CONFIG_SOC_JZ4750
#include <asm/mach-jz4750/jz4750.h>
#endif

#ifdef CONFIG_SOC_JZ4750D
#include <asm/mach-jz4750d/jz4750d.h>
#endif

/*
 * Generic I/O routines
 */
#define readb(addr)	(*(volatile unsigned char *)(addr))
#define readw(addr)	(*(volatile unsigned short *)(addr))
#define readl(addr)	(*(volatile unsigned int *)(addr))

#define writeb(b,addr)	((*(volatile unsigned char *)(addr)) = (b))
#define writew(b,addr)	((*(volatile unsigned short *)(addr)) = (b))
#define writel(b,addr)	((*(volatile unsigned int *)(addr)) = (b))

#endif /* __ASM_JZSOC_H__ */
