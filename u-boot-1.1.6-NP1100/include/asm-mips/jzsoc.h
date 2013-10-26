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

#ifdef CONFIG_JZ4730
#include <asm/jz4730.h>
#endif

#ifdef CONFIG_JZ4740
#include <asm/jz4740.h>
#endif

#ifdef CONFIG_JZ4750
#include <asm/jz4750.h>
#endif

#ifdef CONFIG_JZ4750D
#include <asm/jz4750d.h>
#endif

#ifdef CONFIG_JZ4760
#include <asm/jz4760.h>
#endif


#endif /* __ASM_JZSOC_H__ */
