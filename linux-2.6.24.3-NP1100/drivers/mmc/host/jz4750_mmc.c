/*
 *  linux/drivers/mmc/jz_mmc.c - JZ SD/MMC driver
 *
 *  Copyright (C) 2005 - 2008 Ingenic Semiconductor Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/dma-mapping.h>
#include <linux/mmc/host.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/mmc/host.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>
#include <linux/mmc/sdio.h>
#include <linux/mm.h>
#include <linux/signal.h>
#include <linux/pm.h>
#include <linux/pm_legacy.h>
#include <linux/scatterlist.h>

#include <asm/io.h>
#include <asm/scatterlist.h>
#include <asm/sizes.h>
#include <asm/jzsoc.h>

#include "jz4750_mmc.h"

#define DRIVER_NAME	"jz-mmc"

#define USE_DMA 

static int r_type = 0;
static int rxdmachan = 0;
static int txdmachan = 0;
static int mmc_slot_enable = 0;
static int auto_select_bus = MSC_4BIT_BUS; /* default 4 bit bus*/

/* Start the MMC clock and operation */
static inline int jz_mmc_start_op(void)
{
	REG_MSC_STRPCL(MSC_ID) = MSC_STRPCL_START_OP;

	return MMC_NO_ERROR;
}

static inline u32 jz_mmc_calc_clkrt(int is_low, u32 rate)
{
	u32 clkrt;
	u32 clk_src = is_low ? 24000000 : 48000000;

	clkrt = 0;
	while (rate < clk_src) {
		clkrt++;
		clk_src >>= 1;
	}
	return clkrt;
}

/* Select the MMC clock frequency */
static int jz_mmc_set_clock(u32 rate)
{
	int clkrt;

	/* __cpm_select_msc_clk_high will select 48M clock for MMC/SD card
	 * perhaps this will made some card with bad quality init fail,or
	 * bad stabilization.
	*/
	if (rate > SD_CLOCK_FAST) {
		__cpm_select_msc_clk_high(MSC_ID,1);	/* select clock source from CPM */
		clkrt = jz_mmc_calc_clkrt(0, rate);
	} else {
		__cpm_select_msc_clk(MSC_ID,1);	/* select clock source from CPM */
		clkrt = jz_mmc_calc_clkrt(1, rate);
	}

#ifndef CONFIG_FPGA
	REG_MSC_CLKRT(MSC_ID) = clkrt;
#else
	REG_MSC_CLKRT(MSC_ID) = 7;
#endif
	return MMC_NO_ERROR;
}

static void jz_mmc_enable_irq(struct jz_mmc_host *host, unsigned int mask)
{
	unsigned long flags;

	spin_lock_irqsave(&host->lock, flags);
	host->imask &= ~mask;
	REG_MSC_IMASK(MSC_ID) = host->imask;
	spin_unlock_irqrestore(&host->lock, flags);
}

static void jz_mmc_disable_irq(struct jz_mmc_host *host, unsigned int mask)
{
	unsigned long flags;

	spin_lock_irqsave(&host->lock, flags);
	host->imask |= mask;
	REG_MSC_IMASK(MSC_ID) = host->imask;
	spin_unlock_irqrestore(&host->lock, flags);
}

void jz_set_dma_block_size(int dmanr, int nbyte);

#ifdef USE_DMA
static inline void
jz_mmc_start_dma(int chan, unsigned long phyaddr, int count, int mode)
{
	unsigned long flags;

	flags = claim_dma_lock();
	disable_dma(chan);
	clear_dma_ff(chan);
	jz_set_dma_block_size(chan, 32);
	set_dma_mode(chan, mode);
	set_dma_addr(chan, phyaddr);
	set_dma_count(chan, count + 31);
	enable_dma(chan);
	release_dma_lock(flags);
}

static irqreturn_t jz_mmc_dma_rx_callback(int irq, void *devid)
{
	int chan = rxdmachan;

	disable_dma(chan);
	if (__dmac_channel_address_error_detected(chan)) {
		printk(KERN_DEBUG "%s: DMAC address error.\n",
		       __FUNCTION__);
		__dmac_channel_clear_address_error(chan);
	}
	if (__dmac_channel_transmit_end_detected(chan)) {
		__dmac_channel_clear_transmit_end(chan);
	}
	return IRQ_HANDLED;
}
static irqreturn_t jz_mmc_dma_tx_callback(int irq, void *devid)
{
	int chan = txdmachan;

	disable_dma(chan);
	if (__dmac_channel_address_error_detected(chan)) {
		printk(KERN_DEBUG "%s: DMAC address error.\n",
		       __FUNCTION__);
		__dmac_channel_clear_address_error(chan);
	}
	if (__dmac_channel_transmit_end_detected(chan)) {
		__dmac_channel_clear_transmit_end(chan);
	}
	return IRQ_HANDLED;
}

