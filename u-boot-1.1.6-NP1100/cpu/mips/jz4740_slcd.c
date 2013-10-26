/*
 * JzRISC Smart lcd controller
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
 *  LCD_BPP                           : Bits per pixel, 0 = 1, 1 = 2, 2 = 4, 3 = 8,
 *							4 = 16, 5 = 32
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

#if defined(CONFIG_LCD) && defined(CONFIG_SLCD)

#if defined(CONFIG_JZ4740)
#include <asm/jz4740.h>
#endif


#include "jz4740_slcd.h"


/************************************************************************/


struct jzfb_info {
	unsigned int cfg;	/* panel mode and pin usage etc. */
	unsigned int w;
	unsigned int h;
	unsigned int bpp;	/* bit per pixel */
	unsigned int bus;
	unsigned int fclk;	/* frame clock */
};

static struct jzfb_info jzfb = {
#ifdef CONFIG_JZ_SLCD_LGDP4551
	SLCD_CFG_CS_ACTIVE_LOW | SLCD_CFG_RS_CMD_LOW | SLCD_CFG_TYPE_PARALLEL,
	400, 240, 16, 8, 60 	/*16 bpp, 8 bus*/
#endif

#ifdef CONFIG_JZ_SLCD_SPFD5420A
	SLCD_CFG_CS_ACTIVE_LOW | SLCD_CFG_RS_CMD_LOW | SLCD_CFG_TYPE_PARALLEL,
	400, 240, 18, 18, 60 	/*18 bpp, 18 bus*/
#endif

#ifdef CONFIG_JZ_SLCD_SPFD5408A
	SLCD_CFG_CS_ACTIVE_LOW | SLCD_CFG_RS_CMD_LOW | SLCD_CFG_TYPE_PARALLEL,
	320, 240, 16, 16, 60 	/*16 bpp, 16 bus*/
#endif
};

/************************************************************************/

