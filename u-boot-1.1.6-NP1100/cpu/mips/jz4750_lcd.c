/*
 * JzRISC lcd controller
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/************************************************************************/
/* ** HEADER FILES							*/
/************************************************************************/

/* 
 * Fallowing macro may be used:
 *  CONFIG_LCD                        : LCD support 
 *  LCD_BPP                           : Bits per pixel, 0 = 1, 1 = 2, 2 = 4, 3 = 8
 *  CFG_WHITE_ON_BLACK
 *  CONFIG_LCD_LOGO                   : show logo
 *  CFG_LCD_LOGOONLY_NOINFO           : not display info on lcd screen, only logo
 * -----------------------------------------------------------------------
 * bugs:
 * if BMP_LOGO_HEIGHT > (lcd screen height - 2*VIDEO_FONT_HEIGHT),
 * must not print info onto screen,
 * it means should define CFG_LCD_LOGOONLY_NOINFO.
 */



#include <config.h>
#include <common.h>
#include <devices.h>
#include <lcd.h>

#include <asm/io.h>               /* virt_to_phys() */

#if defined(CONFIG_JZ4750) || defined(CONFIG_JZ4750D) || defined(CONFIG_JZ4750L)
#if defined(CONFIG_LCD) && !defined(CONFIG_SLCD)
//#if defined(CONFIG_4750_LCD)// && !defined(CONFIG_4750_SLCD)
#if defined(CONFIG_JZ4750)
#include <asm/jz4750.h>
#endif
#if defined(CONFIG_JZ4750D)
#include <asm/jz4750d.h>
#endif
#if defined(CONFIG_JZ4750L)
#include <asm/jz4750l.h>
#endif
#include "jz4750_lcd.h"