/* Prepare DMA to start data transfer from the MMC card */
static void jz_mmc_rx_setup_data(struct jz_mmc_host *host,
				 struct mmc_data *data)
{
	unsigned int nob = data->blocks;
	int channelrx = rxdmachan;
	int i;
	u32 size;

	if (data->flags & MMC_DATA_STREAM)
		nob = 0xffff;

	REG_MSC_NOB(MSC_ID) = nob;
	REG_MSC_BLKLEN(MSC_ID) = data->blksz;
	size = nob * data->blksz;

	if (data->flags & MMC_DATA_READ) {
		host->dma.dir = DMA_FROM_DEVICE;
	} else {
		host->dma.dir = DMA_TO_DEVICE;
	}

	host->dma.len =
	    dma_map_sg(mmc_dev(host->mmc), data->sg, data->sg_len,
		       host->dma.dir);

	for (i = 0; i < host->dma.len; i++) {
		host->sg_cpu[i].dtadr = sg_dma_address(&data->sg[i]);
		host->sg_cpu[i].dcmd = sg_dma_len(&data->sg[i]);
		dma_cache_wback_inv((unsigned long)
				    CKSEG0ADDR(sg_dma_address(data->sg)) +
				    data->sg->offset,
				    host->sg_cpu[i].dcmd);
		jz_mmc_start_dma(channelrx, host->sg_cpu[i].dtadr,
				 host->sg_cpu[i].dcmd, DMA_MODE_READ);
	}
}

/* Prepare DMA to start data transfer from the MMC card */
static void jz_mmc_tx_setup_data(struct jz_mmc_host *host,
				 struct mmc_data *data)
{
	unsigned int nob = data->blocks;
	int channeltx = txdmachan;
	int i;
	u32 size;

	if (data->flags & MMC_DATA_STREAM)
		nob = 0xffff;

	REG_MSC_NOB(MSC_ID) = nob;
	REG_MSC_BLKLEN(MSC_ID) = data->blksz;
	size = nob * data->blksz;

	if (data->flags & MMC_DATA_READ) {
		host->dma.dir = DMA_FROM_DEVICE;
	} else {
		host->dma.dir = DMA_TO_DEVICE;
	}

	host->dma.len =
		dma_map_sg(mmc_dev(host->mmc), data->sg, data->sg_len,
			   host->dma.dir);

	for (i = 0; i < host->dma.len; i++) {
		host->sg_cpu[i].dtadr = sg_dma_address(&data->sg[i]);
		host->sg_cpu[i].dcmd = sg_dma_len(&data->sg[i]);
		dma_cache_wback_inv((unsigned long)
				    CKSEG0ADDR(sg_dma_address(data->sg)) +
				    data->sg->offset,
				    host->sg_cpu[i].dcmd);
		jz_mmc_start_dma(channeltx, host->sg_cpu[i].dtadr,
				 host->sg_cpu[i].dcmd, DMA_MODE_WRITE);
	}
}
#else
static void jz_mmc_receive_pio(struct jz_mmc_host *host)
{

	struct mmc_data *data = 0;
	int sg_len = 0, max = 0, count = 0;
	u32 *buf = 0;
	struct scatterlist *sg;
	unsigned int nob;

	data = host->mrq->data;
	nob = data->blocks;
	REG_MSC_NOB(MSC_ID) = nob;
	REG_MSC_BLKLEN(MSC_ID) = data->blksz;

	max = host->pio.len;
	if (host->pio.index < host->dma.len) {
		sg = &data->sg[host->pio.index];
		buf = sg_virt(sg) + host->pio.offset;

		/* This is the space left inside the buffer */
		sg_len = sg_dma_len(&data->sg[host->pio.index]) - host->pio.offset;
		/* Check to if we need less then the size of the sg_buffer */
		if (sg_len < max) max = sg_len;
	}
	max = max / 4;
	for(count = 0; count < max; count++) {
		while (REG_MSC_STAT(MSC_ID) & MSC_STAT_DATA_FIFO_EMPTY)
			;
		*buf++ = REG_MSC_RXFIFO(MSC_ID);
	} 
	host->pio.len -= count;
	host->pio.offset += count;

	if (sg_len && count == sg_len) {
		host->pio.index++;
		host->pio.offset = 0;
	}
}