vidinfo_t panel_info = {
#ifdef CONFIG_JZ_SLCD_LGDP4551
	400, 240, LCD_BPP,
#endif

#ifdef CONFIG_JZ_SLCD_SPFD5420A
	400, 240, LCD_BPP,
#endif

#ifdef CONFIG_JZ_SLCD_SPFD5408A
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
void flush_screen(void *lcdbase);

/************************************************************************/
static void Mcupanel_RegSet(UINT32 cmd, UINT32 data);
static void Mcupanel_Command(UINT32 cmd);
static void Mcupanel_Data(UINT32 data);

/************************************************************************/
static int  jz_lcd_init_mem(void *lcdbase, vidinfo_t *vid);
static void jz_lcd_desc_init(vidinfo_t *vid);
static int  jz_lcd_hw_init( vidinfo_t *vid );
static int slcd_dma_init(vidinfo_t *vid);

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
	jz_lcd_init_mem(lcdbase, &panel_info);
	jz_lcd_hw_init(&panel_info);
	jz_lcd_desc_init(&panel_info);
	__slcd_display_on();
	slcd_dma_init(&panel_info);
//	flush_screen(lcdbase); 	/* If use cpu mode, call this function to flush screen */
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

/* Sent a command and data */
static void Mcupanel_RegSet(UINT32 cmd, UINT32 data)
{
	switch (jzfb.bus) {
	case 8:
		while (REG_SLCD_STATE & SLCD_STATE_BUSY);
		REG_SLCD_DATA = SLCD_DATA_RS_COMMAND | ((cmd&0xff00) >> 8);
		while (REG_SLCD_STATE & SLCD_STATE_BUSY);
		REG_SLCD_DATA = SLCD_DATA_RS_COMMAND | ((cmd&0xff) >> 0);
		while (REG_SLCD_STATE & SLCD_STATE_BUSY);
		REG_SLCD_DATA = SLCD_DATA_RS_DATA | (data&0xffff);
		break;
	case 9:
		data = ((data & 0xff) << 1) | ((data & 0xff00) << 2);
		data = ((data << 6) & 0xfc0000) | ((data << 4) & 0xfc00) | ((data << 2) & 0xfc);
		while (REG_SLCD_STATE & SLCD_STATE_BUSY);
		REG_SLCD_DATA = SLCD_DATA_RS_COMMAND | ((cmd&0xff00) >> 8);
		while (REG_SLCD_STATE & SLCD_STATE_BUSY);
		REG_SLCD_DATA = SLCD_DATA_RS_COMMAND | ((cmd&0xff) >> 0);
		while (REG_SLCD_STATE & SLCD_STATE_BUSY);
		REG_SLCD_DATA = SLCD_DATA_RS_DATA | data;
		break;
	case 16:
		while (REG_SLCD_STATE & SLCD_STATE_BUSY);
		REG_SLCD_DATA = SLCD_DATA_RS_COMMAND | (cmd&0xffff);
		while (REG_SLCD_STATE & SLCD_STATE_BUSY);
		REG_SLCD_DATA = SLCD_DATA_RS_DATA | (data&0xffff);
		break;
	case 18:
		cmd = ((cmd & 0xff) << 1) | ((cmd & 0xff00) << 2); 	
 		data = ((data & 0xff) << 1) | ((data & 0xff00) << 2);
 		while (REG_SLCD_STATE & SLCD_STATE_BUSY);
		REG_SLCD_DATA = SLCD_DATA_RS_COMMAND | cmd;
		while (REG_SLCD_STATE & SLCD_STATE_BUSY);
		REG_SLCD_DATA = SLCD_DATA_RS_DATA | ((data<<6)&0xfc0000)|((data<<4)&0xfc00) | ((data<<2)&0xfc);
		break;
	default:
		printf("Don't support %d bit Bus\n", jzfb.bus );
		break;
	}
}

/* Sent a command only */
static void Mcupanel_Command(UINT32 cmd) {
	switch (jzfb.bus) {
	case 8:
	case 9:
		while (REG_SLCD_STATE & SLCD_STATE_BUSY);
		REG_SLCD_DATA = SLCD_DATA_RS_COMMAND | ((cmd&0xff00) >> 8);
		while (REG_SLCD_STATE & SLCD_STATE_BUSY);
		REG_SLCD_DATA = SLCD_DATA_RS_COMMAND | ((cmd&0xff) >> 0);
		break;
	case 16:
		while (REG_SLCD_STATE & SLCD_STATE_BUSY);
		REG_SLCD_DATA = SLCD_DATA_RS_COMMAND | (cmd&0xffff);
		break;
	case 18:
		while (REG_SLCD_STATE & SLCD_STATE_BUSY);
		REG_SLCD_DATA = SLCD_DATA_RS_COMMAND | ((cmd&0xff00) << 2) | ((cmd&0xff) << 1);
		break;
	default:
		printf("Don't support %d bit Bus\n", jzfb.bus );
		break;
	}
}

/* Sent data only */
static void Mcupanel_Data(UINT32 data) {

	switch (jzfb.bpp) {
	case 8:
		while (REG_SLCD_STATE & SLCD_STATE_BUSY);
		REG_SLCD_DATA = SLCD_DATA_RS_DATA | (((unsigned int)data >> 8)& 0xff);
		break;
		while (REG_SLCD_STATE & SLCD_STATE_BUSY);
		REG_SLCD_DATA = SLCD_DATA_RS_DATA | (((unsigned int)data >> 0) & 0xff);
		break;
	case 15:
	case 16:
		while (REG_SLCD_STATE & SLCD_STATE_BUSY);
		REG_SLCD_DATA = SLCD_DATA_RS_DATA | ((unsigned int)data & 0xffff);
		break;
	case 32:
	default:
		while (REG_SLCD_STATE & SLCD_STATE_BUSY);
		REG_SLCD_DATA = SLCD_DATA_RS_DATA | ((unsigned int)data & 0xffffff);
		break;
	}
}

/*
 * Before enabled lcd controller, lcd registers should be configured correctly.
 */

void lcd_enable (void)
{
}

/*----------------------------------------------------------------------*/


void lcd_disable (void)
{
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
	unsigned int next, pal_size, frm_size;

	fbi = &vid->jz_fb;
	fbi->dmadesc_fblow = (struct jz_fb_dma_descriptor *)((unsigned int)fbi->palette - 3*16);
	fbi->dmadesc_fbhigh = (struct jz_fb_dma_descriptor *)((unsigned int)fbi->palette - 2*16);
	fbi->dmadesc_palette = (struct jz_fb_dma_descriptor *)((unsigned int)fbi->palette - 1*16);


	#define BYTES_PER_PANEL	 (vid->vl_col * vid->vl_row * NBITS(vid->vl_bpix) / 8)

	/*Prepare Frame Descriptor in memory*/
	switch (jzfb.bpp) {
	case 8 ... 16:
		fbi->dmadesc_fblow->dcmd  = DMAC_DCMD_SAI | DMAC_DCMD_RDIL_IGN | DMAC_DCMD_SWDH_32
			| DMAC_DCMD_DWDH_16 | DMAC_DCMD_DS_16BYTE | DMAC_DCMD_TM | DMAC_DCMD_DES_V
			| DMAC_DCMD_DES_VIE | DMAC_DCMD_LINK;
	break;

	case 17 ... 32:
		fbi->dmadesc_fblow->dcmd  = DMAC_DCMD_SAI | DMAC_DCMD_RDIL_IGN | DMAC_DCMD_SWDH_32
			| DMAC_DCMD_DWDH_32 | DMAC_DCMD_DS_32BYTE | DMAC_DCMD_TM | DMAC_DCMD_DES_V
			| DMAC_DCMD_DES_VIE | DMAC_DCMD_LINK;
		break;
	}
	fbi->dmadesc_fblow->dsadr = virt_to_phys((void *)(fbi->screen + BYTES_PER_PANEL));
	fbi->dmadesc_fblow->dtadr  = virt_to_phys((void *)SLCD_FIFO);

	switch (jzfb.bpp) {
	case 8 ... 16:
		fbi->dmadesc_fbhigh->dcmd  = DMAC_DCMD_SAI | DMAC_DCMD_RDIL_IGN | DMAC_DCMD_SWDH_32
			| DMAC_DCMD_DWDH_16 | DMAC_DCMD_DS_16BYTE | DMAC_DCMD_TM | DMAC_DCMD_DES_V
			| DMAC_DCMD_DES_VIE | DMAC_DCMD_LINK;
	break;

	case 17 ... 32:
		fbi->dmadesc_fbhigh->dcmd  = DMAC_DCMD_SAI | DMAC_DCMD_RDIL_IGN | DMAC_DCMD_SWDH_32
			| DMAC_DCMD_DWDH_32 | DMAC_DCMD_DS_32BYTE | DMAC_DCMD_TM | DMAC_DCMD_DES_V
			| DMAC_DCMD_DES_VIE | DMAC_DCMD_LINK;
		break;
	}

	fbi->dmadesc_fbhigh->dsadr = virt_to_phys((void *)(fbi->screen));
	fbi->dmadesc_fbhigh->dtadr  = virt_to_phys((void *)SLCD_FIFO);

	/* palette descriptor */
	fbi->dmadesc_palette->dcmd = DMAC_DCMD_SAI | DMAC_DCMD_RDIL_IGN | DMAC_DCMD_SWDH_32
		| DMAC_DCMD_DWDH_16 | DMAC_DCMD_DS_16BYTE | DMAC_DCMD_TM | DMAC_DCMD_DES_V 
		| DMAC_DCMD_DES_VIE | DMAC_DCMD_LINK;
	fbi->dmadesc_palette->dsadr = virt_to_phys((void *)fbi->palette);
	fbi->dmadesc_palette->dtadr  = virt_to_phys((void *)SLCD_FIFO);

	pal_size = fbi->palette_size;
	frm_size = BYTES_PER_PANEL;

	switch (fbi->dmadesc_palette->dcmd & DMAC_DCMD_DS_MASK) {
	case DMAC_DCMD_DS_32BYTE:
		pal_size /= 32;
		break;
	case DMAC_DCMD_DS_16BYTE:
		pal_size /= 16;
		break;
	case DMAC_DCMD_DS_32BIT:
		pal_size /= 4;
		break;
	case DMAC_DCMD_DS_16BIT:
		pal_size /= 2;
		break;
	case DMAC_DCMD_DS_8BIT:
	default:
		break;
	}
	switch (fbi->dmadesc_fbhigh->dcmd & DMAC_DCMD_DS_MASK) {
	case DMAC_DCMD_DS_32BYTE:
		frm_size /= 32;
		break;
	case DMAC_DCMD_DS_16BYTE:
		frm_size /= 16;
		break;
	case DMAC_DCMD_DS_32BIT:
		frm_size /= 4;
		break;
	case DMAC_DCMD_DS_16BIT:
		frm_size /= 2;
		break;
	case DMAC_DCMD_DS_8BIT:
	default:
		break;
	}
	if( NBITS(vid->vl_bpix) < 12)
	{
		/* assume any mode with <12 bpp is palette driven */

		next = (unsigned long)virt_to_phys(fbi->dmadesc_fbhigh) >> 4;
		fbi->dmadesc_palette->ddadr = /* offset and size*/
			(volatile unsigned int)((next << 24) | (pal_size & 0xffffff)); 

		/* flips back and forth between pal and fbhigh */
		next = (unsigned long)fbi->dmadesc_palette >> 4;
		fbi->dmadesc_fbhigh->ddadr = /* offset and size*/
			(volatile unsigned int)((next << 24) | (frm_size & 0xffffff));

	}
	else
	{
		next = (unsigned int)virt_to_phys(fbi->dmadesc_fbhigh) >> 4;
		/* palette shouldn't be loaded in true-color mode */
		fbi->dmadesc_fbhigh->ddadr = /* offset and size*/
			(volatile unsigned int)((next << 24) | (frm_size & 0xffffff));
	}

	flush_cache_all();
}
static int  jz_lcd_hw_init(vidinfo_t *vid)
{
	struct jz_fb_info *fbi = &vid->jz_fb;
	unsigned int val = 0;
	unsigned int pclk;
	unsigned int stnH;
	int pll_div;

	if ((jzfb.bpp == 18) | (jzfb.bpp == 24))
		jzfb.bpp = 32;
	
	/* Setting Control register */
	REG_LCD_CFG &= ~LCD_CFG_LCDPIN_MASK;
	REG_LCD_CFG |= LCD_CFG_LCDPIN_SLCD;
	/* Configure SLCD module for initialize smart lcd registers*/
	switch (jzfb.bus) {
	case 8:
		REG_SLCD_CFG = SLCD_CFG_BURST_8_WORD | SLCD_CFG_DWIDTH_8_x2 
			| SLCD_CFG_CWIDTH_8BIT | SLCD_CFG_CS_ACTIVE_LOW 
			| SLCD_CFG_RS_CMD_LOW | SLCD_CFG_CLK_ACTIVE_FALLING 
			| SLCD_CFG_TYPE_PARALLEL;
		__gpio_as_slcd_8bit();
		break;
	case 9:
		REG_SLCD_CFG = SLCD_CFG_BURST_8_WORD | SLCD_CFG_DWIDTH_8_x2
			| SLCD_CFG_CWIDTH_8BIT | SLCD_CFG_CS_ACTIVE_LOW 
			| SLCD_CFG_RS_CMD_LOW | SLCD_CFG_CLK_ACTIVE_FALLING 
			| SLCD_CFG_TYPE_PARALLEL;
		__gpio_as_slcd_9bit();
		break;
	case 16:
		REG_SLCD_CFG = SLCD_CFG_BURST_8_WORD | SLCD_CFG_DWIDTH_16
			| SLCD_CFG_CWIDTH_16BIT | SLCD_CFG_CS_ACTIVE_LOW
			| SLCD_CFG_RS_CMD_LOW | SLCD_CFG_CLK_ACTIVE_FALLING
			| SLCD_CFG_TYPE_PARALLEL;
		__gpio_as_slcd_16bit();
		break;
	case 18:
		REG_SLCD_CFG = SLCD_CFG_BURST_8_WORD | SLCD_CFG_DWIDTH_18
			| SLCD_CFG_CWIDTH_18BIT | SLCD_CFG_CS_ACTIVE_LOW 
			| SLCD_CFG_RS_CMD_LOW | SLCD_CFG_CLK_ACTIVE_FALLING 
			| SLCD_CFG_TYPE_PARALLEL;
		__gpio_as_slcd_18bit();
		break;
	default:
		printf("Error: Don't support BUS %d!\n", jzfb.bus);
		break;
	}

	REG_SLCD_CTRL = SLCD_CTRL_DMA_EN;

	/* Timing setting */
	__cpm_stop_lcd();

	val = jzfb.fclk; /* frame clk */
	pclk = val * jzfb.w * jzfb.h; /* Pixclk */

	pll_div = ( REG_CPM_CPCCR & CPM_CPCCR_PCS ); /* clock source,0:pllout/2 1: pllout */
	pll_div = pll_div ? 1 : 2 ;
	val = ( __cpm_get_pllout()/pll_div ) / pclk;
	val--;
	if ( val > 0x1ff ) {
		printf("CPM_LPCDR too large, set it to 0x1ff\n");
		val = 0x1ff;
	}
	__cpm_set_pixdiv(val);

	REG_CPM_CPCCR |= CPM_CPCCR_CE ; /* update divide */

	__cpm_start_lcd();
	udelay(1000);
	__slcd_display_pin_init();
 	__slcd_special_on();

	/* Configure SLCD module for transfer data to smart lcd GRAM*/
	switch (jzfb.bus) {
	case 8:
		switch (jzfb.bpp) {
		case 8:
			REG_SLCD_CFG &= ~SLCD_CFG_DWIDTH_MASK;
			REG_SLCD_CFG |= SLCD_CFG_DWIDTH_8_x1;
			break;
		case 15:
		case 16:
			REG_SLCD_CFG &= ~SLCD_CFG_DWIDTH_MASK;
			REG_SLCD_CFG |= SLCD_CFG_DWIDTH_8_x2;
			break;
		case 17 ... 32:
			REG_SLCD_CFG &= ~SLCD_CFG_DWIDTH_MASK;
			REG_SLCD_CFG |= SLCD_CFG_DWIDTH_8_x3;
			break;
		default:
			printf("The BPP %d is not supported\n", jzfb.bpp);
			break;
		}
		break;
	case 9:
		switch (jzfb.bpp) {
		case 8:
			REG_SLCD_CFG &= ~SLCD_CFG_DWIDTH_MASK;
			REG_SLCD_CFG |= SLCD_CFG_DWIDTH_8_x1;
			break;
		case 15 ... 16:
			REG_SLCD_CFG &= ~SLCD_CFG_DWIDTH_MASK;
			REG_SLCD_CFG |= SLCD_CFG_DWIDTH_8_x2;
			break;
		case 17 ... 32:
			REG_SLCD_CFG &= ~SLCD_CFG_DWIDTH_MASK;
			REG_SLCD_CFG |= SLCD_CFG_DWIDTH_9_x2;
			break;
		default:
			printf("The BPP %d is not supported\n", jzfb.bpp);
			break;
		}
		break;
	case 16:
		switch (jzfb.bpp) {
		case 8:
			REG_SLCD_CFG &= ~SLCD_CFG_DWIDTH_MASK;
			REG_SLCD_CFG |= SLCD_CFG_DWIDTH_8_x1;
			break;
		case 15 ... 16:
			REG_SLCD_CFG &= ~SLCD_CFG_DWIDTH_MASK;
			REG_SLCD_CFG |= SLCD_CFG_DWIDTH_16;
			break;
		case 17 ... 32:
			REG_SLCD_CFG &= ~SLCD_CFG_DWIDTH_MASK;
			REG_SLCD_CFG |= SLCD_CFG_DWIDTH_8_x3;
			break;
		default:
			printf("The BPP %d is not supported\n", jzfb.bpp);
			break;
		}
		break;
	case 18:
		switch (jzfb.bpp) {
		case 8:
			REG_SLCD_CFG &= ~SLCD_CFG_DWIDTH_MASK;
			REG_SLCD_CFG |= SLCD_CFG_DWIDTH_8_x1;
			break;
		case 15:
		case 16:
			REG_SLCD_CFG &= ~SLCD_CFG_DWIDTH_MASK;
			REG_SLCD_CFG |= SLCD_CFG_DWIDTH_16;
			break;
		case 17 ... 32:
			REG_SLCD_CFG &= ~SLCD_CFG_DWIDTH_MASK;
			REG_SLCD_CFG |= SLCD_CFG_DWIDTH_18;
			break;
		default:
			printf("The BPP %d is not supported\n", jzfb.bpp);
			break;
		}
		break;
	default:
		printf("Error: The BUS %d is not supported\n", jzfb.bus);
		break;
	}
	return 0;
}
static int slcd_dma_init(vidinfo_t *vid)
{
	/* Request DMA channel and setup irq handler */
	int dma_chan;
	struct jz_fb_info *fbi = &vid->jz_fb;;

	dma_chan = SLCD_DMA_CHAN_ID;
	printf("DMA channel %d is requested by SLCD!\n", dma_chan);

	/*Init the SLCD DMA and Enable*/
	REG_DMAC_DRSR(dma_chan) = DMAC_DRSR_RS_SLCD;
	REG_DMAC_DMACR = DMAC_DMACR_DMAE;
	REG_DMAC_DCCSR(dma_chan) =  DMAC_DCCSR_EN; /*Descriptor Transfer*/

	if (jzfb.bpp <= 8)
		REG_DMAC_DDA(dma_chan) = virt_to_phys(fbi->dmadesc_palette);
	else
		REG_DMAC_DDA(dma_chan) = virt_to_phys(fbi->dmadesc_fbhigh);

	/* DMA doorbell set -- start DMA now ... */
	__dmac_channel_set_doorbell(dma_chan);
	return 0;
}

/* cpu mode flush screen */
void flush_screen(void *lcdbase)
{
#if LCD_BPP < 5
	unsigned short *fb_end, *ptr_data = (unsigned short *)lcdbase;
	fb_end = (unsigned short *)lcdbase + (jzfb.w * jzfb.h * jzfb.bpp / 16);
#else 
	unsigned int *fb_end, *ptr_data = (unsigned int *)lcdbase;
	fb_end = (unsigned int *)lcdbase + (jzfb.w * jzfb.h * jzfb.bpp / 32);
#endif

	while (ptr_data < fb_end) {
		Mcupanel_Data(*ptr_data);
		ptr_data++;
	}
}

void dump_jz_dma_channel(unsigned int dmanr)
{
	struct dma_chan *chan;

	if (dmanr > MAX_DMA_NUM)
		return;
	printf("DMA%d Registers:\n", dmanr);
	printf("  DMACR  = 0x%08x\n", REG_DMAC_DMACR);
	printf("  DSAR   = 0x%08x\n", REG_DMAC_DSAR(dmanr));
	printf("  DTAR   = 0x%08x\n", REG_DMAC_DTAR(dmanr));
	printf("  DTCR   = 0x%08x\n", REG_DMAC_DTCR(dmanr));
	printf("  DRSR   = 0x%08x\n", REG_DMAC_DRSR(dmanr));
	printf("  DCCSR  = 0x%08x\n", REG_DMAC_DCCSR(dmanr));
	printf("  DCMD  = 0x%08x\n", REG_DMAC_DCMD(dmanr));
	printf("  DDA  = 0x%08x\n", REG_DMAC_DDA(dmanr));
	printf("  DMADBR = 0x%08x\n", REG_DMAC_DMADBR);
}

#endif /* CONFIG_LCD  && CONFIG_SLCD */