struct jz4750lcd_info jzfb = {
#if defined(CONFIG_JZ4750_LCD_SAMSUNG_LTP400WQF02)
	.panel = {
		.cfg = LCD_CFG_LCDPIN_LCD | LCD_CFG_RECOVER | /* Underrun recover */ 
		LCD_CFG_NEWDES | /* 8words descriptor */
		LCD_CFG_MODE_GENERIC_TFT | /* General TFT panel */
		LCD_CFG_MODE_TFT_18BIT | 	/* output 18bpp */
		LCD_CFG_HSP | 	/* Hsync polarity: active low */
		LCD_CFG_VSP,	/* Vsync polarity: leading edge is falling edge */
		.slcd_cfg = 0,
		.ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,	/* 16words burst, enable out FIFO underrun irq */
		480, 272, 60, 41, 10, 2, 2, 2, 2,
	},
	.osd = {
		 .osd_cfg = LCD_OSDC_OSDEN | /* Use OSD mode */
//		 LCD_OSDC_ALPHAEN | /* enable alpha */
		 LCD_OSDC_F0EN,	/* enable Foreground0 */
		 .osd_ctrl = 0,		/* disable ipu,  */
		 .rgb_ctrl = 0,
		 .bgcolor = 0x000000, /* set background color Black */
		 .colorkey0 = 0, /* disable colorkey */
		 .colorkey1 = 0, /* disable colorkey */
		 .alpha = 0xA0,	/* alpha value */
		 .ipu_restart = 0x80001000, /* ipu restart */
		 .fg_change = FG_CHANGE_ALL, /* change all initially */
		 .fg0 = {32, 0, 0, 480, 272}, /* bpp, x, y, w, h */
		 .fg1 = {32, 0, 0, 480, 272}, /* bpp, x, y, w, h */
	 },
#elif defined(CONFIG_JZ4750_LCD_AUO_A043FL01V2)
	.panel = {
		.cfg = LCD_CFG_LCDPIN_LCD | LCD_CFG_RECOVER | /* Underrun recover */ 
		LCD_CFG_NEWDES | /* 8words descriptor */
		LCD_CFG_MODE_GENERIC_TFT | /* General TFT panel */
		LCD_CFG_MODE_TFT_18BIT | 	/* output 18bpp */
		LCD_CFG_HSP | 	/* Hsync polarity: active low */
		LCD_CFG_VSP,	/* Vsync polarity: leading edge is falling edge */
		.slcd_cfg = 0,
		.ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,	/* 16words burst, enable out FIFO underrun irq */
		480, 272, 60, 41, 10, 8, 4, 4, 2,
	},
	.osd = {
		 .osd_cfg = LCD_OSDC_OSDEN | /* Use OSD mode */
//		 LCD_OSDC_ALPHAEN | /* enable alpha */
//		 LCD_OSDC_F1EN | /* enable Foreground1 */
		 LCD_OSDC_F0EN,	/* enable Foreground0 */
		 .osd_ctrl = 0,		/* disable ipu,  */
		 .rgb_ctrl = 0,
		 .bgcolor = 0x0000ff, /* set background color Black */
		 .colorkey0 = 0, /* disable colorkey */
		 .colorkey1 = 0, /* disable colorkey */
		 .alpha = 0xA0,	/* alpha value */
		 .ipu_restart = 0x80001000, /* ipu restart */
		 .fg_change = FG_CHANGE_ALL, /* change all initially */
		 .fg0 = {32, 0, 0, 320, 240}, /* bpp, x, y, w, h */
		 .fg1 = {32, 0, 0, 320, 240}, /* bpp, x, y, w, h */
	 },
#elif defined(CONFIG_JZ4750_LCD_TRULY_TFT_GG1P0319LTSW_W)
	.panel = {
//		 .cfg = LCD_CFG_LCDPIN_SLCD | LCD_CFG_RECOVER | /* Underrun recover*/ 
		 .cfg = LCD_CFG_LCDPIN_SLCD | /* Underrun recover*/ 
		 LCD_CFG_NEWDES | /* 8words descriptor */
		 LCD_CFG_MODE_SLCD, /* TFT Smart LCD panel */
		 .slcd_cfg = SLCD_CFG_DWIDTH_16BIT | SLCD_CFG_CWIDTH_16BIT | SLCD_CFG_CS_ACTIVE_LOW | SLCD_CFG_RS_CMD_LOW | SLCD_CFG_CLK_ACTIVE_FALLING | SLCD_CFG_TYPE_PARALLEL,
		 .ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,	/* 16words burst, enable out FIFO underrun irq */
		 240, 320, 60, 0, 0, 0, 0, 0, 0,
	 },
	.osd = {
		 .osd_cfg = LCD_OSDC_OSDEN | /* Use OSD mode */
//		 LCD_OSDC_ALPHAEN | /* enable alpha */
//		 LCD_OSDC_F1EN | /* enable Foreground0 */
		 LCD_OSDC_F0EN,	/* enable Foreground0 */
		 .osd_ctrl = 0,		/* disable ipu,  */
		 .rgb_ctrl = 0,
		 .bgcolor = 0x000000, /* set background color Black */
		 .colorkey0 = 0, /* disable colorkey */
		 .colorkey1 = 0, /* disable colorkey */
		 .alpha = 0xA0,	/* alpha value */
		 .ipu_restart = 0x80001000, /* ipu restart */
		 .fg_change = FG_CHANGE_ALL, /* change all initially */
		 .fg0 = {32, 0, 0, 240, 320}, /* bpp, x, y, w, h */
		 .fg1 = {32, 0, 0, 240, 320}, /* bpp, x, y, w, h */
	 },

#elif defined(CONFIG_JZ4750_LCD_FOXCONN_PT035TN01)
	.panel = {
		.cfg = LCD_CFG_LCDPIN_LCD | LCD_CFG_RECOVER | /* Underrun recover */ 
		LCD_CFG_NEWDES | /* 8words descriptor */
		LCD_CFG_MODE_GENERIC_TFT | /* General TFT panel */
//		LCD_CFG_MODE_TFT_18BIT | 	/* output 18bpp */
		LCD_CFG_MODE_TFT_24BIT | 	/* output 24bpp */
		LCD_CFG_HSP | 	/* Hsync polarity: active low */
		LCD_CFG_VSP |	/* Vsync polarity: leading edge is falling edge */
		LCD_CFG_PCP,	/* Pix-CLK polarity: data translations at falling edge */
		.slcd_cfg = 0,
		.ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,	/* 16words burst, enable out FIFO underrun irq */
		320, 240, 80, 1, 1, 10, 50, 10, 13
	},
	.osd = {
		 .osd_cfg = LCD_OSDC_OSDEN | /* Use OSD mode */
//		 LCD_OSDC_ALPHAEN | /* enable alpha */
//		 LCD_OSDC_F1EN |	/* enable Foreground1 */
		 LCD_OSDC_F0EN,	/* enable Foreground0 */
		 .osd_ctrl = 0,		/* disable ipu,  */
		 .rgb_ctrl = 0,
		 .bgcolor = 0x000000, /* set background color Black */
		 .colorkey0 = 0, /* disable colorkey */
		 .colorkey1 = 0, /* disable colorkey */
		 .alpha = 0xA0,	/* alpha value */
		 .ipu_restart = 0x80001000, /* ipu restart */
		 .fg_change = FG_CHANGE_ALL, /* change all initially */
		 .fg0 = {32, 0, 0, 320, 240}, /* bpp, x, y, w, h */
		 .fg1 = {32, 0, 0, 320, 240}, /* bpp, x, y, w, h */
	 },
#elif defined(CONFIG_JZ4750_LCD_INNOLUX_PT035TN01_SERIAL)
	.panel = {
		.cfg = LCD_CFG_LCDPIN_LCD | LCD_CFG_RECOVER | /* Underrun recover */ 
		LCD_CFG_NEWDES | /* 8words descriptor */
		LCD_CFG_MODE_SERIAL_TFT | /* Serial TFT panel */
		LCD_CFG_MODE_TFT_18BIT | 	/* output 18bpp */
		LCD_CFG_HSP | 	/* Hsync polarity: active low */
		LCD_CFG_VSP |	/* Vsync polarity: leading edge is falling edge */
		LCD_CFG_PCP,	/* Pix-CLK polarity: data translations at falling edge */
		.slcd_cfg = 0,
		.ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,	/* 16words burst, enable out FIFO underrun irq */
		320, 240, 60, 1, 1, 10, 50, 10, 13
	},
	.osd = {
		 .osd_cfg = LCD_OSDC_OSDEN | /* Use OSD mode */
//		 LCD_OSDC_ALPHAEN | /* enable alpha */
		 LCD_OSDC_F0EN,	/* enable Foreground0 */
		 .osd_ctrl = 0,		/* disable ipu,  */
		 .rgb_ctrl = 0,
		 .bgcolor = 0x000000, /* set background color Black */
		 .colorkey0 = 0, /* disable colorkey */
		 .colorkey1 = 0, /* disable colorkey */
		 .alpha = 0xA0,	/* alpha value */
		 .ipu_restart = 0x80001000, /* ipu restart */
		 .fg_change = FG_CHANGE_ALL, /* change all initially */
		 .fg0 = {32, 0, 0, 320, 240}, /* bpp, x, y, w, h */
		 .fg1 = {32, 0, 0, 320, 240}, /* bpp, x, y, w, h */
	 },
#elif defined(CONFIG_JZ4750_SLCD_KGM701A3_TFT_SPFD5420A)
	.panel = {
//		 .cfg = LCD_CFG_LCDPIN_SLCD | LCD_CFG_RECOVER | /* Underrun recover*/ 
		 .cfg = LCD_CFG_LCDPIN_SLCD | /* Underrun recover*/ 
//		 LCD_CFG_DITHER | /* dither */
		 LCD_CFG_NEWDES | /* 8words descriptor */
		 LCD_CFG_MODE_SLCD, /* TFT Smart LCD panel */
		 .slcd_cfg = SLCD_CFG_DWIDTH_18BIT | SLCD_CFG_CWIDTH_18BIT | SLCD_CFG_CS_ACTIVE_LOW | SLCD_CFG_RS_CMD_LOW | SLCD_CFG_CLK_ACTIVE_FALLING | SLCD_CFG_TYPE_PARALLEL,
		 .ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,	/* 16words burst, enable out FIFO underrun irq */
		 400, 240, 60, 0, 0, 0, 0, 0, 0,
	 },
	.osd = {
		 .osd_cfg = LCD_OSDC_OSDEN | /* Use OSD mode */
//		 LCD_OSDC_ALPHAEN | /* enable alpha */
//		 LCD_OSDC_ALPHAMD | /* alpha blending mode */
//		 LCD_OSDC_F1EN | /* enable Foreground1 */
		 LCD_OSDC_F0EN,	/* enable Foreground0 */
		 .osd_ctrl = 0,		/* disable ipu,  */
		 .rgb_ctrl = 0,
		 .bgcolor = 0x000000, /* set background color Black */
		 .colorkey0 = 0, /* disable colorkey */
		 .colorkey1 = 0, /* disable colorkey */
		 .alpha = 0xA0,	/* alpha value */
		 .ipu_restart = 0x80001000, /* ipu restart */
		 .fg_change = FG_CHANGE_ALL, /* change all initially */
		 .fg0 = {32, 0, 0, 400, 240}, /* bpp, x, y, w, h */
		 .fg1 = {32, 0, 0, 400, 240}, /* bpp, x, y, w, h */
	 },
#else
#error "Select LCD panel first!!!"
#endif
};


