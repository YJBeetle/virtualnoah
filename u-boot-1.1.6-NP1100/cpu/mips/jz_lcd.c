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

#if defined(CONFIG_JZ4740) || defined(CONFIG_JZ4730)
#if defined(CONFIG_LCD) && !defined(CONFIG_SLCD)

#if defined(CONFIG_JZ4730)
#include <asm/jz4730.h>
#endif
#if defined(CONFIG_JZ4740)
#include <asm/jz4740.h>
#endif
#if defined(CONFIG_JZ5730)
#include <asm/jz5730.h>
#endif

#include "jz_lcd.h"



/************************************************************************/

struct jzfb_info {
	unsigned int cfg;	/* panel mode and pin usage etc. */
	unsigned int w;
	unsigned int h;
	unsigned int bpp;	/* bit per pixel */
	unsigned int fclk;	/* frame clk */
	unsigned int hsw;	/* hsync width, in pclk */
	unsigned int vsw;	/* vsync width, in line count */
	unsigned int elw;	/* end of line, in pclk */
	unsigned int blw;	/* begin of line, in pclk */
	unsigned int efw;	/* end of frame, in line count */
	unsigned int bfw;	/* begin of frame, in line count */
};


static struct jzfb_info jzfb = {
#if defined(CONFIG_JZLCD_SHARP_LQ035Q7)
	MODE_TFT_SHARP | PCLK_N | VSYNC_N,
	240, 320, 16, 60, 1, 2, 1, 2, 0, 6
#endif
#if defined(CONFIG_JZLCD_SAMSUNG_LTS350Q1)
	MODE_TFT_SAMSUNG | PCLK_N,
	240, 320, 16, 60, 1, 2, (254-240), 0, 7, 0
#endif
#if defined(CONFIG_JZLCD_SAMSUNG_LTV350QVF04)
	MODE_TFT_GEN | HSYNC_N | VSYNC_N,
	320, 240, 16, 70, 19, 4, 20, 14, 18, 6
#endif
#if defined(CONFIG_JZLCD_SAMSUNG_LTP400WQF01)
	MODE_TFT_GEN | HSYNC_N | VSYNC_N,
	480, 272, 16, 60, 41, 10, 2, 2, 2, 2
#endif
#if defined(CONFIG_JZLCD_SAMSUNG_LTP400WQF02_18BIT)
	MODE_TFT_GEN | MODE_TFT_18BIT | HSYNC_N | VSYNC_N, /* MODE_TFT_18BIT: JZ4740@ version */
	480, 272, 32, 60, 41, 10, 2, 2, 2, 2
#endif
#if defined(CONFIG_JZLCD_TRULY_TFTG320240DTSW)
	MODE_TFT_GEN | HSYNC_N | VSYNC_N | PCLK_N,
	320, 240, 16, 85, 30, 3, 38, 20, 11, 8
#endif
#if defined(CONFIG_JZLCD_TRULY_TFTG320240DTSW_SERIAL)
	MODE_8BIT_SERIAL_TFT | HSYNC_N | VSYNC_N | PCLK_N,
	/* serial mode 280 lines, parallel mode 240 lines */
	320, 280, 32, 60, (30*3), 3, (20*3), (38*3), 46, 23 
#endif
#if defined(CONFIG_JZLCD_TRULY_TFTG240320UTSW_63W_E)
	MODE_TFT_GEN | HSYNC_N | VSYNC_N | PCLK_N | DE_N,
	//240, 320, 16, 60, 3, 3, 3, 3, 3, 3 /* 240x320 */
	320, 240, 16, 60, 3, 3, 3, 3, 3, 85 /* 320x240 */
#endif
#if defined(CONFIG_JZLCD_HYNIX_HT10X21)
	MODE_TFT_GEN | PCLK_N,
	1024, 768, 16, 45, 1, 1, 75, 0, 3, 0
#endif
#if defined(CONFIG_JZLCD_TOSHIBA_LTM084P363)
	MODE_TFT_GEN | PCLK_N,
	800, 600, 16, 50, 1, 2, 199, 0, 2, 0
#endif
#if defined(CONFIG_JZLCD_FOXCONN_PT035TN01) && (!defined(CONFIG_PAVO))
	MODE_TFT_GEN | HSYNC_N | VSYNC_N | PCLK_N,
	320, 240, 16, 110, 1, 1, 10, 50, 10, 13
#endif
#if defined(CONFIG_JZLCD_FOXCONN_PT035TN01) && defined(CONFIG_PAVO)
	MODE_TFT_GEN | HSYNC_N | VSYNC_N | MODE_TFT_18BIT | PCLK_N,
	320, 240, 18, 110, 1, 1, 10, 50, 10, 13
#endif
#if defined(CONFIG_JZLCD_INNOLUX_PT035TN01_SERIAL)
	MODE_8BIT_SERIAL_TFT | PCLK_N | HSYNC_N | VSYNC_N,
	320, 240, 32, 60, 1, 1, 10, 50, 10, 13
#endif
#if defined(CONFIG_JZLCD_INNOLUX_AT080TN42)
	MODE_TFT_SHARP | PCLK_N,
	800, 600, 16, 50, 1, 1, 255, 0, 34, 0 
#endif
#if defined(CONFIG_JZLCD_CSTN_800x600)
	MODE_STN_COLOR_DUAL | STN_DAT_PIN8,
	800, 600, 16, 30, 8, 1, 0, 0, 0, 0
#endif
#if defined(CONFIG_JZLCD_CSTN_320x240)
	MODE_STN_COLOR_SINGLE | STN_DAT_PIN8,
	320, 240, 16, 120, 8, 1, 8, 0, 0, 0
#endif
#if defined(CONFIG_JZLCD_MSTN_640x480)
	MODE_STN_MONO_DUAL | STN_DAT_PIN4,
	640, 480, 8, 110, 4, 1, 4, 0, 0, 0
#endif
#if defined(CONFIG_JZLCD_MSTN_320x240)
	MODE_STN_MONO_SINGLE | STN_DAT_PIN4,
	320, 240, 8, 110, 4, 1, 4, 0, 0, 0
#endif
#if defined(CONFIG_JZLCD_MSTN_480x320)
	MODE_STN_MONO_SINGLE | STN_DAT_PIN8
#if defined(CONFIG_JZLCD_MSTN_INVERSE)
	| DATA_INVERSE
#endif
	, 480, 320, 8, 65, 8, 1, 8, 0, 0, 0
#endif
#if defined(CONFIG_JZLCD_MSTN_240x128)
	MODE_STN_MONO_SINGLE | STN_DAT_PIN1
#if defined(CONFIG_JZLCD_MSTN_INVERSE)
	| DATA_INVERSE
#endif
	, 240, 128, 8, 100, 1, 1, 1, 0, 0, 0 
#endif
};