static void jz_mmc_send_pio(struct jz_mmc_host *host)
{

	struct mmc_data *data = 0;
	int sg_len, max, count = 0;
	u32 *wbuf = 0;
	struct scatterlist *sg;
	unsigned int nob;

	data = host->mrq->data;
	nob = data->blocks;

	REG_MSC_NOB(MSC_ID) = nob;
	REG_MSC_BLKLEN(MSC_ID) = data->blksz;

	/* This is the pointer to the data buffer */
	sg = &data->sg[host->pio.index];
	wbuf = sg_virt(sg) + host->pio.offset;

	/* This is the space left inside the buffer */
	sg_len = data->sg[host->pio.index].length - host->pio.offset;

	/* Check to if we need less then the size of the sg_buffer */
	max = (sg_len > host->pio.len) ? host->pio.len : sg_len;
	max = max / 4;
	for(count = 0; count < max; count++ ) {
		while (REG_MSC_STAT(MSC_ID) & MSC_STAT_DATA_FIFO_FULL)
				;
		REG_MSC_TXFIFO(MSC_ID) = *wbuf++;
	}

	host->pio.len -= count;
	host->pio.offset += count;

	if (count == sg_len) {
		host->pio.index++;
		host->pio.offset = 0;
	}
}

static int
jz_mmc_prepare_data(struct jz_mmc_host *host, struct mmc_data *data)
{
	int datalen = data->blocks * data->blksz;

	host->dma.dir = DMA_BIDIRECTIONAL;
	host->dma.len = dma_map_sg(mmc_dev(host->mmc), data->sg,
				   data->sg_len, host->dma.dir);
	if (host->dma.len == 0)
		return -ETIMEDOUT;

	host->pio.index = 0;
	host->pio.offset = 0;
	host->pio.len = datalen;
	return 0;
}
#endif

static int jz_mmc_cmd_done(struct jz_mmc_host *host, unsigned int stat);

static void jz_mmc_finish_request(struct jz_mmc_host *host, struct mmc_request *mrq)
{
	host->mrq = NULL;
	host->cmd = NULL;
	host->data = NULL;
	mmc_request_done(host->mmc, mrq);
}

static void jz_mmc_start_cmd(struct jz_mmc_host *host,
			     struct mmc_command *cmd, unsigned int cmdat)
{
	u32 timeout = 0x3fffff;
	unsigned int stat;
	struct jz_mmc_host *hst = host;
	WARN_ON(host->cmd != NULL);
	host->cmd = cmd;

	/* mask interrupts */
	REG_MSC_IMASK(MSC_ID) = 0xffff;

	/* clear status */
	REG_MSC_IREG(MSC_ID) = 0xffff;

	if (cmd->flags & MMC_RSP_BUSY)
		cmdat |= MSC_CMDAT_BUSY;

#define RSP_TYPE(x)	((x) & ~(MMC_RSP_BUSY|MMC_RSP_OPCODE))
	switch (RSP_TYPE(mmc_resp_type(cmd))) {
	case RSP_TYPE(MMC_RSP_R1):	/* r1,r1b, r6, r7 */
		cmdat |= MSC_CMDAT_RESPONSE_R1;
		r_type = 1;
		break;
	case RSP_TYPE(MMC_RSP_R3):
		cmdat |= MSC_CMDAT_RESPONSE_R3;
		r_type = 1;
		break;
	case RSP_TYPE(MMC_RSP_R2):
		cmdat |= MSC_CMDAT_RESPONSE_R2;
		r_type = 2;
		break;
	default:
		break;
	}

	REG_MSC_CMD(MSC_ID) = cmd->opcode;

	/* Set argument */
#ifdef CONFIG_MSC0_JZ4750
#ifdef CONFIG_JZ4750_MSC0_BUS_1
	if (cmd->opcode == 6) {
		/* set  1 bit sd card bus*/
		if (cmd->arg ==2)  
			REG_MSC_ARG(MSC_ID) = 0;

		/* set  1 bit mmc card bus*/
		if (cmd->arg == 0x3b70101) {
			REG_MSC_ARG(MSC_ID) = 0x3b70001;
		}
	} else
		REG_MSC_ARG(MSC_ID) = cmd->arg;

#elif defined CONFIG_JZ4750_MSC0_BUS_8
	if (cmd->opcode == 6) {
		/* set  8 bit mmc card bus*/
		if (cmd->arg == 0x3b70101) 
			REG_MSC_ARG(MSC_ID) = 0x3b70201;
		else
			REG_MSC_ARG(MSC_ID) = cmd->arg;
					
	} else
		REG_MSC_ARG(MSC_ID) = cmd->arg;
#else
	REG_MSC_ARG(MSC_ID) = cmd->arg;
#endif /* CONFIG_JZ4750_MSC0_BUS_1 */
#else 
#ifdef CONFIG_JZ4750_MSC1_BUS_1
	if (cmd->opcode == 6) {
		/* set  1 bit sd card bus*/
		if (cmd->arg ==2)  
			REG_MSC_ARG(MSC_ID) = 0;

		/* set  1 bit mmc card bus*/
		if (cmd->arg == 0x3b70101) {
			REG_MSC_ARG(MSC_ID) = 0x3b70001;
		}
	} else
		REG_MSC_ARG(MSC_ID) = cmd->arg;

#else
	REG_MSC_ARG(MSC_ID) = cmd->arg;
#endif /* CONFIG_JZ4750_MSC1_BUS_1 */
#endif /* CONFIG_MSC0_JZ4750*/