/************************************************************************/

vidinfo_t panel_info = {
#if defined(CONFIG_JZ4750_LCD_SAMSUNG_LTP400WQF02)
	480, 272, LCD_BPP,
#elif defined(CONFIG_JZ4750_LCD_AUO_A043FL01V2)
	480, 272, LCD_BPP,
#elif defined(CONFIG_JZ4750_LCD_TRULY_TFT_GG1P0319LTSW_W)
	240, 320, LCD_BPP,
#elif defined(CONFIG_JZ4750_LCD_FOXCONN_PT035TN01)
	320, 240, LCD_BPP,
#elif defined(CONFIG_JZ4750_LCD_INNOLUX_PT035TN01_SERIAL)
	320, 240, LCD_BPP,
#elif defined(CONFIG_JZ4750_SLCD_KGM701A3_TFT_SPFD5420A)
	400, 240, LCD_BPP,
#else
#error "Select LCD panel first!!!"
#endif
//#if defined(CONFIG_JZLCD_SHARP_LQ035Q7)
//	240, 320, LCD_BPP,

};


/*----------------------------------------------------------------------*/

int lcd_line_length;

int lcd_color_fg;
int lcd_color_bg;

/*
 * Frame buffer memory information
 */
void *lcd_base;			/* Start of framebuffer memory	*/
void *lcd_console_address;	/* Start of console buffer	*/