/************************************************************************/

vidinfo_t panel_info = {
#if defined(CONFIG_JZLCD_SHARP_LQ035Q7)
	240, 320, LCD_BPP,
#endif
#if defined(CONFIG_JZLCD_SAMSUNG_LTS350Q1)
	240, 320, LCD_BPP,
#endif
#if defined(CONFIG_JZLCD_SAMSUNG_LTV350QVF04)
	320, 240, LCD_BPP,
#endif
#if defined(CONFIG_JZLCD_SAMSUNG_LTP400WQF01)
	480, 272, LCD_BPP,
#endif
#if defined(CONFIG_JZLCD_SAMSUNG_LTP400WQF02_18BIT)
	480, 272, LCD_BPP,
#endif
#if defined(CONFIG_JZLCD_TRULY_TFTG320240DTSW)
	320, 240, LCD_BPP,
#endif
#if defined(CONFIG_JZLCD_TRULY_TFTG320240DTSW_SERIAL)
	320, 280, LCD_BPP,
#endif
#if defined(CONFIG_JZLCD_TRULY_TFTG240320UTSW_63W_E)
	320, 240, LCD_BPP,
#endif
#if defined(CONFIG_JZLCD_FOXCONN_PT035TN01)
	320, 240, LCD_BPP,
#endif
#if defined(CONFIG_JZLCD_INNOLUX_PT035TN01_SERIAL)
	320, 240, LCD_BPP,
#endif
#if defined(CONFIG_JZLCD_HYNIX_HT10X21)
	1024, 768, LCD_BPP,
#endif
#if defined(CONFIG_JZLCD_TOSHIBA_LTM084P363)
	800, 600, LCD_BPP,
#endif
#if defined(CONFIG_JZLCD_INNOLUX_AT080TN42)
	800, 600, LCD_BPP,
#endif
#if defined(CONFIG_JZLCD_CSTN_800x600)
	800, 600, LCD_BPP,
#endif
#if defined(CONFIG_JZLCD_CSTN_320x240)
	320, 240, LCD_BPP,
#endif
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
	__lcd_display_pin_init();

	jz_lcd_init_mem(lcdbase, &panel_info);
	jz_lcd_desc_init(&panel_info);
	jz_lcd_hw_init(&panel_info);

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
	REG_LCD_CTRL &= ~(1<<4); /* LCDCTRL.DIS */
	REG_LCD_CTRL |= 1<<3;    /* LCDCTRL.ENA*/
}

