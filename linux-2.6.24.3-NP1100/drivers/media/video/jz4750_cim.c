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

//#include <linux/config.h>
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
#include <asm/pgtable.h>
#include <asm/uaccess.h>
#include <asm/jzsoc.h>
#include <asm/cacheflush.h>

#include <linux/videodev.h>
#include <media/v4l2-common.h>
#include <linux/video_decoder.h>

#include "jz4750_cim.h"

#define CIM_NAME        "cim"

MODULE_AUTHOR("Jianli Wei<jlwei@ingenic.cn>");
MODULE_DESCRIPTION("JzSOC Camera Interface Module driver");
MODULE_LICENSE("GPL");

#if defined(CONFIG_SOC_JZ4750D)
//#define USE_CIM_OFRCV 1 /* Test overflow recovery */
#define USE_CIM_DMA_SYNC 1 //set da every time
#endif

//#define CIM_DEBUG
#undef CIM_DEBUG
#ifdef CIM_DEBUG
#define dprintk(x...)	printk(x)
#else
#define dprintk(x...)
#endif
/*
 * Define the Max Image Size
 */
#define MAX_IMAGE_WIDTH  2048
#define MAX_IMAGE_HEIGHT 2048
#define MAX_IMAGE_BPP    16
#define MAX_FRAME_SIZE   (MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT * MAX_IMAGE_BPP / 8)
#define	CIM_RAM_ADDR		(CIM_BASE + 0x1000)

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
	u32 size;
	u32 offs;
} cim_config_t;

/*
 * IOCTL_XXX commands
 */
#define IOCTL_SET_IMG_PARAM	0	// arg type: img_param_t *
#define IOCTL_CIM_CONFIG	1	// arg type: cim_config_t *
#define IOCTL_STOP_CIM		2       // arg type: void
#define IOCTL_GET_IMG_PARAM	3	// arg type: img_param_t *
#define IOCTL_GET_CIM_CONFIG	4	// arg type: cim_config_t *
#define IOCTL_TEST_CIM_RAM	5	// no arg type *
#define IOCTL_START_CIM		6       // arg type: void

/*
 * CIM DMA descriptor
 */
struct cim_desc {
	u32 nextdesc;   /* Physical address of next desc */
	u32 framebuf;   /* Physical address of frame buffer */
	u32 frameid;    /* Frame ID */ 
	u32 dmacmd;     /* DMA command */
	u32 pagenum;
};

/*
 * CIM device structure
 */
struct cim_device {
	struct video_device *jz_cim;
	unsigned char *framebuf;
	unsigned int frame_size;
	unsigned int page_order;
	wait_queue_head_t wait_queue;
	struct cim_desc *frame_desc __attribute__ ((aligned (16)));
};

/* global*/
static struct cim_device *cim_dev;
static int start_init = 1;
static int irq_sleep;
/*==========================================================================
 * CIM init routines
 *========================================================================*/

static void cim_image_area(img_param_t *c) {
	/*set the image data area start 0, 0, lines_per_frame and pixels_per_line*/
	REG_CIM_SIZE = 0;
	REG_CIM_OFFSET = 0;
#if defined(CONFIG_SOC_JZ4750D)
	if (REG_CIM_CTRL & CIM_CTRL_WIN_EN) {
		REG_CIM_SIZE =  (c->height << CIM_SIZE_LPF_BIT) | (c->width << CIM_SIZE_PPL_BIT);
//		REG_CIM_OFFSET = (0 << CIM_OFFSET_V_BIT) | (0 << CIM_OFFSET_H_BIT);
//		REG_CIM_OFFSET = (100 << CIM_OFFSET_V_BIT) | (50 << CIM_OFFSET_H_BIT);
		REG_CIM_OFFSET = (200 << CIM_OFFSET_V_BIT) | (300 << CIM_OFFSET_H_BIT);
	}
#endif
}