short console_col;
short console_row;

/************************************************************************/

void lcd_ctrl_init (void *lcdbase);

void lcd_enable (void);
void lcd_disable (void);
#if defined(CONFIG_JZ4750D)
static int lcd_open(void);
#endif

/************************************************************************/

static int  jz_lcd_init_mem(void *lcdbase, vidinfo_t *vid);
static void jz_lcd_desc_init(vidinfo_t *vid);
static int  jz_lcd_hw_init( vidinfo_t *vid );
extern int flush_cache_all(void);

#if LCD_BPP == LCD_COLOR8
void lcd_setcolreg (ushort regno, ushort red, ushort green, ushort blue);
#endif
#if LCD_BPP == LCD_MONOCHROME
void lcd_initcolregs (void);
#endif

/************************************************************************/

void lcd_ctrl_init (void *lcdbase)
{

	__lcd_close_backlight();
	__lcd_display_pin_init();

	jz_lcd_init_mem(lcdbase, &panel_info);
	jz_lcd_desc_init(&panel_info);
	jz_lcd_hw_init(&panel_info);

	lcd_enable();
#if defined(CONFIG_JZ4750D)
	lcd_open();
#endif
	__lcd_display_on() ;
}

/*----------------------------------------------------------------------*/
#if LCD_BPP == LCD_COLOR8
void
lcd_setcolreg (ushort regno, ushort red, ushort green, ushort blue)
{
}
#endif
/*----------------------------------------------------------------------*/

