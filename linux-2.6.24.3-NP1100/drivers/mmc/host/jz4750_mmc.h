#ifndef __JZ4750_MMC_H__
#define __JZ4750_MMC_H__

#define MMC_CLOCK_SLOW    400000      /* 400 kHz for initial setup */
#define MMC_CLOCK_FAST  20000000      /* 20 MHz for maximum for normal operation */
#define SD_CLOCK_FAST   24000000      /* 24 MHz for SD Cards */
#define SD_CLOCK_HIGH   24000000      /* 24 MHz for SD Cards */
#define MMC_NO_ERROR  0 

#define NR_SG	1

#ifdef CONFIG_MSC0_JZ4750
#define MSC_ID 0
#define MSC_HOTPLUG_IRQ MSC0_HOTPLUG_IRQ
#define IRQ_MSC IRQ_MSC0
#define DMA_ID_MSC_RX DMA_ID_MSC0_RX
#define DMA_ID_MSC_TX DMA_ID_MSC0_TX
#define MSC_HOTPLUG_PIN MSC0_HOTPLUG_PIN
#else
#define MSC_ID 1
#define MSC_HOTPLUG_IRQ MSC1_HOTPLUG_IRQ
#define IRQ_MSC IRQ_MSC1
#define DMA_ID_MSC_RX DMA_ID_MSC1_RX
#define DMA_ID_MSC_TX DMA_ID_MSC1_TX
#define MSC_HOTPLUG_PIN MSC1_HOTPLUG_PIN
#endif

#define MSC_1BIT_BUS 0
#define MSC_4BIT_BUS 1
#define MSC_8BIT_BUS 2

#define SZ_4K                           0x00001000

struct jz_mmc_host {
	struct mmc_host *mmc;
	spinlock_t lock;
	struct {
		int len;
		int dir;
	} dma;
	struct {
		int index;
		int offset;
		int len;
	} pio;
	int irq;
	unsigned int clkrt;
	unsigned int cmdat;
	unsigned int imask;
	unsigned int power_mode;
	struct jz_mmc_platform_data *pdata;
	struct mmc_request *mrq;
	struct mmc_command *cmd;
	struct mmc_data *data;
	dma_addr_t sg_dma;
	struct jzsoc_dma_desc *sg_cpu;
	unsigned int dma_len;
	unsigned int dma_dir;
};

#define MMC_IRQ_MASK()				\
do {						\
	REG_MSC_IMASK(MSC_ID) = 0xffff;		\
	REG_MSC_IREG(MSC_ID) = 0xffff;		\
} while (0)

typedef struct jzsoc_dma_desc {
	volatile u32 ddadr;	/* Points to the next descriptor + flags */
	volatile u32 dsadr;	/* DSADR value for the current transfer */
	volatile u32 dtadr;	/* DTADR value for the current transfer */
	volatile u32 dcmd;	/* DCMD value for the current transfer */
} jzsoc_dma_desc;

#include <linux/interrupt.h>

struct device;
struct mmc_host;

struct jz_mmc_platform_data {
	unsigned int ocr_mask;			/* available voltages */
	unsigned long detect_delay;		/* delay in jiffies before detecting cards after interrupt */
	int (*init)(struct device *, irq_handler_t , void *);
	int (*get_ro)(struct device *);
	void (*setpower)(struct device *, unsigned int);
	void (*exit)(struct device *, void *);
};

#endif /* __JZ4750_MMC_H__ */