static void cim_config(cim_config_t *c)
{
	REG_CIM_CFG = c->cfg;
	REG_CIM_CTRL = c->ctrl;
	REG_CIM_SIZE = c->size;
	REG_CIM_OFFSET = c->offs;

	dprintk("REG_CIM_SIZE = 0x%08x\n", REG_CIM_SIZE);
	dprintk("REG_CIM_OFFSET = 0x%08x\n", REG_CIM_OFFSET);
	/* Set the master clock output */
	/* If use pll clock, enable it */
//	__cim_set_master_clk(__cpm_get_hclk(), c->mclk);

	/* Enable sof, eof and stop interrupts*/

//	__cim_enable_sof_intr();
	__cim_enable_eof_intr();
#if defined(USE_CIM_EEOFINT)
	__cim_enable_eeof_intr();
#endif
//	__cim_enable_stop_intr();
//	__cim_enable_trig_intr();
//	__cim_enable_rxfifo_overflow_intr();
//	__cim_enable_vdd_intr();
//	printk("hclk=%d, mclk = %d\n", __cpm_get_hclk(),c->mclk);
	dprintk("REG_CIM_CTRL = 0x%08x\n", REG_CIM_CTRL);
}

/*==========================================================================
 * CIM start/stop operations
 *========================================================================*/
static int cim_start_dma(char *ubuf)
{

	struct cim_desc *jz_frame_desc;
	int cim_frame_size = 0;
	dprintk("==========start_init = %d\n", start_init);
	__cim_disable();
	__cim_set_da(virt_to_phys(cim_dev->frame_desc));
	__cim_clear_state();	// clear state register
	__cim_reset_rxfifo();	// resetting rxfifo
	__cim_unreset_rxfifo();
	__cim_enable_dma();	// enable dma
	__cim_enable();
	interruptible_sleep_on(&cim_dev->wait_queue);

#if 1
	dprintk("interruptible_sleep_on\n");
	dprintk("REG_CIM_DA = 0x%08x\n", REG_CIM_DA);
	dprintk("REG_CIM_FA = 0x%08x\n", REG_CIM_FA);
	dprintk("REG_CIM_FID = 0x%08x\n", REG_CIM_FID);
	dprintk("REG_CIM_CMD = 0x%08x\n", REG_CIM_CMD);
	dprintk("REG_CIM_CFG = 0x%08x\n", REG_CIM_CFG);
	dprintk("REG_CIM_STATE = 0x%08x\n", REG_CIM_STATE);
	dprintk("REG_CIM_CTRL = 0x%08x\n", REG_CIM_CTRL);
	dprintk("REG_CIM_SIZE = 0x%08x\n", REG_CIM_SIZE);
	dprintk("REG_CIM_OFFSET = 0x%08x\n", REG_CIM_OFFSET);
	dprintk("REG_CIM_CMD_3 = %x\n", REG_CIM_CMD);
	dprintk("REG_CIM_FA = %x\n", REG_CIM_FA);
#endif
	/* copy frame data to user buffer */
#if 0
	jz_frame_desc = cim_dev->frame_desc;

	while (jz_frame_desc != NULL)
	{
		dprintk("ubuf = %x, framebuf = %x,frame_size= %d\n", (u32)ubuf,(u32) jz_frame_desc->framebuf, jz_frame_desc->dmacmd & 0xffffff);
		memcpy(ubuf, phys_to_virt(jz_frame_desc->framebuf), ((jz_frame_desc->dmacmd & CIM_CMD_LEN_MASK) * 4));
		ubuf += (jz_frame_desc->dmacmd & CIM_CMD_LEN_MASK) * 4;
		cim_frame_size += (jz_frame_desc->dmacmd & CIM_CMD_LEN_MASK) * 4;
		jz_frame_desc = (struct cim_desc *)phys_to_virt(jz_frame_desc->nextdesc);
	}
#endif
	dprintk("---------**********-----\n");
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
	int pages;
	struct cim_desc *jz_frame_desc, *p_desc;
	__cim_disable_dma();
	__cim_disable();

	dprintk("cim_dev->frame_desc = %x\n", (u32)cim_dev->frame_desc);
	if (cim_dev->frame_desc == NULL) {
		printk("Original memory is NULL\n");
		return;
	}
	jz_frame_desc = cim_dev->frame_desc;
//	while (jz_frame_desc != NULL) {
//	while (jz_frame_desc != cim_dev->frame_desc) {
		dprintk("framebuf = %x,thisdesc = %x,frame_size= %d\n", (u32) jz_frame_desc->framebuf, (unsigned int)jz_frame_desc, (jz_frame_desc->dmacmd & 0xffffff) * 4);
		p_desc = (struct cim_desc *)phys_to_virt(jz_frame_desc->nextdesc);
		pages = jz_frame_desc->pagenum;
		dprintk("page_order = %d\n", pages);
		free_pages((unsigned long)phys_to_virt(jz_frame_desc->framebuf), pages);
		kfree(jz_frame_desc);
		jz_frame_desc = p_desc;
//	}
	cim_dev->frame_desc = NULL;
	start_init = 1;
}