#if LCD_BPP == LCD_MONOCHROME
static
void lcd_initcolregs (void)
{
}
#endif

/*----------------------------------------------------------------------*/

/*
 * Before enabled lcd controller, lcd registers should be configured correctly.
 */

void lcd_enable (void)
{
	__lcd_clr_dis();
	__lcd_set_ena();
}

/*----------------------------------------------------------------------*/


void lcd_disable (void)
{
	__lcd_set_dis();
	/* __lcd_clr_ena(); */  /* quikly disable */
}

/************************************************************************/


static int jz_lcd_init_mem(void *lcdbase, vidinfo_t *vid)
{
	u_long palette_mem_size;
	struct jz_fb_info *fbi = &vid->jz_fb;
	int fb_size = vid->vl_row * (vid->vl_col * NBITS (vid->vl_bpix)) / 8;

	fbi->screen = (u_long)lcdbase;
	fbi->palette_size = 256;
	palette_mem_size = fbi->palette_size * sizeof(u16);

	debug("palette_mem_size = 0x%08lx\n", (u_long) palette_mem_size);
	/* locate palette and descs at end of page following fb */
	fbi->palette = (u_long)lcdbase + fb_size + PAGE_SIZE - palette_mem_size;

	return 0;
}

static void jz_lcd_desc_init(vidinfo_t *vid)
{
	struct jz_fb_info * fbi;
	fbi = &vid->jz_fb;
	fbi->dmadesc_fblow = (struct jz_fb_dma_descriptor *)((unsigned int)fbi->palette - 3*32);
	fbi->dmadesc_fbhigh = (struct jz_fb_dma_descriptor *)((unsigned int)fbi->palette - 2*32);
	fbi->dmadesc_palette = (struct jz_fb_dma_descriptor *)((unsigned int)fbi->palette - 1*32);


//	#define BYTES_PER_PANEL	 (vid->vl_col * vid->vl_row * NBITS(vid->vl_bpix) / 8)
#define BYTES_PER_PANEL	 (((vid->vl_col * NBITS(vid->vl_bpix) / 8 + 3) >> 2 << 2) * vid->vl_row)

	/* populate descriptors */
	fbi->dmadesc_fblow->fdadr = virt_to_phys(fbi->dmadesc_fblow);
	fbi->dmadesc_fblow->fsadr = virt_to_phys((void *)(fbi->screen + BYTES_PER_PANEL));
	fbi->dmadesc_fblow->fidr  = 1;
	fbi->dmadesc_fblow->ldcmd = BYTES_PER_PANEL / 4 ;
	fbi->dmadesc_fblow->offsize = 0;
	fbi->dmadesc_fblow->page_width = 0;
	fbi->dmadesc_fblow->desc_size = jzfb.osd.fg1.h << 16 | jzfb.osd.fg1.w;
	REG_LCD_SIZE1 = (jzfb.osd.fg1.h<<16)|jzfb.osd.fg1.w;

	fbi->fdadr1 = virt_to_phys(fbi->dmadesc_fblow); /* only used in dual-panel mode */

	fbi->dmadesc_fbhigh->fsadr = virt_to_phys((void *)fbi->screen); 
	fbi->dmadesc_fbhigh->fidr = 0;
	fbi->dmadesc_fbhigh->ldcmd =  BYTES_PER_PANEL / 4; /* length in word */
	fbi->dmadesc_fbhigh->offsize = 0;
	fbi->dmadesc_fbhigh->page_width = 0;
	fbi->dmadesc_fbhigh->desc_size = jzfb.osd.fg0.h << 16 | jzfb.osd.fg0.w;
	REG_LCD_SIZE0 = jzfb.osd.fg0.h << 16|jzfb.osd.fg0.w;

	fbi->dmadesc_palette->fsadr = virt_to_phys((void *)fbi->palette);
	fbi->dmadesc_palette->fidr  = 0;
	fbi->dmadesc_palette->ldcmd = (fbi->palette_size * 2)/4 | (1<<28);

	if( NBITS(vid->vl_bpix) < 12)
	{
		/* assume any mode with <12 bpp is palette driven */
		fbi->dmadesc_palette->fdadr = virt_to_phys(fbi->dmadesc_fbhigh);
		fbi->dmadesc_fbhigh->fdadr = virt_to_phys(fbi->dmadesc_palette);
		/* flips back and forth between pal and fbhigh */
		fbi->fdadr0 = virt_to_phys(fbi->dmadesc_palette);
	}
	else
	{
		/* palette shouldn't be loaded in true-color mode */
		fbi->dmadesc_fbhigh->fdadr = virt_to_phys((void *)fbi->dmadesc_fbhigh);
		fbi->dmadesc_fblow->fdadr = virt_to_phys((void *)fbi->dmadesc_fblow);
		fbi->fdadr0 = virt_to_phys(fbi->dmadesc_fbhigh); /* no pal just fbhigh */
		fbi->fdadr1 = virt_to_phys(fbi->dmadesc_fblow); /* just fblow */
	}
//	print_lcdc_desc(fbi);
	flush_cache_all();
//	print_lcdc_desc(fbi);
}