/*----------------------------------------------------------------------*/


void lcd_disable (void)
{
	REG_LCD_CTRL |= (1<<4); /* LCDCTRL.DIS, regular disable */
	/* REG_LCD_CTRL |= (1<<3); */  /* LCDCTRL.DIS, quikly disable */
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
	fbi->dmadesc_fblow = (struct jz_fb_dma_descriptor *)((unsigned int)fbi->palette - 3*16);
	fbi->dmadesc_fbhigh = (struct jz_fb_dma_descriptor *)((unsigned int)fbi->palette - 2*16);
	fbi->dmadesc_palette = (struct jz_fb_dma_descriptor *)((unsigned int)fbi->palette - 1*16);


	#define BYTES_PER_PANEL	 (vid->vl_col * vid->vl_row * NBITS(vid->vl_bpix) / 8)

	/* populate descriptors */
	fbi->dmadesc_fblow->fdadr = virt_to_phys(fbi->dmadesc_fblow);
	fbi->dmadesc_fblow->fsadr = virt_to_phys((void *)(fbi->screen + BYTES_PER_PANEL));
	fbi->dmadesc_fblow->fidr  = 0;
	fbi->dmadesc_fblow->ldcmd = BYTES_PER_PANEL / 4 ;

	fbi->fdadr1 = virt_to_phys(fbi->dmadesc_fblow); /* only used in dual-panel mode */

	fbi->dmadesc_fbhigh->fsadr = virt_to_phys((void *)fbi->screen); 
	fbi->dmadesc_fbhigh->fidr = 0;
	fbi->dmadesc_fbhigh->ldcmd =  BYTES_PER_PANEL / 4; /* length in word */

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
		fbi->fdadr0 = virt_to_phys(fbi->dmadesc_fbhigh); /* no pal just fbhigh */
	}

	flush_cache_all();
}