	/* Set command */
	REG_MSC_CMDAT(MSC_ID) = cmdat;

	/* Send command */
	jz_mmc_start_op();

	while (timeout-- && !(REG_MSC_STAT(MSC_ID) & MSC_STAT_END_CMD_RES))
		;

	REG_MSC_IREG(MSC_ID) = MSC_IREG_END_CMD_RES;	/* clear irq flag */
	if (cmd->opcode == 12) {
		while (timeout-- && !(REG_MSC_IREG(MSC_ID) & MSC_IREG_PRG_DONE))
			;
		REG_MSC_IREG(MSC_ID) = MSC_IREG_PRG_DONE;	/* clear status */
	}
	if (!mmc_slot_enable) {
		/* It seems that MSC can't report the MSC_STAT_TIME_OUT_RES when
		 * card was removed. We force to return here.
		 */
		cmd->error = -ETIMEDOUT;
		jz_mmc_finish_request(hst, hst->mrq);
		return;
	}

	if (SD_IO_SEND_OP_COND == cmd->opcode) {
		/* 
		 * Don't support SDIO card currently.
		 */
		cmd->error = -ETIMEDOUT;
		jz_mmc_finish_request(hst, hst->mrq);
		return;
	}

	/* Check for status */
	stat = REG_MSC_STAT(MSC_ID);
	jz_mmc_cmd_done(hst, stat);
	if (host->data) {
		if (cmd->opcode == MMC_WRITE_BLOCK || cmd->opcode == MMC_WRITE_MULTIPLE_BLOCK)
#ifdef USE_DMA
			jz_mmc_tx_setup_data(host, host->data);
#else
			jz_mmc_send_pio(host);
		else 
			jz_mmc_receive_pio(host);
#endif
	}
}

static int jz_mmc_cmd_done(struct jz_mmc_host *host, unsigned int stat)
{
	struct mmc_command *cmd = host->cmd;
	int i, temp[16];
	u8 *buf;
	u32 data, v, w1, w2;

	if (!cmd)
		return 0;

	host->cmd = NULL;
	buf = (u8 *) temp;
	switch (r_type) {
	case 1:
	{
		data = REG_MSC_RES(MSC_ID);
		buf[0] = (data >> 8) & 0xff;
		buf[1] = data & 0xff;
		data = REG_MSC_RES(MSC_ID);
		buf[2] = (data >> 8) & 0xff;
		buf[3] = data & 0xff;
		data = REG_MSC_RES(MSC_ID);
		buf[4] = data & 0xff;
		cmd->resp[0] =
			buf[1] << 24 | buf[2] << 16 | buf[3] << 8 |
			buf[4];
		break;
	}
	case 2:
	{
		data = REG_MSC_RES(MSC_ID);
		v = data & 0xffff;
		for (i = 0; i < 4; i++) {
			data = REG_MSC_RES(MSC_ID);
			w1 = data & 0xffff;
			data = REG_MSC_RES(MSC_ID);
			w2 = data & 0xffff;
			cmd->resp[i] = v << 24 | w1 << 8 | w2 >> 8;
			v = w2;
	}
		break;
	}
	case 0:
		break;
	}
	if (stat & MSC_STAT_TIME_OUT_RES) {
		printk("MSC_STAT_TIME_OUT_RES\n");
		cmd->error = -ETIMEDOUT;
	} else if (stat & MSC_STAT_CRC_RES_ERR && cmd->flags & MMC_RSP_CRC) {
		printk("MSC_STAT_CRC\n");
		if (cmd->opcode == MMC_ALL_SEND_CID ||
		    cmd->opcode == MMC_SEND_CSD ||
		    cmd->opcode == MMC_SEND_CID) {
			/* a bogus CRC error can appear if the msb of
			   the 15 byte response is a one */
			if ((cmd->resp[0] & 0x80000000) == 0)
				cmd->error = -EILSEQ;
		}
	}
	/*
	 * Did I mention this is Sick.  We always need to
	 * discard the upper 8 bits of the first 16-bit word.
	 */
	if (host->data && cmd->error == 0)
		jz_mmc_enable_irq(host, MSC_IMASK_DATA_TRAN_DONE);
	else
		jz_mmc_finish_request(host, host->mrq);

	return 1;
}

