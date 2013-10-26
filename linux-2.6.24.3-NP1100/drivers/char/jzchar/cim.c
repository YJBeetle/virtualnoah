/*
 * linux/drivers/char/jzchar/cim.c
 *
 * Camera Interface Module (CIM) driver for JzSOC
 * This driver is independent of the camera sensor
 *
 * Copyright (C) 2005  JunZheng semiconductor
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/major.h>
#include <linux/string.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/spinlock.h>

#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/jzsoc.h>

#include "jzchars.h"

#define CIM_NAME        "cim"

MODULE_AUTHOR("Jianli Wei<jlwei@ingenic.cn>");
MODULE_DESCRIPTION("JzSOC Camera Interface Module driver");
MODULE_LICENSE("GPL");

/*
 * Define the Max Image Size
 */
#define MAX_IMAGE_WIDTH  640
#define MAX_IMAGE_HEIGHT 480
#define MAX_IMAGE_BPP    16
#define MAX_FRAME_SIZE   (MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT * MAX_IMAGE_BPP / 8)

typedef struct
{
	u32 width;
	u32 height;
	u32 bpp;
} img_param_t;

typedef struct
{
	u32 cfg;
	u32 ctrl;
	u32 mclk;
} cim_config_t;

/*
 * IOCTL_XXX commands
 */
#define IOCTL_SET_IMG_PARAM	0	// arg type: img_param_t *
#define IOCTL_CIM_CONFIG	1	// arg type: cim_config_t *

/* Actual image size, must less than max values */
static int img_width = MAX_IMAGE_WIDTH, img_height = MAX_IMAGE_HEIGHT, img_bpp = MAX_IMAGE_BPP;

/*
 * CIM DMA descriptor
 */
struct cim_desc {
	u32 nextdesc;   /* Physical address of next desc */
	u32 framebuf;   /* Physical address of frame buffer */
	u32 frameid;    /* Frame ID */ 
	u32 dmacmd;     /* DMA command */
};

/*
 * CIM device structure
 */
struct cim_device {
	unsigned char *framebuf;
	unsigned int frame_size;
	unsigned int page_order;
	wait_queue_head_t wait_queue;
	struct cim_desc frame_desc __attribute__ ((aligned (16)));
};

// global
static struct cim_device *cim_dev;

/*==========================================================================
 * CIM init routines
 *========================================================================*/

static void cim_config(cim_config_t *c)
{
	REG_CIM_CFG = c->cfg;
	REG_CIM_CTRL = c->ctrl;
	// Set the master clock output
#if defined(CONFIG_SOC_JZ4730)
	__cim_set_master_clk(__cpm_get_sclk(), c->mclk);
#elif defined(CONFIG_SOC_JZ4740) || defined(CONFIG_SOC_JZ4750)
	__cim_set_master_clk(__cpm_get_hclk(), c->mclk);
#else
	__cim_set_master_clk(__cpm_get_sclk(), c->mclk);
#endif
	// Enable sof, eof and stop interrupts
	__cim_enable_sof_intr();
	__cim_enable_eof_intr();
	__cim_enable_stop_intr();
}

/*==========================================================================
 * CIM start/stop operations
 *========================================================================*/

static int cim_start_dma(char *ubuf)
{
	__cim_disable();

	dma_cache_wback((unsigned long)cim_dev->framebuf, (2 ^ (cim_dev->page_order)) * 4096);

	// set the desc addr
	__cim_set_da(virt_to_phys(&(cim_dev->frame_desc)));

	__cim_clear_state();	// clear state register
	__cim_reset_rxfifo();	// resetting rxfifo
	__cim_unreset_rxfifo();
	__cim_enable_dma();	// enable dma

	// start
	__cim_enable();

	// wait for interrupts
	interruptible_sleep_on(&cim_dev->wait_queue);

	// copy frame data to user buffer
	memcpy(ubuf, cim_dev->framebuf, cim_dev->frame_size);

	return cim_dev->frame_size;
}

static void cim_stop(void)
{
	__cim_disable();
	__cim_clear_state();
}

/*==========================================================================
 * Framebuffer allocation and destroy
 *========================================================================*/

static void cim_fb_destroy(void)
{
	if (cim_dev->framebuf) {
		free_pages((unsigned long)(cim_dev->framebuf), cim_dev->page_order);
		cim_dev->framebuf = NULL;
	}
}

static int cim_fb_alloc(void)
{
	cim_dev->frame_size = img_width * img_height * (img_bpp/8);
	cim_dev->page_order = get_order(cim_dev->frame_size);

	/* frame buffer */
	cim_dev->framebuf = (unsigned char *)__get_free_pages(GFP_KERNEL, cim_dev->page_order);
	if ( !(cim_dev->framebuf) ) {
		return -ENOMEM;
	}

	cim_dev->frame_desc.nextdesc = virt_to_phys(&(cim_dev->frame_desc));
	cim_dev->frame_desc.framebuf = virt_to_phys(cim_dev->framebuf);
	cim_dev->frame_desc.frameid = 0x52052018;
	cim_dev->frame_desc.dmacmd = CIM_CMD_EOFINT | CIM_CMD_STOP | (cim_dev->frame_size >> 2); // stop after capturing a frame

	dma_cache_wback((unsigned long)(&(cim_dev->frame_desc)), 16);

	return 0;
}