static int  jz_lcd_hw_init(vidinfo_t *vid)
{
	struct jz_fb_info *fbi = &vid->jz_fb;
	unsigned int val = 0;
	unsigned int pclk;
	unsigned int stnH;
#if defined(CONFIG_MIPS_JZ4740)
	int pll_div;
#endif

	/* Setting Control register */
	switch (jzfb.bpp) {
	case 1:
		val |= LCD_CTRL_BPP_1;
		break;
	case 2:
		val |= LCD_CTRL_BPP_2;
		break;
	case 4:
		val |= LCD_CTRL_BPP_4;
		break;
	case 8:
		val |= LCD_CTRL_BPP_8;
		break;
	case 15:
		val |= LCD_CTRL_RGB555;
	case 16:
		val |= LCD_CTRL_BPP_16;
		break;
#if defined(CONFIG_MIPS_JZ4740)
	case 17 ... 32:
		val |= LCD_CTRL_BPP_18_24;	/* target is 4bytes/pixel */
		break;
#endif
	default:
		printf("The BPP %d is not supported\n", jzfb.bpp);
		val |= LCD_CTRL_BPP_16;
		break;
	}

	switch (jzfb.cfg & MODE_MASK) {
	case MODE_STN_MONO_DUAL:
	case MODE_STN_COLOR_DUAL:
	case MODE_STN_MONO_SINGLE:
	case MODE_STN_COLOR_SINGLE:
		switch (jzfb.bpp) {
		case 1:
			//val |= LCD_CTRL_PEDN;
		case 2:
			val |= LCD_CTRL_FRC_2;
			break;
		case 4:
			val |= LCD_CTRL_FRC_4;
			break;
		case 8:
		default:
			val |= LCD_CTRL_FRC_16;
			break;
		}
		break;
	}

	val |= LCD_CTRL_BST_16;		/* Burst Length is 16WORD=64Byte */
	val |= LCD_CTRL_OFUP;		/* OutFIFO underrun protect */

	switch (jzfb.cfg & MODE_MASK) {
	case MODE_STN_MONO_DUAL:
	case MODE_STN_COLOR_DUAL:
	case MODE_STN_MONO_SINGLE:
	case MODE_STN_COLOR_SINGLE:
		switch (jzfb.cfg & STN_DAT_PINMASK) {
#define align2(n) (n)=((((n)+1)>>1)<<1)
#define align4(n) (n)=((((n)+3)>>2)<<2)
#define align8(n) (n)=((((n)+7)>>3)<<3)
		case STN_DAT_PIN1:
			/* Do not adjust the hori-param value. */
			break;
		case STN_DAT_PIN2:
			align2(jzfb.hsw);
			align2(jzfb.elw);
			align2(jzfb.blw);
			break;
		case STN_DAT_PIN4:
			align4(jzfb.hsw);
			align4(jzfb.elw);
			align4(jzfb.blw);
			break;
		case STN_DAT_PIN8:
			align8(jzfb.hsw);
			align8(jzfb.elw);
			align8(jzfb.blw);
			break;
		}
		break;
	}

	REG_LCD_CTRL = val;

	switch (jzfb.cfg & MODE_MASK) {
	case MODE_STN_MONO_DUAL:
	case MODE_STN_COLOR_DUAL:
	case MODE_STN_MONO_SINGLE:
	case MODE_STN_COLOR_SINGLE:
		if (((jzfb.cfg & MODE_MASK) == MODE_STN_MONO_DUAL) ||
		    ((jzfb.cfg & MODE_MASK) == MODE_STN_COLOR_DUAL))
			stnH = jzfb.h >> 1;
		else
			stnH = jzfb.h;

		REG_LCD_VSYNC = (0 << 16) | jzfb.vsw;
		REG_LCD_HSYNC = ((jzfb.blw+jzfb.w) << 16) | (jzfb.blw+jzfb.w+jzfb.hsw);

		/* Screen setting */
		REG_LCD_VAT = ((jzfb.blw + jzfb.w + jzfb.hsw + jzfb.elw) << 16) | (stnH + jzfb.vsw + jzfb.bfw + jzfb.efw);
		REG_LCD_DAH = (jzfb.blw << 16) | (jzfb.blw + jzfb.w);
		REG_LCD_DAV = (0 << 16) | (stnH);

		/* AC BIAs signal */
		REG_LCD_PS = (0 << 16) | (stnH+jzfb.vsw+jzfb.efw+jzfb.bfw);

		break;

	case MODE_TFT_GEN:
	case MODE_TFT_SHARP:
	case MODE_TFT_CASIO:
	case MODE_TFT_SAMSUNG:
	case MODE_8BIT_SERIAL_TFT:
	case MODE_TFT_18BIT:
		REG_LCD_VSYNC = (0 << 16) | jzfb.vsw;
		REG_LCD_HSYNC = (0 << 16) | jzfb.hsw;
#if defined(CONFIG_JZLCD_INNOLUX_AT080TN42)
		REG_LCD_DAV = (0 << 16) | ( jzfb.h );
#else
		REG_LCD_DAV =((jzfb.vsw+jzfb.bfw) << 16) | (jzfb.vsw +jzfb.bfw+jzfb.h);
#endif /*#if defined(CONFIG_JZLCD_INNOLUX_AT080TN42)*/
		REG_LCD_DAH = ((jzfb.hsw + jzfb.blw) << 16) | (jzfb.hsw + jzfb.blw + jzfb.w );
		REG_LCD_VAT = (((jzfb.blw + jzfb.w + jzfb.elw + jzfb.hsw)) << 16) \
			| (jzfb.vsw + jzfb.bfw + jzfb.h + jzfb.efw);
		break;
	}

	switch (jzfb.cfg & MODE_MASK) {
	case MODE_TFT_SAMSUNG:
	{
		unsigned int total, tp_s, tp_e, ckv_s, ckv_e;
		unsigned int rev_s, rev_e, inv_s, inv_e;

		pclk = val * (jzfb.w + jzfb.hsw + jzfb.elw + jzfb.blw) *
			(jzfb.h + jzfb.vsw + jzfb.efw + jzfb.bfw); /* Pixclk */

		total = jzfb.blw + jzfb.w + jzfb.elw + jzfb.hsw;
		tp_s = jzfb.blw + jzfb.w + 1;
		tp_e = tp_s + 1;
		//ckv_s = tp_s - jz_clocks.pixclk/(1000000000/4100);
		ckv_s = tp_s - pclk/(1000000000/4100);
		ckv_e = tp_s + total;
		rev_s = tp_s - 11;	/* -11.5 clk */
		rev_e = rev_s + total;
		inv_s = tp_s;
		inv_e = inv_s + total;
		REG_LCD_CLS = (tp_s << 16) | tp_e;
		REG_LCD_PS = (ckv_s << 16) | ckv_e;
		REG_LCD_SPL = (rev_s << 16) | rev_e;
		REG_LCD_REV = (inv_s << 16) | inv_e;
		jzfb.cfg |= STFT_REVHI | STFT_SPLHI;
		break;
	}
	case MODE_TFT_SHARP:
	{
		unsigned int total, cls_s, cls_e, ps_s, ps_e;
		unsigned int spl_s, spl_e, rev_s, rev_e;
		total = jzfb.blw + jzfb.w + jzfb.elw + jzfb.hsw;
#if !defined(CONFIG_JZLCD_INNOLUX_AT080TN42)
		spl_s = 1;
		spl_e = spl_s + 1;
		cls_s = 0;
		cls_e = total - 60;	/* > 4us (pclk = 80ns) */
		ps_s = cls_s;
		ps_e = cls_e;
		rev_s = total - 40;	/* > 3us (pclk = 80ns) */
		rev_e = rev_s + total;
		jzfb.cfg |= STFT_PSHI; 
#else           /*#if defined(CONFIG_JZLCD_INNOLUX_AT080TN42)*/
		spl_s = total - 5; /* LD */
		spl_e = total -3;
		cls_s = 32;	/* CKV */
		cls_e = 145;
		ps_s  = 0;      /* OEV */
		ps_e  = 45;
		rev_s = 0;	/* POL */
		rev_e = 0;
#endif          /*#if defined(CONFIG_JZLCD_INNOLUX_AT080TN42)*/
		REG_LCD_SPL = (spl_s << 16) | spl_e;
		REG_LCD_CLS = (cls_s << 16) | cls_e;
		REG_LCD_PS = (ps_s << 16) | ps_e;
		REG_LCD_REV = (rev_s << 16) | rev_e;
		break;
	}
	case MODE_TFT_CASIO:
		break;
	}

	/* Configure the LCD panel */
	REG_LCD_CFG = jzfb.cfg;

	/* Timing setting */
	__cpm_stop_lcd();

	val = jzfb.fclk; /* frame clk */
	if ( (jzfb.cfg & MODE_MASK) != MODE_8BIT_SERIAL_TFT) {
		pclk = val * (jzfb.w + jzfb.hsw + jzfb.elw + jzfb.blw) *
			(jzfb.h + jzfb.vsw + jzfb.efw + jzfb.bfw); /* Pixclk */
	}
	else {
		/* serial mode: Hsync period = 3*Width_Pixel */
		pclk = val * (jzfb.w*3 + jzfb.hsw + jzfb.elw + jzfb.blw) *
			(jzfb.h + jzfb.vsw + jzfb.efw + jzfb.bfw); /* Pixclk */
	}

	if (((jzfb.cfg & MODE_MASK) == MODE_STN_COLOR_SINGLE) ||
	    ((jzfb.cfg & MODE_MASK) == MODE_STN_COLOR_DUAL))
		pclk = (pclk * 3);

	if (((jzfb.cfg & MODE_MASK) == MODE_STN_COLOR_SINGLE) ||
	    ((jzfb.cfg & MODE_MASK) == MODE_STN_COLOR_DUAL) ||
	    ((jzfb.cfg & MODE_MASK) == MODE_STN_MONO_SINGLE) ||
	    ((jzfb.cfg & MODE_MASK) == MODE_STN_MONO_DUAL))
		pclk = pclk >> ((jzfb.cfg & STN_DAT_PINMASK) >> 4);

	if (((jzfb.cfg & MODE_MASK) == MODE_STN_COLOR_DUAL) ||
	    ((jzfb.cfg & MODE_MASK) == MODE_STN_MONO_DUAL))
		pclk >>= 1;

#if defined(CONFIG_MIPS_JZ4730)
	val = __cpm_get_pllout() / pclk;
	REG_CPM_CFCR2 = val - 1;
	val = pclk * 4 ;
	if ( val > 150000000 ) {
		printf("Warning: LCDClock=%d\n, LCDClock must less or equal to 150MHz.\n", val);
		printf("Change LCDClock to 150MHz\n");
		val = 150000000;
	}
	val = __cpm_get_pllout() / val;
	val--;
	if ( val > 0xF ) 
		val = 0xF;
	__cpm_set_lcdclk_div(val);
	REG_CPM_CFCR |= CPM_CFCR_UPE;

#elif defined(CONFIG_MIPS_JZ4740)
	pll_div = ( REG_CPM_CPCCR & CPM_CPCCR_PCS ); /* clock source,0:pllout/2 1: pllout */
	pll_div = pll_div ? 1 : 2 ;
	val = ( __cpm_get_pllout()/pll_div ) / pclk;
	val--;
	if ( val > 0x1ff ) {
		printf("CPM_LPCDR too large, set it to 0x1ff\n");
		val = 0x1ff;
	}
	__cpm_set_pixdiv(val);

	val = pclk * 3 ;	/* LCDClock > 2.5*Pixclock */
	if ( val > 150000000 ) {
		printf("Warning: LCDClock=%d\n, LCDClock must less or equal to 150MHz.\n", val);
		printf("Change LCDClock to 150MHz\n");
		val = 150000000;
	}
	val = ( __cpm_get_pllout()/pll_div ) / val;
	val--;
	if ( val > 0x1f ) {
		printf("CPM_CPCCR.LDIV too large, set it to 0x1f\n");
		val = 0x1f;
	}
	__cpm_set_ldiv( val );
	REG_CPM_CPCCR |= CPM_CPCCR_CE ; /* update divide */

#else
#error "drivers/video/Jzlcd.c, CONFIG_MIPS_JZ4730, please set chip type."
#endif /*#ifdef CONFIG_MIPS_JZ4730 */

	__cpm_start_lcd();
	udelay(1000);

	REG_LCD_DA0 = fbi->fdadr0; /* frame descripter*/

	if (((jzfb.cfg & MODE_MASK) == MODE_STN_COLOR_DUAL) ||
	    ((jzfb.cfg & MODE_MASK) == MODE_STN_MONO_DUAL))
		REG_LCD_DA1 = fbi->fdadr1; /* frame descripter*/

	return 0;
}
#endif /* CONFIG_LCD */
#endif /* Jz4730 && Jz4740*/