static int jz_mmc_data_done(struct jz_mmc_host *host, unsigned int stat)
{
	struct mmc_data *data = host->data;

	if (!data)
		return 0;
	REG_MSC_IREG(MSC_ID) = MSC_IREG_DATA_TRAN_DONE;	/* clear status */
	dma_unmap_sg(mmc_dev(host->mmc), data->sg, host->dma_len,
		     host->dma_dir);
	if (stat & MSC_STAT_TIME_OUT_READ) {
		printk("MMC/SD timeout, MMC_STAT 0x%x\n", stat);
		data->error = -ETIMEDOUT;
	} else if (REG_MSC_STAT(MSC_ID) &
		   (MSC_STAT_CRC_READ_ERROR | MSC_STAT_CRC_WRITE_ERROR)) {
		printk("MMC/SD CRC error, MMC_STAT 0x%x\n", stat);
		data->error = -EILSEQ;
	}
	/*
	 * There appears to be a hardware design bug here.  There seems to
	 * be no way to find out how much data was transferred to the card.
	 * This means that if there was an error on any block, we mark all
	 * data blocks as being in error.
	 */
	if (data->error == 0)
		data->bytes_xfered = data->blocks * data->blksz;
	else
		data->bytes_xfered = 0;

	jz_mmc_disable_irq(host, MSC_IMASK_DATA_TRAN_DONE);
	host->data = NULL;
	if (host->mrq->stop) {
		jz_mmc_start_cmd(host, host->mrq->stop, 0);
	} else {
		jz_mmc_finish_request(host, host->mrq);
	}
	return 1;
}

static void jz_mmc_request(struct mmc_host *mmc, struct mmc_request *mrq)
{
	struct jz_mmc_host *host = mmc_priv(mmc);
	unsigned int cmdat;

	/* Save current request for the future processing */
	host->mrq = mrq;
	host->data = mrq->data;
	cmdat = host->cmdat;
	host->cmdat &= ~MSC_CMDAT_INIT;

	if (mrq->data) {
		cmdat &= ~MSC_CMDAT_BUSY;
#ifdef USE_DMA
		if ((mrq->cmd->opcode == 51) | (mrq->cmd->opcode == 8) | (mrq->cmd->opcode == 6))

			cmdat |=
				MSC_CMDAT_BUS_WIDTH_1BIT | MSC_CMDAT_DATA_EN |
				MSC_CMDAT_DMA_EN;
		else {
#ifdef CONFIG_MSC0_JZ4750
#ifdef CONFIG_JZ4750_MSC0_BUS_1
			cmdat &= ~MSC_CMDAT_BUS_WIDTH_MASK;	
			cmdat |= MSC_CMDAT_BUS_WIDTH_1BIT | MSC_CMDAT_DATA_EN |
				MSC_CMDAT_DMA_EN;
#elif defined CONFIG_JZ4750_MSC0_BUS_4
			if(auto_select_bus == MSC_1BIT_BUS) {
				cmdat &= ~MSC_CMDAT_BUS_WIDTH_MASK;	
				cmdat |= MSC_CMDAT_BUS_WIDTH_1BIT | MSC_CMDAT_DATA_EN |
					MSC_CMDAT_DMA_EN;
			} else {
				cmdat &= ~MSC_CMDAT_BUS_WIDTH_MASK;	
				cmdat |= MSC_CMDAT_BUS_WIDTH_4BIT | MSC_CMDAT_DATA_EN |
					MSC_CMDAT_DMA_EN;
			}
#else
			cmdat |= MSC_CMDAT_DATA_EN | MSC_CMDAT_DMA_EN;
#endif /* CONFIG_JZ4750_MSC0_BUS_1 */
#else
#ifdef CONFIG_JZ4750_MSC1_BUS_1
			cmdat &= ~MSC_CMDAT_BUS_WIDTH_MASK;	
			cmdat |= MSC_CMDAT_BUS_WIDTH_1BIT | MSC_CMDAT_DATA_EN |
				MSC_CMDAT_DMA_EN;
#else
			cmdat |= MSC_CMDAT_DATA_EN | MSC_CMDAT_DMA_EN;
#endif /* CONFIG_JZ4750_MSC1_BUS_1 */
#endif /* CONFIG_MSC0_JZ4750 */
		}
		if (mrq->data->flags & MMC_DATA_WRITE)
			cmdat |= MSC_CMDAT_WRITE;

		if (mrq->data->flags & MMC_DATA_STREAM)
			cmdat |= MSC_CMDAT_STREAM_BLOCK;
		if (mrq->cmd->opcode != MMC_WRITE_BLOCK
		    && mrq->cmd->opcode != MMC_WRITE_MULTIPLE_BLOCK)
			jz_mmc_rx_setup_data(host, mrq->data);
#else /*USE_DMA*/

		if ((mrq->cmd->opcode == 51) | (mrq->cmd->opcode == 8) | (mrq->cmd->opcode == 6))
			cmdat |= MSC_CMDAT_BUS_WIDTH_1BIT | MSC_CMDAT_DATA_EN;
		else {
#ifdef CONFIG_MSC0_JZ4750
#ifdef CONFIG_JZ4750_MSC0_BUS_1
			cmdat &= ~MSC_CMDAT_BUS_WIDTH_MASK;	
			cmdat |= MSC_CMDAT_BUS_WIDTH_1BIT | MSC_CMDAT_DATA_EN;
#elif defined CONFIG_JZ4750_MSC0_BUS_4
			if(auto_select_bus == MSC_1BIT_BUS) {
				cmdat &= ~MSC_CMDAT_BUS_WIDTH_MASK;	
				cmdat |= MSC_CMDAT_BUS_WIDTH_1BIT | MSC_CMDAT_DATA_EN; 
			} else {
				cmdat &= ~MSC_CMDAT_BUS_WIDTH_MASK;	
				cmdat |= MSC_CMDAT_BUS_WIDTH_4BIT | MSC_CMDAT_DATA_EN;
			}
#else
			cmdat |= MSC_CMDAT_DATA_EN;
#endif
#else 
#ifdef CONFIG_JZ4750_MSC1_BUS_1
			cmdat &= ~MSC_CMDAT_BUS_WIDTH_MASK;	
			cmdat |= MSC_CMDAT_BUS_WIDTH_1BIT | MSC_CMDAT_DATA_EN;
#else
			cmdat |= MSC_CMDAT_DATA_EN;
#endif /* CONFIG_JZ4750_MSC1_BUS_1 */
#endif /* CONFIG_MSC0_JZ4750 */
		}
		if (mrq->data->flags & MMC_DATA_WRITE)
			cmdat |= MSC_CMDAT_WRITE;

		if (mrq->data->flags & MMC_DATA_STREAM)
			cmdat |= MSC_CMDAT_STREAM_BLOCK;
		jz_mmc_prepare_data(host, host->data);
#endif /*USE_DMA*/
	}
	jz_mmc_start_cmd(host, mrq->cmd, cmdat);
}