static struct cim_desc *get_desc_list(int page_order)
{
	int num, page_nums = 0;
	unsigned char *p_buf;
	struct cim_desc *desc_list_head __attribute__ ((aligned (16)));
	struct cim_desc *desc_list_tail __attribute__ ((aligned (16)));
	struct cim_desc *p_desc;
//	num = page_order - 1;
	num = page_order;
	desc_list_head = desc_list_tail = NULL;

	while(page_nums < (1 << page_order)) {
		p_desc = (struct cim_desc *)kmalloc(sizeof(struct cim_desc), GFP_KERNEL);
		if (NULL == p_desc)
			return NULL;
		//return -ENOMEM;
	cim_realloc_pages:
		p_buf = (unsigned char *)__get_free_pages(GFP_KERNEL, num);
		if ( !(p_buf) && num != 0) {
			num --;
			goto cim_realloc_pages;
		}
		else if ( !(p_buf) && num == 0)	{
			printk("No memory can be alloc!\n");
			//return -ENOMEM;
			return NULL;
		}
		else {
			if (desc_list_head == NULL) {
				dprintk("Page_list_head\n");
				desc_list_head = p_desc;
			}
			
			else
				desc_list_tail->nextdesc = virt_to_phys(p_desc);

			desc_list_tail = p_desc;
			desc_list_tail->framebuf = virt_to_phys(p_buf);
			dprintk("framebuf addr is 0x%08x\n", (u32)desc_list_tail->framebuf);
			dprintk("frame_desc addr is 0x%08x\n",(u32)virt_to_phys(desc_list_tail));

			desc_list_tail->frameid = 0x52052018;
			desc_list_tail->pagenum = num;
			if ((page_nums + (1<< num)) < (1 << page_order)) {
				desc_list_tail->dmacmd = ((1 << num) * 4096) >> 2 ;
			}
			else
				desc_list_tail->dmacmd = 
					(cim_dev->frame_size - page_nums * 4096) >> 2 ;
			dprintk("the desc_list_tail->dmacmd is 0x%08x\n", desc_list_tail->dmacmd);
			page_nums += (1 << num);
			dprintk("the pages_num is %d\n", page_nums);
			dma_cache_wback((unsigned long)(desc_list_tail), 16);
		}
	}

//	desc_list_tail->nextdesc = virt_to_phys(NULL);
	desc_list_tail->nextdesc = virt_to_phys(desc_list_head);
	desc_list_tail->dmacmd |= CIM_CMD_EOFINT;
#if defined(CONFIG_SOC_JZ4750D)
#if defined(USE_CIM_OFRCV)
	desc_list_tail->dmacmd |= (CIM_CMD_EOFINT | CIM_CMD_OFRCV);
#endif
#if defined(USE_CIM_DMA_SYNC) /* wake ervry time */
	desc_list_tail->nextdesc = virt_to_phys(NULL);
	desc_list_tail->dmacmd |= (CIM_CMD_STOP | CIM_CMD_EOFINT | CIM_CMD_OFRCV);
#endif
#if defined(USE_CIM_EEOFINT)
//	desc_list_tail->dmacmd |= CIM_CMD_EEOFINT;
	desc_list_tail->dmacmd |= (CIM_CMD_STOP | CIM_CMD_EOFINT | CIM_CMD_EEOFINT);
#endif
#endif
	/* stop after capturing a frame */
//	desc_list_tail->dmacmd |= (CIM_CMD_STOP | CIM_CMD_EOFINT | CIM_CMD_SOFINT);




	dma_cache_wback((unsigned long)(desc_list_tail), 16);
	dprintk("the desc_list_tail->dmacmd is 0x%08x\n", desc_list_tail->dmacmd);
 
	return desc_list_head;
}