#if defined(CONFIG_JZ4750D)
static int lcd_open(void)
{
 	unsigned int enable = 1, i;

	if(!(REG_LCD_CTRL & LCD_CTRL_ENA))
		REG_LCD_CTRL |= LCD_CTRL_ENA;

	for (i = 0; i < 10; i++) {
		if (!(REG_LCD_CTRL & LCD_CTRL_ENA)) {
			enable = 0;
			break;
		}
	}

	if (enable == 0) {
//		printf("Put CPU into hibernate mode.\n");
		jz_sync();
		REG_RTC_RCR = RTC_RCR_AIE | RTC_RCR_AE | RTC_RCR_RTCE;
		REG_RTC_HWRSR = 0;
		REG_RTC_HSPR = 0x12345678;
		REG_RTC_RSAR = REG_RTC_RSR + 2;
		REG_RTC_HWCR |= RTC_HWCR_EALM;
		REG_RTC_HRCR |= 0x0fe0;
		REG_RTC_HWFCR |= (0x00FF << 4);
	   	REG_RTC_HCR |= RTC_HCR_PD;
	}
	else {
		if (jzfb.panel.cfg & LCD_CFG_RECOVER) {
			REG_LCD_CTRL &= ~LCD_CTRL_ENA;
			REG_LCD_CFG |= LCD_CFG_RECOVER;
			REG_LCD_CTRL |= LCD_CTRL_ENA;
		}
	}
	return 0;
}
#endif
static int  jz_lcd_hw_init(vidinfo_t *vid)
{
	struct jz_fb_info *fbi = &vid->jz_fb;
	unsigned int val = 0;
	unsigned int pclk;


	/* Setting Control register */
	val = jzfb.panel.ctrl;

	switch (vid->vl_bpix) {
	case 0:
		val |= LCD_CTRL_BPP_1;
		break;
	case 1:
		val |= LCD_CTRL_BPP_2;
		break;
	case 2:
		val |= LCD_CTRL_BPP_4;
		break;
	case 3:
		val |= LCD_CTRL_BPP_8;
		break;
	case 4:
		val |= LCD_CTRL_BPP_16;
		break;
	case 5:
		val |= LCD_CTRL_BPP_18_24;	/* target is 4bytes/pixel */
		break;
	default:
		printf("The BPP %d is not supported\n", 1 << panel_info.vl_bpix);
		val |= LCD_CTRL_BPP_18_24;
		break;
	}


//	val |= LCD_CTRL_BST_16;		/* Burst Length is 16WORD=64Byte */
//	val |= LCD_CTRL_OFUP;		/* OutFIFO underrun protect */

	jzfb.panel.ctrl = val;
	REG_LCD_CTRL = val;

	switch ( jzfb.panel.cfg & LCD_CFG_MODE_MASK ) {
	case LCD_CFG_MODE_GENERIC_TFT:
	case LCD_CFG_MODE_INTER_CCIR656:
	case LCD_CFG_MODE_NONINTER_CCIR656:
	case LCD_CFG_MODE_SLCD:
	default:		/* only support TFT16 TFT32, not support STN and Special TFT by now(10-06-2008)*/
		REG_LCD_VAT = (((jzfb.panel.blw + jzfb.panel.w + jzfb.panel.elw + jzfb.panel.hsw)) << 16) | (jzfb.panel.vsw + jzfb.panel.bfw + jzfb.panel.h + jzfb.panel.efw);
		REG_LCD_DAH = ((jzfb.panel.hsw + jzfb.panel.blw) << 16) | (jzfb.panel.hsw + jzfb.panel.blw + jzfb.panel.w);
		REG_LCD_DAV = ((jzfb.panel.vsw + jzfb.panel.bfw) << 16) | (jzfb.panel.vsw + jzfb.panel.bfw + jzfb.panel.h);
		REG_LCD_HSYNC = (0 << 16) | jzfb.panel.hsw;
		REG_LCD_VSYNC = (0 << 16) | jzfb.panel.vsw;
		break;
	}
	/* Configure the LCD panel */
	REG_LCD_CFG = jzfb.panel.cfg;
	REG_LCD_OSDCTRL = jzfb.osd.osd_ctrl; /* IPUEN, bpp */
	REG_LCD_RGBC  	= jzfb.osd.rgb_ctrl;
	REG_LCD_BGC  	= jzfb.osd.bgcolor;
	REG_LCD_KEY0 	= jzfb.osd.colorkey0;
	REG_LCD_KEY1 	= jzfb.osd.colorkey1;
	REG_LCD_ALPHA 	= jzfb.osd.alpha;
	REG_LCD_IPUR 	= jzfb.osd.ipu_restart;

	/* Timing setting */
	__cpm_stop_lcd();

	val = jzfb.panel.fclk; /* frame clk */
	if ( (jzfb.panel.cfg & LCD_CFG_MODE_MASK) != LCD_CFG_MODE_SERIAL_TFT) {
		pclk = val * (jzfb.panel.w + jzfb.panel.hsw + jzfb.panel.elw + jzfb.panel.blw) * (jzfb.panel.h + jzfb.panel.vsw + jzfb.panel.efw + jzfb.panel.bfw); /* Pixclk */
	}
	else {
		/* serial mode: Hsync period = 3*Width_Pixel */
		pclk = val * (jzfb.panel.w*3 + jzfb.panel.hsw + jzfb.panel.elw + jzfb.panel.blw) * (jzfb.panel.h + jzfb.panel.vsw + jzfb.panel.efw + jzfb.panel.bfw); /* Pixclk */
	}

	val = __cpm_get_pllout2() / pclk; /* pclk */
	val--;
	if ( val > 0x7ff ) {
		printf("pixel clock divid is too large, set it to 0x7ff\n");
		val = 0x7ff;
	}
	
	__cpm_set_pixdiv(val);
#if defined(CONFIG_JZ4750)	
	val = pclk * 3 ;	/* LCDClock > 2.5*Pixclock */
	val =__cpm_get_pllout2() / val;
	if ( val > 0x1f ) {
		printf("lcd clock divide is too large, set it to 0x1f\n");
		val = 0x1f;
	}
	__cpm_set_ldiv( val );
#endif
	REG_CPM_CPCCR |= CPM_CPCCR_CE ; /* update divide */

	__cpm_start_lcd();
	udelay(1000);

	REG_LCD_DA0 = fbi->fdadr0; /* foreground 0 descripter*/
	REG_LCD_DA1 = fbi->fdadr1; /* foreground 1 descripter*/
	return 0;
}

#endif /* CONFIG_LCD */
#endif //CONFIG_JZ4750