static irqreturn_t jz_mmc_irq(int irq, void *devid)
{
	struct jz_mmc_host *host = devid;
	unsigned int ireg;
	int handled = 0;

	ireg = REG_MSC_IREG(MSC_ID);

	if (ireg) {
		unsigned stat = REG_MSC_STAT(MSC_ID);
		if (ireg & MSC_IREG_DATA_TRAN_DONE)
			handled |= jz_mmc_data_done(host, stat);
	}
	return IRQ_RETVAL(handled);
}

/* Returns true if MMC slot is empty */
static int jz_mmc_slot_is_empty(int slot)
{
	int empty;

#ifdef CONFIG_FPGA
	return 0;
#endif

#ifdef CONFIG_MSC1_JZ4750
	empty = (__msc1_card_detected(slot) == 0) ? 1 : 0;
#else
	empty = (__msc0_card_detected(slot) == 0) ? 1 : 0;

#endif
	if (empty) {
		/* wait for card insertion */
#ifdef CONFIG_MSC1_JZ4750
		__gpio_as_irq_rise_edge(MSC_HOTPLUG_PIN);
#else
		__gpio_as_irq_fall_edge(MSC_HOTPLUG_PIN);
#endif
	} else {
		/* wait for card removal */
#ifdef CONFIG_MSC1_JZ4750
		__gpio_as_irq_fall_edge(MSC_HOTPLUG_PIN);
#else
		__gpio_as_irq_rise_edge(MSC_HOTPLUG_PIN);
#endif
	}

	return empty;
}

static irqreturn_t jz_mmc_detect_irq(int irq, void *devid)
{
	struct jz_mmc_host *host = (struct jz_mmc_host *) devid;

	auto_select_bus = MSC_4BIT_BUS;
	if (jz_mmc_slot_is_empty(0)) {
		mmc_slot_enable = 0;
		mmc_detect_change(host->mmc, 50);
	} else {
		mmc_slot_enable = 1;
		mmc_detect_change(host->mmc, 50);
	}
	return IRQ_HANDLED;
}

static int jz_mmc_get_ro(struct mmc_host *mmc)
{
	struct jz_mmc_host *host = mmc_priv(mmc);

	if (host->pdata && host->pdata->get_ro)
		return host->pdata->get_ro(mmc_dev(mmc));
	/* Host doesn't support read only detection so assume writeable */
	return 0;
}