static int cim_fb_alloc(int img_width, int img_height, int img_bpp)
{
	if ((REG_CIM_CFG & (CIM_CFG_DF_MASK | CIM_CFG_BYPASS_MASK)) == 0)
		cim_dev->frame_size = img_width * (img_height-1) * (img_bpp/8);
	else
		cim_dev->frame_size = img_width * img_height * (img_bpp/8);

	cim_dev->page_order = get_order(cim_dev->frame_size);
	dprintk("cim_dev->page_order=%d\n", cim_dev->page_order);
	/* frame buffer ?? need large mem ??*/
	cim_dev->frame_desc = get_desc_list(cim_dev->page_order);
	if (cim_dev->frame_desc == NULL)
		return -ENOMEM;
	dma_cache_wback((unsigned long)(cim_dev->frame_desc), 16);
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
static int cim_mmap(struct file *file, struct vm_area_struct *vma);

static struct file_operations cim_fops = 
{
	open:		cim_open,
	release:	cim_release,
	read:		cim_read,
	write:		cim_write,
	ioctl:		cim_ioctl,
	compat_ioctl:	v4l_compat_ioctl32,
	mmap:		cim_mmap
};

static struct video_device jz_v4l_device = {
	.name		= "jz cim",
	//.type		= VID_TYPE_CAPTURE | VID_TYPE_SUBCAPTURE |
	//	VID_TYPE_CLIPPING | VID_TYPE_SCALES, VID_TYPE_OVERLAY
	.fops		= &cim_fops,
	.minor		= -1,
	.owner		= THIS_MODULE,
	.release 	= video_device_release,
};

static int cim_open(struct inode *inode, struct file *filp)
{
	
 	try_module_get(THIS_MODULE);
	return 0;
}

static int cim_release(struct inode *inode, struct file *filp)
{
	dprintk("%s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);
	cim_fb_destroy();
	cim_stop();

 	module_put(THIS_MODULE);
	return 0;
}

static ssize_t cim_read(struct file *filp, char *buf, size_t size, loff_t *l)
{
	printk("============cim error: write is not implemented\n");
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
	void __user *argp = (void __user *)arg;
	switch (cmd) {
	case IOCTL_GET_IMG_PARAM:
	{
		unsigned int i;
		i = cim_dev->frame_desc->framebuf;
//		printk("cim_dev->frame_desc->framebuf = 0x%08x\n", cim_dev->frame_desc->framebuf);
		dprintk("&&cim_dev->frame_desc->framebuf = 0x%08x\n", i);

		return copy_to_user(argp, &i, sizeof(unsigned int)) ? -EFAULT : 0;
	}
	case IOCTL_STOP_CIM:
	{
		__cim_disable_dma();	// enable dma
		__cim_disable();

//		cim_fb_destroy();
		return 0;
	}
	case IOCTL_START_CIM:
	{
		__cim_set_da(virt_to_phys(cim_dev->frame_desc));
		__cim_clear_state();	// clear state register
		__cim_reset_rxfifo();	// resetting rxfifo
		__cim_unreset_rxfifo();
		__cim_enable_dma();	// enable dma
		__cim_enable();
		return 0;
	}
	case IOCTL_SET_IMG_PARAM:
	{
		img_param_t i;
		int img_width, img_height, img_bpp;
		if (copy_from_user((void *)&i, (void *)arg, sizeof(img_param_t)))
			return -EFAULT;
		img_width = i.width;
		img_height = i.height;
		img_bpp = i.bpp;
		printk("ALLOC =========\n");
		if ((img_width * img_height * img_bpp/8) > MAX_FRAME_SIZE){
			printk("ERROR! Image is too large!\n");
			return -EINVAL;
		}
 		/* allocate frame buffers */
		if (cim_dev->frame_desc == NULL){
			if (cim_fb_alloc(img_width, img_height, img_bpp) < 0){
				printk("ERROR! Init & alloc cim fail!\n");
				return -ENOMEM;
			}
		}
		else
			if ((img_width * img_height * img_bpp/8) > cim_dev->frame_size){
				/* realloc the buffer */
	dprintk("%s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);
				cim_fb_destroy();
				if (cim_fb_alloc(img_width, img_height, img_bpp) < 0){
					printk("ERRROR! Init & alloc cim fail!\n");
					return -ENOMEM;
				}
			}
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
	case IOCTL_TEST_CIM_RAM:
	{
		
		int i;
		volatile unsigned int *ptr;
		ptr = (volatile unsigned int *)(CIM_RAM_ADDR);
		dprintk("RAM test!\n");
		dprintk("CIM_RAM_ADDR = 0x%08x\n", CIM_RAM_ADDR);
		for (i = 0; i < 1024; ptr++, i++)
			*ptr = i;
		ptr = (volatile unsigned int *)(CIM_RAM_ADDR);
		dma_cache_wback((unsigned long)CIM_RAM_ADDR,0xffc);

		for (i = 0; i < 1024; i++) {
			if (i != *ptr)
				dprintk("*ptr!=i, *ptr=%d, i=%d\n", *ptr, i);
			if (i%32 == 0) {
				if (i%128 == 0)
					dprintk("\n");
				dprintk("*ptr=%04d, i=%04d | ", *ptr, i);
			}
			ptr++;
		}
		dprintk("\n");
		break;
	}
	default:
		printk("Not supported command: 0x%x\n", cmd);
		return -EINVAL;
		break;
	}
	return 0;
}

/* Use mmap /dev/fb can only get a non-cacheable Virtual Address. */
static int cim_mmap(struct file *file, struct vm_area_struct *vma)
{
	unsigned long start;
	unsigned long off;
	u32 len;

	dprintk("%s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);
	off = vma->vm_pgoff << PAGE_SHIFT;

	/* frame buffer memory */
	start = cim_dev->frame_desc->framebuf;
	len = PAGE_ALIGN((start & ~PAGE_MASK) + (cim_dev->frame_desc->dmacmd & CIM_CMD_LEN_MASK)*4);
	start &= PAGE_MASK;
	printk("vma->vm_end = 0x%08lx,\nvma->vm_start = 0x%08lx,\noff = 0x%08lx,\n len = 0x%08x\n\n", vma->vm_end, vma->vm_start, off, len);
	if ((vma->vm_end - vma->vm_start + off) > len) {
		printk("Error: vma is larger than memory length\n");
		return -EINVAL;
	}
	off += start;

	vma->vm_pgoff = off >> PAGE_SHIFT;
	vma->vm_flags |= VM_IO;
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);	/* Uncacheable */

#if  defined(CONFIG_MIPS32)
 	pgprot_val(vma->vm_page_prot) &= ~_CACHE_MASK;
 	pgprot_val(vma->vm_page_prot) |= _CACHE_UNCACHED;		/* Uncacheable */
#endif

	if (io_remap_pfn_range(vma, vma->vm_start, off >> PAGE_SHIFT,
			       vma->vm_end - vma->vm_start,
			       vma->vm_page_prot)) {
		return -EAGAIN;
	}
	return 0;
}
/*==========================================================================
 * Interrupt handler
 *========================================================================*/

static irqreturn_t cim_irq_handler(int irq, void *dev_id)
{
	u32 state = REG_CIM_STATE;
	dprintk("REG_CIM_STATE = %x\n", REG_CIM_STATE);
	dprintk("IRQ:REG_CIM_CTRL = %x\n", REG_CIM_CTRL);

#if 0 //sof
	/* recommed don't open it */
	if ((REG_CIM_CTRL & CIM_CTRL_DMA_SOFM) && (state & CIM_STATE_DMA_SOF)) {
		dprintk("SOF interrupt!\n");
		REG_CIM_STATE &= ~CIM_STATE_DMA_SOF;
	}
#endif
#if 0 //eeof
	if ((REG_CIM_CTRL & CIM_CTRL_DMA_EEOFM) && (state & CIM_STATE_DMA_EEOF)) {
		dprintk("EEOF interrupt!\n");
		__cim_disable_dma();
		__cim_disable();
		wake_up_interruptible(&cim_dev->wait_queue);
		REG_CIM_STATE &= ~CIM_STATE_DMA_EEOF;
	}
#endif

#if 1 //eof
	if ((REG_CIM_CTRL & CIM_CTRL_DMA_EOFM) && (state & CIM_STATE_DMA_EOF)) {
//	if (state & CIM_STATE_DMA_EOF) {
		dprintk("EOF interrupt!\n");

#if defined(USE_CIM_DMA_SYNC) /* wake ervry time */
//		__cim_disable_dma();
//		__cim_disable();
		wake_up_interruptible(&cim_dev->wait_queue);
#else
//		if(irq_sleep == 1)
		wake_up_interruptible(&cim_dev->wait_queue);
#endif
		REG_CIM_STATE &= ~CIM_STATE_DMA_EOF;
		return IRQ_HANDLED;
	}
#endif
#if 0 //overflow
	if (state & CIM_STATE_RXF_OF) {
		printk("OverFlow interrupt!\n");
		REG_CIM_STATE &= ~CIM_STATE_RXF_OF;
//		dprintk("REG_CIM_STATE = %x\n", REG_CIM_STATE);
		return IRQ_HANDLED;
	}
#endif
#if 1 // stop
	if ((REG_CIM_CTRL & CIM_CTRL_DMA_STOPM) && (state & CIM_STATE_DMA_STOP)) {
		// Got a frame, wake up wait routine
//#if defined(USE_CIM_DMA_SYNC) /* wake ervry time */
		__cim_disable_dma();
//		__cim_disable();

		dprintk("Stop interrupt!\n");
//		wake_up_interruptible(&cim_dev->wait_queue);
		REG_CIM_STATE &= ~CIM_STATE_DMA_STOP;
	}
#endif

#if 0 //trig
	if ((REG_CIM_CTRL & CIM_CTRL_RXF_TRIGM) && (state & CIM_STATE_RXF_TRIG)) {
		REG_CIM_STATE &= ~CIM_STATE_RXF_TRIG;
		dprintk("Trig interrupt!\n");
	}
#endif

#if 0 //vdd
	/* only happen disable cim during DMA transfer*/
	if ((REG_CIM_CTRL & CIM_CTRL_VDDM) && (state & CIM_STATE_VDD)) {
		dprintk(">>CIM Disable Done Interrupt!\n");
		REG_CIM_STATE &= ~CIM_STATE_VDD;
	}
#endif
	/* clear status flags*/
	dprintk("before clear REG_CIM_STATE = %x\n", REG_CIM_STATE);
//	REG_CIM_STATE = 0;

 	return IRQ_HANDLED;
}

/*Camera gpio init, different operationg according sensor*/
static void camera_gpio_init(void) {

	__gpio_as_cim();
	__gpio_as_i2c();
	__sensor_gpio_init();
}

static int v4l_device_init(void)
{
	camera_gpio_init();
	cim_dev = kzalloc(sizeof(struct cim_device), GFP_KERNEL);
	if (!cim_dev) return -ENOMEM;
	cim_dev->jz_cim = video_device_alloc();
	if (!cim_dev->jz_cim) {
		return -ENOMEM;
	}
	memcpy(cim_dev->jz_cim, &jz_v4l_device, sizeof(struct video_device));
	cim_dev->frame_desc = NULL;
	cim_dev->frame_size = 0;
	cim_dev->page_order = 0;
	return 0;
}
/*==========================================================================
 * Module init and exit
 *========================================================================*/

static int __init jz4750_cim_init(void)
{
	struct cim_device *dev;
	int ret;
	/* allocate device */
	ret = v4l_device_init();
	if (ret)
		return ret;
	/* record device */
	dev = cim_dev;
	init_waitqueue_head(&dev->wait_queue);

	ret = video_register_device(dev->jz_cim, VFL_TYPE_GRABBER, -1);
	if (ret < 0) {
		printk(KERN_ERR "CIM Video4Linux-device "
		       "registration failed\n");
		return -EINVAL;
	}

	if (ret < 0) {
	dprintk("%s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);
		cim_fb_destroy();
		kfree(dev);
		return ret;
	}

	if ((ret = request_irq(IRQ_CIM, cim_irq_handler, IRQF_DISABLED, 
			       CIM_NAME, dev))) {
		printk(KERN_ERR "request_irq return error, ret=%d\n", ret);
	dprintk("%s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);
		cim_fb_destroy();
		kfree(dev);
		printk(KERN_ERR "CIM could not get IRQ\n");
		return ret;
	}

	printk("JzSOC Camera Interface Module (CIM) driver registered\n");

	return 0;
}

static void __exit jz4750_cim_exit(void)
{
	free_irq(IRQ_CIM, cim_dev);
	kfree(cim_dev);
	video_unregister_device(cim_dev->jz_cim);
}

module_init(jz4750_cim_init);
module_exit(jz4750_cim_exit);