/*==========================================================================
 * File operations
 *========================================================================*/

static int cim_open(struct inode *inode, struct file *filp);
static int cim_release(struct inode *inode, struct file *filp);
static ssize_t cim_read(struct file *filp, char *buf, size_t size, loff_t *l);
static ssize_t cim_write(struct file *filp, const char *buf, size_t size, loff_t *l);
static int cim_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations cim_fops = 
{
	open:		cim_open,
	release:	cim_release,
	read:		cim_read,
	write:		cim_write,
	ioctl:		cim_ioctl
};

static int cim_open(struct inode *inode, struct file *filp)
{
 	try_module_get(THIS_MODULE);
	return 0;
}

static int cim_release(struct inode *inode, struct file *filp)
{
	cim_stop();

 	module_put(THIS_MODULE);
	return 0;
}

static ssize_t cim_read(struct file *filp, char *buf, size_t size, loff_t *l)
{
	if (size < cim_dev->frame_size)
		return -EINVAL;

	return cim_start_dma(buf);
}

static ssize_t cim_write(struct file *filp, const char *buf, size_t size, loff_t *l)
{
	printk("cim error: write is not implemented\n");
	return -1;
}

static int cim_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case IOCTL_SET_IMG_PARAM:
	{
		img_param_t i;

		if (copy_from_user((void *)&i, (void *)arg, sizeof(img_param_t)))
			return -EFAULT;

		img_width = i.width;
		img_height = i.height;
		img_bpp = i.bpp;

		if ((img_width * img_height * img_bpp/8) > MAX_FRAME_SIZE) {
			/* realloc the buffer */
			cim_fb_destroy();
			if (cim_fb_alloc() < 0)
				return -ENOMEM;
		}

		cim_dev->frame_size = img_width * img_height * (img_bpp/8);

		cim_dev->frame_desc.dmacmd = CIM_CMD_EOFINT | CIM_CMD_STOP | (cim_dev->frame_size >> 2); // stop after capturing a frame

		dma_cache_wback((unsigned long)(&(cim_dev->frame_desc)), 16);

		break;
	}
	case IOCTL_CIM_CONFIG:
	{
		cim_config_t c;

		if (copy_from_user((void *)&c, (void *)arg, sizeof(cim_config_t)))
			return -EFAULT;

		cim_config(&c);

		break;
	} 
	default:
		printk("Not supported command: 0x%x\n", cmd);
		return -EINVAL;
		break;
	}
	return 0;
}

/*==========================================================================
 * Interrupt handler
 *========================================================================*/

static irqreturn_t cim_irq_handler(int irq, void *dev_id)
{
	u32 state = REG_CIM_STATE;
#if 0
	if (state & CIM_STATE_DMA_EOF) {
		wake_up_interruptible(&cim_dev->wait_queue);
	}
#endif
	if (state & CIM_STATE_DMA_STOP) {
		// Got a frame, wake up wait routine
		wake_up_interruptible(&cim_dev->wait_queue);
	}

	// clear status flags
	REG_CIM_STATE = 0;
 	return IRQ_HANDLED;
}

/*==========================================================================
 * Module init and exit
 *========================================================================*/

static int __init cim_init(void)
{
	struct cim_device *dev;
	int ret;

	/* allocate device */
	dev = kmalloc(sizeof(struct cim_device), GFP_KERNEL);
	if (!dev) return -ENOMEM;

	/* record device */
	cim_dev = dev;

	/* allocate a frame buffer */
	if (cim_fb_alloc() < 0) {
		kfree(dev);
		return -ENOMEM;
	}

	init_waitqueue_head(&dev->wait_queue);

	ret = jz_register_chrdev(CIM_MINOR, CIM_NAME, &cim_fops, dev);
	if (ret < 0) {
		cim_fb_destroy();
		kfree(dev);
		return ret;
	}

	if ((ret = request_irq(IRQ_CIM, cim_irq_handler, IRQF_DISABLED, 
			       CIM_NAME, dev))) {
		cim_fb_destroy();
		kfree(dev);
		printk(KERN_ERR "CIM could not get IRQ");
		return ret;
	}

	printk("JzSOC Camera Interface Module (CIM) driver registered\n");

	return 0;
}

static void __exit cim_exit(void)
{
	free_irq(IRQ_CIM, cim_dev);
	jz_unregister_chrdev(CIM_MINOR, CIM_NAME);
	cim_fb_destroy();
	kfree(cim_dev);
}

module_init(cim_init);
module_exit(cim_exit);