/* set clock and power */
static void jz_mmc_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct jz_mmc_host *host = mmc_priv(mmc);

	if (ios->clock)
		jz_mmc_set_clock(ios->clock);

	if (host->power_mode != ios->power_mode) {
		host->power_mode = ios->power_mode;

		if (ios->power_mode == MMC_POWER_ON)
			host->cmdat |= CMDAT_INIT;
	}

	if (ios->bus_width == MMC_BUS_WIDTH_4) {
		auto_select_bus = MSC_4BIT_BUS;
		host->cmdat |= MSC_CMDAT_BUS_WIDTH_4BIT;
	}
	else if (ios->bus_width == MMC_BUS_WIDTH_8) {
		host->cmdat |= MSC_CMDAT_BUS_WIDTH_8BIT;	
		auto_select_bus = MSC_8BIT_BUS;
	} else {
		/* 1 bit bus*/
		host->cmdat &= ~MSC_CMDAT_BUS_WIDTH_8BIT;	
		auto_select_bus = MSC_1BIT_BUS;
	}
}

static const struct mmc_host_ops jz_mmc_ops = {
	.request = jz_mmc_request,
	.get_ro = jz_mmc_get_ro,
	.set_ios = jz_mmc_set_ios,
};
static int jz_mmc_pm_callback(struct pm_dev *pm_dev,
			      pm_request_t req, void *data);

static int jz_mmc_probe(struct platform_device *pdev)
{
	int retval;
	struct mmc_host *mmc;
	struct jz_mmc_host *host = NULL;
	int irq;
	struct resource *r;

#ifdef CONFIG_MSC0_JZ4750
#ifdef CONFIG_SOC_JZ4750
	__gpio_as_msc0_8bit(); // for jz4750
#else
	__gpio_as_msc0_4bit(); // for jz4750d
#endif
	__msc0_init_io();
	__msc0_enable_power();
#else
	__gpio_as_msc1_4bit();
	__msc1_init_io();
	__msc1_enable_power();
#endif
	__msc_reset(MSC_ID);
	REG_MSC_LPM(MSC_ID) = 0x1;

	MMC_IRQ_MASK();
	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	irq = platform_get_irq(pdev, 0);
	if (!r || irq < 0)
		return -ENXIO;

	r = request_mem_region(r->start, SZ_4K, DRIVER_NAME);
	if (!r)
		return -EBUSY;

	mmc = mmc_alloc_host(sizeof(struct jz_mmc_host), &pdev->dev);
	if (!mmc) {
		retval = -ENOMEM;
		goto out;
	}
	mmc->ops = &jz_mmc_ops;
	mmc->f_min = MMC_CLOCK_SLOW;
	mmc->f_max = SD_CLOCK_HIGH;
	/*
	 * We can do SG-DMA, but we don't because we never know how much
	 * data we successfully wrote to the card.
	 */
	mmc->max_phys_segs = NR_SG;

	mmc->max_seg_size = PAGE_SIZE * 16;
	mmc->max_req_size = mmc->max_seg_size;
	mmc->max_blk_size = 4095;
	/*
	 * Block count register is 16 bits.
	 */
	mmc->max_blk_count = 65535;
	host = mmc_priv(mmc);
	host->mmc = mmc;
	host->pdata = pdev->dev.platform_data;
	mmc->ocr_avail = host->pdata ?
		host->pdata->ocr_mask : MMC_VDD_32_33 | MMC_VDD_33_34;
	host->mmc->caps =
		MMC_CAP_4_BIT_DATA | MMC_CAP_MULTIWRITE | MMC_CAP_SD_HIGHSPEED
		| MMC_CAP_MMC_HIGHSPEED;
	/*
	 *MMC_CAP_4_BIT_DATA    (1 << 0)    The host can do 4 bit transfers  
	 *
	 */
	host->sg_cpu =
		dma_alloc_coherent(&pdev->dev, PAGE_SIZE, &host->sg_dma,
				   GFP_KERNEL);
	if (!host->sg_cpu) {
		retval = -ENOMEM;
		goto out;
	}
	spin_lock_init(&host->lock);
	host->irq = IRQ_MSC; /* is it useful ?*/
	host->imask = 0xffff;
	/*
	 * Ensure that the host controller is shut down, and setup
	 * with our defaults.
	 */
	retval = request_irq(IRQ_MSC, jz_mmc_irq, 0, "MMC/SD", host);
	if (retval) {
		printk(KERN_ERR "MMC/SD: can't request MMC/SD IRQ\n");
		return retval;
	}

	jz_mmc_slot_is_empty(0);
	/* Request card detect interrupt */

	retval = request_irq(MSC_HOTPLUG_IRQ, jz_mmc_detect_irq, 0,	//SA_INTERRUPT,
			     "MMC card detect", host);
	if (retval) {
		printk(KERN_ERR "MMC/SD: can't request card detect IRQ\n");
		goto err1;
	}
#ifdef USE_DMA
	/* Request MMC Rx DMA channel */
	rxdmachan =
		jz_request_dma(DMA_ID_MSC_RX, "MMC Rx", jz_mmc_dma_rx_callback,
			       0, host);
	if (rxdmachan < 0) {
		printk(KERN_ERR "jz_request_dma failed for MMC Rx\n");
		goto err2;
	}

	if (rxdmachan < HALF_DMA_NUM)
		REG_DMAC_DMACR(0) |= DMAC_DMACR_FMSC;
	else
		REG_DMAC_DMACR(1) |= DMAC_DMACR_FMSC;

	/* Request MMC Tx DMA channel */
	txdmachan =
		jz_request_dma(DMA_ID_MSC_TX, "MMC Tx", jz_mmc_dma_tx_callback,
			       0, host);
	if (txdmachan < 0) {
		printk(KERN_ERR "jz_request_dma failed for MMC Tx\n");
		goto err3;
	}

	if (txdmachan < HALF_DMA_NUM)
		REG_DMAC_DMACR(0) |= DMAC_DMACR_FMSC;
	else
		REG_DMAC_DMACR(1) |= DMAC_DMACR_FMSC;

#endif
	platform_set_drvdata(pdev, mmc);
	mmc_add_host(mmc);
#ifdef CONFIG_PM
	/* Register MMC slot as as power-managed device */
	pm_register(PM_UNKNOWN_DEV, PM_SYS_UNKNOWN, jz_mmc_pm_callback);
#endif
	printk("JZ SD/MMC card driver registered\n");

	/* Detect card during initialization */
#if defined(CONFIG_SOC_JZ4750) || defined(CONFIG_SOC_JZ4750D)
	if (!jz_mmc_slot_is_empty(0)) {
		mmc_slot_enable = 1;
		mmc_detect_change(host->mmc, 0);
	}
#endif
	return 0;

err1:free_irq(IRQ_MSC, &host);
#ifdef USE_DMA
 err2:jz_free_dma(rxdmachan);
 err3:jz_free_dma(txdmachan);
#endif
out:
	if (host) {
		if (host->sg_cpu)
			dma_free_coherent(&pdev->dev, PAGE_SIZE,
					  host->sg_cpu, host->sg_dma);
	}
	if (mmc)
		mmc_free_host(mmc);
	return -1;
}

static int jz_mmc_remove(struct platform_device *pdev)
{
	struct mmc_host *mmc = platform_get_drvdata(pdev);
	long flags;

	platform_set_drvdata(pdev, NULL);

	if (mmc) {
		struct jz_mmc_host *host = mmc_priv(mmc);

		if (host->pdata && host->pdata->exit)
			host->pdata->exit(&pdev->dev, mmc);

		mmc_remove_host(mmc);

		local_irq_save(flags);
		__msc0_disable_power();
		jz_free_dma(rxdmachan);
		jz_free_dma(txdmachan);
		free_irq(IRQ_MSC, host);
		local_irq_restore(flags);
		mmc_free_host(mmc);
	}
	return 0;
}

#ifdef CONFIG_PM
pm_message_t state;
static int jz_mmc_suspend(struct platform_device *dev, pm_message_t state)
{
	struct mmc_host *mmc = platform_get_drvdata(dev);
	int ret = 0;

	if (mmc)
		ret = mmc_suspend_host(mmc, state);

	return ret;
}

static int jz_mmc_resume(struct platform_device *dev)
{
	struct mmc_host *mmc = platform_get_drvdata(dev);
	int ret = 0;

	if (mmc)
		ret = mmc_resume_host(mmc);

	return ret;
}
static int jz_mmc_pm_callback(struct pm_dev *pm_dev,
                               pm_request_t req, void *data)
{
	struct platform_device *pdev = (struct platform_device *)pm_dev; 

	switch(req) {
	case PM_RESUME:
		jz_mmc_resume(pdev);
		break;
	case PM_SUSPEND:
	/* state has no use */
		jz_mmc_suspend(pdev,state);
		break;
	default:
		printk("MMC/SD: invalid PM request %d\n", req);
		break;
	}
	return 0;
}
#else
#define jz_mmc_suspend	NULL
#define jz_mmc_resume	NULL
#endif

static struct platform_driver jz_mmc_driver = {
	.probe = jz_mmc_probe,
	.remove = jz_mmc_remove,
	.suspend = jz_mmc_suspend,
	.resume = jz_mmc_resume,
	.driver = {
		   .name = DRIVER_NAME,
		   },
};

static int __init jz_mmc_init(void)
{
	return platform_driver_register(&jz_mmc_driver);
}

static void __exit jz_mmc_exit(void)
{
	platform_driver_unregister(&jz_mmc_driver);
}

module_init(jz_mmc_init);
module_exit(jz_mmc_exit);

MODULE_DESCRIPTION("JZ47XX SD/Multimedia Card Interface Driver");
MODULE_LICENSE("GPL");
