/*
 * Include file for Ingenic Semiconductor's JZ4730 CPU.
 */
#ifndef __JZ4730_H__
#define __JZ4730_H__

#ifndef __ASSEMBLY__

#include <asm/addrspace.h>
#include <asm/cacheops.h>

#define cache_unroll(base,op)	        	\
	__asm__ __volatile__("	         	\
		.set noreorder;		        \
		.set mips3;		        \
		cache %1, (%0);	                \
		.set mips0;			\
		.set reorder"			\
		:				\
		: "r" (base),			\
		  "i" (op));

static inline void jz_flush_dcache(void)
{
	unsigned long start;
	unsigned long end;

	start = KSEG0;
	end = start + CFG_DCACHE_SIZE;
	while (start < end) {
		cache_unroll(start,Index_Writeback_Inv_D);
		start += CFG_CACHELINE_SIZE;
	}
}

static inline void jz_flush_icache(void)
{
	unsigned long start;
	unsigned long end;

	start = KSEG0;
	end = start + CFG_ICACHE_SIZE;
	while(start < end) {
		cache_unroll(start,Index_Invalidate_I);
		start += CFG_CACHELINE_SIZE;
	}
}

/* cpu pipeline flush */
static inline void jz_sync(void)
{
	__asm__ volatile ("sync");
}

static inline void jz_writeb(u32 address, u8 value)
{
	*((volatile u8 *)address) = value;
}

static inline void jz_writew(u32 address, u8 value)
{
	*((volatile u16 *)address) = value;
}

static inline void jz_writel(u32 address, u32 value)
{
	*((volatile u32 *)address) = value;
}

static inline u8 jz_readb(u32 address)
{
	return *((volatile u8 *)address);
}

static inline u16 jz_readw(u32 address)
{
	return *((volatile u16 *)address);
}

static inline u32 jz_readl(u32 address)
{
	return *((volatile u32 *)address);
}

#define REG8(addr)	*((volatile u8 *)(addr))
#define REG16(addr)	*((volatile u16 *)(addr))
#define REG32(addr)	*((volatile u32 *)(addr))

#else

#define REG8(addr)	(addr)
#define REG16(addr)	(addr)
#define REG32(addr)	(addr)

#endif /* !ASSEMBLY */

#define	HARB_BASE	0xB3000000
#define	EMC_BASE	0xB3010000
#define	DMAC_BASE	0xB3020000
#define	UHC_BASE	0xB3030000
#define	UDC_BASE	0xB3040000
#define	LCD_BASE	0xB3050000
#define	CIM_BASE	0xB3060000
#define	ETH_BASE	0xB3100000
#define	NBM_BASE	0xB3F00000

#define	CPM_BASE	0xB0000000
#define	INTC_BASE	0xB0001000
#define	OST_BASE	0xB0002000
#define	RTC_BASE	0xB0003000
#define	WDT_BASE	0xB0004000
#define	GPIO_BASE	0xB0010000
#define	AIC_BASE	0xB0020000
#define	MSC_BASE	0xB0021000
#define	UART0_BASE	0xB0030000
#define	UART1_BASE	0xB0031000
#define	UART2_BASE	0xB0032000
#define	UART3_BASE	0xB0033000
#define	FIR_BASE	0xB0040000
#define	SCC_BASE	0xB0041000
#define	SCC0_BASE	0xB0041000
#define	I2C_BASE	0xB0042000
#define	SSI_BASE	0xB0043000
#define	SCC1_BASE	0xB0044000
#define	PWM0_BASE	0xB0050000
#define	PWM1_BASE	0xB0051000
#define	DES_BASE	0xB0060000
#define	UPRT_BASE	0xB0061000
#define KBC_BASE	0xB0062000




/*************************************************************************
 * MSC
 *************************************************************************/
#define	MSC_STRPCL		(MSC_BASE + 0x000)
#define	MSC_STAT		(MSC_BASE + 0x004)
#define	MSC_CLKRT		(MSC_BASE + 0x008)
#define	MSC_CMDAT		(MSC_BASE + 0x00C)
#define	MSC_RESTO		(MSC_BASE + 0x010)
#define	MSC_RDTO		(MSC_BASE + 0x014)
#define	MSC_BLKLEN		(MSC_BASE + 0x018)
#define	MSC_NOB			(MSC_BASE + 0x01C)
#define	MSC_SNOB		(MSC_BASE + 0x020)
#define	MSC_IMASK		(MSC_BASE + 0x024)
#define	MSC_IREG		(MSC_BASE + 0x028)
#define	MSC_CMD			(MSC_BASE + 0x02C)
#define	MSC_ARG			(MSC_BASE + 0x030)
#define	MSC_RES			(MSC_BASE + 0x034)
#define	MSC_RXFIFO		(MSC_BASE + 0x038)
#define	MSC_TXFIFO		(MSC_BASE + 0x03C)

#define	REG_MSC_STRPCL		REG16(MSC_STRPCL)
#define	REG_MSC_STAT		REG32(MSC_STAT)
#define	REG_MSC_CLKRT		REG16(MSC_CLKRT)
#define	REG_MSC_CMDAT		REG32(MSC_CMDAT)
#define	REG_MSC_RESTO		REG16(MSC_RESTO)
#define	REG_MSC_RDTO		REG16(MSC_RDTO)
#define	REG_MSC_BLKLEN		REG16(MSC_BLKLEN)
#define	REG_MSC_NOB		REG16(MSC_NOB)
#define	REG_MSC_SNOB		REG16(MSC_SNOB)
#define	REG_MSC_IMASK		REG16(MSC_IMASK)
#define	REG_MSC_IREG		REG16(MSC_IREG)
#define	REG_MSC_CMD		REG8(MSC_CMD)
#define	REG_MSC_ARG		REG32(MSC_ARG)
#define	REG_MSC_RES		REG16(MSC_RES)
#define	REG_MSC_RXFIFO		REG32(MSC_RXFIFO)
#define	REG_MSC_TXFIFO		REG32(MSC_TXFIFO)

/* MSC Clock and Control Register (MSC_STRPCL) */

#define MSC_STRPCL_EXIT_MULTIPLE	(1 << 7)
#define MSC_STRPCL_EXIT_TRANSFER	(1 << 6)
#define MSC_STRPCL_START_READWAIT	(1 << 5)
#define MSC_STRPCL_STOP_READWAIT	(1 << 4)
#define MSC_STRPCL_RESET		(1 << 3)
#define MSC_STRPCL_START_OP		(1 << 2)
#define MSC_STRPCL_CLOCK_CONTROL_BIT	0
#define MSC_STRPCL_CLOCK_CONTROL_MASK	(0x3 << MSC_STRPCL_CLOCK_CONTROL_BIT)
  #define MSC_STRPCL_CLOCK_CONTROL_STOP	  (0x1 << MSC_STRPCL_CLOCK_CONTROL_BIT) /* Stop MMC/SD clock */
  #define MSC_STRPCL_CLOCK_CONTROL_START  (0x2 << MSC_STRPCL_CLOCK_CONTROL_BIT) /* Start MMC/SD clock */

/* MSC Status Register (MSC_STAT) */

#define MSC_STAT_IS_RESETTING		(1 << 15)
#define MSC_STAT_SDIO_INT_ACTIVE	(1 << 14)
#define MSC_STAT_PRG_DONE		(1 << 13)
#define MSC_STAT_DATA_TRAN_DONE		(1 << 12)
#define MSC_STAT_END_CMD_RES		(1 << 11)
#define MSC_STAT_DATA_FIFO_AFULL	(1 << 10)
#define MSC_STAT_IS_READWAIT		(1 << 9)
#define MSC_STAT_CLK_EN			(1 << 8)
#define MSC_STAT_DATA_FIFO_FULL		(1 << 7)
#define MSC_STAT_DATA_FIFO_EMPTY	(1 << 6)
#define MSC_STAT_CRC_RES_ERR		(1 << 5)
#define MSC_STAT_CRC_READ_ERROR		(1 << 4)
#define MSC_STAT_CRC_WRITE_ERROR_BIT	2
#define MSC_STAT_CRC_WRITE_ERROR_MASK	(0x3 << MSC_STAT_CRC_WRITE_ERROR_BIT)
  #define MSC_STAT_CRC_WRITE_ERROR_NO		(0 << MSC_STAT_CRC_WRITE_ERROR_BIT) /* No error on transmission of data */
  #define MSC_STAT_CRC_WRITE_ERROR		(1 << MSC_STAT_CRC_WRITE_ERROR_BIT) /* Card observed erroneous transmission of data */
  #define MSC_STAT_CRC_WRITE_ERROR_NOSTS	(2 << MSC_STAT_CRC_WRITE_ERROR_BIT) /* No CRC status is sent back */
#define MSC_STAT_TIME_OUT_RES		(1 << 1)
#define MSC_STAT_TIME_OUT_READ		(1 << 0)

/* MSC Bus Clock Control Register (MSC_CLKRT) */

#define	MSC_CLKRT_CLK_RATE_BIT		0
#define	MSC_CLKRT_CLK_RATE_MASK		(0x7 << MSC_CLKRT_CLK_RATE_BIT)
  #define MSC_CLKRT_CLK_RATE_DIV_1	  (0x0 << MSC_CLKRT_CLK_RATE_BIT) /* CLK_SRC */
  #define MSC_CLKRT_CLK_RATE_DIV_2	  (0x1 << MSC_CLKRT_CLK_RATE_BIT) /* 1/2 of CLK_SRC */
  #define MSC_CLKRT_CLK_RATE_DIV_4	  (0x2 << MSC_CLKRT_CLK_RATE_BIT) /* 1/4 of CLK_SRC */
  #define MSC_CLKRT_CLK_RATE_DIV_8	  (0x3 << MSC_CLKRT_CLK_RATE_BIT) /* 1/8 of CLK_SRC */
  #define MSC_CLKRT_CLK_RATE_DIV_16	  (0x4 << MSC_CLKRT_CLK_RATE_BIT) /* 1/16 of CLK_SRC */
  #define MSC_CLKRT_CLK_RATE_DIV_32	  (0x5 << MSC_CLKRT_CLK_RATE_BIT) /* 1/32 of CLK_SRC */
  #define MSC_CLKRT_CLK_RATE_DIV_64	  (0x6 << MSC_CLKRT_CLK_RATE_BIT) /* 1/64 of CLK_SRC */
  #define MSC_CLKRT_CLK_RATE_DIV_128	  (0x7 << MSC_CLKRT_CLK_RATE_BIT) /* 1/128 of CLK_SRC */

/* MSC Command Sequence Control Register (MSC_CMDAT) */

#define	MSC_CMDAT_IO_ABORT		(1 << 11)
#define	MSC_CMDAT_BUS_WIDTH_BIT		9
#define	MSC_CMDAT_BUS_WIDTH_MASK	(0x3 << MSC_CMDAT_BUS_WIDTH_BIT)
  #define MSC_CMDAT_BUS_WIDTH_1BIT	  (0x0 << MSC_CMDAT_BUS_WIDTH_BIT) /* 1-bit data bus */
  #define MSC_CMDAT_BUS_WIDTH_4BIT	  (0x2 << MSC_CMDAT_BUS_WIDTH_BIT) /* 4-bit data bus */
  #define CMDAT_BUS_WIDTH1	  (0x0 << MSC_CMDAT_BUS_WIDTH_BIT)
  #define CMDAT_BUS_WIDTH4	  (0x2 << MSC_CMDAT_BUS_WIDTH_BIT)
#define	MSC_CMDAT_DMA_EN		(1 << 8)
#define	MSC_CMDAT_INIT			(1 << 7)
#define	MSC_CMDAT_BUSY			(1 << 6)
#define	MSC_CMDAT_STREAM_BLOCK		(1 << 5)
#define	MSC_CMDAT_WRITE			(1 << 4)
#define	MSC_CMDAT_READ			(0 << 4)
#define	MSC_CMDAT_DATA_EN		(1 << 3)
#define	MSC_CMDAT_RESPONSE_BIT	0
#define	MSC_CMDAT_RESPONSE_MASK	(0x7 << MSC_CMDAT_RESPONSE_BIT)
  #define MSC_CMDAT_RESPONSE_NONE  (0x0 << MSC_CMDAT_RESPONSE_BIT) /* No response */
  #define MSC_CMDAT_RESPONSE_R1	  (0x1 << MSC_CMDAT_RESPONSE_BIT) /* Format R1 and R1b */
  #define MSC_CMDAT_RESPONSE_R2	  (0x2 << MSC_CMDAT_RESPONSE_BIT) /* Format R2 */
  #define MSC_CMDAT_RESPONSE_R3	  (0x3 << MSC_CMDAT_RESPONSE_BIT) /* Format R3 */
  #define MSC_CMDAT_RESPONSE_R4	  (0x4 << MSC_CMDAT_RESPONSE_BIT) /* Format R4 */
  #define MSC_CMDAT_RESPONSE_R5	  (0x5 << MSC_CMDAT_RESPONSE_BIT) /* Format R5 */
  #define MSC_CMDAT_RESPONSE_R6	  (0x6 << MSC_CMDAT_RESPONSE_BIT) /* Format R6 */

#define	CMDAT_DMA_EN	(1 << 8)
#define	CMDAT_INIT	(1 << 7)
#define	CMDAT_BUSY	(1 << 6)
#define	CMDAT_STREAM	(1 << 5)
#define	CMDAT_WRITE	(1 << 4)
#define	CMDAT_DATA_EN	(1 << 3)

/* MSC Interrupts Mask Register (MSC_IMASK) */

#define	MSC_IMASK_SDIO			(1 << 7)
#define	MSC_IMASK_TXFIFO_WR_REQ		(1 << 6)
#define	MSC_IMASK_RXFIFO_RD_REQ		(1 << 5)
#define	MSC_IMASK_END_CMD_RES		(1 << 2)
#define	MSC_IMASK_PRG_DONE		(1 << 1)
#define	MSC_IMASK_DATA_TRAN_DONE	(1 << 0)


/* MSC Interrupts Status Register (MSC_IREG) */

#define	MSC_IREG_SDIO			(1 << 7)
#define	MSC_IREG_TXFIFO_WR_REQ		(1 << 6)
#define	MSC_IREG_RXFIFO_RD_REQ		(1 << 5)
#define	MSC_IREG_END_CMD_RES		(1 << 2)
#define	MSC_IREG_PRG_DONE		(1 << 1)
#define	MSC_IREG_DATA_TRAN_DONE		(1 << 0)

/*************************************************************************
 * RTC
 *************************************************************************/
#define RTC_RCR		(RTC_BASE + 0x00)
#define RTC_RSR		(RTC_BASE + 0x04)
#define RTC_RSAR	(RTC_BASE + 0x08)
#define RTC_RGR		(RTC_BASE + 0x0c)

#define REG_RTC_RCR	REG32(RTC_RCR)
#define REG_RTC_RSR	REG32(RTC_RSR)
#define REG_RTC_RSAR	REG32(RTC_RSAR)
#define REG_RTC_RGR	REG32(RTC_RGR)

#define RTC_RCR_HZ	(1 << 6)
#define RTC_RCR_HZIE	(1 << 5)
#define RTC_RCR_AF	(1 << 4)
#define RTC_RCR_AIE	(1 << 3)
#define RTC_RCR_AE	(1 << 2)
#define RTC_RCR_START	(1 << 0)

#define RTC_RGR_LOCK		(1 << 31)
#define RTC_RGR_ADJ_BIT		16
#define RTC_RGR_ADJ_MASK	(0x3ff << RTC_RGR_ADJ_BIT)
#define RTC_RGR_DIV_BIT		0
#define RTC_REG_DIV_MASK	(0xff << RTC_RGR_DIV_BIT)




/*************************************************************************
 * FIR
 *************************************************************************/
#define	FIR_TDR			(FIR_BASE + 0x000)
#define	FIR_RDR			(FIR_BASE + 0x004)
#define	FIR_TFLR		(FIR_BASE + 0x008)
#define	FIR_AR			(FIR_BASE + 0x00C)
#define	FIR_CR1			(FIR_BASE + 0x010)
#define	FIR_CR2			(FIR_BASE + 0x014)
#define	FIR_SR			(FIR_BASE + 0x018)

#define	REG_FIR_TDR		REG8(FIR_TDR)
#define	REG_FIR_RDR		REG8(FIR_RDR)
#define REG_FIR_TFLR		REG16(FIR_TFLR)
#define REG_FIR_AR		REG8(FIR_AR)
#define	REG_FIR_CR1		REG8(FIR_CR1)
#define	REG_FIR_CR2		REG16(FIR_CR2)
#define REG_FIR_SR		REG16(FIR_SR)

/* FIR Control Register 1 (FIR_CR1) */

#define FIR_CR1_FIRUE		(1 << 7)
#define FIR_CR1_ACE		(1 << 6)
#define FIR_CR1_EOUS		(1 << 5)
#define FIR_CR1_TIIE		(1 << 4)
#define FIR_CR1_TFIE		(1 << 3)
#define FIR_CR1_RFIE		(1 << 2)
#define FIR_CR1_TXE		(1 << 1)
#define FIR_CR1_RXE		(1 << 0)

/* FIR Control Register 2 (FIR_CR2) */

#define FIR_CR2_SIPE		(1 << 10)
#define FIR_CR2_BCRC		(1 << 9)
#define FIR_CR2_TFLRS		(1 << 8)
#define FIR_CR2_ISS		(1 << 7)
#define FIR_CR2_LMS		(1 << 6)
#define FIR_CR2_TPPS		(1 << 5)
#define FIR_CR2_RPPS		(1 << 4)
#define FIR_CR2_TTRG_BIT	2
#define FIR_CR2_TTRG_MASK	(0x3 << FIR_CR2_TTRG_BIT)
  #define FIR_CR2_TTRG_16	  (0 << FIR_CR2_TTRG_BIT) /* Transmit Trigger Level is 16 */
  #define FIR_CR2_TTRG_32	  (1 << FIR_CR2_TTRG_BIT) /* Transmit Trigger Level is 32 */
  #define FIR_CR2_TTRG_64	  (2 << FIR_CR2_TTRG_BIT) /* Transmit Trigger Level is 64 */
  #define FIR_CR2_TTRG_128	  (3 << FIR_CR2_TTRG_BIT) /* Transmit Trigger Level is 128 */
#define FIR_CR2_RTRG_BIT	0
#define FIR_CR2_RTRG_MASK	(0x3 << FIR_CR2_RTRG_BIT)
  #define FIR_CR2_RTRG_16	  (0 << FIR_CR2_RTRG_BIT) /* Receive Trigger Level is 16 */
  #define FIR_CR2_RTRG_32	  (1 << FIR_CR2_RTRG_BIT) /* Receive Trigger Level is 32 */
  #define FIR_CR2_RTRG_64	  (2 << FIR_CR2_RTRG_BIT) /* Receive Trigger Level is 64 */
  #define FIR_CR2_RTRG_128	  (3 << FIR_CR2_RTRG_BIT) /* Receive Trigger Level is 128 */

/* FIR Status Register (FIR_SR) */

#define FIR_SR_RFW		(1 << 12)
#define FIR_SR_RFA		(1 << 11)
#define FIR_SR_TFRTL		(1 << 10)
#define FIR_SR_RFRTL		(1 << 9)
#define FIR_SR_URUN		(1 << 8)
#define FIR_SR_RFTE		(1 << 7)
#define FIR_SR_ORUN		(1 << 6)
#define FIR_SR_CRCE		(1 << 5)
#define FIR_SR_FEND		(1 << 4)
#define FIR_SR_TFF		(1 << 3)
#define FIR_SR_RFE		(1 << 2)
#define FIR_SR_TIDLE		(1 << 1)
#define FIR_SR_RB		(1 << 0)




/*************************************************************************
 * SCC
 *************************************************************************/
#define	SCC_DR(base)		((base) + 0x000)
#define	SCC_FDR(base)		((base) + 0x004)
#define	SCC_CR(base)		((base) + 0x008)
#define	SCC_SR(base)		((base) + 0x00C)
#define	SCC_TFR(base)		((base) + 0x010)
#define	SCC_EGTR(base)		((base) + 0x014)
#define	SCC_ECR(base)		((base) + 0x018)
#define	SCC_RTOR(base)		((base) + 0x01C)

#define REG_SCC_DR(base)	REG8(SCC_DR(base))
#define REG_SCC_FDR(base)	REG8(SCC_FDR(base))
#define REG_SCC_CR(base)	REG32(SCC_CR(base))
#define REG_SCC_SR(base)	REG16(SCC_SR(base))
#define REG_SCC_TFR(base)	REG16(SCC_TFR(base))
#define REG_SCC_EGTR(base)	REG8(SCC_EGTR(base))
#define REG_SCC_ECR(base)	REG32(SCC_ECR(base))
#define REG_SCC_RTOR(base)	REG8(SCC_RTOR(base))

/* SCC FIFO Data Count Register (SCC_FDR) */

#define SCC_FDR_EMPTY		0x00
#define SCC_FDR_FULL		0x10

/* SCC Control Register (SCC_CR) */

#define SCC_CR_SCCE		(1 << 31)
#define SCC_CR_TRS		(1 << 30)
#define SCC_CR_T2R		(1 << 29)
#define SCC_CR_FDIV_BIT		24
#define SCC_CR_FDIV_MASK	(0x3 << SCC_CR_FDIV_BIT)
  #define SCC_CR_FDIV_1		  (0 << SCC_CR_FDIV_BIT) /* SCC_CLK frequency is the same as device clock */
  #define SCC_CR_FDIV_2		  (1 << SCC_CR_FDIV_BIT) /* SCC_CLK frequency is half of device clock */
#define SCC_CR_FLUSH		(1 << 23)
#define SCC_CR_TRIG_BIT		16
#define SCC_CR_TRIG_MASK	(0x3 << SCC_CR_TRIG_BIT)
  #define SCC_CR_TRIG_1		  (0 << SCC_CR_TRIG_BIT) /* Receive/Transmit-FIFO Trigger is 1 */
  #define SCC_CR_TRIG_4		  (1 << SCC_CR_TRIG_BIT) /* Receive/Transmit-FIFO Trigger is 4 */
  #define SCC_CR_TRIG_8		  (2 << SCC_CR_TRIG_BIT) /* Receive/Transmit-FIFO Trigger is 8 */
  #define SCC_CR_TRIG_14	  (3 << SCC_CR_TRIG_BIT) /* Receive/Transmit-FIFO Trigger is 14 */
#define SCC_CR_TP		(1 << 15)
#define SCC_CR_CONV		(1 << 14)
#define SCC_CR_TXIE		(1 << 13)
#define SCC_CR_RXIE		(1 << 12)
#define SCC_CR_TENDIE		(1 << 11)
#define SCC_CR_RTOIE		(1 << 10)
#define SCC_CR_ECIE		(1 << 9)
#define SCC_CR_EPIE		(1 << 8)
#define SCC_CR_RETIE		(1 << 7)
#define SCC_CR_EOIE		(1 << 6)
#define SCC_CR_TSEND		(1 << 3)
#define SCC_CR_PX_BIT		1
#define SCC_CR_PX_MASK		(0x3 << SCC_CR_PX_BIT)
  #define SCC_CR_PX_NOT_SUPPORT	  (0 << SCC_CR_PX_BIT) /* SCC does not support clock stop */
  #define SCC_CR_PX_STOP_LOW	  (1 << SCC_CR_PX_BIT) /* SCC_CLK stops at state low */
  #define SCC_CR_PX_STOP_HIGH	  (2 << SCC_CR_PX_BIT) /* SCC_CLK stops at state high */
#define SCC_CR_CLKSTP		(1 << 0)

/* SCC Status Register (SCC_SR) */

#define SCC_SR_TRANS		(1 << 15)
#define SCC_SR_ORER		(1 << 12)
#define SCC_SR_RTO		(1 << 11)
#define SCC_SR_PER		(1 << 10)
#define SCC_SR_TFTG		(1 << 9)
#define SCC_SR_RFTG		(1 << 8)
#define SCC_SR_TEND		(1 << 7)
#define SCC_SR_RETR_3		(1 << 4)
#define SCC_SR_ECNTO		(1 << 0)




/*************************************************************************
 * ETH
 *************************************************************************/
#define ETH_BMR		(ETH_BASE + 0x1000)
#define ETH_TPDR	(ETH_BASE + 0x1004)
#define ETH_RPDR	(ETH_BASE + 0x1008)
#define ETH_RAR		(ETH_BASE + 0x100C)
#define ETH_TAR		(ETH_BASE + 0x1010)
#define ETH_SR		(ETH_BASE + 0x1014)
#define ETH_CR		(ETH_BASE + 0x1018)
#define ETH_IER		(ETH_BASE + 0x101C)
#define ETH_MFCR	(ETH_BASE + 0x1020)
#define ETH_CTAR	(ETH_BASE + 0x1050)
#define ETH_CRAR	(ETH_BASE + 0x1054)
#define ETH_MCR		(ETH_BASE + 0x0000)
#define ETH_MAHR	(ETH_BASE + 0x0004)
#define ETH_MALR	(ETH_BASE + 0x0008)
#define ETH_HTHR	(ETH_BASE + 0x000C)
#define ETH_HTLR	(ETH_BASE + 0x0010)
#define ETH_MIAR	(ETH_BASE + 0x0014)
#define ETH_MIDR	(ETH_BASE + 0x0018)
#define ETH_FCR		(ETH_BASE + 0x001C)
#define ETH_VTR1	(ETH_BASE + 0x0020)
#define ETH_VTR2	(ETH_BASE + 0x0024)
#define ETH_WKFR	(ETH_BASE + 0x0028)
#define ETH_PMTR	(ETH_BASE + 0x002C)

#define REG_ETH_BMR	REG32(ETH_BMR)
#define REG_ETH_TPDR	REG32(ETH_TPDR)
#define REG_ETH_RPDR	REG32(ETH_RPDR)
#define REG_ETH_RAR	REG32(ETH_RAR)
#define REG_ETH_TAR	REG32(ETH_TAR)
#define REG_ETH_SR	REG32(ETH_SR)
#define REG_ETH_CR	REG32(ETH_CR)
#define REG_ETH_IER	REG32(ETH_IER)
#define REG_ETH_MFCR	REG32(ETH_MFCR)
#define REG_ETH_CTAR	REG32(ETH_CTAR)
#define REG_ETH_CRAR	REG32(ETH_CRAR)
#define REG_ETH_MCR	REG32(ETH_MCR)
#define REG_ETH_MAHR	REG32(ETH_MAHR)
#define REG_ETH_MALR	REG32(ETH_MALR)
#define REG_ETH_HTHR	REG32(ETH_HTHR)
#define REG_ETH_HTLR	REG32(ETH_HTLR)
#define REG_ETH_MIAR	REG32(ETH_MIAR)
#define REG_ETH_MIDR	REG32(ETH_MIDR)
#define REG_ETH_FCR	REG32(ETH_FCR)
#define REG_ETH_VTR1	REG32(ETH_VTR1)
#define REG_ETH_VTR2	REG32(ETH_VTR2)
#define REG_ETH_WKFR	REG32(ETH_WKFR)
#define REG_ETH_PMTR	REG32(ETH_PMTR)

/* Bus Mode Register (ETH_BMR) */

#define ETH_BMR_DBO		(1 << 20)
#define ETH_BMR_PBL_BIT		8
#define ETH_BMR_PBL_MASK	(0x3f << ETH_BMR_PBL_BIT)
  #define ETH_BMR_PBL_1		  (0x1 << ETH_BMR_PBL_BIT)
  #define ETH_BMR_PBL_4		  (0x4 << ETH_BMR_PBL_BIT)
#define ETH_BMR_BLE		(1 << 7)
#define ETH_BMR_DSL_BIT		2
#define ETH_BMR_DSL_MASK	(0x1f << ETH_BMR_DSL_BIT)
  #define ETH_BMR_DSL_0		  (0x0 << ETH_BMR_DSL_BIT)
  #define ETH_BMR_DSL_1		  (0x1 << ETH_BMR_DSL_BIT)
  #define ETH_BMR_DSL_2		  (0x2 << ETH_BMR_DSL_BIT)
  #define ETH_BMR_DSL_4		  (0x4 << ETH_BMR_DSL_BIT)
  #define ETH_BMR_DSL_8		  (0x8 << ETH_BMR_DSL_BIT)
#define ETH_BMR_SWR		(1 << 0)

/* DMA Status Register (ETH_SR) */

#define ETH_SR_EB_BIT		23
#define ETH_SR_EB_MASK		(0x7 << ETH_SR_EB_BIT)
  #define ETH_SR_EB_TX_ABORT	  (0x1 << ETH_SR_EB_BIT)
  #define ETH_SR_EB_RX_ABORT	  (0x2 << ETH_SR_EB_BIT)
#define ETH_SR_TS_BIT		20
#define ETH_SR_TS_MASK		(0x7 << ETH_SR_TS_BIT)
  #define ETH_SR_TS_STOP	  (0x0 << ETH_SR_TS_BIT)
  #define ETH_SR_TS_FTD		  (0x1 << ETH_SR_TS_BIT)
  #define ETH_SR_TS_WEOT	  (0x2 << ETH_SR_TS_BIT)
  #define ETH_SR_TS_QDAT	  (0x3 << ETH_SR_TS_BIT)
  #define ETH_SR_TS_SUSPEND	  (0x6 << ETH_SR_TS_BIT)
  #define ETH_SR_TS_CTD		  (0x7 << ETH_SR_TS_BIT)
#define ETH_SR_RS_BIT		17
#define ETH_SR_RS_MASK		(0x7 << ETH_SR_RS_BIT)
  #define ETH_SR_RS_STOP	  (0x0 << ETH_SR_RS_BIT)
  #define ETH_SR_RS_FRD		  (0x1 << ETH_SR_RS_BIT)
  #define ETH_SR_RS_CEOR	  (0x2 << ETH_SR_RS_BIT)
  #define ETH_SR_RS_WRP		  (0x3 << ETH_SR_RS_BIT)
  #define ETH_SR_RS_SUSPEND	  (0x4 << ETH_SR_RS_BIT)
  #define ETH_SR_RS_CRD		  (0x5 << ETH_SR_RS_BIT)
  #define ETH_SR_RS_FCF		  (0x6 << ETH_SR_RS_BIT)
  #define ETH_SR_RS_QRF		  (0x7 << ETH_SR_RS_BIT)
#define ETH_SR_NIS		(1 << 16)
#define ETH_SR_AIS		(1 << 15)
#define ETH_SR_ERI		(1 << 14)
#define ETH_SR_FBE		(1 << 13)
#define ETH_SR_ETI		(1 << 10)
#define ETH_SR_RWT		(1 << 9)
#define ETH_SR_RPS		(1 << 8)
#define ETH_SR_RU		(1 << 7)
#define ETH_SR_RI		(1 << 6)
#define ETH_SR_UNF		(1 << 5)
#define ETH_SR_TJT		(1 << 3)
#define ETH_SR_TU		(1 << 2)
#define ETH_SR_TPS		(1 << 1)
#define ETH_SR_TI		(1 << 0)

/* Control (Operation Mode) Register (ETH_CR) */

#define ETH_CR_TTM		(1 << 22)
#define ETH_CR_SF		(1 << 21)
#define ETH_CR_TR_BIT		14
#define ETH_CR_TR_MASK		(0x3 << ETH_CR_TR_BIT)
#define ETH_CR_ST		(1 << 13)
#define ETH_CR_OSF		(1 << 2)
#define ETH_CR_SR		(1 << 1)

/* Interrupt Enable Register (ETH_IER) */

#define ETH_IER_NI		(1 << 16)
#define ETH_IER_AI		(1 << 15)
#define ETH_IER_ERE		(1 << 14)
#define ETH_IER_FBE		(1 << 13)
#define ETH_IER_ET		(1 << 10)
#define ETH_IER_RWE		(1 << 9)
#define ETH_IER_RS		(1 << 8)
#define ETH_IER_RU		(1 << 7)
#define ETH_IER_RI		(1 << 6)
#define ETH_IER_UN		(1 << 5)
#define ETH_IER_TJ		(1 << 3)
#define ETH_IER_TU		(1 << 2)
#define ETH_IER_TS		(1 << 1)
#define ETH_IER_TI		(1 << 0)

/* Missed Frame and Buffer Overflow Counter Register (ETH_MFCR) */

#define ETH_MFCR_OVERFLOW_BIT	17
#define ETH_MFCR_OVERFLOW_MASK	(0x7ff << ETH_MFCR_OVERFLOW_BIT)
#define ETH_MFCR_MFC_BIT	0
#define ETH_MFCR_MFC_MASK	(0xffff << ETH_MFCR_MFC_BIT)

/* MAC Control Register (ETH_MCR) */

#define ETH_MCR_RA		(1 << 31)
#define ETH_MCR_HBD		(1 << 28)
#define ETH_MCR_PS		(1 << 27)
#define ETH_MCR_DRO		(1 << 23)
#define ETH_MCR_OM_BIT		21
#define ETH_MCR_OM_MASK		(0x3 << ETH_MCR_OM_BIT)
  #define ETH_MCR_OM_NORMAL	  (0x0 << ETH_MCR_OM_BIT)
  #define ETH_MCR_OM_INTERNAL	  (0x1 << ETH_MCR_OM_BIT)
  #define ETH_MCR_OM_EXTERNAL	  (0x2 << ETH_MCR_OM_BIT)
#define ETH_MCR_F		(1 << 20)
#define ETH_MCR_PM		(1 << 19)
#define ETH_MCR_PR		(1 << 18)
#define ETH_MCR_IF		(1 << 17)
#define ETH_MCR_PB		(1 << 16)
#define ETH_MCR_HO		(1 << 15)
#define ETH_MCR_HP		(1 << 13)
#define ETH_MCR_LCC		(1 << 12)
#define ETH_MCR_DBF		(1 << 11)
#define ETH_MCR_DTRY		(1 << 10)
#define ETH_MCR_ASTP		(1 << 8)
#define ETH_MCR_BOLMT_BIT	6
#define ETH_MCR_BOLMT_MASK	(0x3 << ETH_MCR_BOLMT_BIT)
  #define ETH_MCR_BOLMT_10	  (0 << ETH_MCR_BOLMT_BIT)
  #define ETH_MCR_BOLMT_8	  (1 << ETH_MCR_BOLMT_BIT)
  #define ETH_MCR_BOLMT_4	  (2 << ETH_MCR_BOLMT_BIT)
  #define ETH_MCR_BOLMT_1	  (3 << ETH_MCR_BOLMT_BIT)
#define ETH_MCR_DC		(1 << 5)
#define ETH_MCR_TE		(1 << 3)
#define ETH_MCR_RE		(1 << 2)

/* MII Address Register (ETH_MIAR) */

#define ETH_MIAR_PHY_ADDR_BIT	11
#define ETH_MIAR_PHY_ADDR_MASK	(0x1f << ETH_MIAR_PHY_ADDR_BIT)
#define ETH_MIAR_MII_REG_BIT	6
#define ETH_MIAR_MII_REG_MASK	(0x1f << ETH_MIAR_MII_REG_BIT)
#define ETH_MIAR_MII_WRITE	(1 << 1)
#define ETH_MIAR_MII_BUSY	(1 << 0)

/* Flow Control Register (ETH_FCR) */

#define	ETH_FCR_PAUSE_TIME_BIT	16
#define	ETH_FCR_PAUSE_TIME_MASK	(0xffff << ETH_FCR_PAUSE_TIME_BIT)
#define	ETH_FCR_PCF		(1 << 2)
#define	ETH_FCR_FCE		(1 << 1)
#define	ETH_FCR_BUSY		(1 << 0)

/* PMT Control and Status Register (ETH_PMTR) */

#define ETH_PMTR_GU		(1 << 9)
#define ETH_PMTR_RF		(1 << 6)
#define ETH_PMTR_MF		(1 << 5)
#define ETH_PMTR_RWK		(1 << 2)
#define ETH_PMTR_MPK		(1 << 1)

/* Receive Descriptor 0 (ETH_RD0) Bits */

#define ETH_RD0_OWN		(1 << 31)
#define ETH_RD0_FF		(1 << 30)
#define ETH_RD0_FL_BIT		16
#define ETH_RD0_FL_MASK		(0x3fff << ETH_RD0_FL_BIT)
#define ETH_RD0_ES		(1 << 15)
#define ETH_RD0_DE		(1 << 14)
#define ETH_RD0_LE		(1 << 12)
#define ETH_RD0_RF		(1 << 11)
#define ETH_RD0_MF		(1 << 10)
#define ETH_RD0_FD		(1 << 9)
#define ETH_RD0_LD		(1 << 8)
#define ETH_RD0_TL		(1 << 7)
#define ETH_RD0_CS		(1 << 6)
#define ETH_RD0_FT		(1 << 5)
#define ETH_RD0_WT		(1 << 4)
#define ETH_RD0_ME		(1 << 3)
#define ETH_RD0_DB		(1 << 2)
#define ETH_RD0_CE		(1 << 1)

/* Receive Descriptor 1 (ETH_RD1) Bits */

#define ETH_RD1_RER		(1 << 25)
#define ETH_RD1_RCH		(1 << 24)
#define ETH_RD1_RBS2_BIT	11
#define ETH_RD1_RBS2_MASK	(0x7ff << ETH_RD1_RBS2_BIT)
#define ETH_RD1_RBS1_BIT	0
#define ETH_RD1_RBS1_MASK	(0x7ff << ETH_RD1_RBS1_BIT)

/* Transmit Descriptor 0 (ETH_TD0) Bits */

#define ETH_TD0_OWN		(1 << 31)
#define ETH_TD0_FA		(1 << 15)
#define ETH_TD0_LOC		(1 << 11)
#define ETH_TD0_NC		(1 << 10)
#define ETH_TD0_LC		(1 << 9)
#define ETH_TD0_EC		(1 << 8)
#define ETH_TD0_HBF		(1 << 7)
#define ETH_TD0_CC_BIT		3
#define ETH_TD0_CC_MASK		(0xf << ETH_TD0_CC_BIT)
#define ETH_TD0_ED		(1 << 2)
#define ETH_TD0_UF		(1 << 1)
#define ETH_TD0_DF		(1 << 0)

/* Transmit Descriptor 1 (ETH_TD1) Bits */

#define ETH_TD1_IC		(1 << 31)
#define ETH_TD1_LS		(1 << 30)
#define ETH_TD1_FS		(1 << 29)
#define ETH_TD1_AC		(1 << 26)
#define ETH_TD1_TER		(1 << 25)
#define ETH_TD1_TCH		(1 << 24)
#define ETH_TD1_DPD		(1 << 23)
#define ETH_TD1_TBS2_BIT	11
#define ETH_TD1_TBS2_MASK	(0x7ff << ETH_TD1_TBS2_BIT)
#define ETH_TD1_TBS1_BIT	0
#define ETH_TD1_TBS1_MASK	(0x7ff << ETH_TD1_TBS1_BIT)




/*************************************************************************
 * WDT
 *************************************************************************/
#define WDT_WTCSR	(WDT_BASE + 0x00)
#define WDT_WTCNT	(WDT_BASE + 0x04)

#define REG_WDT_WTCSR	REG8(WDT_WTCSR)
#define REG_WDT_WTCNT	REG32(WDT_WTCNT)

#define WDT_WTCSR_START	(1 << 4)




/*************************************************************************
 * OST
 *************************************************************************/
#define OST_TER		(OST_BASE + 0x00)
#define OST_TRDR(n)	(OST_BASE + 0x10 + ((n) * 0x20))
#define OST_TCNT(n)	(OST_BASE + 0x14 + ((n) * 0x20))
#define OST_TCSR(n)	(OST_BASE + 0x18 + ((n) * 0x20))
#define OST_TCRB(n)	(OST_BASE + 0x1c + ((n) * 0x20))

#define REG_OST_TER	REG8(OST_TER)
#define REG_OST_TRDR(n)	REG32(OST_TRDR((n)))
#define REG_OST_TCNT(n)	REG32(OST_TCNT((n)))
#define REG_OST_TCSR(n)	REG16(OST_TCSR((n)))
#define REG_OST_TCRB(n)	REG32(OST_TCRB((n)))

#define OST_TCSR_BUSY		(1 << 7)
#define OST_TCSR_UF		(1 << 6)
#define OST_TCSR_UIE		(1 << 5)
#define OST_TCSR_CKS_BIT	0
#define OST_TCSR_CKS_MASK	(0x07 << OST_TCSR_CKS_BIT)
  #define OST_TCSR_CKS_PCLK_4	(0 << OST_TCSR_CKS_BIT)
  #define OST_TCSR_CKS_PCLK_16	(1 << OST_TCSR_CKS_BIT)
  #define OST_TCSR_CKS_PCLK_64	(2 << OST_TCSR_CKS_BIT)
  #define OST_TCSR_CKS_PCLK_256	(3 << OST_TCSR_CKS_BIT)
  #define OST_TCSR_CKS_RTCCLK	(4 << OST_TCSR_CKS_BIT)
  #define OST_TCSR_CKS_EXTAL	(5 << OST_TCSR_CKS_BIT)

#define OST_TCSR0       OST_TCSR(0)
#define OST_TCSR1       OST_TCSR(1)
#define OST_TCSR2       OST_TCSR(2)
#define OST_TRDR0       OST_TRDR(0)
#define OST_TRDR1       OST_TRDR(1)
#define OST_TRDR2       OST_TRDR(2)
#define OST_TCNT0       OST_TCNT(0)
#define OST_TCNT1       OST_TCNT(1)
#define OST_TCNT2       OST_TCNT(2)
#define OST_TCRB0       OST_TCRB(0)
#define OST_TCRB1       OST_TCRB(1)
#define OST_TCRB2       OST_TCRB(2)

/*************************************************************************
 * UART
 *************************************************************************/

#define IRDA_BASE	UART0_BASE
#define UART_BASE	UART0_BASE
#define UART_OFF	0x1000

/* register offset */
#define OFF_RDR		(0x00)	/* R  8b H'xx */
#define OFF_TDR		(0x00)	/* W  8b H'xx */
#define OFF_DLLR	(0x00)	/* RW 8b H'00 */
#define OFF_DLHR	(0x04)	/* RW 8b H'00 */
#define OFF_IER		(0x04)	/* RW 8b H'00 */
#define OFF_ISR		(0x08)	/* R  8b H'01 */
#define OFF_FCR		(0x08)	/* W  8b H'00 */
#define OFF_LCR		(0x0C)	/* RW 8b H'00 */
#define OFF_MCR		(0x10)	/* RW 8b H'00 */
#define OFF_LSR		(0x14)	/* R  8b H'00 */
#define OFF_MSR		(0x18)	/* R  8b H'00 */
#define OFF_SPR		(0x1C)	/* RW 8b H'00 */
#define OFF_MCR		(0x10)	/* RW 8b H'00 */
#define OFF_SIRCR	(0x20)	/* RW 8b H'00, UART0 */

/* register address */
#define UART0_RDR	(UART0_BASE + OFF_RDR)
#define UART0_TDR	(UART0_BASE + OFF_TDR)
#define UART0_DLLR	(UART0_BASE + OFF_DLLR)
#define UART0_DLHR	(UART0_BASE + OFF_DLHR)
#define UART0_IER	(UART0_BASE + OFF_IER)
#define UART0_ISR	(UART0_BASE + OFF_ISR)
#define UART0_FCR	(UART0_BASE + OFF_FCR)
#define UART0_LCR	(UART0_BASE + OFF_LCR)
#define UART0_MCR	(UART0_BASE + OFF_MCR)
#define UART0_LSR	(UART0_BASE + OFF_LSR)
#define UART0_MSR	(UART0_BASE + OFF_MSR)
#define UART0_SPR	(UART0_BASE + OFF_SPR)
#define UART0_SIRCR	(UART0_BASE + OFF_SIRCR)

#define UART1_RDR	(UART1_BASE + OFF_RDR)
#define UART1_TDR	(UART1_BASE + OFF_TDR)
#define UART1_DLLR	(UART1_BASE + OFF_DLLR)
#define UART1_DLHR	(UART1_BASE + OFF_DLHR)
#define UART1_IER	(UART1_BASE + OFF_IER)
#define UART1_ISR	(UART1_BASE + OFF_ISR)
#define UART1_FCR	(UART1_BASE + OFF_FCR)
#define UART1_LCR	(UART1_BASE + OFF_LCR)
#define UART1_MCR	(UART1_BASE + OFF_MCR)
#define UART1_LSR	(UART1_BASE + OFF_LSR)
#define UART1_MSR	(UART1_BASE + OFF_MSR)
#define UART1_SPR	(UART1_BASE + OFF_SPR)
#define UART1_SIRCR	(UART1_BASE + OFF_SIRCR)

#define UART2_RDR	(UART2_BASE + OFF_RDR)
#define UART2_TDR	(UART2_BASE + OFF_TDR)
#define UART2_DLLR	(UART2_BASE + OFF_DLLR)
#define UART2_DLHR	(UART2_BASE + OFF_DLHR)
#define UART2_IER	(UART2_BASE + OFF_IER)
#define UART2_ISR	(UART2_BASE + OFF_ISR)
#define UART2_FCR	(UART2_BASE + OFF_FCR)
#define UART2_LCR	(UART2_BASE + OFF_LCR)
#define UART2_MCR	(UART2_BASE + OFF_MCR)
#define UART2_LSR	(UART2_BASE + OFF_LSR)
#define UART2_MSR	(UART2_BASE + OFF_MSR)
#define UART2_SPR	(UART2_BASE + OFF_SPR)
#define UART2_SIRCR	(UART2_BASE + OFF_SIRCR)

#define UART3_RDR	(UART3_BASE + OFF_RDR)
#define UART3_TDR	(UART3_BASE + OFF_TDR)
#define UART3_DLLR	(UART3_BASE + OFF_DLLR)
#define UART3_DLHR	(UART3_BASE + OFF_DLHR)
#define UART3_IER	(UART3_BASE + OFF_IER)
#define UART3_ISR	(UART3_BASE + OFF_ISR)
#define UART3_FCR	(UART3_BASE + OFF_FCR)
#define UART3_LCR	(UART3_BASE + OFF_LCR)
#define UART3_MCR	(UART3_BASE + OFF_MCR)
#define UART3_LSR	(UART3_BASE + OFF_LSR)
#define UART3_MSR	(UART3_BASE + OFF_MSR)
#define UART3_SPR	(UART3_BASE + OFF_SPR)
#define UART3_SIRCR	(UART3_BASE + OFF_SIRCR)

/*
 * Define macros for UART_IER
 * UART Interrupt Enable Register
 */
#define UART_IER_RIE	(1 << 0)	/* 0: receive fifo "full" interrupt disable */
#define UART_IER_TIE	(1 << 1)	/* 0: transmit fifo "empty" interrupt disable */
#define UART_IER_RLIE	(1 << 2)	/* 0: receive line status interrupt disable */
#define UART_IER_MIE	(1 << 3)	/* 0: modem status interrupt disable */
#define UART_IER_RTIE	(1 << 4)	/* 0: receive timeout interrupt disable */

/*
 * Define macros for UART_ISR
 * UART Interrupt Status Register
 */
#define UART_ISR_IP	(1 << 0)	/* 0: interrupt is pending  1: no interrupt */
#define UART_ISR_IID	(7 << 1)	/* Source of Interrupt */
#define UART_ISR_IID_MSI		(0 << 1)	/* Modem status interrupt */
#define UART_ISR_IID_THRI	(1 << 1)	/* Transmitter holding register empty */
#define UART_ISR_IID_RDI		(2 << 1)	/* Receiver data interrupt */
#define UART_ISR_IID_RLSI	(3 << 1)	/* Receiver line status interrupt */
#define UART_ISR_FFMS	(3 << 6)	/* FIFO mode select, set when UART_FCR.FE is set to 1 */
#define UART_ISR_FFMS_NO_FIFO	(0 << 6)
#define UART_ISR_FFMS_FIFO_MODE	(3 << 6)

/*
 * Define macros for UART_FCR
 * UART FIFO Control Register
 */
#define UART_FCR_FE	(1 << 0)	/* 0: non-FIFO mode  1: FIFO mode */
#define UART_FCR_RFLS	(1 << 1)	/* write 1 to flush receive FIFO */
#define UART_FCR_TFLS	(1 << 2)	/* write 1 to flush transmit FIFO */
#define UART_FCR_DMS	(1 << 3)	/* 0: disable DMA mode */
#define UART_FCR_UUE	(1 << 4)	/* 0: disable UART */
#define UART_FCR_RTRG	(3 << 6)	/* Receive FIFO Data Trigger */
#define UART_FCR_RTRG_1	(0 << 6)
#define UART_FCR_RTRG_4	(1 << 6)
#define UART_FCR_RTRG_8	(2 << 6)
#define UART_FCR_RTRG_15	(3 << 6)

/*
 * Define macros for UART_LCR
 * UART Line Control Register
 */
#define UART_LCR_WLEN	(3 << 0)	/* word length */
#define UART_LCR_WLEN_5	(0 << 0)
#define UART_LCR_WLEN_6	(1 << 0)
#define UART_LCR_WLEN_7	(2 << 0)
#define UART_LCR_WLEN_8	(3 << 0)
#define UART_LCR_STOP	(1 << 2)	/* 0: 1 stop bit when word length is 5,6,7,8
					   1: 1.5 stop bits when 5; 2 stop bits when 6,7,8 */
#define UART_LCR_STOP_1	(0 << 2)	/* 0: 1 stop bit when word length is 5,6,7,8
					   1: 1.5 stop bits when 5; 2 stop bits when 6,7,8 */
#define UART_LCR_STOP_2	(1 << 2)	/* 0: 1 stop bit when word length is 5,6,7,8
					   1: 1.5 stop bits when 5; 2 stop bits when 6,7,8 */

#define UART_LCR_PE	(1 << 3)	/* 0: parity disable */
#define UART_LCR_PROE	(1 << 4)	/* 0: even parity  1: odd parity */
#define UART_LCR_SPAR	(1 << 5)	/* 0: sticky parity disable */
#define UART_LCR_SBRK	(1 << 6)	/* write 0 normal, write 1 send break */
#define UART_LCR_DLAB	(1 << 7)	/* 0: access UART_RDR/TDR/IER  1: access UART_DLLR/DLHR */

/*
 * Define macros for UART_LSR
 * UART Line Status Register
 */
#define UART_LSR_DR	(1 << 0)	/* 0: receive FIFO is empty  1: receive data is ready */
#define UART_LSR_ORER	(1 << 1)	/* 0: no overrun error */
#define UART_LSR_PER	(1 << 2)	/* 0: no parity error */
#define UART_LSR_FER	(1 << 3)	/* 0; no framing error */
#define UART_LSR_BRK	(1 << 4)	/* 0: no break detected  1: receive a break signal */
#define UART_LSR_TDRQ	(1 << 5)	/* 1: transmit FIFO half "empty" */
#define UART_LSR_TEMT	(1 << 6)	/* 1: transmit FIFO and shift registers empty */
#define UART_LSR_RFER	(1 << 7)	/* 0: no receive error  1: receive error in FIFO mode */

/*
 * Define macros for UART_MCR
 * UART Modem Control Register
 */
#define UART_MCR_DTR	(1 << 0)	/* 0: DTR_ ouput high */
#define UART_MCR_RTS	(1 << 1)	/* 0: RTS_ output high */
#define UART_MCR_OUT1	(1 << 2)	/* 0: UART_MSR.RI is set to 0 and RI_ input high */
#define UART_MCR_OUT2	(1 << 3)	/* 0: UART_MSR.DCD is set to 0 and DCD_ input high */
#define UART_MCR_LOOP	(1 << 4)	/* 0: normal  1: loopback mode */
#define UART_MCR_MCE	(1 << 7)	/* 0: modem function is disable */

/*
 * Define macros for UART_MSR
 * UART Modem Status Register
 */
#define UART_MSR_DCTS	(1 << 0)	/* 0: no change on CTS_ pin since last read of UART_MSR */
#define UART_MSR_DDSR	(1 << 1)	/* 0: no change on DSR_ pin since last read of UART_MSR */
#define UART_MSR_DRI	(1 << 2)	/* 0: no change on RI_ pin since last read of UART_MSR */
#define UART_MSR_DDCD	(1 << 3)	/* 0: no change on DCD_ pin since last read of UART_MSR */
#define UART_MSR_CTS	(1 << 4)	/* 0: CTS_ pin is high */
#define UART_MSR_DSR	(1 << 5)	/* 0: DSR_ pin is high */
#define UART_MSR_RI	(1 << 6)	/* 0: RI_ pin is high */
#define UART_MSR_DCD	(1 << 7)	/* 0: DCD_ pin is high */

/*
 * Define macros for SIRCR
 * Slow IrDA Control Register
 */
#define SIRCR_TSIRE	(1 << 0)	/* 0: transmitter is in UART mode  1: IrDA mode */
#define SIRCR_RSIRE	(1 << 1)	/* 0: receiver is in UART mode  1: IrDA mode */
#define SIRCR_TPWS	(1 << 2)	/* 0: transmit 0 pulse width is 3/16 of bit length
					   1: 0 pulse width is 1.6us for 115.2Kbps */
#define SIRCR_TXPL	(1 << 3)	/* 0: encoder generates a positive pulse for 0 */
#define SIRCR_RXPL	(1 << 4)	/* 0: decoder interprets positive pulse as 0 */



/*************************************************************************
 * INTC
 *************************************************************************/
#define INTC_ISR	(INTC_BASE + 0x00)
#define INTC_IMR	(INTC_BASE + 0x04)
#define INTC_IMSR	(INTC_BASE + 0x08)
#define INTC_IMCR	(INTC_BASE + 0x0c)
#define INTC_IPR	(INTC_BASE + 0x10)

#define REG_INTC_ISR	REG32(INTC_ISR)
#define REG_INTC_IMR	REG32(INTC_IMR)
#define REG_INTC_IMSR	REG32(INTC_IMSR)
#define REG_INTC_IMCR	REG32(INTC_IMCR)
#define REG_INTC_IPR	REG32(INTC_IPR)

#define IRQ_I2C		1
#define IRQ_PS2		2
#define IRQ_UPRT	3
#define IRQ_CORE	4
#define IRQ_UART3	6
#define IRQ_UART2	7
#define IRQ_UART1	8
#define IRQ_UART0	9
#define IRQ_SCC1	10
#define IRQ_SCC0	11
#define IRQ_UDC		12
#define IRQ_UHC		13
#define IRQ_MSC		14
#define IRQ_RTC		15
#define IRQ_FIR		16
#define IRQ_SSI		17
#define IRQ_CIM		18
#define IRQ_ETH		19
#define IRQ_AIC		20
#define IRQ_DMAC	21
#define IRQ_OST2	22
#define IRQ_OST1	23
#define IRQ_OST0	24
#define IRQ_GPIO3	25
#define IRQ_GPIO2	26
#define IRQ_GPIO1	27
#define IRQ_GPIO0	28
#define IRQ_LCD		30




/*************************************************************************
 * CIM
 *************************************************************************/
#define	CIM_CFG			(CIM_BASE + 0x0000)
#define	CIM_CTRL		(CIM_BASE + 0x0004)
#define	CIM_STATE		(CIM_BASE + 0x0008)
#define	CIM_IID			(CIM_BASE + 0x000C)
#define	CIM_RXFIFO		(CIM_BASE + 0x0010)
#define	CIM_DA			(CIM_BASE + 0x0020)
#define	CIM_FA			(CIM_BASE + 0x0024)
#define	CIM_FID			(CIM_BASE + 0x0028)
#define	CIM_CMD			(CIM_BASE + 0x002C)

#define	REG_CIM_CFG		REG32(CIM_CFG)
#define	REG_CIM_CTRL		REG32(CIM_CTRL)
#define	REG_CIM_STATE		REG32(CIM_STATE)
#define	REG_CIM_IID		REG32(CIM_IID)
#define	REG_CIM_RXFIFO		REG32(CIM_RXFIFO)
#define	REG_CIM_DA		REG32(CIM_DA)
#define	REG_CIM_FA		REG32(CIM_FA)
#define	REG_CIM_FID		REG32(CIM_FID)
#define	REG_CIM_CMD		REG32(CIM_CMD)

/* CIM Configuration Register  (CIM_CFG) */

#define	CIM_CFG_INV_DAT		(1 << 15)
#define	CIM_CFG_VSP		(1 << 14)
#define	CIM_CFG_HSP		(1 << 13)
#define	CIM_CFG_PCP		(1 << 12)
#define	CIM_CFG_DUMMY_ZERO	(1 << 9)
#define	CIM_CFG_EXT_VSYNC	(1 << 8)
#define	CIM_CFG_PACK_BIT	4
#define	CIM_CFG_PACK_MASK	(0x7 << CIM_CFG_PACK_BIT)
  #define CIM_CFG_PACK_0	  (0 << CIM_CFG_PACK_BIT)
  #define CIM_CFG_PACK_1	  (1 << CIM_CFG_PACK_BIT)
  #define CIM_CFG_PACK_2	  (2 << CIM_CFG_PACK_BIT)
  #define CIM_CFG_PACK_3	  (3 << CIM_CFG_PACK_BIT)
  #define CIM_CFG_PACK_4	  (4 << CIM_CFG_PACK_BIT)
  #define CIM_CFG_PACK_5	  (5 << CIM_CFG_PACK_BIT)
  #define CIM_CFG_PACK_6	  (6 << CIM_CFG_PACK_BIT)
  #define CIM_CFG_PACK_7	  (7 << CIM_CFG_PACK_BIT)
#define	CIM_CFG_DSM_BIT		0
#define	CIM_CFG_DSM_MASK	(0x3 << CIM_CFG_DSM_BIT)
  #define CIM_CFG_DSM_CPM	  (0 << CIM_CFG_DSM_BIT) /* CCIR656 Progressive Mode */
  #define CIM_CFG_DSM_CIM	  (1 << CIM_CFG_DSM_BIT) /* CCIR656 Interlace Mode */
  #define CIM_CFG_DSM_GCM	  (2 << CIM_CFG_DSM_BIT) /* Gated Clock Mode */
  #define CIM_CFG_DSM_NGCM	  (3 << CIM_CFG_DSM_BIT) /* Non-Gated Clock Mode */

/* CIM Control Register  (CIM_CTRL) */

#define	CIM_CTRL_MCLKDIV_BIT	24
#define	CIM_CTRL_MCLKDIV_MASK	(0xff << CIM_CTRL_MCLKDIV_BIT)
#define	CIM_CTRL_FRC_BIT	16
#define	CIM_CTRL_FRC_MASK	(0xf << CIM_CTRL_FRC_BIT)
  #define CIM_CTRL_FRC_1	  (0x0 << CIM_CTRL_FRC_BIT) /* Sample every frame */
  #define CIM_CTRL_FRC_2	  (0x1 << CIM_CTRL_FRC_BIT) /* Sample 1/2 frame */
  #define CIM_CTRL_FRC_3	  (0x2 << CIM_CTRL_FRC_BIT) /* Sample 1/3 frame */
  #define CIM_CTRL_FRC_4	  (0x3 << CIM_CTRL_FRC_BIT) /* Sample 1/4 frame */
  #define CIM_CTRL_FRC_5	  (0x4 << CIM_CTRL_FRC_BIT) /* Sample 1/5 frame */
  #define CIM_CTRL_FRC_6	  (0x5 << CIM_CTRL_FRC_BIT) /* Sample 1/6 frame */
  #define CIM_CTRL_FRC_7	  (0x6 << CIM_CTRL_FRC_BIT) /* Sample 1/7 frame */
  #define CIM_CTRL_FRC_8	  (0x7 << CIM_CTRL_FRC_BIT) /* Sample 1/8 frame */
  #define CIM_CTRL_FRC_9	  (0x8 << CIM_CTRL_FRC_BIT) /* Sample 1/9 frame */
  #define CIM_CTRL_FRC_10	  (0x9 << CIM_CTRL_FRC_BIT) /* Sample 1/10 frame */
  #define CIM_CTRL_FRC_11	  (0xA << CIM_CTRL_FRC_BIT) /* Sample 1/11 frame */
  #define CIM_CTRL_FRC_12	  (0xB << CIM_CTRL_FRC_BIT) /* Sample 1/12 frame */
  #define CIM_CTRL_FRC_13	  (0xC << CIM_CTRL_FRC_BIT) /* Sample 1/13 frame */
  #define CIM_CTRL_FRC_14	  (0xD << CIM_CTRL_FRC_BIT) /* Sample 1/14 frame */
  #define CIM_CTRL_FRC_15	  (0xE << CIM_CTRL_FRC_BIT) /* Sample 1/15 frame */
  #define CIM_CTRL_FRC_16	  (0xF << CIM_CTRL_FRC_BIT) /* Sample 1/16 frame */
#define	CIM_CTRL_VDDM		(1 << 13)
#define	CIM_CTRL_DMA_SOFM	(1 << 12)
#define	CIM_CTRL_DMA_EOFM	(1 << 11)
#define	CIM_CTRL_DMA_STOPM	(1 << 10)
#define	CIM_CTRL_RXF_TRIGM	(1 << 9)
#define	CIM_CTRL_RXF_OFM	(1 << 8)
#define	CIM_CTRL_RXF_TRIG_BIT	4
#define	CIM_CTRL_RXF_TRIG_MASK	(0x7 << CIM_CTRL_RXF_TRIG_BIT)
  #define CIM_CTRL_RXF_TRIG_4	  (0 << CIM_CTRL_RXF_TRIG_BIT) /* RXFIFO Trigger Value is 4 */
  #define CIM_CTRL_RXF_TRIG_8	  (1 << CIM_CTRL_RXF_TRIG_BIT) /* RXFIFO Trigger Value is 8 */
  #define CIM_CTRL_RXF_TRIG_12	  (2 << CIM_CTRL_RXF_TRIG_BIT) /* RXFIFO Trigger Value is 12 */
  #define CIM_CTRL_RXF_TRIG_16	  (3 << CIM_CTRL_RXF_TRIG_BIT) /* RXFIFO Trigger Value is 16 */
  #define CIM_CTRL_RXF_TRIG_20	  (4 << CIM_CTRL_RXF_TRIG_BIT) /* RXFIFO Trigger Value is 20 */
  #define CIM_CTRL_RXF_TRIG_24	  (5 << CIM_CTRL_RXF_TRIG_BIT) /* RXFIFO Trigger Value is 24 */
  #define CIM_CTRL_RXF_TRIG_28	  (6 << CIM_CTRL_RXF_TRIG_BIT) /* RXFIFO Trigger Value is 28 */
  #define CIM_CTRL_RXF_TRIG_32	  (7 << CIM_CTRL_RXF_TRIG_BIT) /* RXFIFO Trigger Value is 32 */
#define	CIM_CTRL_DMA_EN		(1 << 2)
#define	CIM_CTRL_RXF_RST	(1 << 1)
#define	CIM_CTRL_ENA		(1 << 0)

/* CIM State Register  (CIM_STATE) */

#define	CIM_STATE_DMA_SOF	(1 << 6)
#define	CIM_STATE_DMA_EOF	(1 << 5)
#define	CIM_STATE_DMA_STOP	(1 << 4)
#define	CIM_STATE_RXF_OF	(1 << 3)
#define	CIM_STATE_RXF_TRIG	(1 << 2)
#define	CIM_STATE_RXF_EMPTY	(1 << 1)
#define	CIM_STATE_VDD		(1 << 0)

/* CIM DMA Command Register (CIM_CMD) */

#define	CIM_CMD_SOFINT		(1 << 31)
#define	CIM_CMD_EOFINT		(1 << 30)
#define	CIM_CMD_STOP		(1 << 28)
#define	CIM_CMD_LEN_BIT		0
#define	CIM_CMD_LEN_MASK	(0xffffff << CIM_CMD_LEN_BIT)




/*************************************************************************
 * PWM
 *************************************************************************/
#define	PWM_CTR(n)		(PWM##n##_BASE + 0x000)
#define	PWM_PER(n)		(PWM##n##_BASE + 0x004)
#define	PWM_DUT(n)		(PWM##n##_BASE + 0x008)

#define	REG_PWM_CTR(n)		REG8(PWM_CTR(n))
#define	REG_PWM_PER(n)		REG16(PWM_PER(n))
#define REG_PWM_DUT(n)		REG16(PWM_DUT(n))

/* PWM Control Register (PWM_CTR) */

#define	PWM_CTR_EN		(1 << 7)
#define	PWM_CTR_SD		(1 << 6)
#define	PWM_CTR_PRESCALE_BIT	0
#define	PWM_CTR_PRESCALE_MASK	(0x3f << PWM_CTR_PRESCALE_BIT)

/* PWM Period Register (PWM_PER) */

#define	PWM_PER_PERIOD_BIT	0
#define	PWM_PER_PERIOD_MASK	(0x3ff << PWM_PER_PERIOD_BIT)

/* PWM Duty Register (PWM_DUT) */

#define PWM_DUT_FDUTY		(1 << 10)
#define PWM_DUT_DUTY_BIT	0
#define PWM_DUT_DUTY_MASK	(0x3ff << PWM_DUT_DUTY_BIT)




/*************************************************************************
 * EMC
 *************************************************************************/
#define EMC_BCR		(EMC_BASE + 0x00)
#define EMC_SMCR0	(EMC_BASE + 0x10)
#define EMC_SMCR1	(EMC_BASE + 0x14)
#define EMC_SMCR2	(EMC_BASE + 0x18)
#define EMC_SMCR3	(EMC_BASE + 0x1c)
#define EMC_SMCR4	(EMC_BASE + 0x20)
#define EMC_SMCR5	(EMC_BASE + 0x24)
#define EMC_SMCR6	(EMC_BASE + 0x28)
#define EMC_SMCR7	(EMC_BASE + 0x2c)
#define EMC_SACR0	(EMC_BASE + 0x30)
#define EMC_SACR1	(EMC_BASE + 0x34)
#define EMC_SACR2	(EMC_BASE + 0x38)
#define EMC_SACR3	(EMC_BASE + 0x3c)
#define EMC_SACR4	(EMC_BASE + 0x40)
#define EMC_SACR5	(EMC_BASE + 0x44)
#define EMC_SACR6	(EMC_BASE + 0x48)
#define EMC_SACR7	(EMC_BASE + 0x4c)
#define EMC_NFCSR	(EMC_BASE + 0x50)
#define EMC_NFECC	(EMC_BASE + 0x54)
#define EMC_PCCR1	(EMC_BASE + 0x60)
#define EMC_PCCR2	(EMC_BASE + 0x64)
#define EMC_PCCR3	(EMC_BASE + 0x68)
#define EMC_PCCR4	(EMC_BASE + 0x6c)
#define EMC_DMCR	(EMC_BASE + 0x80)
#define EMC_RTCSR	(EMC_BASE + 0x84)
#define EMC_RTCNT	(EMC_BASE + 0x88)
#define EMC_RTCOR	(EMC_BASE + 0x8c)
#define EMC_DMAR1	(EMC_BASE + 0x90)
#define EMC_DMAR2	(EMC_BASE + 0x94)
#define EMC_DMAR3	(EMC_BASE + 0x98)
#define EMC_DMAR4	(EMC_BASE + 0x9c)

#define EMC_SDMR0	(EMC_BASE + 0xa000)
#define EMC_SDMR1	(EMC_BASE + 0xb000)
#define EMC_SDMR2	(EMC_BASE + 0xc000)
#define EMC_SDMR3	(EMC_BASE + 0xd000)

/* NAND command/address/data port */
#define NAND_DATAPORT    0xB4000000  /* read-write area */
#define NAND_COMMPORT    0xB4040000  /* write only area */
#define NAND_ADDRPORT    0xB4080000  /* write only area */

#define REG_EMC_BCR	REG32(EMC_BCR)
#define REG_EMC_SMCR0	REG32(EMC_SMCR0)
#define REG_EMC_SMCR1	REG32(EMC_SMCR1)
#define REG_EMC_SMCR2	REG32(EMC_SMCR2)
#define REG_EMC_SMCR3	REG32(EMC_SMCR3)
#define REG_EMC_SMCR4	REG32(EMC_SMCR4)
#define REG_EMC_SMCR5	REG32(EMC_SMCR5)
#define REG_EMC_SMCR6	REG32(EMC_SMCR6)
#define REG_EMC_SMCR7	REG32(EMC_SMCR7)
#define REG_EMC_SACR0	REG32(EMC_SACR0)
#define REG_EMC_SACR1	REG32(EMC_SACR1)
#define REG_EMC_SACR2	REG32(EMC_SACR2)
#define REG_EMC_SACR3	REG32(EMC_SACR3)
#define REG_EMC_SACR4	REG32(EMC_SACR4)
#define REG_EMC_SACR5	REG32(EMC_SACR5)
#define REG_EMC_SACR6	REG32(EMC_SACR6)
#define REG_EMC_SACR7	REG32(EMC_SACR7)
#define REG_EMC_NFCSR	REG32(EMC_NFCSR)
#define REG_EMC_NFECC	REG32(EMC_NFECC)
#define REG_EMC_DMCR	REG32(EMC_DMCR)
#define REG_EMC_RTCSR	REG16(EMC_RTCSR)
#define REG_EMC_RTCNT	REG16(EMC_RTCNT)
#define REG_EMC_RTCOR	REG16(EMC_RTCOR)
#define REG_EMC_DMAR1	REG32(EMC_DMAR1)
#define REG_EMC_DMAR2	REG32(EMC_DMAR2)
#define REG_EMC_DMAR3	REG32(EMC_DMAR3)
#define REG_EMC_DMAR4	REG32(EMC_DMAR4)
#define REG_EMC_PCCR1	REG32(EMC_PCCR1)
#define REG_EMC_PCCR2	REG32(EMC_PCCR2)
#define REG_EMC_PCCR3	REG32(EMC_PCCR3)
#define REG_EMC_PCCR4	REG32(EMC_PCCR4)


#define EMC_BCR_BRE		(1 << 1)

#define EMC_SMCR_STRV_BIT	24
#define EMC_SMCR_STRV_MASK	(0x0f << EMC_SMCR_STRV_BIT)
#define EMC_SMCR_TAW_BIT	20
#define EMC_SMCR_TAW_MASK	(0x0f << EMC_SMCR_TAW_BIT)
#define EMC_SMCR_TBP_BIT	16
#define EMC_SMCR_TBP_MASK	(0x0f << EMC_SMCR_TBP_BIT)
#define EMC_SMCR_TAH_BIT	12
#define EMC_SMCR_TAH_MASK	(0x07 << EMC_SMCR_TAH_BIT)
#define EMC_SMCR_TAS_BIT	8
#define EMC_SMCR_TAS_MASK	(0x07 << EMC_SMCR_TAS_BIT)
#define EMC_SMCR_BW_BIT		6
#define EMC_SMCR_BW_MASK	(0x03 << EMC_SMCR_BW_BIT)
  #define EMC_SMCR_BW_8BIT	(0 << EMC_SMCR_BW_BIT)
  #define EMC_SMCR_BW_16BIT	(1 << EMC_SMCR_BW_BIT)
  #define EMC_SMCR_BW_32BIT	(2 << EMC_SMCR_BW_BIT)
#define EMC_SMCR_BCM		(1 << 3)
#define EMC_SMCR_BL_BIT		1
#define EMC_SMCR_BL_MASK	(0x03 << EMC_SMCR_BL_BIT)
  #define EMC_SMCR_BL_4		(0 << EMC_SMCR_BL_BIT)
  #define EMC_SMCR_BL_8		(1 << EMC_SMCR_BL_BIT)
  #define EMC_SMCR_BL_16	(2 << EMC_SMCR_BL_BIT)
  #define EMC_SMCR_BL_32	(3 << EMC_SMCR_BL_BIT)
#define EMC_SMCR_SMT		(1 << 0)

#define EMC_SACR_BASE_BIT	8
#define EMC_SACR_BASE_MASK	(0xff << EMC_SACR_BASE_BIT)
#define EMC_SACR_MASK_BIT	0
#define EMC_SACR_MASK_MASK	(0xff << EMC_SACR_MASK_BIT)

#define EMC_NFCSR_RB		(1 << 7)
#define EMC_NFCSR_BOOT_SEL_BIT	4
#define EMC_NFCSR_BOOT_SEL_MASK	(0x07 << EMC_NFCSR_BOOT_SEL_BIT)
#define EMC_NFCSR_ERST		(1 << 3)
#define EMC_NFCSR_ECCE		(1 << 2)
#define EMC_NFCSR_FCE		(1 << 1)
#define EMC_NFCSR_NFE		(1 << 0)

#define EMC_NFECC_ECC2_BIT	16
#define EMC_NFECC_ECC2_MASK	(0xff << EMC_NFECC_ECC2_BIT)
#define EMC_NFECC_ECC1_BIT	8
#define EMC_NFECC_ECC1_MASK	(0xff << EMC_NFECC_ECC1_BIT)
#define EMC_NFECC_ECC0_BIT	0
#define EMC_NFECC_ECC0_MASK	(0xff << EMC_NFECC_ECC0_BIT)

#define EMC_DMCR_BW_BIT		31
#define EMC_DMCR_BW		(1 << EMC_DMCR_BW_BIT)
  #define EMC_DMCR_BW_32	(0 << EMC_DMCR_BW_BIT)
  #define EMC_DMCR_BW_16	(1 << EMC_DMCR_BW_BIT)
#define EMC_DMCR_CA_BIT		26
#define EMC_DMCR_CA_MASK	(0x07 << EMC_DMCR_CA_BIT)
  #define EMC_DMCR_CA_8		(0 << EMC_DMCR_CA_BIT)
  #define EMC_DMCR_CA_9		(1 << EMC_DMCR_CA_BIT)
  #define EMC_DMCR_CA_10	(2 << EMC_DMCR_CA_BIT)
  #define EMC_DMCR_CA_11	(3 << EMC_DMCR_CA_BIT)
  #define EMC_DMCR_CA_12	(4 << EMC_DMCR_CA_BIT)
#define EMC_DMCR_RMODE		(1 << 25)
#define EMC_DMCR_RFSH		(1 << 24)
#define EMC_DMCR_MRSET		(1 << 23)
#define EMC_DMCR_RA_BIT		20
#define EMC_DMCR_RA_MASK	(0x03 << EMC_DMCR_RA_BIT)
  #define EMC_DMCR_RA_11	(0 << EMC_DMCR_RA_BIT)
  #define EMC_DMCR_RA_12	(1 << EMC_DMCR_RA_BIT)
  #define EMC_DMCR_RA_13	(2 << EMC_DMCR_RA_BIT)
#define EMC_DMCR_BA_BIT		19
#define EMC_DMCR_BA		(1 << EMC_DMCR_BA_BIT)
  #define EMC_DMCR_BA_2		(0 << EMC_DMCR_BA_BIT)
  #define EMC_DMCR_BA_4		(1 << EMC_DMCR_BA_BIT)
#define EMC_DMCR_PDM		(1 << 18)
#define EMC_DMCR_EPIN		(1 << 17)
#define EMC_DMCR_TRAS_BIT	13
#define EMC_DMCR_TRAS_MASK	(0x07 << EMC_DMCR_TRAS_BIT)
#define EMC_DMCR_RCD_BIT	11
#define EMC_DMCR_RCD_MASK	(0x03 << EMC_DMCR_RCD_BIT)
#define EMC_DMCR_TPC_BIT	8
#define EMC_DMCR_TPC_MASK	(0x07 << EMC_DMCR_TPC_BIT)
#define EMC_DMCR_TRWL_BIT	5
#define EMC_DMCR_TRWL_MASK	(0x03 << EMC_DMCR_TRWL_BIT)
#define EMC_DMCR_TRC_BIT	2
#define EMC_DMCR_TRC_MASK	(0x07 << EMC_DMCR_TRC_BIT)
#define EMC_DMCR_TCL_BIT	0
#define EMC_DMCR_TCL_MASK	(0x03 << EMC_DMCR_TCL_BIT)
  #define EMC_DMCR_CASL_2	(1 << EMC_DMCR_TCL_BIT)
  #define EMC_DMCR_CASL_3	(2 << EMC_DMCR_TCL_BIT)

#define EMC_RTCSR_CMF		(1 << 7)
#define EMC_RTCSR_CKS_BIT	0
#define EMC_RTCSR_CKS_MASK	(0x07 << EMC_RTCSR_CKS_BIT)
  #define EMC_RTCSR_CKS_DISABLE	(0 << EMC_RTCSR_CKS_BIT)
  #define EMC_RTCSR_CKS_4	(1 << EMC_RTCSR_CKS_BIT)
  #define EMC_RTCSR_CKS_16	(2 << EMC_RTCSR_CKS_BIT)
  #define EMC_RTCSR_CKS_64	(3 << EMC_RTCSR_CKS_BIT)
  #define EMC_RTCSR_CKS_256	(4 << EMC_RTCSR_CKS_BIT)
  #define EMC_RTCSR_CKS_1024	(5 << EMC_RTCSR_CKS_BIT)
  #define EMC_RTCSR_CKS_2048	(6 << EMC_RTCSR_CKS_BIT)
  #define EMC_RTCSR_CKS_4096	(7 << EMC_RTCSR_CKS_BIT)

#define EMC_DMAR_BASE_BIT	8
#define EMC_DMAR_BASE_MASK	(0xff << EMC_DMAR_BASE_BIT)
#define EMC_DMAR_MASK_BIT	0
#define EMC_DMAR_MASK_MASK	(0xff << EMC_DMAR_MASK_BIT)

#define EMC_SDMR_BM		(1 << 9)
#define EMC_SDMR_OM_BIT		7
#define EMC_SDMR_OM_MASK	(3 << EMC_SDMR_OM_BIT)
  #define EMC_SDMR_OM_NORMAL	(0 << EMC_SDMR_OM_BIT)
#define EMC_SDMR_CAS_BIT	4
#define EMC_SDMR_CAS_MASK	(7 << EMC_SDMR_CAS_BIT)
  #define EMC_SDMR_CAS_1	(1 << EMC_SDMR_CAS_BIT)
  #define EMC_SDMR_CAS_2	(2 << EMC_SDMR_CAS_BIT)
  #define EMC_SDMR_CAS_3	(3 << EMC_SDMR_CAS_BIT)
#define EMC_SDMR_BT_BIT		3
#define EMC_SDMR_BT_MASK	(1 << EMC_SDMR_BT_BIT)
  #define EMC_SDMR_BT_SEQ	(0 << EMC_SDMR_BT_BIT)
  #define EMC_SDMR_BT_INTR	(1 << EMC_SDMR_BT_BIT)
#define EMC_SDMR_BL_BIT		0
#define EMC_SDMR_BL_MASK	(7 << EMC_SDMR_BL_BIT)
  #define EMC_SDMR_BL_1		(0 << EMC_SDMR_BL_BIT)
  #define EMC_SDMR_BL_2		(1 << EMC_SDMR_BL_BIT)
  #define EMC_SDMR_BL_4		(2 << EMC_SDMR_BL_BIT)
  #define EMC_SDMR_BL_8		(3 << EMC_SDMR_BL_BIT)

#define EMC_SDMR_CAS2_16BIT \
  (EMC_SDMR_CAS_2 | EMC_SDMR_BT_SEQ | EMC_SDMR_BL_2)
#define EMC_SDMR_CAS2_32BIT \
  (EMC_SDMR_CAS_2 | EMC_SDMR_BT_SEQ | EMC_SDMR_BL_4)
#define EMC_SDMR_CAS3_16BIT \
  (EMC_SDMR_CAS_3 | EMC_SDMR_BT_SEQ | EMC_SDMR_BL_2)
#define EMC_SDMR_CAS3_32BIT \
  (EMC_SDMR_CAS_3 | EMC_SDMR_BT_SEQ | EMC_SDMR_BL_4)

#define EMC_PCCR12_AMW		(1 << 31)
#define EMC_PCCR12_AMAS_BIT	28
#define EMC_PCCR12_AMAS_MASK	(0x07 << EMC_PCCR12_AMAS_BIT)
#define EMC_PCCR12_AMAH_BIT	24
#define EMC_PCCR12_AMAH_MASK	(0x07 << EMC_PCCR12_AMAH_BIT)
#define EMC_PCCR12_AMPW_BIT	20
#define EMC_PCCR12_AMPW_MASK	(0x0f << EMC_PCCR12_AMPW_BIT)
#define EMC_PCCR12_AMRT_BIT	16
#define EMC_PCCR12_AMRT_MASK	(0x0f << EMC_PCCR12_AMRT_BIT)
#define EMC_PCCR12_CMW		(1 << 15)
#define EMC_PCCR12_CMAS_BIT	12
#define EMC_PCCR12_CMAS_MASK	(0x07 << EMC_PCCR12_CMAS_BIT)
#define EMC_PCCR12_CMAH_BIT	8
#define EMC_PCCR12_CMAH_MASK	(0x07 << EMC_PCCR12_CMAH_BIT)
#define EMC_PCCR12_CMPW_BIT	4
#define EMC_PCCR12_CMPW_MASK	(0x0f << EMC_PCCR12_CMPW_BIT)
#define EMC_PCCR12_CMRT_BIT	0
#define EMC_PCCR12_CMRT_MASK	(0x07 << EMC_PCCR12_CMRT_BIT)

#define EMC_PCCR34_DRS_BIT	16
#define EMC_PCCR34_DRS_MASK	(0x03 << EMC_PCCR34_DRS_BIT)
  #define EMC_PCCR34_DRS_SPKR	(1 << EMC_PCCR34_DRS_BIT)
  #define EMC_PCCR34_DRS_IOIS16	(2 << EMC_PCCR34_DRS_BIT)
  #define EMC_PCCR34_DRS_INPACK	(3 << EMC_PCCR34_DRS_BIT)
#define EMC_PCCR34_IOIS16	(1 << 15)
#define EMC_PCCR34_IOW		(1 << 14)
#define EMC_PCCR34_TCB_BIT	12
#define EMC_PCCR34_TCB_MASK	(0x03 << EMC_PCCR34_TCB_BIT)
#define EMC_PCCR34_IORT_BIT	8
#define EMC_PCCR34_IORT_MASK	(0x07 << EMC_PCCR34_IORT_BIT)
#define EMC_PCCR34_IOAE_BIT	6
#define EMC_PCCR34_IOAE_MASK	(0x03 << EMC_PCCR34_IOAE_BIT)
  #define EMC_PCCR34_IOAE_NONE	(0 << EMC_PCCR34_IOAE_BIT)
  #define EMC_PCCR34_IOAE_1	(1 << EMC_PCCR34_IOAE_BIT)
  #define EMC_PCCR34_IOAE_2	(2 << EMC_PCCR34_IOAE_BIT)
  #define EMC_PCCR34_IOAE_5	(3 << EMC_PCCR34_IOAE_BIT)
#define EMC_PCCR34_IOAH_BIT	4
#define EMC_PCCR34_IOAH_MASK	(0x03 << EMC_PCCR34_IOAH_BIT)
  #define EMC_PCCR34_IOAH_NONE	(0 << EMC_PCCR34_IOAH_BIT)
  #define EMC_PCCR34_IOAH_1	(1 << EMC_PCCR34_IOAH_BIT)
  #define EMC_PCCR34_IOAH_2	(2 << EMC_PCCR34_IOAH_BIT)
  #define EMC_PCCR34_IOAH_5	(3 << EMC_PCCR34_IOAH_BIT)
#define EMC_PCCR34_IOPW_BIT	0
#define EMC_PCCR34_IOPW_MASK	(0x0f << EMC_PCCR34_IOPW_BIT)




/*************************************************************************
 * GPIO
 *************************************************************************/
#define GPIO_GPDR(n)	(GPIO_BASE + (0x00 + (n)*0x30))
#define GPIO_GPDIR(n)	(GPIO_BASE + (0x04 + (n)*0x30))
#define GPIO_GPODR(n)	(GPIO_BASE + (0x08 + (n)*0x30))
#define GPIO_GPPUR(n)	(GPIO_BASE + (0x0c + (n)*0x30))
#define GPIO_GPALR(n)	(GPIO_BASE + (0x10 + (n)*0x30))
#define GPIO_GPAUR(n)	(GPIO_BASE + (0x14 + (n)*0x30))
#define GPIO_GPIDLR(n)	(GPIO_BASE + (0x18 + (n)*0x30))
#define GPIO_GPIDUR(n)	(GPIO_BASE + (0x1c + (n)*0x30))
#define GPIO_GPIER(n)	(GPIO_BASE + (0x20 + (n)*0x30))
#define GPIO_GPIMR(n)	(GPIO_BASE + (0x24 + (n)*0x30))
#define GPIO_GPFR(n)	(GPIO_BASE + (0x28 + (n)*0x30))

#define REG_GPIO_GPDR(n)	REG32(GPIO_GPDR((n)))
#define REG_GPIO_GPDIR(n)	REG32(GPIO_GPDIR((n)))
#define REG_GPIO_GPODR(n)	REG32(GPIO_GPODR((n)))
#define REG_GPIO_GPPUR(n)	REG32(GPIO_GPPUR((n)))
#define REG_GPIO_GPALR(n)	REG32(GPIO_GPALR((n)))
#define REG_GPIO_GPAUR(n)	REG32(GPIO_GPAUR((n)))
#define REG_GPIO_GPIDLR(n)	REG32(GPIO_GPIDLR((n)))
#define REG_GPIO_GPIDUR(n)	REG32(GPIO_GPIDUR((n)))
#define REG_GPIO_GPIER(n)	REG32(GPIO_GPIER((n)))
#define REG_GPIO_GPIMR(n)	REG32(GPIO_GPIMR((n)))
#define REG_GPIO_GPFR(n)	REG32(GPIO_GPFR((n)))

#define GPIO_IRQ_LOLEVEL  0
#define GPIO_IRQ_HILEVEL  1
#define GPIO_IRQ_FALLEDG  2
#define GPIO_IRQ_RAISEDG  3

#define IRQ_GPIO_0	48
#define NUM_GPIO	100

#define GPIO_GPDR0      GPIO_GPDR(0)
#define GPIO_GPDR1      GPIO_GPDR(1)
#define GPIO_GPDR2      GPIO_GPDR(2)
#define GPIO_GPDR3      GPIO_GPDR(3)
#define GPIO_GPDIR0     GPIO_GPDIR(0)
#define GPIO_GPDIR1     GPIO_GPDIR(1)
#define GPIO_GPDIR2     GPIO_GPDIR(2)
#define GPIO_GPDIR3     GPIO_GPDIR(3)
#define GPIO_GPODR0     GPIO_GPODR(0)
#define GPIO_GPODR1     GPIO_GPODR(1)
#define GPIO_GPODR2     GPIO_GPODR(2)
#define GPIO_GPODR3     GPIO_GPODR(3)
#define GPIO_GPPUR0     GPIO_GPPUR(0)
#define GPIO_GPPUR1     GPIO_GPPUR(1)
#define GPIO_GPPUR2     GPIO_GPPUR(2)
#define GPIO_GPPUR3     GPIO_GPPUR(3)
#define GPIO_GPALR0     GPIO_GPALR(0)
#define GPIO_GPALR1     GPIO_GPALR(1)
#define GPIO_GPALR2     GPIO_GPALR(2)
#define GPIO_GPALR3     GPIO_GPALR(3)
#define GPIO_GPAUR0     GPIO_GPAUR(0)
#define GPIO_GPAUR1     GPIO_GPAUR(1)
#define GPIO_GPAUR2     GPIO_GPAUR(2)
#define GPIO_GPAUR3     GPIO_GPAUR(3)
#define GPIO_GPIDLR0    GPIO_GPIDLR(0)
#define GPIO_GPIDLR1    GPIO_GPIDLR(1)
#define GPIO_GPIDLR2    GPIO_GPIDLR(2)
#define GPIO_GPIDLR3    GPIO_GPIDLR(3)
#define GPIO_GPIDUR0    GPIO_GPIDUR(0)
#define GPIO_GPIDUR1    GPIO_GPIDUR(1)
#define GPIO_GPIDUR2    GPIO_GPIDUR(2)
#define GPIO_GPIDUR3    GPIO_GPIDUR(3)
#define GPIO_GPIER0     GPIO_GPIER(0)
#define GPIO_GPIER1     GPIO_GPIER(1)
#define GPIO_GPIER2     GPIO_GPIER(2)
#define GPIO_GPIER3     GPIO_GPIER(3)
#define GPIO_GPIMR0     GPIO_GPIMR(0)
#define GPIO_GPIMR1     GPIO_GPIMR(1)
#define GPIO_GPIMR2     GPIO_GPIMR(2)
#define GPIO_GPIMR3     GPIO_GPIMR(3)
#define GPIO_GPFR0      GPIO_GPFR(0)
#define GPIO_GPFR1      GPIO_GPFR(1)
#define GPIO_GPFR2      GPIO_GPFR(2)
#define GPIO_GPFR3      GPIO_GPFR(3)


/*************************************************************************
 * HARB
 *************************************************************************/
#define	HARB_HAPOR		(HARB_BASE + 0x000)
#define	HARB_HMCTR		(HARB_BASE + 0x010)
#define	HARB_HME8H		(HARB_BASE + 0x014)
#define	HARB_HMCR1		(HARB_BASE + 0x018)
#define	HARB_HMER2		(HARB_BASE + 0x01C)
#define	HARB_HMER3		(HARB_BASE + 0x020)
#define	HARB_HMLTR		(HARB_BASE + 0x024)

#define	REG_HARB_HAPOR		REG32(HARB_HAPOR)
#define	REG_HARB_HMCTR		REG32(HARB_HMCTR)
#define	REG_HARB_HME8H		REG32(HARB_HME8H)
#define	REG_HARB_HMCR1		REG32(HARB_HMCR1)
#define	REG_HARB_HMER2		REG32(HARB_HMER2)
#define	REG_HARB_HMER3		REG32(HARB_HMER3)
#define	REG_HARB_HMLTR		REG32(HARB_HMLTR)

/* HARB Priority Order Register (HARB_HAPOR) */

#define	HARB_HAPOR_UCHSEL 		(1 << 7)
#define	HARB_HAPOR_PRIO_BIT		0
#define	HARB_HAPOR_PRIO_MASK		(0xf << HARB_HAPOR_PRIO_BIT)

/* AHB Monitor Control Register (HARB_HMCTR) */

#define	HARB_HMCTR_HET3_BIT		20
#define	HARB_HMCTR_HET3_MASK		(0xf << HARB_HMCTR_HET3_BIT)
#define	HARB_HMCTR_HMS3_BIT		16
#define	HARB_HMCTR_HMS3_MASK		(0xf << HARB_HMCTR_HMS3_BIT)
#define	HARB_HMCTR_HET2_BIT		12
#define	HARB_HMCTR_HET2_MASK		(0xf << HARB_HMCTR_HET2_BIT)
#define	HARB_HMCTR_HMS2_BIT		8
#define	HARB_HMCTR_HMS2_MASK		(0xf << HARB_HMCTR_HMS2_BIT)
#define	HARB_HMCTR_HOVF3		(1 << 7)
#define	HARB_HMCTR_HOVF2		(1 << 6)
#define	HARB_HMCTR_HOVF1		(1 << 5)
#define	HARB_HMCTR_HRST			(1 << 4)
#define	HARB_HMCTR_HEE3			(1 << 2)
#define	HARB_HMCTR_HEE2			(1 << 1)
#define	HARB_HMCTR_HEE1			(1 << 0)

/* AHB Monitor Event 8bits High Register (HARB_HME8H) */

#define HARB_HME8H_HC8H1_BIT		16
#define HARB_HME8H_HC8H1_MASK		(0xff << HARB_HME8H_HC8H1_BIT)
#define HARB_HME8H_HC8H2_BIT		8
#define HARB_HME8H_HC8H2_MASK		(0xff << HARB_HME8H_HC8H2_BIT)
#define HARB_HME8H_HC8H3_BIT		0
#define HARB_HME8H_HC8H3_MASK		(0xff << HARB_HME8H_HC8H3_BIT)

/* AHB Monitor Latency Register (HARB_HMLTR) */

#define HARB_HMLTR_HLT2_BIT		16
#define HARB_HMLTR_HLT2_MASK		(0xffff << HARB_HMLTR_HLT2_BIT)
#define HARB_HMLTR_HLT3_BIT		0
#define HARB_HMLTR_HLT3_MASK		(0xffff << HARB_HMLTR_HLT3_BIT)




/*************************************************************************
 * I2C
 *************************************************************************/
#define	I2C_DR			(I2C_BASE + 0x000)
#define	I2C_CR			(I2C_BASE + 0x004)
#define	I2C_SR			(I2C_BASE + 0x008)
#define	I2C_GR			(I2C_BASE + 0x00C)

#define	REG_I2C_DR		REG8(I2C_DR)
#define	REG_I2C_CR		REG8(I2C_CR)
#define REG_I2C_SR		REG8(I2C_SR)
#define REG_I2C_GR		REG16(I2C_GR)

/* I2C Control Register (I2C_CR) */

#define I2C_CR_IEN		(1 << 4)
#define I2C_CR_STA		(1 << 3)
#define I2C_CR_STO		(1 << 2)
#define I2C_CR_AC		(1 << 1)
#define I2C_CR_I2CE		(1 << 0)

/* I2C Status Register (I2C_SR) */

#define I2C_SR_STX		(1 << 4)
#define I2C_SR_BUSY		(1 << 3)
#define I2C_SR_TEND		(1 << 2)
#define I2C_SR_DRF		(1 << 1)
#define I2C_SR_ACKF		(1 << 0)




/*************************************************************************
 * UDC
 *************************************************************************/
#define UDC_EP0InCR	(UDC_BASE + 0x00)
#define UDC_EP0InSR	(UDC_BASE + 0x04)
#define UDC_EP0InBSR	(UDC_BASE + 0x08)
#define UDC_EP0InMPSR	(UDC_BASE + 0x0c)
#define UDC_EP0InDesR	(UDC_BASE + 0x14)
#define UDC_EP1InCR	(UDC_BASE + 0x20)
#define UDC_EP1InSR	(UDC_BASE + 0x24)
#define UDC_EP1InBSR	(UDC_BASE + 0x28)
#define UDC_EP1InMPSR	(UDC_BASE + 0x2c)
#define UDC_EP1InDesR	(UDC_BASE + 0x34)
#define UDC_EP2InCR	(UDC_BASE + 0x40)
#define UDC_EP2InSR	(UDC_BASE + 0x44)
#define UDC_EP2InBSR	(UDC_BASE + 0x48)
#define UDC_EP2InMPSR	(UDC_BASE + 0x4c)
#define UDC_EP2InDesR	(UDC_BASE + 0x54)
#define UDC_EP3InCR	(UDC_BASE + 0x60)
#define UDC_EP3InSR	(UDC_BASE + 0x64)
#define UDC_EP3InBSR	(UDC_BASE + 0x68)
#define UDC_EP3InMPSR	(UDC_BASE + 0x6c)
#define UDC_EP3InDesR	(UDC_BASE + 0x74)
#define UDC_EP4InCR	(UDC_BASE + 0x80)
#define UDC_EP4InSR	(UDC_BASE + 0x84)
#define UDC_EP4InBSR	(UDC_BASE + 0x88)
#define UDC_EP4InMPSR	(UDC_BASE + 0x8c)
#define UDC_EP4InDesR	(UDC_BASE + 0x94)

#define UDC_EP0OutCR	(UDC_BASE + 0x200)
#define UDC_EP0OutSR	(UDC_BASE + 0x204)
#define UDC_EP0OutPFNR	(UDC_BASE + 0x208)
#define UDC_EP0OutMPSR	(UDC_BASE + 0x20c)
#define UDC_EP0OutSBPR	(UDC_BASE + 0x210)
#define UDC_EP0OutDesR	(UDC_BASE + 0x214)
#define UDC_EP5OutCR	(UDC_BASE + 0x2a0)
#define UDC_EP5OutSR	(UDC_BASE + 0x2a4)
#define UDC_EP5OutPFNR	(UDC_BASE + 0x2a8)
#define UDC_EP5OutMPSR	(UDC_BASE + 0x2ac)
#define UDC_EP5OutDesR	(UDC_BASE + 0x2b4)
#define UDC_EP6OutCR	(UDC_BASE + 0x2c0)
#define UDC_EP6OutSR	(UDC_BASE + 0x2c4)
#define UDC_EP6OutPFNR	(UDC_BASE + 0x2c8)
#define UDC_EP6OutMPSR	(UDC_BASE + 0x2cc)
#define UDC_EP6OutDesR	(UDC_BASE + 0x2d4)
#define UDC_EP7OutCR	(UDC_BASE + 0x2e0)
#define UDC_EP7OutSR	(UDC_BASE + 0x2e4)
#define UDC_EP7OutPFNR	(UDC_BASE + 0x2e8)
#define UDC_EP7OutMPSR	(UDC_BASE + 0x2ec)
#define UDC_EP7OutDesR	(UDC_BASE + 0x2f4)

#define UDC_DevCFGR	(UDC_BASE + 0x400)
#define UDC_DevCR	(UDC_BASE + 0x404)
#define UDC_DevSR	(UDC_BASE + 0x408)
#define UDC_DevIntR	(UDC_BASE + 0x40c)
#define UDC_DevIntMR	(UDC_BASE + 0x410)
#define UDC_EPIntR	(UDC_BASE + 0x414)
#define UDC_EPIntMR	(UDC_BASE + 0x418)

#define UDC_STCMAR	(UDC_BASE + 0x500)
#define UDC_EP0InfR	(UDC_BASE + 0x504)
#define UDC_EP1InfR	(UDC_BASE + 0x508)
#define UDC_EP2InfR	(UDC_BASE + 0x50c)
#define UDC_EP3InfR	(UDC_BASE + 0x510)
#define UDC_EP4InfR	(UDC_BASE + 0x514)
#define UDC_EP5InfR	(UDC_BASE + 0x518)
#define UDC_EP6InfR	(UDC_BASE + 0x51c)
#define UDC_EP7InfR	(UDC_BASE + 0x520)

#define UDC_TXCONFIRM	(UDC_BASE + 0x41C)
#define UDC_TXZLP	(UDC_BASE + 0x420)
#define UDC_RXCONFIRM	(UDC_BASE + 0x41C)

#define UDC_RXFIFO	(UDC_BASE + 0x800)
#define UDC_TXFIFOEP0	(UDC_BASE + 0x840)

#define REG_UDC_EP0InCR		REG32(UDC_EP0InCR)
#define REG_UDC_EP0InSR		REG32(UDC_EP0InSR)
#define REG_UDC_EP0InBSR	REG32(UDC_EP0InBSR)
#define REG_UDC_EP0InMPSR	REG32(UDC_EP0InMPSR)
#define REG_UDC_EP0InDesR	REG32(UDC_EP0InDesR)
#define REG_UDC_EP1InCR		REG32(UDC_EP1InCR)
#define REG_UDC_EP1InSR		REG32(UDC_EP1InSR)
#define REG_UDC_EP1InBSR	REG32(UDC_EP1InBSR)
#define REG_UDC_EP1InMPSR	REG32(UDC_EP1InMPSR)
#define REG_UDC_EP1InDesR	REG32(UDC_EP1InDesR)
#define REG_UDC_EP2InCR		REG32(UDC_EP2InCR)
#define REG_UDC_EP2InSR		REG32(UDC_EP2InSR)
#define REG_UDC_EP2InBSR	REG32(UDC_EP2InBSR)
#define REG_UDC_EP2InMPSR	REG32(UDC_EP2InMPSR)
#define REG_UDC_EP2InDesR	REG32(UDC_EP2InDesR)
#define REG_UDC_EP3InCR		REG32(UDC_EP3InCR)
#define REG_UDC_EP3InSR		REG32(UDC_EP3InSR)
#define REG_UDC_EP3InBSR	REG32(UDC_EP3InBSR)
#define REG_UDC_EP3InMPSR	REG32(UDC_EP3InMPSR)
#define REG_UDC_EP3InDesR	REG32(UDC_EP3InDesR)
#define REG_UDC_EP4InCR		REG32(UDC_EP4InCR)
#define REG_UDC_EP4InSR		REG32(UDC_EP4InSR)
#define REG_UDC_EP4InBSR	REG32(UDC_EP4InBSR)
#define REG_UDC_EP4InMPSR	REG32(UDC_EP4InMPSR)
#define REG_UDC_EP4InDesR	REG32(UDC_EP4InDesR)

#define REG_UDC_EP0OutCR	REG32(UDC_EP0OutCR)
#define REG_UDC_EP0OutSR	REG32(UDC_EP0OutSR)
#define REG_UDC_EP0OutPFNR	REG32(UDC_EP0OutPFNR)
#define REG_UDC_EP0OutMPSR	REG32(UDC_EP0OutMPSR)
#define REG_UDC_EP0OutSBPR	REG32(UDC_EP0OutSBPR)
#define REG_UDC_EP0OutDesR	REG32(UDC_EP0OutDesR)
#define REG_UDC_EP5OutCR	REG32(UDC_EP5OutCR)
#define REG_UDC_EP5OutSR	REG32(UDC_EP5OutSR)
#define REG_UDC_EP5OutPFNR	REG32(UDC_EP5OutPFNR)
#define REG_UDC_EP5OutMPSR	REG32(UDC_EP5OutMPSR)
#define REG_UDC_EP5OutDesR	REG32(UDC_EP5OutDesR)
#define REG_UDC_EP6OutCR	REG32(UDC_EP6OutCR)
#define REG_UDC_EP6OutSR	REG32(UDC_EP6OutSR)
#define REG_UDC_EP6OutPFNR	REG32(UDC_EP6OutPFNR)
#define REG_UDC_EP6OutMPSR	REG32(UDC_EP6OutMPSR)
#define REG_UDC_EP6OutDesR	REG32(UDC_EP6OutDesR)
#define REG_UDC_EP7OutCR	REG32(UDC_EP7OutCR)
#define REG_UDC_EP7OutSR	REG32(UDC_EP7OutSR)
#define REG_UDC_EP7OutPFNR	REG32(UDC_EP7OutPFNR)
#define REG_UDC_EP7OutMPSR	REG32(UDC_EP7OutMPSR)
#define REG_UDC_EP7OutDesR	REG32(UDC_EP7OutDesR)

#define REG_UDC_DevCFGR		REG32(UDC_DevCFGR)
#define REG_UDC_DevCR		REG32(UDC_DevCR)
#define REG_UDC_DevSR		REG32(UDC_DevSR)
#define REG_UDC_DevIntR		REG32(UDC_DevIntR)
#define REG_UDC_DevIntMR	REG32(UDC_DevIntMR)
#define REG_UDC_EPIntR		REG32(UDC_EPIntR)
#define REG_UDC_EPIntMR		REG32(UDC_EPIntMR)

#define REG_UDC_STCMAR		REG32(UDC_STCMAR)
#define REG_UDC_EP0InfR		REG32(UDC_EP0InfR)
#define REG_UDC_EP1InfR		REG32(UDC_EP1InfR)
#define REG_UDC_EP2InfR		REG32(UDC_EP2InfR)
#define REG_UDC_EP3InfR		REG32(UDC_EP3InfR)
#define REG_UDC_EP4InfR		REG32(UDC_EP4InfR)
#define REG_UDC_EP5InfR		REG32(UDC_EP5InfR)
#define REG_UDC_EP6InfR		REG32(UDC_EP6InfR)
#define REG_UDC_EP7InfR		REG32(UDC_EP7InfR)

#define UDC_DevCFGR_PI		(1 << 5)
#define UDC_DevCFGR_SS		(1 << 4)
#define UDC_DevCFGR_SP		(1 << 3)
#define UDC_DevCFGR_RW		(1 << 2)
#define UDC_DevCFGR_SPD_BIT	0
#define UDC_DevCFGR_SPD_MASK	(0x03 << UDC_DevCFGR_SPD_BIT)
  #define UDC_DevCFGR_SPD_HS	(0 << UDC_DevCFGR_SPD_BIT)
  #define UDC_DevCFGR_SPD_LS	(2 << UDC_DevCFGR_SPD_BIT)
  #define UDC_DevCFGR_SPD_FS	(3 << UDC_DevCFGR_SPD_BIT)

#define UDC_DevCR_DM		(1 << 9)
#define UDC_DevCR_BE		(1 << 5)
#define UDC_DevCR_RES		(1 << 0)

#define UDC_DevSR_ENUMSPD_BIT	13
#define UDC_DevSR_ENUMSPD_MASK	(0x03 << UDC_DevSR_ENUMSPD_BIT)
  #define UDC_DevSR_ENUMSPD_HS	(0 << UDC_DevSR_ENUMSPD_BIT)
  #define UDC_DevSR_ENUMSPD_LS	(2 << UDC_DevSR_ENUMSPD_BIT)
  #define UDC_DevSR_ENUMSPD_FS	(3 << UDC_DevSR_ENUMSPD_BIT)
#define UDC_DevSR_SUSP		(1 << 12)
#define UDC_DevSR_ALT_BIT	8
#define UDC_DevSR_ALT_MASK	(0x0f << UDC_DevSR_ALT_BIT)
#define UDC_DevSR_INTF_BIT	4
#define UDC_DevSR_INTF_MASK	(0x0f << UDC_DevSR_INTF_BIT)
#define UDC_DevSR_CFG_BIT	0
#define UDC_DevSR_CFG_MASK	(0x0f << UDC_DevSR_CFG_BIT)

#define UDC_DevIntR_ENUM	(1 << 6)
#define UDC_DevIntR_SOF		(1 << 5)
#define UDC_DevIntR_US		(1 << 4)
#define UDC_DevIntR_UR		(1 << 3)
#define UDC_DevIntR_SI		(1 << 1)
#define UDC_DevIntR_SC		(1 << 0)

#define UDC_EPIntR_OUTEP_BIT	16
#define UDC_EPIntR_OUTEP_MASK	(0xffff << UDC_EPIntR_OUTEP_BIT)
#define UDC_EPIntR_OUTEP0       0x00010000
#define UDC_EPIntR_OUTEP5       0x00200000
#define UDC_EPIntR_OUTEP6       0x00400000
#define UDC_EPIntR_OUTEP7       0x00800000
#define UDC_EPIntR_INEP_BIT	0
#define UDC_EPIntR_INEP_MASK	(0xffff << UDC_EPIntR_INEP_BIT)
#define UDC_EPIntR_INEP0        0x00000001
#define UDC_EPIntR_INEP1        0x00000002
#define UDC_EPIntR_INEP2        0x00000004
#define UDC_EPIntR_INEP3        0x00000008
#define UDC_EPIntR_INEP4        0x00000010


#define UDC_EPIntMR_OUTEP_BIT	16
#define UDC_EPIntMR_OUTEP_MASK	(0xffff << UDC_EPIntMR_OUTEP_BIT)
#define UDC_EPIntMR_INEP_BIT	0
#define UDC_EPIntMR_INEP_MASK	(0xffff << UDC_EPIntMR_INEP_BIT)

#define UDC_EPCR_ET_BIT		4
#define UDC_EPCR_ET_MASK	(0x03 << UDC_EPCR_ET_BIT)
  #define UDC_EPCR_ET_CTRL	(0 << UDC_EPCR_ET_BIT)
  #define UDC_EPCR_ET_ISO	(1 << UDC_EPCR_ET_BIT)
  #define UDC_EPCR_ET_BULK	(2 << UDC_EPCR_ET_BIT)
  #define UDC_EPCR_ET_INTR	(3 << UDC_EPCR_ET_BIT)
#define UDC_EPCR_SN		(1 << 2)
#define UDC_EPCR_F		(1 << 1)
#define UDC_EPCR_S		(1 << 0)

#define UDC_EPSR_RXPKTSIZE_BIT	11
#define UDC_EPSR_RXPKTSIZE_MASK	(0x7ff << UDC_EPSR_RXPKTSIZE_BIT)
#define UDC_EPSR_IN		(1 << 6)
#define UDC_EPSR_OUT_BIT	4
#define UDC_EPSR_OUT_MASK	(0x03 << UDC_EPSR_OUT_BIT)
  #define UDC_EPSR_OUT_NONE	(0 << UDC_EPSR_OUT_BIT)
  #define UDC_EPSR_OUT_RCVDATA	(1 << UDC_EPSR_OUT_BIT)
  #define UDC_EPSR_OUT_RCVSETUP	(2 << UDC_EPSR_OUT_BIT)
#define UDC_EPSR_PID_BIT	0
#define UDC_EPSR_PID_MASK	(0x0f << UDC_EPSR_PID_BIT)

#define UDC_EPInfR_MPS_BIT	19
#define UDC_EPInfR_MPS_MASK	(0x3ff << UDC_EPInfR_MPS_BIT)
#define UDC_EPInfR_ALTS_BIT	15
#define UDC_EPInfR_ALTS_MASK	(0x0f << UDC_EPInfR_ALTS_BIT)
#define UDC_EPInfR_IFN_BIT	11
#define UDC_EPInfR_IFN_MASK	(0x0f << UDC_EPInfR_IFN_BIT)
#define UDC_EPInfR_CGN_BIT	7
#define UDC_EPInfR_CGN_MASK	(0x0f << UDC_EPInfR_CGN_BIT)
#define UDC_EPInfR_EPT_BIT	5
#define UDC_EPInfR_EPT_MASK	(0x03 << UDC_EPInfR_EPT_BIT)
  #define UDC_EPInfR_EPT_CTRL	(0 << UDC_EPInfR_EPT_BIT)
  #define UDC_EPInfR_EPT_ISO	(1 << UDC_EPInfR_EPT_BIT)
  #define UDC_EPInfR_EPT_BULK	(2 << UDC_EPInfR_EPT_BIT)
  #define UDC_EPInfR_EPT_INTR	(3 << UDC_EPInfR_EPT_BIT)
#define UDC_EPInfR_EPD		(1 << 4)
  #define UDC_EPInfR_EPD_OUT	(0 << 4)
  #define UDC_EPInfR_EPD_IN	(1 << 4)

#define UDC_EPInfR_EPN_BIT	0
#define UDC_EPInfR_EPN_MASK	(0xf << UDC_EPInfR_EPN_BIT)




/*************************************************************************
 * DMAC 
 *************************************************************************/
#define DMAC_DSAR(n)	(DMAC_BASE + (0x00 + (n) * 0x20))
#define DMAC_DDAR(n)	(DMAC_BASE + (0x04 + (n) * 0x20))
#define DMAC_DTCR(n)	(DMAC_BASE + (0x08 + (n) * 0x20))
#define DMAC_DRSR(n)	(DMAC_BASE + (0x0c + (n) * 0x20))
#define DMAC_DCCSR(n)	(DMAC_BASE + (0x10 + (n) * 0x20))
#define DMAC_DMAIPR	(DMAC_BASE + 0xf8)
#define DMAC_DMACR	(DMAC_BASE + 0xfc)

#define REG_DMAC_DSAR(n)	REG32(DMAC_DSAR((n)))
#define REG_DMAC_DDAR(n)	REG32(DMAC_DDAR((n)))
#define REG_DMAC_DTCR(n)	REG32(DMAC_DTCR((n)))
#define REG_DMAC_DRSR(n)	REG32(DMAC_DRSR((n)))
#define REG_DMAC_DCCSR(n)	REG32(DMAC_DCCSR((n)))
#define REG_DMAC_DMAIPR		REG32(DMAC_DMAIPR)
#define REG_DMAC_DMACR		REG32(DMAC_DMACR)

#define DMAC_DRSR_RS_BIT	0
#define DMAC_DRSR_RS_MASK	(0x1f << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_EXTREXTR		(0 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_PCMCIAOUT	(4 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_PCMCIAIN		(5 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_AUTO		(8 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_DESOUT		(10 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_DESIN		(11 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_UART3OUT		(14 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_UART3IN		(15 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_UART2OUT		(16 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_UART2IN		(17 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_UART1OUT		(18 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_UART1IN		(19 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_UART0OUT		(20 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_UART0IN		(21 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_SSIOUT		(22 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_SSIIN		(23 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_AICOUT		(24 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_AICIN		(25 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_MSCOUT		(26 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_MSCIN		(27 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_OST2		(28 << DMAC_DRSR_RS_BIT)

#define DMAC_DCCSR_EACKS	(1 << 31)
#define DMAC_DCCSR_EACKM	(1 << 30)
#define DMAC_DCCSR_ERDM_BIT	28
#define DMAC_DCCSR_ERDM_MASK	(0x03 << DMAC_DCCSR_ERDM_BIT)
  #define DMAC_DCCSR_ERDM_LLEVEL	(0 << DMAC_DCCSR_ERDM_BIT)
  #define DMAC_DCCSR_ERDM_FEDGE		(1 << DMAC_DCCSR_ERDM_BIT)
  #define DMAC_DCCSR_ERDM_HLEVEL	(2 << DMAC_DCCSR_ERDM_BIT)
  #define DMAC_DCCSR_ERDM_REDGE		(3 << DMAC_DCCSR_ERDM_BIT)
#define DMAC_DCCSR_EOPM		(1 << 27)
#define DMAC_DCCSR_SAM		(1 << 23)
#define DMAC_DCCSR_DAM		(1 << 22)
#define DMAC_DCCSR_RDIL_BIT	16
#define DMAC_DCCSR_RDIL_MASK	(0x0f << DMAC_DCCSR_RDIL_BIT)
  #define DMAC_DCCSR_RDIL_IGN	(0 << DMAC_DCCSR_RDIL_BIT)
  #define DMAC_DCCSR_RDIL_2	(1 << DMAC_DCCSR_RDIL_BIT)
  #define DMAC_DCCSR_RDIL_4	(2 << DMAC_DCCSR_RDIL_BIT)
  #define DMAC_DCCSR_RDIL_8	(3 << DMAC_DCCSR_RDIL_BIT)
  #define DMAC_DCCSR_RDIL_12	(4 << DMAC_DCCSR_RDIL_BIT)
  #define DMAC_DCCSR_RDIL_16	(5 << DMAC_DCCSR_RDIL_BIT)
  #define DMAC_DCCSR_RDIL_20	(6 << DMAC_DCCSR_RDIL_BIT)
  #define DMAC_DCCSR_RDIL_24	(7 << DMAC_DCCSR_RDIL_BIT)
  #define DMAC_DCCSR_RDIL_28	(8 << DMAC_DCCSR_RDIL_BIT)
  #define DMAC_DCCSR_RDIL_32	(9 << DMAC_DCCSR_RDIL_BIT)
  #define DMAC_DCCSR_RDIL_48	(10 << DMAC_DCCSR_RDIL_BIT)
  #define DMAC_DCCSR_RDIL_60	(11 << DMAC_DCCSR_RDIL_BIT)
  #define DMAC_DCCSR_RDIL_64	(12 << DMAC_DCCSR_RDIL_BIT)
  #define DMAC_DCCSR_RDIL_124	(13 << DMAC_DCCSR_RDIL_BIT)
  #define DMAC_DCCSR_RDIL_128	(14 << DMAC_DCCSR_RDIL_BIT)
  #define DMAC_DCCSR_RDIL_200	(15 << DMAC_DCCSR_RDIL_BIT)
#define DMAC_DCCSR_SWDH_BIT	14
#define DMAC_DCCSR_SWDH_MASK	(0x03 << DMAC_DCCSR_SWDH_BIT)
  #define DMAC_DCCSR_SWDH_32	(0 << DMAC_DCCSR_SWDH_BIT)
  #define DMAC_DCCSR_SWDH_8	(1 << DMAC_DCCSR_SWDH_BIT)
  #define DMAC_DCCSR_SWDH_16	(2 << DMAC_DCCSR_SWDH_BIT)
#define DMAC_DCCSR_DWDH_BIT	12
#define DMAC_DCCSR_DWDH_MASK	(0x03 << DMAC_DCCSR_DWDH_BIT)
  #define DMAC_DCCSR_DWDH_32	(0 << DMAC_DCCSR_DWDH_BIT)
  #define DMAC_DCCSR_DWDH_8	(1 << DMAC_DCCSR_DWDH_BIT)
  #define DMAC_DCCSR_DWDH_16	(2 << DMAC_DCCSR_DWDH_BIT)
#define DMAC_DCCSR_DS_BIT	8
#define DMAC_DCCSR_DS_MASK	(0x07 << DMAC_DCCSR_DS_BIT)
  #define DMAC_DCCSR_DS_32b	(0 << DMAC_DCCSR_DS_BIT)
  #define DMAC_DCCSR_DS_8b	(1 << DMAC_DCCSR_DS_BIT)
  #define DMAC_DCCSR_DS_16b	(2 << DMAC_DCCSR_DS_BIT)
  #define DMAC_DCCSR_DS_16B	(3 << DMAC_DCCSR_DS_BIT)
  #define DMAC_DCCSR_DS_32B	(4 << DMAC_DCCSR_DS_BIT)
#define DMAC_DCCSR_TM		(1 << 7)
#define DMAC_DCCSR_AR		(1 << 4)
#define DMAC_DCCSR_TC		(1 << 3)
#define DMAC_DCCSR_HLT		(1 << 2)
#define DMAC_DCCSR_TCIE		(1 << 1)
#define DMAC_DCCSR_CHDE		(1 << 0)

#define DMAC_DMAIPR_CINT_BIT	8
#define DMAC_DMAIPR_CINT_MASK	(0xff << DMAC_DMAIPR_CINT_BIT)

#define DMAC_DMACR_PR_BIT	8
#define DMAC_DMACR_PR_MASK	(0x03 << DMAC_DMACR_PR_BIT)
  #define DMAC_DMACR_PR_01234567	(0 << DMAC_DMACR_PR_BIT)
  #define DMAC_DMACR_PR_02314675	(1 << DMAC_DMACR_PR_BIT)
  #define DMAC_DMACR_PR_20136457	(2 << DMAC_DMACR_PR_BIT)
  #define DMAC_DMACR_PR_ROUNDROBIN	(3 << DMAC_DMACR_PR_BIT)
#define DMAC_DMACR_HTR		(1 << 3)
#define DMAC_DMACR_AER		(1 << 2)
#define DMAC_DMACR_DME		(1 << 0)

#define IRQ_DMA_0	32
#define NUM_DMA		6


/*************************************************************************
 * AIC 
 *************************************************************************/
#define	AIC_FR			(AIC_BASE + 0x000)
#define	AIC_CR			(AIC_BASE + 0x004)
#define	AIC_ACCR1		(AIC_BASE + 0x008)
#define	AIC_ACCR2		(AIC_BASE + 0x00C)
#define	AIC_I2SCR		(AIC_BASE + 0x010)
#define	AIC_SR			(AIC_BASE + 0x014)
#define	AIC_ACSR		(AIC_BASE + 0x018)
#define	AIC_I2SSR		(AIC_BASE + 0x01C)
#define	AIC_ACCAR		(AIC_BASE + 0x020)
#define	AIC_ACCDR		(AIC_BASE + 0x024)
#define	AIC_ACSAR		(AIC_BASE + 0x028)
#define	AIC_ACSDR		(AIC_BASE + 0x02C)
#define	AIC_I2SDIV		(AIC_BASE + 0x030)
#define	AIC_DR			(AIC_BASE + 0x034)

#define	REG_AIC_FR		REG32(AIC_FR)
#define	REG_AIC_CR		REG32(AIC_CR)
#define	REG_AIC_ACCR1		REG32(AIC_ACCR1)
#define	REG_AIC_ACCR2		REG32(AIC_ACCR2)
#define	REG_AIC_I2SCR		REG32(AIC_I2SCR)
#define	REG_AIC_SR		REG32(AIC_SR)
#define	REG_AIC_ACSR		REG32(AIC_ACSR)
#define	REG_AIC_I2SSR		REG32(AIC_I2SSR)
#define	REG_AIC_ACCAR		REG32(AIC_ACCAR)
#define	REG_AIC_ACCDR		REG32(AIC_ACCDR)
#define	REG_AIC_ACSAR		REG32(AIC_ACSAR)
#define	REG_AIC_ACSDR		REG32(AIC_ACSDR)
#define	REG_AIC_I2SDIV		REG32(AIC_I2SDIV)
#define	REG_AIC_DR		REG32(AIC_DR)

/* AIC Controller Configuration Register (AIC_FR) */

#define	AIC_FR_RFTH_BIT		12
#define	AIC_FR_RFTH_MASK	(0xf << AIC_FR_RFTH_BIT)
#define	AIC_FR_TFTH_BIT		8
#define	AIC_FR_TFTH_MASK	(0xf << AIC_FR_TFTH_BIT)
#define	AIC_FR_AUSEL		(1 << 4)
#define	AIC_FR_RST		(1 << 3)
#define	AIC_FR_BCKD		(1 << 2)
#define	AIC_FR_SYNCD		(1 << 1)
#define	AIC_FR_ENB		(1 << 0)

/* AIC Controller Common Control Register (AIC_CR) */

#define	AIC_CR_RDMS		(1 << 15)
#define	AIC_CR_TDMS		(1 << 14)
#define	AIC_CR_FLUSH		(1 << 8)
#define	AIC_CR_EROR		(1 << 6)
#define	AIC_CR_ETUR		(1 << 5)
#define	AIC_CR_ERFS		(1 << 4)
#define	AIC_CR_ETFS		(1 << 3)
#define	AIC_CR_ENLBF		(1 << 2)
#define	AIC_CR_ERPL		(1 << 1)
#define	AIC_CR_EREC		(1 << 0)

/* AIC Controller AC-link Control Register 1 (AIC_ACCR1) */

#define	AIC_ACCR1_RS_BIT	16
#define	AIC_ACCR1_RS_MASK	(0x3ff << AIC_ACCR1_RS_BIT)
  #define AIC_ACCR1_RS_SLOT12	  (1 << 25) /* Slot 12 valid bit */
  #define AIC_ACCR1_RS_SLOT11	  (1 << 24) /* Slot 11 valid bit */
  #define AIC_ACCR1_RS_SLOT10	  (1 << 23) /* Slot 10 valid bit */
  #define AIC_ACCR1_RS_SLOT9	  (1 << 22) /* Slot 9 valid bit */
  #define AIC_ACCR1_RS_SLOT8	  (1 << 21) /* Slot 8 valid bit */
  #define AIC_ACCR1_RS_SLOT7	  (1 << 20) /* Slot 7 valid bit */
  #define AIC_ACCR1_RS_SLOT6	  (1 << 19) /* Slot 6 valid bit */
  #define AIC_ACCR1_RS_SLOT5	  (1 << 18) /* Slot 5 valid bit */
  #define AIC_ACCR1_RS_SLOT4	  (1 << 17) /* Slot 4 valid bit */
  #define AIC_ACCR1_RS_SLOT3	  (1 << 16) /* Slot 3 valid bit */
#define	AIC_ACCR1_XS_BIT	0
#define	AIC_ACCR1_XS_MASK	(0x3ff << AIC_ACCR1_XS_BIT)
  #define AIC_ACCR1_XS_SLOT12	  (1 << 9) /* Slot 12 valid bit */
  #define AIC_ACCR1_XS_SLOT11	  (1 << 8) /* Slot 11 valid bit */
  #define AIC_ACCR1_XS_SLOT10	  (1 << 7) /* Slot 10 valid bit */
  #define AIC_ACCR1_XS_SLOT9	  (1 << 6) /* Slot 9 valid bit */
  #define AIC_ACCR1_XS_SLOT8	  (1 << 5) /* Slot 8 valid bit */
  #define AIC_ACCR1_XS_SLOT7	  (1 << 4) /* Slot 7 valid bit */
  #define AIC_ACCR1_XS_SLOT6	  (1 << 3) /* Slot 6 valid bit */
  #define AIC_ACCR1_XS_SLOT5	  (1 << 2) /* Slot 5 valid bit */
  #define AIC_ACCR1_XS_SLOT4	  (1 << 1) /* Slot 4 valid bit */
  #define AIC_ACCR1_XS_SLOT3	  (1 << 0) /* Slot 3 valid bit */

/* AIC Controller AC-link Control Register 2 (AIC_ACCR2) */

#define	AIC_ACCR2_ERSTO		(1 << 18)
#define	AIC_ACCR2_ESADR		(1 << 17)
#define	AIC_ACCR2_ECADT		(1 << 16)
#define	AIC_ACCR2_OASS_BIT	8
#define	AIC_ACCR2_OASS_MASK	(0x3 << AIC_ACCR2_OASS_BIT)
  #define AIC_ACCR2_OASS_20BIT	  (0 << AIC_ACCR2_OASS_BIT) /* Output Audio Sample Size is 20-bit */
  #define AIC_ACCR2_OASS_18BIT	  (1 << AIC_ACCR2_OASS_BIT) /* Output Audio Sample Size is 18-bit */
  #define AIC_ACCR2_OASS_16BIT	  (2 << AIC_ACCR2_OASS_BIT) /* Output Audio Sample Size is 16-bit */
  #define AIC_ACCR2_OASS_8BIT	  (3 << AIC_ACCR2_OASS_BIT) /* Output Audio Sample Size is 8-bit */
#define	AIC_ACCR2_IASS_BIT	6
#define	AIC_ACCR2_IASS_MASK	(0x3 << AIC_ACCR2_IASS_BIT)
  #define AIC_ACCR2_IASS_20BIT	  (0 << AIC_ACCR2_IASS_BIT) /* Input Audio Sample Size is 20-bit */
  #define AIC_ACCR2_IASS_18BIT	  (1 << AIC_ACCR2_IASS_BIT) /* Input Audio Sample Size is 18-bit */
  #define AIC_ACCR2_IASS_16BIT	  (2 << AIC_ACCR2_IASS_BIT) /* Input Audio Sample Size is 16-bit */
  #define AIC_ACCR2_IASS_8BIT	  (3 << AIC_ACCR2_IASS_BIT) /* Input Audio Sample Size is 8-bit */
#define	AIC_ACCR2_SO		(1 << 3)
#define	AIC_ACCR2_SR		(1 << 2)
#define	AIC_ACCR2_SS		(1 << 1)
#define	AIC_ACCR2_SA		(1 << 0)

/* AIC Controller I2S/MSB-justified Control Register (AIC_I2SCR) */

#define	AIC_I2SCR_STPBK		(1 << 12)
#define	AIC_I2SCR_WL_BIT	1
#define	AIC_I2SCR_WL_MASK	(0x7 << AIC_I2SCR_WL_BIT)
  #define AIC_I2SCR_WL_24BIT	  (0 << AIC_I2SCR_WL_BIT) /* Word Length is 24 bit */
  #define AIC_I2SCR_WL_20BIT	  (1 << AIC_I2SCR_WL_BIT) /* Word Length is 20 bit */
  #define AIC_I2SCR_WL_18BIT	  (2 << AIC_I2SCR_WL_BIT) /* Word Length is 18 bit */
  #define AIC_I2SCR_WL_16BIT	  (3 << AIC_I2SCR_WL_BIT) /* Word Length is 16 bit */
  #define AIC_I2SCR_WL_8BIT	  (4 << AIC_I2SCR_WL_BIT) /* Word Length is 8 bit */
#define	AIC_I2SCR_AMSL		(1 << 0)

/* AIC Controller FIFO Status Register (AIC_SR) */

#define	AIC_SR_RFL_BIT		24
#define	AIC_SR_RFL_MASK		(0x1f << AIC_SR_RFL_BIT)
#define	AIC_SR_TFL_BIT		8
#define	AIC_SR_TFL_MASK		(0x1f << AIC_SR_TFL_BIT)
#define	AIC_SR_ROR		(1 << 6)
#define	AIC_SR_TUR		(1 << 5)
#define	AIC_SR_RFS		(1 << 4)
#define	AIC_SR_TFS		(1 << 3)

/* AIC Controller AC-link Status Register (AIC_ACSR) */

#define	AIC_ACSR_CRDY		(1 << 20)
#define	AIC_ACSR_CLPM		(1 << 19)
#define	AIC_ACSR_RSTO		(1 << 18)
#define	AIC_ACSR_SADR		(1 << 17)
#define	AIC_ACSR_CADT		(1 << 16)

/* AIC Controller I2S/MSB-justified Status Register (AIC_I2SSR) */

#define	AIC_I2SSR_BSY		(1 << 2)

/* AIC Controller AC97 codec Command Address Register (AIC_ACCAR) */

#define	AIC_ACCAR_CAR_BIT	0
#define	AIC_ACCAR_CAR_MASK	(0xfffff << AIC_ACCAR_CAR_BIT)

/* AIC Controller AC97 codec Command Data Register (AIC_ACCDR) */

#define	AIC_ACCDR_CDR_BIT	0
#define	AIC_ACCDR_CDR_MASK	(0xfffff << AIC_ACCDR_CDR_BIT)

/* AIC Controller AC97 codec Status Address Register (AIC_ACSAR) */

#define	AIC_ACSAR_SAR_BIT	0
#define	AIC_ACSAR_SAR_MASK	(0xfffff << AIC_ACSAR_SAR_BIT)

/* AIC Controller AC97 codec Status Data Register (AIC_ACSDR) */

#define	AIC_ACSDR_SDR_BIT	0
#define	AIC_ACSDR_SDR_MASK	(0xfffff << AIC_ACSDR_SDR_BIT)

/* AIC Controller I2S/MSB-justified Clock Divider Register (AIC_I2SDIV) */

#define	AIC_I2SDIV_DIV_BIT	0
#define	AIC_I2SDIV_DIV_MASK	(0x7f << AIC_I2SDIV_DIV_BIT)
  #define AIC_I2SDIV_BITCLK_3072KHZ	(0x0C << AIC_I2SDIV_DIV_BIT) /* BIT_CLK of 3.072MHz */
  #define AIC_I2SDIV_BITCLK_2836KHZ	(0x0D << AIC_I2SDIV_DIV_BIT) /* BIT_CLK of 2.836MHz */
  #define AIC_I2SDIV_BITCLK_1418KHZ	(0x1A << AIC_I2SDIV_DIV_BIT) /* BIT_CLK of 1.418MHz */
  #define AIC_I2SDIV_BITCLK_1024KHZ	(0x24 << AIC_I2SDIV_DIV_BIT) /* BIT_CLK of 1.024MHz */
  #define AIC_I2SDIV_BITCLK_7089KHZ	(0x34 << AIC_I2SDIV_DIV_BIT) /* BIT_CLK of 708.92KHz */
  #define AIC_I2SDIV_BITCLK_512KHZ	(0x48 << AIC_I2SDIV_DIV_BIT) /* BIT_CLK of 512.00KHz */




/*************************************************************************
 * LCD 
 *************************************************************************/
#define LCD_CFG		(LCD_BASE + 0x00)
#define LCD_VSYNC	(LCD_BASE + 0x04)
#define LCD_HSYNC	(LCD_BASE + 0x08)
#define LCD_VAT		(LCD_BASE + 0x0c)
#define LCD_DAH		(LCD_BASE + 0x10)
#define LCD_DAV		(LCD_BASE + 0x14)
#define LCD_PS		(LCD_BASE + 0x18)
#define LCD_CLS		(LCD_BASE + 0x1c)
#define LCD_SPL		(LCD_BASE + 0x20)
#define LCD_REV		(LCD_BASE + 0x24)
#define LCD_CTRL	(LCD_BASE + 0x30)
#define LCD_STATE	(LCD_BASE + 0x34)
#define LCD_IID		(LCD_BASE + 0x38)
#define LCD_DA0		(LCD_BASE + 0x40)
#define LCD_SA0		(LCD_BASE + 0x44)
#define LCD_FID0	(LCD_BASE + 0x48)
#define LCD_CMD0	(LCD_BASE + 0x4c)
#define LCD_DA1		(LCD_BASE + 0x50)
#define LCD_SA1		(LCD_BASE + 0x54)
#define LCD_FID1	(LCD_BASE + 0x58)
#define LCD_CMD1	(LCD_BASE + 0x5c)

#define REG_LCD_CFG	REG32(LCD_CFG)
#define REG_LCD_VSYNC	REG32(LCD_VSYNC)
#define REG_LCD_HSYNC	REG32(LCD_HSYNC)
#define REG_LCD_VAT	REG32(LCD_VAT)
#define REG_LCD_DAH	REG32(LCD_DAH)
#define REG_LCD_DAV	REG32(LCD_DAV)
#define REG_LCD_PS	REG32(LCD_PS)
#define REG_LCD_CLS	REG32(LCD_CLS)
#define REG_LCD_SPL	REG32(LCD_SPL)
#define REG_LCD_REV	REG32(LCD_REV)
#define REG_LCD_CTRL	REG32(LCD_CTRL)
#define REG_LCD_STATE	REG32(LCD_STATE)
#define REG_LCD_IID	REG32(LCD_IID)
#define REG_LCD_DA0	REG32(LCD_DA0)
#define REG_LCD_SA0	REG32(LCD_SA0)
#define REG_LCD_FID0	REG32(LCD_FID0)
#define REG_LCD_CMD0	REG32(LCD_CMD0)
#define REG_LCD_DA1	REG32(LCD_DA1)
#define REG_LCD_SA1	REG32(LCD_SA1)
#define REG_LCD_FID1	REG32(LCD_FID1)
#define REG_LCD_CMD1	REG32(LCD_CMD1)

#define LCD_CFG_PDW_BIT		4
#define LCD_CFG_PDW_MASK	(0x03 << LCD_DEV_PDW_BIT)
  #define LCD_CFG_PDW_1		(0 << LCD_DEV_PDW_BIT)
  #define LCD_CFG_PDW_2		(1 << LCD_DEV_PDW_BIT)
  #define LCD_CFG_PDW_4		(2 << LCD_DEV_PDW_BIT)
  #define LCD_CFG_PDW_8		(3 << LCD_DEV_PDW_BIT)
#define LCD_CFG_MODE_BIT	0
#define LCD_CFG_MODE_MASK	(0x0f << LCD_DEV_MODE_BIT)
  #define LCD_CFG_MODE_GENERIC_TFT	(0 << LCD_DEV_MODE_BIT)
  #define LCD_CFG_MODE_SHARP_HR		(1 << LCD_DEV_MODE_BIT)
  #define LCD_CFG_MODE_CASIO_TFT	(2 << LCD_DEV_MODE_BIT)
  #define LCD_CFG_MODE_SAMSUNG_ALPHA	(3 << LCD_DEV_MODE_BIT)
  #define LCD_CFG_MODE_NONINTER_CCIR656	(4 << LCD_DEV_MODE_BIT)
  #define LCD_CFG_MODE_INTER_CCIR656	(5 << LCD_DEV_MODE_BIT)
  #define LCD_CFG_MODE_SINGLE_CSTN	(8 << LCD_DEV_MODE_BIT)
  #define LCD_CFG_MODE_SINGLE_MSTN	(9 << LCD_DEV_MODE_BIT)
  #define LCD_CFG_MODE_DUAL_CSTN	(10 << LCD_DEV_MODE_BIT)
  #define LCD_CFG_MODE_DUAL_MSTN	(11 << LCD_DEV_MODE_BIT)

#define LCD_VSYNC_VPS_BIT	16
#define LCD_VSYNC_VPS_MASK	(0xffff << LCD_VSYNC_VPS_BIT)
#define LCD_VSYNC_VPE_BIT	0
#define LCD_VSYNC_VPE_MASK	(0xffff << LCD_VSYNC_VPS_BIT)

#define LCD_HSYNC_HPS_BIT	16
#define LCD_HSYNC_HPS_MASK	(0xffff << LCD_HSYNC_HPS_BIT)
#define LCD_HSYNC_HPE_BIT	0
#define LCD_HSYNC_HPE_MASK	(0xffff << LCD_HSYNC_HPE_BIT)

#define LCD_VAT_HT_BIT		16
#define LCD_VAT_HT_MASK		(0xffff << LCD_VAT_HT_BIT)
#define LCD_VAT_VT_BIT		0
#define LCD_VAT_VT_MASK		(0xffff << LCD_VAT_VT_BIT)

#define LCD_DAH_HDS_BIT		16
#define LCD_DAH_HDS_MASK	(0xffff << LCD_DAH_HDS_BIT)
#define LCD_DAH_HDE_BIT		0
#define LCD_DAH_HDE_MASK	(0xffff << LCD_DAH_HDE_BIT)

#define LCD_DAV_VDS_BIT		16
#define LCD_DAV_VDS_MASK	(0xffff << LCD_DAV_VDS_BIT)
#define LCD_DAV_VDE_BIT		0
#define LCD_DAV_VDE_MASK	(0xffff << LCD_DAV_VDE_BIT)

#define LCD_CTRL_BST_BIT	28
#define LCD_CTRL_BST_MASK	(0x03 << LCD_CTRL_BST_BIT)
  #define LCD_CTRL_BST_4	(0 << LCD_CTRL_BST_BIT)
  #define LCD_CTRL_BST_8	(1 << LCD_CTRL_BST_BIT)
  #define LCD_CTRL_BST_16	(2 << LCD_CTRL_BST_BIT)
#define LCD_CTRL_RGB555		(1 << 27)
#define LCD_CTRL_OFUP		(1 << 26)
#define LCD_CTRL_FRC_BIT	24
#define LCD_CTRL_FRC_MASK	(0x03 << LCD_CTRL_FRC_BIT)
  #define LCD_CTRL_FRC_16	(0 << LCD_CTRL_FRC_BIT)
  #define LCD_CTRL_FRC_4	(1 << LCD_CTRL_FRC_BIT)
  #define LCD_CTRL_FRC_2	(2 << LCD_CTRL_FRC_BIT)
#define LCD_CTRL_PDD_BIT	16
#define LCD_CTRL_PDD_MASK	(0xff << LCD_CTRL_PDD_BIT)
#define LCD_CTRL_EOFM		(1 << 13)
#define LCD_CTRL_SOFM		(1 << 12)
#define LCD_CTRL_OFUM		(1 << 11)
#define LCD_CTRL_IFUM0		(1 << 10)
#define LCD_CTRL_IFUM1		(1 << 9)
#define LCD_CTRL_LDDM		(1 << 8)
#define LCD_CTRL_QDM		(1 << 7)
#define LCD_CTRL_BEDN		(1 << 6)
#define LCD_CTRL_PEDN		(1 << 5)
#define LCD_CTRL_DIS		(1 << 4)
#define LCD_CTRL_ENA		(1 << 3)
#define LCD_CTRL_BPP_BIT	0
#define LCD_CTRL_BPP_MASK	(0x07 << LCD_CTRL_BPP_BIT)
  #define LCD_CTRL_BPP_1	(0 << LCD_CTRL_BPP_BIT)
  #define LCD_CTRL_BPP_2	(1 << LCD_CTRL_BPP_BIT)
  #define LCD_CTRL_BPP_4	(2 << LCD_CTRL_BPP_BIT)
  #define LCD_CTRL_BPP_8	(3 << LCD_CTRL_BPP_BIT)
  #define LCD_CTRL_BPP_16	(4 << LCD_CTRL_BPP_BIT)

#define LCD_STATE_QD		(1 << 7)
#define LCD_STATE_EOF		(1 << 5)
#define LCD_STATE_SOF		(1 << 4)
#define LCD_STATE_OFU		(1 << 3)
#define LCD_STATE_IFU0		(1 << 2)
#define LCD_STATE_IFU1		(1 << 1)
#define LCD_STATE_LDD		(1 << 0)

#define LCD_CMD_SOFINT		(1 << 31)
#define LCD_CMD_EOFINT		(1 << 30)
#define LCD_CMD_PAL		(1 << 28)
#define LCD_CMD_LEN_BIT		0
#define LCD_CMD_LEN_MASK	(0xffffff << LCD_CMD_LEN_BIT)




/*************************************************************************
 * DES
 *************************************************************************/
#define	DES_CR1			(DES_BASE + 0x000)
#define	DES_CR2			(DES_BASE + 0x004)
#define	DES_SR			(DES_BASE + 0x008)
#define	DES_K1L			(DES_BASE + 0x010)
#define	DES_K1R			(DES_BASE + 0x014)
#define	DES_K2L			(DES_BASE + 0x018)
#define	DES_K2R			(DES_BASE + 0x01C)
#define	DES_K3L			(DES_BASE + 0x020)
#define	DES_K3R			(DES_BASE + 0x024)
#define	DES_IVL			(DES_BASE + 0x028)
#define	DES_IVR			(DES_BASE + 0x02C)
#define	DES_DIN			(DES_BASE + 0x030)
#define	DES_DOUT		(DES_BASE + 0x034)

#define REG_DES_CR1		REG32(DES_CR1)
#define REG_DES_CR2		REG32(DES_CR2)
#define REG_DES_SR		REG32(DES_SR)
#define REG_DES_K1L		REG32(DES_K1L)
#define REG_DES_K1R		REG32(DES_K1R)
#define REG_DES_K2L		REG32(DES_K2L)
#define REG_DES_K2R		REG32(DES_K2R)
#define REG_DES_K3L		REG32(DES_K3L)
#define REG_DES_K3R		REG32(DES_K3R)
#define REG_DES_IVL		REG32(DES_IVL)
#define REG_DES_IVR		REG32(DES_IVR)
#define REG_DES_DIN		REG32(DES_DIN)
#define REG_DES_DOUT		REG32(DES_DOUT)

/* DES Control Register 1 (DES_CR1) */

#define	DES_CR1_EN 		(1 << 0)

/* DES Control Register 2 (DES_CR2) */

#define	DES_CR2_ENDEC 		(1 << 3)
#define	DES_CR2_MODE 		(1 << 2)
#define	DES_CR2_ALG 		(1 << 1)
#define	DES_CR2_DMAE		(1 << 0)

/* DES State Register (DES_SR) */

#define DES_SR_IN_FULL		(1 << 5)
#define DES_SR_IN_LHF		(1 << 4)
#define DES_SR_IN_EMPTY		(1 << 3)
#define DES_SR_OUT_FULL		(1 << 2)
#define DES_SR_OUT_GHF		(1 << 1)
#define DES_SR_OUT_EMPTY	(1 << 0)




/*************************************************************************
 * CPM
 *************************************************************************/
#define CPM_CFCR	(CPM_BASE+0x00)
#define CPM_PLCR1	(CPM_BASE+0x10)
#define CPM_OCR		(CPM_BASE+0x1c)
#define CPM_CFCR2	(CPM_BASE+0x60)
#define CPM_LPCR	(CPM_BASE+0x04)
#define CPM_RSTR	(CPM_BASE+0x08)
#define CPM_MSCR	(CPM_BASE+0x20)
#define CPM_SCR		(CPM_BASE+0x24)
#define CPM_WRER	(CPM_BASE+0x28)
#define CPM_WFER	(CPM_BASE+0x2c)
#define CPM_WER		(CPM_BASE+0x30)
#define CPM_WSR		(CPM_BASE+0x34)
#define CPM_GSR0	(CPM_BASE+0x38)
#define CPM_GSR1	(CPM_BASE+0x3c)
#define CPM_GSR2	(CPM_BASE+0x40)
#define CPM_SPR		(CPM_BASE+0x44)
#define CPM_GSR3	(CPM_BASE+0x48)

#define REG_CPM_CFCR	REG32(CPM_CFCR)
#define REG_CPM_PLCR1	REG32(CPM_PLCR1)
#define REG_CPM_OCR	REG32(CPM_OCR)
#define REG_CPM_CFCR2	REG32(CPM_CFCR2)
#define REG_CPM_LPCR	REG32(CPM_LPCR)
#define REG_CPM_RSTR	REG32(CPM_RSTR)
#define REG_CPM_MSCR	REG32(CPM_MSCR)
#define REG_CPM_SCR	REG32(CPM_SCR)
#define REG_CPM_WRER	REG32(CPM_WRER)
#define REG_CPM_WFER	REG32(CPM_WFER)
#define REG_CPM_WER	REG32(CPM_WER)
#define REG_CPM_WSR	REG32(CPM_WSR)
#define REG_CPM_GSR0	REG32(CPM_GSR0)
#define REG_CPM_GSR1	REG32(CPM_GSR1)
#define REG_CPM_GSR2	REG32(CPM_GSR2)
#define REG_CPM_SPR	REG32(CPM_SPR)
#define REG_CPM_GSR3	REG32(CPM_GSR3)

#define CPM_CFCR_SSI		(1 << 31)
#define CPM_CFCR_LCD		(1 << 30)
#define CPM_CFCR_I2S		(1 << 29)
#define CPM_CFCR_UCS		(1 << 28)
#define CPM_CFCR_UFR_BIT	25
#define CPM_CFCR_UFR_MASK	(0x07 << CPM_CFCR_UFR_BIT)
#define CPM_CFCR_MSC		(1 << 24)
#define CPM_CFCR_CKOEN2		(1 << 23)
#define CPM_CFCR_CKOEN1		(1 << 22)
#define CPM_CFCR_UPE		(1 << 20)
#define CPM_CFCR_MFR_BIT	16
#define CPM_CFCR_MFR_MASK	(0x0f << CPM_CFCR_MFR_BIT)
  #define CFCR_MDIV_1		(0 << CPM_CFCR_MFR_BIT)
  #define CFCR_MDIV_2		(1 << CPM_CFCR_MFR_BIT)
  #define CFCR_MDIV_3		(2 << CPM_CFCR_MFR_BIT)
  #define CFCR_MDIV_4		(3 << CPM_CFCR_MFR_BIT)
  #define CFCR_MDIV_6		(4 << CPM_CFCR_MFR_BIT)
  #define CFCR_MDIV_8		(5 << CPM_CFCR_MFR_BIT)
  #define CFCR_MDIV_12		(6 << CPM_CFCR_MFR_BIT)
  #define CFCR_MDIV_16		(7 << CPM_CFCR_MFR_BIT)
  #define CFCR_MDIV_24		(8 << CPM_CFCR_MFR_BIT)
  #define CFCR_MDIV_32		(9 << CPM_CFCR_MFR_BIT)
#define CPM_CFCR_LFR_BIT	12
#define CPM_CFCR_LFR_MASK	(0x0f << CPM_CFCR_LFR_BIT)
#define CPM_CFCR_PFR_BIT	8
#define CPM_CFCR_PFR_MASK	(0x0f << CPM_CFCR_PFR_BIT)
  #define CFCR_PDIV_1		(0 << CPM_CFCR_PFR_BIT)
  #define CFCR_PDIV_2		(1 << CPM_CFCR_PFR_BIT)
  #define CFCR_PDIV_3		(2 << CPM_CFCR_PFR_BIT)
  #define CFCR_PDIV_4		(3 << CPM_CFCR_PFR_BIT)
  #define CFCR_PDIV_6		(4 << CPM_CFCR_PFR_BIT)
  #define CFCR_PDIV_8		(5 << CPM_CFCR_PFR_BIT)
  #define CFCR_PDIV_12		(6 << CPM_CFCR_PFR_BIT)
  #define CFCR_PDIV_16		(7 << CPM_CFCR_PFR_BIT)
  #define CFCR_PDIV_24		(8 << CPM_CFCR_PFR_BIT)
  #define CFCR_PDIV_32		(9 << CPM_CFCR_PFR_BIT)
#define CPM_CFCR_SFR_BIT	4
#define CPM_CFCR_SFR_MASK	(0x0f << CPM_CFCR_SFR_BIT)
  #define CFCR_SDIV_1		(0 << CPM_CFCR_SFR_BIT)
  #define CFCR_SDIV_2		(1 << CPM_CFCR_SFR_BIT)
  #define CFCR_SDIV_3		(2 << CPM_CFCR_SFR_BIT)
  #define CFCR_SDIV_4		(3 << CPM_CFCR_SFR_BIT)
  #define CFCR_SDIV_6		(4 << CPM_CFCR_SFR_BIT)
  #define CFCR_SDIV_8		(5 << CPM_CFCR_SFR_BIT)
  #define CFCR_SDIV_12		(6 << CPM_CFCR_SFR_BIT)
  #define CFCR_SDIV_16		(7 << CPM_CFCR_SFR_BIT)
  #define CFCR_SDIV_24		(8 << CPM_CFCR_SFR_BIT)
  #define CFCR_SDIV_32		(9 << CPM_CFCR_SFR_BIT)
#define CPM_CFCR_IFR_BIT	0
#define CPM_CFCR_IFR_MASK	(0x0f << CPM_CFCR_IFR_BIT)
  #define CFCR_IDIV_1		(0 << CPM_CFCR_IFR_BIT)
  #define CFCR_IDIV_2		(1 << CPM_CFCR_IFR_BIT)
  #define CFCR_IDIV_3		(2 << CPM_CFCR_IFR_BIT)
  #define CFCR_IDIV_4		(3 << CPM_CFCR_IFR_BIT)
  #define CFCR_IDIV_6		(4 << CPM_CFCR_IFR_BIT)
  #define CFCR_IDIV_8		(5 << CPM_CFCR_IFR_BIT)
  #define CFCR_IDIV_12		(6 << CPM_CFCR_IFR_BIT)
  #define CFCR_IDIV_16		(7 << CPM_CFCR_IFR_BIT)
  #define CFCR_IDIV_24		(8 << CPM_CFCR_IFR_BIT)
  #define CFCR_IDIV_32		(9 << CPM_CFCR_IFR_BIT)

#define CPM_PLCR1_PLL1FD_BIT	23
#define CPM_PLCR1_PLL1FD_MASK	(0x1ff << CPM_PLCR1_PLL1FD_BIT)
#define CPM_PLCR1_PLL1RD_BIT	18
#define CPM_PLCR1_PLL1RD_MASK	(0x1f << CPM_PLCR1_PLL1RD_BIT)
#define CPM_PLCR1_PLL1OD_BIT	16
#define CPM_PLCR1_PLL1OD_MASK	(0x03 << CPM_PLCR1_PLL1OD_BIT)
#define CPM_PLCR1_PLL1S		(1 << 10)
#define CPM_PLCR1_PLL1BP	(1 << 9)
#define CPM_PLCR1_PLL1EN	(1 << 8)
#define CPM_PLCR1_PLL1ST_BIT	0
#define CPM_PLCR1_PLL1ST_MASK	(0xff << CPM_PLCR1_PLL1ST_BIT)

#define CPM_OCR_O1ST_BIT	16
#define CPM_OCR_O1ST_MASK	(0xff << CPM_OCR_O1ST_BIT)
#define CPM_OCR_EXT_RTC_CLK	(1<<8)
#define CPM_OCR_SUSPEND_PHY1	(1<<7)
#define CPM_OCR_SUSPEND_PHY0	(1<<6)

#define CPM_CFCR2_PXFR_BIT	0
#define CPM_CFCR2_PXFR_MASK	(0x1ff << CPM_CFCR2_PXFR_BIT)

#define CPM_LPCR_DUTY_BIT	3
#define CPM_LPCR_DUTY_MASK	(0x1f << CPM_LPCR_DUTY_BIT)
#define CPM_LPCR_DOZE		(1 << 2)
#define CPM_LPCR_LPM_BIT	0
#define CPM_LPCR_LPM_MASK	(0x03 << CPM_LPCR_LPM_BIT)
  #define CPM_LPCR_LPM_IDLE		(0 << CPM_LPCR_LPM_BIT)
  #define CPM_LPCR_LPM_SLEEP		(1 << CPM_LPCR_LPM_BIT)
  #define CPM_LPCR_LPM_HIBERNATE	(2 << CPM_LPCR_LPM_BIT)

#define CPM_RSTR_SR		(1 << 2)
#define CPM_RSTR_WR		(1 << 1)
#define CPM_RSTR_HR		(1 << 0)

#define CPM_MSCR_MSTP_BIT	0
#define CPM_MSCR_MSTP_MASK	(0x1ffffff << CPM_MSCR_MSTP_BIT)
  #define CPM_MSCR_MSTP_UART0	0
  #define CPM_MSCR_MSTP_UART1	1
  #define CPM_MSCR_MSTP_UART2	2
  #define CPM_MSCR_MSTP_OST	3
  #define CPM_MSCR_MSTP_DMAC	5
  #define CPM_MSCR_MSTP_UHC	6
  #define CPM_MSCR_MSTP_LCD	7
  #define CPM_MSCR_MSTP_I2C	8
  #define CPM_MSCR_MSTP_AICPCLK 9
  #define CPM_MSCR_MSTP_PWM0	10
  #define CPM_MSCR_MSTP_PWM1	11
  #define CPM_MSCR_MSTP_SSI	12
  #define CPM_MSCR_MSTP_MSC	13
  #define CPM_MSCR_MSTP_SCC	14
  #define CPM_MSCR_MSTP_AICBCLK	18
  #define CPM_MSCR_MSTP_UART3	20
  #define CPM_MSCR_MSTP_ETH	21
  #define CPM_MSCR_MSTP_KBC	22
  #define CPM_MSCR_MSTP_CIM	23
  #define CPM_MSCR_MSTP_UDC	24
  #define CPM_MSCR_MSTP_UPRT	25

#define CPM_SCR_O1SE		(1 << 4)
#define CPM_SCR_HGP		(1 << 3)
#define CPM_SCR_HZP		(1 << 2)
#define CPM_SCR_HZM		(1 << 1)

#define CPM_WRER_RE_BIT		0
#define CPM_WRER_RE_MASK	(0xffff << CPM_WRER_RE_BIT)

#define CPM_WFER_FE_BIT		0
#define CPM_WFER_FE_MASK	(0xffff << CPM_WFER_FE_BIT)

#define CPM_WER_WERTC		(1 << 31)
#define CPM_WER_WEETH		(1 << 30)
#define CPM_WER_WE_BIT		0
#define CPM_WER_WE_MASK		(0xffff << CPM_WER_WE_BIT)

#define CPM_WSR_WSRTC		(1 << 31)
#define CPM_WSR_WSETH		(1 << 30)
#define CPM_WSR_WS_BIT		0
#define CPM_WSR_WS_MASK		(0xffff << CPM_WSR_WS_BIT)




/*************************************************************************
 * SSI
 *************************************************************************/
#define	SSI_DR			(SSI_BASE + 0x000)
#define	SSI_CR0			(SSI_BASE + 0x004)
#define	SSI_CR1			(SSI_BASE + 0x008)
#define	SSI_SR			(SSI_BASE + 0x00C)
#define	SSI_ITR			(SSI_BASE + 0x010)
#define	SSI_ICR			(SSI_BASE + 0x014)
#define	SSI_GR			(SSI_BASE + 0x018)

#define	REG_SSI_DR		REG32(SSI_DR)
#define	REG_SSI_CR0		REG16(SSI_CR0)
#define	REG_SSI_CR1		REG32(SSI_CR1)
#define	REG_SSI_SR		REG32(SSI_SR)
#define	REG_SSI_ITR		REG16(SSI_ITR)
#define	REG_SSI_ICR		REG8(SSI_ICR)
#define	REG_SSI_GR		REG16(SSI_GR)

/* SSI Data Register (SSI_DR) */

#define	SSI_DR_GPC_BIT		0
#define	SSI_DR_GPC_MASK		(0x1ff << SSI_DR_GPC_BIT)

/* SSI Control Register 0 (SSI_CR0) */

#define SSI_CR0_SSIE		(1 << 15)
#define SSI_CR0_TIE		(1 << 14)
#define SSI_CR0_RIE		(1 << 13)
#define SSI_CR0_TEIE		(1 << 12)
#define SSI_CR0_REIE		(1 << 11)
#define SSI_CR0_LOOP		(1 << 10)
#define SSI_CR0_RFINE		(1 << 9)
#define SSI_CR0_RFINC		(1 << 8)
#define SSI_CR0_FSEL		(1 << 6)
#define SSI_CR0_TFLUSH		(1 << 2)
#define SSI_CR0_RFLUSH		(1 << 1)
#define SSI_CR0_DISREV		(1 << 0)

/* SSI Control Register 1 (SSI_CR1) */

#define SSI_CR1_FRMHL_BIT	30
#define SSI_CR1_FRMHL_MASK	(0x3 << SSI_CR1_FRMHL_BIT)
  #define SSI_CR1_FRMHL_CELOW_CE2LOW	(0 << SSI_CR1_FRMHL_BIT) /* SSI_CE_ is low valid and SSI_CE2_ is low valid */
  #define SSI_CR1_FRMHL_CEHIGH_CE2LOW	(1 << SSI_CR1_FRMHL_BIT) /* SSI_CE_ is high valid and SSI_CE2_ is low valid */
  #define SSI_CR1_FRMHL_CELOW_CE2HIGH	(2 << SSI_CR1_FRMHL_BIT) /* SSI_CE_ is low valid  and SSI_CE2_ is high valid */
  #define SSI_CR1_FRMHL_CEHIGH_CE2HIGH	(3 << SSI_CR1_FRMHL_BIT) /* SSI_CE_ is high valid and SSI_CE2_ is high valid */
#define SSI_CR1_TFVCK_BIT	28
#define SSI_CR1_TFVCK_MASK	(0x3 << SSI_CR1_TFVCK_BIT)
  #define SSI_CR1_TFVCK_0	  (0 << SSI_CR1_TFVCK_BIT)
  #define SSI_CR1_TFVCK_1	  (1 << SSI_CR1_TFVCK_BIT)
  #define SSI_CR1_TFVCK_2	  (2 << SSI_CR1_TFVCK_BIT)
  #define SSI_CR1_TFVCK_3	  (3 << SSI_CR1_TFVCK_BIT)
#define SSI_CR1_TCKFI_BIT	26
#define SSI_CR1_TCKFI_MASK	(0x3 << SSI_CR1_TCKFI_BIT)
  #define SSI_CR1_TCKFI_0	  (0 << SSI_CR1_TCKFI_BIT)
  #define SSI_CR1_TCKFI_1	  (1 << SSI_CR1_TCKFI_BIT)
  #define SSI_CR1_TCKFI_2	  (2 << SSI_CR1_TCKFI_BIT)
  #define SSI_CR1_TCKFI_3	  (3 << SSI_CR1_TCKFI_BIT)
#define SSI_CR1_LFST		(1 << 25)
#define SSI_CR1_ITFRM		(1 << 24)
#define SSI_CR1_UNFIN		(1 << 23)
#define SSI_CR1_MULTS		(1 << 22)
#define SSI_CR1_FMAT_BIT	20
#define SSI_CR1_FMAT_MASK	(0x3 << SSI_CR1_FMAT_BIT)
  #define SSI_CR1_FMAT_SPI	  (0 << SSI_CR1_FMAT_BIT) /* Motorola¡¯s SPI format */
  #define SSI_CR1_FMAT_SSP	  (1 << SSI_CR1_FMAT_BIT) /* TI's SSP format */
  #define SSI_CR1_FMAT_MW1	  (2 << SSI_CR1_FMAT_BIT) /* National Microwire 1 format */
  #define SSI_CR1_FMAT_MW2	  (3 << SSI_CR1_FMAT_BIT) /* National Microwire 2 format */
#define SSI_CR1_MCOM_BIT	12
#define SSI_CR1_MCOM_MASK	(0xf << SSI_CR1_MCOM_BIT)
  #define SSI_CR1_MCOM_1BIT	  (0x0 << SSI_CR1_MCOM_BIT) /* 1-bit command selected */
  #define SSI_CR1_MCOM_2BIT	  (0x1 << SSI_CR1_MCOM_BIT) /* 2-bit command selected */
  #define SSI_CR1_MCOM_3BIT	  (0x2 << SSI_CR1_MCOM_BIT) /* 3-bit command selected */
  #define SSI_CR1_MCOM_4BIT	  (0x3 << SSI_CR1_MCOM_BIT) /* 4-bit command selected */
  #define SSI_CR1_MCOM_5BIT	  (0x4 << SSI_CR1_MCOM_BIT) /* 5-bit command selected */
  #define SSI_CR1_MCOM_6BIT	  (0x5 << SSI_CR1_MCOM_BIT) /* 6-bit command selected */
  #define SSI_CR1_MCOM_7BIT	  (0x6 << SSI_CR1_MCOM_BIT) /* 7-bit command selected */
  #define SSI_CR1_MCOM_8BIT	  (0x7 << SSI_CR1_MCOM_BIT) /* 8-bit command selected */
  #define SSI_CR1_MCOM_9BIT	  (0x8 << SSI_CR1_MCOM_BIT) /* 9-bit command selected */
  #define SSI_CR1_MCOM_10BIT	  (0x9 << SSI_CR1_MCOM_BIT) /* 10-bit command selected */
  #define SSI_CR1_MCOM_11BIT	  (0xA << SSI_CR1_MCOM_BIT) /* 11-bit command selected */
  #define SSI_CR1_MCOM_12BIT	  (0xB << SSI_CR1_MCOM_BIT) /* 12-bit command selected */
  #define SSI_CR1_MCOM_13BIT	  (0xC << SSI_CR1_MCOM_BIT) /* 13-bit command selected */
  #define SSI_CR1_MCOM_14BIT	  (0xD << SSI_CR1_MCOM_BIT) /* 14-bit command selected */
  #define SSI_CR1_MCOM_15BIT	  (0xE << SSI_CR1_MCOM_BIT) /* 15-bit command selected */
  #define SSI_CR1_MCOM_16BIT	  (0xF << SSI_CR1_MCOM_BIT) /* 16-bit command selected */
#define SSI_CR1_TTRG_BIT	10
#define SSI_CR1_TTRG_MASK	(0x3 << SSI_CR1_TTRG_BIT)
  #define SSI_CR1_TTRG_1	  (0 << SSI_CR1_TTRG_BIT)/* Less than or equal to 1 */
  #define SSI_CR1_TTRG_4	  (1 << SSI_CR1_TTRG_BIT) /* Less than or equal to 4 */
  #define SSI_CR1_TTRG_8	  (2 << SSI_CR1_TTRG_BIT) /* Less than or equal to 8 */
  #define SSI_CR1_TTRG_14	  (3 << SSI_CR1_TTRG_BIT) /* Less than or equal to 14 */
#define SSI_CR1_RTRG_BIT	8
#define SSI_CR1_RTRG_MASK	(0x3 << SSI_CR1_RTRG_BIT)
  #define SSI_CR1_RTRG_1	  (0 << SSI_CR1_RTRG_BIT) /* More than or equal to 1 */
  #define SSI_CR1_RTRG_4	  (1 << SSI_CR1_RTRG_BIT) /* More than or equal to 4 */
  #define SSI_CR1_RTRG_8	  (2 << SSI_CR1_RTRG_BIT) /* More than or equal to 8 */
  #define SSI_CR1_RTRG_14	  (3 << SSI_CR1_RTRG_BIT) /* More than or equal to 14 */
#define SSI_CR1_FLEN_BIT	4
#define SSI_CR1_FLEN_MASK	(0xf << SSI_CR1_FLEN_BIT)
  #define SSI_CR1_FLEN_2BIT	  (0x0 << SSI_CR1_FLEN_BIT)
  #define SSI_CR1_FLEN_3BIT	  (0x1 << SSI_CR1_FLEN_BIT)
  #define SSI_CR1_FLEN_4BIT	  (0x2 << SSI_CR1_FLEN_BIT)
  #define SSI_CR1_FLEN_5BIT	  (0x3 << SSI_CR1_FLEN_BIT)
  #define SSI_CR1_FLEN_6BIT	  (0x4 << SSI_CR1_FLEN_BIT)
  #define SSI_CR1_FLEN_7BIT	  (0x5 << SSI_CR1_FLEN_BIT)
  #define SSI_CR1_FLEN_8BIT	  (0x6 << SSI_CR1_FLEN_BIT)
  #define SSI_CR1_FLEN_9BIT	  (0x7 << SSI_CR1_FLEN_BIT)
  #define SSI_CR1_FLEN_10BIT	  (0x8 << SSI_CR1_FLEN_BIT)
  #define SSI_CR1_FLEN_11BIT	  (0x9 << SSI_CR1_FLEN_BIT)
  #define SSI_CR1_FLEN_12BIT	  (0xA << SSI_CR1_FLEN_BIT)
  #define SSI_CR1_FLEN_13BIT	  (0xB << SSI_CR1_FLEN_BIT)
  #define SSI_CR1_FLEN_14BIT	  (0xC << SSI_CR1_FLEN_BIT)
  #define SSI_CR1_FLEN_15BIT	  (0xD << SSI_CR1_FLEN_BIT)
  #define SSI_CR1_FLEN_16BIT	  (0xE << SSI_CR1_FLEN_BIT)
  #define SSI_CR1_FLEN_17BIT	  (0xF << SSI_CR1_FLEN_BIT)
#define SSI_CR1_PHA		(1 << 1)
#define SSI_CR1_POL		(1 << 0)

/* SSI Status Register (SSI_SR) */

#define SSI_SR_TFIFONUM_BIT	13
#define SSI_SR_TFIFONUM_MASK	(0x1f << SSI_SR_TFIFONUM_BIT)
#define SSI_SR_RFIFONUM_BIT	8
#define SSI_SR_RFIFONUM_MASK	(0x1f << SSI_SR_RFIFONUM_BIT)
#define SSI_SR_END		(1 << 7)
#define SSI_SR_BUSY		(1 << 6)
#define SSI_SR_TFF		(1 << 5)
#define SSI_SR_RFE		(1 << 4)
#define SSI_SR_TFHE		(1 << 3)
#define SSI_SR_RFHF		(1 << 2)
#define SSI_SR_UNDR		(1 << 1)
#define SSI_SR_OVER		(1 << 0)

/* SSI Interval Time Control Register (SSI_ITR) */

#define	SSI_ITR_CNTCLK		(1 << 15)
#define SSI_ITR_IVLTM_BIT	0
#define SSI_ITR_IVLTM_MASK	(0x7fff << SSI_ITR_IVLTM_BIT)

#ifndef __ASSEMBLY__

/***************************************************************************
 * MSC
 ***************************************************************************/

#define __msc_start_op() \
  ( REG_MSC_STRPCL = MSC_STRPCL_START_OP | MSC_STRPCL_CLOCK_CONTROL_START )

#define __msc_set_resto(to) 	( REG_MSC_RESTO = to )
#define __msc_set_rdto(to) 	( REG_MSC_RDTO = to )
#define __msc_set_cmd(cmd) 	( REG_MSC_CMD = cmd )
#define __msc_set_arg(arg) 	( REG_MSC_ARG = arg )
#define __msc_set_nob(nob) 	( REG_MSC_NOB = nob )
#define __msc_get_nob() 	( REG_MSC_NOB )
#define __msc_set_blklen(len) 	( REG_MSC_BLKLEN = len )
#define __msc_set_cmdat(cmdat) 	( REG_MSC_CMDAT = cmdat )
#define __msc_set_cmdat_ioabort() 	( REG_MSC_CMDAT |= MSC_CMDAT_IO_ABORT )
#define __msc_clear_cmdat_ioabort() 	( REG_MSC_CMDAT &= ~MSC_CMDAT_IO_ABORT )

#define __msc_set_cmdat_bus_width1() 			\
do { 							\
	REG_MSC_CMDAT &= ~MSC_CMDAT_BUS_WIDTH_MASK; 	\
	REG_MSC_CMDAT |= MSC_CMDAT_BUS_WIDTH_1BIT; 	\
} while(0)

#define __msc_set_cmdat_bus_width4() 			\
do { 							\
	REG_MSC_CMDAT &= ~MSC_CMDAT_BUS_WIDTH_MASK; 	\
	REG_MSC_CMDAT |= MSC_CMDAT_BUS_WIDTH_4BIT; 	\
} while(0)

#define __msc_set_cmdat_dma_en() ( REG_MSC_CMDAT |= MSC_CMDAT_DMA_EN )
#define __msc_set_cmdat_init() 	( REG_MSC_CMDAT |= MSC_CMDAT_INIT )
#define __msc_set_cmdat_busy() 	( REG_MSC_CMDAT |= MSC_CMDAT_BUSY )
#define __msc_set_cmdat_stream() ( REG_MSC_CMDAT |= MSC_CMDAT_STREAM_BLOCK )
#define __msc_set_cmdat_block() ( REG_MSC_CMDAT &= ~MSC_CMDAT_STREAM_BLOCK )
#define __msc_set_cmdat_read() 	( REG_MSC_CMDAT &= ~MSC_CMDAT_WRITE_READ )
#define __msc_set_cmdat_write() ( REG_MSC_CMDAT |= MSC_CMDAT_WRITE_READ )
#define __msc_set_cmdat_data_en() ( REG_MSC_CMDAT |= MSC_CMDAT_DATA_EN )

/* r is MSC_CMDAT_RESPONSE_FORMAT_Rx or MSC_CMDAT_RESPONSE_FORMAT_NONE */
#define __msc_set_cmdat_res_format(r) 				\
do { 								\
	REG_MSC_CMDAT &= ~MSC_CMDAT_RESPONSE_FORMAT_MASK; 	\
	REG_MSC_CMDAT |= (r); 					\
} while(0)

#define __msc_clear_cmdat() \
  REG_MSC_CMDAT &= ~( MSC_CMDAT_IO_ABORT | MSC_CMDAT_DMA_EN | MSC_CMDAT_INIT| \
  MSC_CMDAT_BUSY | MSC_CMDAT_STREAM_BLOCK | MSC_CMDAT_WRITE_READ | \
  MSC_CMDAT_DATA_EN | MSC_CMDAT_RESPONSE_FORMAT_MASK )

#define __msc_get_imask() 		( REG_MSC_IMASK )
#define __msc_mask_all_intrs() 		( REG_MSC_IMASK = 0xff )
#define __msc_unmask_all_intrs() 	( REG_MSC_IMASK = 0x00 )
#define __msc_mask_rd() 		( REG_MSC_IMASK |= MSC_IMASK_RXFIFO_RD_REQ )
#define __msc_unmask_rd() 		( REG_MSC_IMASK &= ~MSC_IMASK_RXFIFO_RD_REQ )
#define __msc_mask_wr() 		( REG_MSC_IMASK |= MSC_IMASK_TXFIFO_WR_REQ )
#define __msc_unmask_wr() 		( REG_MSC_IMASK &= ~MSC_IMASK_TXFIFO_WR_REQ )
#define __msc_mask_endcmdres() 		( REG_MSC_IMASK |= MSC_IMASK_END_CMD_RES )
#define __msc_unmask_endcmdres() 	( REG_MSC_IMASK &= ~MSC_IMASK_END_CMD_RES )
#define __msc_mask_datatrandone() 	( REG_MSC_IMASK |= MSC_IMASK_DATA_TRAN_DONE )
#define __msc_unmask_datatrandone() 	( REG_MSC_IMASK &= ~MSC_IMASK_DATA_TRAN_DONE )
#define __msc_mask_prgdone() 		( REG_MSC_IMASK |= MSC_IMASK_PRG_DONE )
#define __msc_unmask_prgdone() 		( REG_MSC_IMASK &= ~MSC_IMASK_PRG_DONE )

/* n=1,2,4,8,16,32,64,128 */
#define __msc_set_clkrt_div(n) 				\
do { 							\
	REG_MSC_CLKRT &= ~MSC_CLKRT_CLK_RATE_MASK; 	\
	REG_MSC_CLKRT |= MSC_CLKRT_CLK_RATE_DIV_##n;	\
} while(0)

#define __msc_get_ireg() 		( REG_MSC_IREG )
#define __msc_ireg_rd() 		( REG_MSC_IREG & MSC_IREG_RXFIFO_RD_REQ )
#define __msc_ireg_wr() 		( REG_MSC_IREG & MSC_IREG_TXFIFO_WR_REQ )
#define __msc_ireg_end_cmd_res() 	( REG_MSC_IREG & MSC_IREG_END_CMD_RES )
#define __msc_ireg_data_tran_done() 	( REG_MSC_IREG & MSC_IREG_DATA_TRAN_DONE )
#define __msc_ireg_prg_done() 		( REG_MSC_IREG & MSC_IREG_PRG_DONE )
#define __msc_ireg_clear_end_cmd_res() 	( REG_MSC_IREG = MSC_IREG_END_CMD_RES )
#define __msc_ireg_clear_data_tran_done() ( REG_MSC_IREG = MSC_IREG_DATA_TRAN_DONE )
#define __msc_ireg_clear_prg_done() 	( REG_MSC_IREG = MSC_IREG_PRG_DONE )

#define __msc_get_stat() 		( REG_MSC_STAT )
#define __msc_stat_not_end_cmd_res() 	( (REG_MSC_STAT & MSC_STAT_END_CMD_RES) == 0)
#define __msc_stat_crc_err() \
  ( REG_MSC_STAT & (MSC_STAT_CRC_RES_ERR | MSC_STAT_CRC_READ_ERROR | MSC_STAT_CRC_WRITE_ERROR_YES) )
#define __msc_stat_res_crc_err() 	( REG_MSC_STAT & MSC_STAT_CRC_RES_ERR )
#define __msc_stat_rd_crc_err() 	( REG_MSC_STAT & MSC_STAT_CRC_READ_ERROR )
#define __msc_stat_wr_crc_err() 	( REG_MSC_STAT & MSC_STAT_CRC_WRITE_ERROR_YES )
#define __msc_stat_resto_err() 		( REG_MSC_STAT & MSC_STAT_TIME_OUT_RES )
#define __msc_stat_rdto_err() 		( REG_MSC_STAT & MSC_STAT_TIME_OUT_READ )

#define __msc_rd_resfifo() 		( REG_MSC_RES )
#define __msc_rd_rxfifo()  		( REG_MSC_RXFIFO )
#define __msc_wr_txfifo(v)  		( REG_MSC_TXFIFO = v )

#define __msc_reset() 						\
do { 								\
	REG_MSC_STRPCL = MSC_STRPCL_RESET;			\
 	while (REG_MSC_STAT & MSC_STAT_IS_RESETTING);		\
} while (0)

#define __msc_start_clk() 					\
do { 								\
	REG_MSC_STRPCL &= ~MSC_STRPCL_CLOCK_CONTROL_MASK;	\
	REG_MSC_STRPCL |= MSC_STRPCL_CLOCK_CONTROL_START;	\
} while (0)

#define __msc_stop_clk() 					\
do { 								\
	REG_MSC_STRPCL &= ~MSC_STRPCL_CLOCK_CONTROL_MASK;	\
	REG_MSC_STRPCL |= MSC_STRPCL_CLOCK_CONTROL_STOP;	\
} while (0)

#define MMC_CLK 19169200
#define SD_CLK  24576000

/* msc_clk should little than pclk and little than clk retrieve from card */
#define __msc_calc_clk_divisor(type,dev_clk,msc_clk,lv)		\
do {								\
	unsigned int rate, pclk, i;				\
	pclk = dev_clk;						\
	rate = type?SD_CLK:MMC_CLK;				\
  	if (msc_clk && msc_clk < pclk)				\
    		pclk = msc_clk;					\
	i = 0;							\
  	while (pclk < rate)					\
    	{							\
      		i ++;						\
      		rate >>= 1;					\
    	}							\
  	lv = i;							\
} while(0)

/* divide rate to little than or equal to 400kHz */
#define __msc_calc_slow_clk_divisor(type, lv)			\
do {								\
	unsigned int rate, i;					\
	rate = (type?SD_CLK:MMC_CLK)/1000/400;			\
	i = 0;							\
	while (rate > 0)					\
    	{							\
      		rate >>= 1;					\
      		i ++;						\
    	}							\
  	lv = i;							\
} while(0)

/***************************************************************************
 * RTC
 ***************************************************************************/

#define __rtc_start()	                ( REG_RTC_RCR |= RTC_RCR_START )
#define __rtc_stop()	                ( REG_RTC_RCR &= ~RTC_RCR_START )

#define __rtc_enable_alarm()	        ( REG_RTC_RCR |= RTC_RCR_AE )
#define __rtc_disable_alarm()	        ( REG_RTC_RCR &= ~RTC_RCR_AE )
#define __rtc_enable_alarm_irq()	( REG_RTC_RCR |= RTC_RCR_AIE )
#define __rtc_disable_alarm_irq()	( REG_RTC_RCR &= ~RTC_RCR_AIE )

#define __rtc_enable_1hz_irq()		( REG_RTC_RCR |= RTC_RCR_HZIE )
#define __rtc_disable_1hz_irq()		( REG_RTC_RCR &= ~RTC_RCR_HZIE )

#define __rtc_is_alarm_flag()		( REG_RTC_RCR & RTC_RCR_AF )
#define __rtc_is_1hz_flag()		( REG_RTC_RCR & RTC_RCR_HZ )
#define __rtc_clear_alarm_flag()	( REG_RTC_RCR &= ~RTC_RCR_AF )
#define __rtc_clear_1hz_flag()		( REG_RTC_RCR &= ~RTC_RCR_HZ )

#define __rtc_set_second(s)	        ( REG_RTC_RSR = (s) )
#define __rtc_get_second()	        REG_RTC_RSR
#define __rtc_set_alarm(s)	        ( REG_RTC_RSAR = (s) )
#define __rtc_get_alarm()	        REG_RTC_RSAR

#define __rtc_adjust_1hz(f32k) \
  ( REG_RTC_RGR = (REG_RTC_RGR & ~(RTC_REG_DIV_MASK | RTC_RGR_ADJ_MASK)) | f32k | 0 )
#define __rtc_lock_1hz()	( REG_RTC_RGR |= RTC_RGR_LOCK )


/***************************************************************************
 * FIR
 ***************************************************************************/

/* enable/disable fir unit */
#define __fir_enable()		( REG_FIR_CR1 |= FIR_CR1_FIRUE )
#define __fir_disable()		( REG_FIR_CR1 &= ~FIR_CR1_FIRUE )

/* enable/disable address comparison */
#define __fir_enable_ac()	( REG_FIR_CR1 |= FIR_CR1_ACE )
#define __fir_disable_ac()	( REG_FIR_CR1 &= ~FIR_CR1_ACE )

/* select frame end mode as underrun or normal */
#define __fir_set_eous()	( REG_FIR_CR1 |= FIR_CR1_EOUS )
#define __fir_clear_eous()	( REG_FIR_CR1 &= ~FIR_CR1_EOUS )

/* enable/disable transmitter idle interrupt */
#define __fir_enable_tii()	( REG_FIR_CR1 |= FIR_CR1_TIIE )
#define __fir_disable_tii()	( REG_FIR_CR1 &= ~FIR_CR1_TIIE )

/* enable/disable transmit FIFO service request interrupt */
#define __fir_enable_tfi()	( REG_FIR_CR1 |= FIR_CR1_TFIE )
#define __fir_disable_tfi()	( REG_FIR_CR1 &= ~FIR_CR1_TFIE )

/* enable/disable receive FIFO service request interrupt */
#define __fir_enable_rfi()	( REG_FIR_CR1 |= FIR_CR1_RFIE )
#define __fir_disable_rfi()	( REG_FIR_CR1 &= ~FIR_CR1_RFIE )

/* enable/disable tx function */
#define __fir_tx_enable()	( REG_FIR_CR1 |= FIR_CR1_TXE )
#define __fir_tx_disable()	( REG_FIR_CR1 &= ~FIR_CR1_TXE )

/* enable/disable rx function */
#define __fir_rx_enable()	( REG_FIR_CR1 |= FIR_CR1_RXE )
#define __fir_rx_disable()	( REG_FIR_CR1 &= ~FIR_CR1_RXE )


/* enable/disable serial infrared interaction pulse (SIP) */
#define __fir_enable_sip()	( REG_FIR_CR2 |= FIR_CR2_SIPE )
#define __fir_disable_sip()	( REG_FIR_CR2 &= ~FIR_CR2_SIPE )

/* un-inverted CRC value is sent out */
#define __fir_enable_bcrc()	( REG_FIR_CR2 |= FIR_CR2_BCRC )

/* inverted CRC value is sent out */
#define __fir_disable_bcrc()	( REG_FIR_CR2 &= ~FIR_CR2_BCRC )

/* enable/disable Transmit Frame Length Register */
#define __fir_enable_tflr()	( REG_FIR_CR2 |= FIR_CR2_TFLRS )
#define __fir_disable_tflr()	( REG_FIR_CR2 &= ~FIR_CR2_TFLRS )

/* Preamble is transmitted in idle state */
#define __fir_set_iss()	( REG_FIR_CR2 |= FIR_CR2_ISS )

/* Abort symbol is transmitted in idle state */
#define __fir_clear_iss()	( REG_FIR_CR2 &= ~FIR_CR2_ISS )

/* enable/disable loopback mode */
#define __fir_enable_loopback()	( REG_FIR_CR2 |= FIR_CR2_LMS )
#define __fir_disable_loopback()	( REG_FIR_CR2 &= ~FIR_CR2_LMS )

/* select transmit pin polarity */
#define __fir_tpp_negative()	( REG_FIR_CR2 |= FIR_CR2_TPPS )
#define __fir_tpp_positive()	( REG_FIR_CR2 &= ~FIR_CR2_TPPS )

/* select receive pin polarity */
#define __fir_rpp_negative()	( REG_FIR_CR2 |= FIR_CR2_RPPS )
#define __fir_rpp_positive()	( REG_FIR_CR2 &= ~FIR_CR2_RPPS )

/* n=16,32,64,128 */
#define __fir_set_txfifo_trigger(n) 		\
do { 						\
	REG_FIR_CR2 &= ~FIR_CR2_TTRG_MASK;	\
	REG_FIR_CR2 |= FIR_CR2_TTRG_##n;	\
} while (0)

/* n=16,32,64,128 */
#define __fir_set_rxfifo_trigger(n) 		\
do { 						\
	REG_FIR_CR2 &= ~FIR_CR2_RTRG_MASK;	\
	REG_FIR_CR2 |= FIR_CR2_RTRG_##n;	\
} while (0)


/* FIR status checking */

#define __fir_test_rfw()	( REG_FIR_SR & FIR_SR_RFW )
#define __fir_test_rfa()	( REG_FIR_SR & FIR_SR_RFA )
#define __fir_test_tfrtl()	( REG_FIR_SR & FIR_SR_TFRTL )
#define __fir_test_rfrtl()	( REG_FIR_SR & FIR_SR_RFRTL )
#define __fir_test_urun()	( REG_FIR_SR & FIR_SR_URUN )
#define __fir_test_rfte()	( REG_FIR_SR & FIR_SR_RFTE )
#define __fir_test_orun()	( REG_FIR_SR & FIR_SR_ORUN )
#define __fir_test_crce()	( REG_FIR_SR & FIR_SR_CRCE )
#define __fir_test_fend()	( REG_FIR_SR & FIR_SR_FEND )
#define __fir_test_tff()	( REG_FIR_SR & FIR_SR_TFF )
#define __fir_test_rfe()	( REG_FIR_SR & FIR_SR_RFE )
#define __fir_test_tidle()	( REG_FIR_SR & FIR_SR_TIDLE )
#define __fir_test_rb()		( REG_FIR_SR & FIR_SR_RB )

#define __fir_clear_status()					\
do { 								\
	REG_FIR_SR |= FIR_SR_RFW | FIR_SR_RFA | FIR_SR_URUN;	\
} while (0)

#define __fir_clear_rfw()	( REG_FIR_SR |= FIR_SR_RFW )
#define __fir_clear_rfa()	( REG_FIR_SR |= FIR_SR_RFA )
#define __fir_clear_urun()	( REG_FIR_SR |= FIR_SR_URUN )

#define __fir_set_tflr(len)			\
do { 						\
	REG_FIR_TFLR = len; 			\
} while (0)

#define __fir_set_addr(a)	( REG_FIR_AR = (a) )

#define __fir_write_data(data)	( REG_FIR_TDR = data )
#define __fir_read_data(data)	( data = REG_FIR_RDR )

/***************************************************************************
 * SCC
 ***************************************************************************/

#define __scc_enable(base)	( REG_SCC_CR(base) |= SCC_CR_SCCE )
#define __scc_disable(base)	( REG_SCC_CR(base) &= ~SCC_CR_SCCE )

#define __scc_set_tx_mode(base)	( REG_SCC_CR(base) |= SCC_CR_TRS )
#define __scc_set_rx_mode(base)	( REG_SCC_CR(base) &= ~SCC_CR_TRS )

#define __scc_enable_t2r(base)	( REG_SCC_CR(base) |= SCC_CR_T2R )
#define __scc_disable_t2r(base)	( REG_SCC_CR(base) &= ~SCC_CR_T2R )

#define __scc_clk_as_devclk(base)		\
do {						\
  REG_SCC_CR(base) &= ~SCC_CR_FDIV_MASK;	\
  REG_SCC_CR(base) |= SCC_CR_FDIV_1;		\
} while (0)

#define __scc_clk_as_half_devclk(base)		\
do {						\
  REG_SCC_CR(base) &= ~SCC_CR_FDIV_MASK;	\
  REG_SCC_CR(base) |= SCC_CR_FDIV_2;		\
} while (0)

/* n=1,4,8,14 */
#define __scc_set_fifo_trigger(base, n)		\
do {						\
  REG_SCC_CR(base) &= ~SCC_CR_TRIG_MASK;	\
  REG_SCC_CR(base) |= SCC_CR_TRIG_##n;		\
} while (0)

#define __scc_set_protocol(base, p)		\
do {						\
	if (p)					\
	  	REG_SCC_CR(base) |= SCC_CR_TP;	\
	else					\
	 	REG_SCC_CR(base) &= ~SCC_CR_TP;	\
} while (0)

#define __scc_flush_fifo(base)	( REG_SCC_CR(base) |= SCC_CR_FLUSH )

#define __scc_set_invert_mode(base)	( REG_SCC_CR(base) |= SCC_CR_CONV )
#define __scc_set_direct_mode(base)	( REG_SCC_CR(base) &= ~SCC_CR_CONV )

#define SCC_ERR_INTRS \
    ( SCC_CR_ECIE | SCC_CR_EPIE | SCC_CR_RETIE | SCC_CR_EOIE )
#define SCC_ALL_INTRS \
    ( SCC_CR_TXIE | SCC_CR_RXIE | SCC_CR_TENDIE | SCC_CR_RTOIE | \
      SCC_CR_ECIE | SCC_CR_EPIE | SCC_CR_RETIE | SCC_CR_EOIE )

#define __scc_enable_err_intrs(base)	( REG_SCC_CR(base) |= SCC_ERR_INTRS )
#define __scc_disable_err_intrs(base)	( REG_SCC_CR(base) &= ~SCC_ERR_INTRS )

#define SCC_ALL_ERRORS \
    ( SCC_SR_ORER | SCC_SR_RTO | SCC_SR_PER | SCC_SR_RETR_3 | SCC_SR_ECNTO)

#define __scc_clear_errors(base)	( REG_SCC_SR(base) &= ~SCC_ALL_ERRORS )

#define __scc_enable_all_intrs(base)	( REG_SCC_CR(base) |= SCC_ALL_INTRS )
#define __scc_disable_all_intrs(base)	( REG_SCC_CR(base) &= ~SCC_ALL_INTRS )

#define __scc_enable_tx_intr(base)	( REG_SCC_CR(base) |= SCC_CR_TXIE | SCC_CR_TENDIE )
#define __scc_disable_tx_intr(base)	( REG_SCC_CR(base) &= ~(SCC_CR_TXIE | SCC_CR_TENDIE) )

#define __scc_enable_rx_intr(base)	( REG_SCC_CR(base) |= SCC_CR_RXIE)
#define __scc_disable_rx_intr(base)	( REG_SCC_CR(base) &= ~SCC_CR_RXIE)

#define __scc_set_tsend(base)		( REG_SCC_CR(base) |= SCC_CR_TSEND )
#define __scc_clear_tsend(base)		( REG_SCC_CR(base) &= ~SCC_CR_TSEND )

#define __scc_set_clockstop(base)	( REG_SCC_CR(base) |= SCC_CR_CLKSTP )
#define __scc_clear_clockstop(base)	( REG_SCC_CR(base) &= ~SCC_CR_CLKSTP )

#define __scc_clockstop_low(base)		\
do {						\
  REG_SCC_CR(base) &= ~SCC_CR_PX_MASK;		\
  REG_SCC_CR(base) |= SCC_CR_PX_STOP_LOW;	\
} while (0)

#define __scc_clockstop_high(base)		\
do {						\
  REG_SCC_CR(base) &= ~SCC_CR_PX_MASK;		\
  REG_SCC_CR(base) |= SCC_CR_PX_STOP_HIGH;	\
} while (0)


/* SCC status checking */
#define __scc_check_transfer_status(base)  ( REG_SCC_SR(base) & SCC_SR_TRANS )
#define __scc_check_rx_overrun_error(base) ( REG_SCC_SR(base) & SCC_SR_ORER )
#define __scc_check_rx_timeout(base)	   ( REG_SCC_SR(base) & SCC_SR_RTO )
#define __scc_check_parity_error(base)	   ( REG_SCC_SR(base) & SCC_SR_PER )
#define __scc_check_txfifo_trigger(base)   ( REG_SCC_SR(base) & SCC_SR_TFTG )
#define __scc_check_rxfifo_trigger(base)   ( REG_SCC_SR(base) & SCC_SR_RFTG )
#define __scc_check_tx_end(base)	   ( REG_SCC_SR(base) & SCC_SR_TEND )
#define __scc_check_retx_3(base)	   ( REG_SCC_SR(base) & SCC_SR_RETR_3 )
#define __scc_check_ecnt_overflow(base)	   ( REG_SCC_SR(base) & SCC_SR_ECNTO )


/***************************************************************************
 * WDT
 ***************************************************************************/

#define __wdt_set_count(count) ( REG_WDT_WTCNT = (count) )
#define __wdt_start()          ( REG_WDT_WTCSR |= WDT_WTCSR_START )
#define __wdt_stop()           ( REG_WDT_WTCSR &= ~WDT_WTCSR_START )


/***************************************************************************
 * OST
 ***************************************************************************/

#define __ost_enable_all()         ( REG_OST_TER |= 0x07 )
#define __ost_disable_all()        ( REG_OST_TER &= ~0x07 )
#define __ost_enable_channel(n)    ( REG_OST_TER |= (1 << (n)) )
#define __ost_disable_channel(n)   ( REG_OST_TER &= ~(1 << (n)) )
#define __ost_set_reload(n, val)   ( REG_OST_TRDR(n) = (val) )
#define __ost_set_count(n, val)    ( REG_OST_TCNT(n) = (val) )
#define __ost_get_count(n)         ( REG_OST_TCNT(n) )
#define __ost_set_clock(n, cs)     ( REG_OST_TCSR(n) |= (cs) )
#define __ost_set_mode(n, val)     ( REG_OST_TCSR(n) = (val) )
#define __ost_enable_interrupt(n)  ( REG_OST_TCSR(n) |= OST_TCSR_UIE )
#define __ost_disable_interrupt(n) ( REG_OST_TCSR(n) &= ~OST_TCSR_UIE )
#define __ost_uf_detected(n)       ( REG_OST_TCSR(n) & OST_TCSR_UF )
#define __ost_clear_uf(n)          ( REG_OST_TCSR(n) &= ~OST_TCSR_UF )
#define __ost_is_busy(n)           ( REG_OST_TCSR(n) & OST_TCSR_BUSY )
#define __ost_clear_busy(n)        ( REG_OST_TCSR(n) &= ~OST_TCSR_BUSY )


/***************************************************************************
 * UART
 ***************************************************************************/

#define __uart_enable(n) \
  ( REG8(UART_BASE + UART_OFF*(n) + OFF_FCR) = UARTFCR_UUE | UARTFCR_FE )
#define __uart_disable(n) \
  ( REG8(UART_BASE + UART_OFF*(n) + OFF_FCR) = ~UARTFCR_UUE )

#define __uart_enable_transmit_irq(n) \
  ( REG8(UART_BASE + UART_OFF*(n) + OFF_IER) |= UARTIER_TIE )
#define __uart_disable_transmit_irq(n) \
  ( REG8(UART_BASE + UART_OFF*(n) + OFF_IER) &= ~UARTIER_TIE )

#define __uart_enable_receive_irq(n) \
  ( REG8(UART_BASE + UART_OFF*(n) + OFF_IER) |= UARTIER_RIE | UARTIER_RLIE | UARTIER_RTIE )
#define __uart_disable_receive_irq(n) \
  ( REG8(UART_BASE + UART_OFF*(n) + OFF_IER) &= ~(UARTIER_RIE | UARTIER_RLIE | UARTIER_RTIE) )

#define __uart_enable_loopback(n) \
  ( REG8(UART_BASE + UART_OFF*(n) + OFF_MCR) |= UARTMCR_LOOP )
#define __uart_disable_loopback(n) \
  ( REG8(UART_BASE + UART_OFF*(n) + OFF_MCR) &= ~UARTMCR_LOOP )

#define __uart_set_8n1(n) \
  ( REG8(UART_BASE + UART_OFF*(n) + OFF_LCR) = UARTLCR_WLEN_8 )

#define __uart_set_baud(n, devclk, baud)						\
  do {											\
	REG8(UART_BASE + UART_OFF*(n) + OFF_LCR) |= UARTLCR_DLAB;			\
	REG8(UART_BASE + UART_OFF*(n) + OFF_DLLR) = (devclk / 16 / baud) & 0xff;	\
	REG8(UART_BASE + UART_OFF*(n) + OFF_DLHR) = ((devclk / 16 / baud) >> 8) & 0xff;	\
	REG8(UART_BASE + UART_OFF*(n) + OFF_LCR) &= ~UARTLCR_DLAB;			\
  } while (0)

#define __uart_parity_error(n) \
  ( (REG8(UART_BASE + UART_OFF*(n) + OFF_LSR) & UARTLSR_PER) != 0 )

#define __uart_clear_errors(n) \
  ( REG8(UART_BASE + UART_OFF*(n) + OFF_LSR) &= ~(UARTLSR_ORER | UARTLSR_BRK | UARTLSR_FER | UARTLSR_PER | UARTSR_RFER) )

#define __uart_transmit_fifo_empty(n) \
  ( (REG8(UART_BASE + UART_OFF*(n) + OFF_LSR) & UARTLSR_TDRQ) != 0 )

#define __uart_transmit_end(n) \
  ( (REG8(UART_BASE + UART_OFF*(n) + OFF_LSR) & UARTLSR_TEMT) != 0 )

#define __uart_transmit_char(n, ch) \
  REG8(UART_BASE + UART_OFF*(n) + OFF_TDR) = (ch)

#define __uart_receive_fifo_full(n) \
  ( (REG8(UART_BASE + UART_OFF*(n) + OFF_LSR) & UARTLSR_DR) != 0 )

#define __uart_receive_ready(n) \
  ( (REG8(UART_BASE + UART_OFF*(n) + OFF_LSR) & UARTLSR_DR) != 0 )

#define __uart_receive_char(n) \
  REG8(UART_BASE + UART_OFF*(n) + OFF_RDR)

#define __uart_disable_irda() \
  ( REG8(IRDA_BASE + OFF_SIRCR) &= ~(SIRCR_TSIRE | SIRCR_RSIRE) )
#define __uart_enable_irda() \
  /* Tx high pulse as 0, Rx low pulse as 0 */ \
  ( REG8(IRDA_BASE + OFF_SIRCR) = SIRCR_TSIRE | SIRCR_RSIRE | SIRCR_RXPL | SIRCR_TPWS )


/***************************************************************************
 * INTC
 ***************************************************************************/
#define __intc_unmask_irq(n)	( REG_INTC_IMCR = (1 << (n)) )
#define __intc_mask_irq(n)	( REG_INTC_IMSR = (1 << (n)) )
#define __intc_ack_irq(n)	( REG_INTC_IPR = (1 << (n)) )

/***************************************************************************
 * CIM
 ***************************************************************************/

#define __cim_enable()	( REG_CIM_CTRL |= CIM_CTRL_ENA )
#define __cim_disable()	( REG_CIM_CTRL &= ~CIM_CTRL_ENA )

#define __cim_input_data_inverse()	( REG_CIM_CFG |= CIM_CFG_INV_DAT )
#define __cim_input_data_normal()	( REG_CIM_CFG &= ~CIM_CFG_INV_DAT )

#define __cim_vsync_active_low()	( REG_CIM_CFG |= CIM_CFG_VSP )
#define __cim_vsync_active_high()	( REG_CIM_CFG &= ~CIM_CFG_VSP )

#define __cim_hsync_active_low()	( REG_CIM_CFG |= CIM_CFG_HSP )
#define __cim_hsync_active_high()	( REG_CIM_CFG &= ~CIM_CFG_HSP )

#define __cim_sample_data_at_pclk_falling_edge() \
  ( REG_CIM_CFG |= CIM_CFG_PCP )
#define __cim_sample_data_at_pclk_rising_edge() \
  ( REG_CIM_CFG &= ~CIM_CFG_PCP )

#define __cim_enable_dummy_zero()	( REG_CIM_CFG |= CIM_CFG_DUMMY_ZERO )
#define __cim_disable_dummy_zero()	( REG_CIM_CFG &= ~CIM_CFG_DUMMY_ZERO )

#define __cim_select_external_vsync()	( REG_CIM_CFG |= CIM_CFG_EXT_VSYNC )
#define __cim_select_internal_vsync()	( REG_CIM_CFG &= ~CIM_CFG_EXT_VSYNC )

/* n=0-7 */
#define __cim_set_data_packing_mode(n) 		\
do {						\
    REG_CIM_CFG &= ~CIM_CFG_PACK_MASK; 		\
    REG_CIM_CFG |= (CIM_CFG_PACK_##n); 		\
} while (0)

#define __cim_enable_ccir656_progressive_mode()	\
do {						\
    REG_CIM_CFG &= ~CIM_CFG_DSM_MASK; 		\
    REG_CIM_CFG |= CIM_CFG_DSM_CPM; 		\
} while (0)

#define __cim_enable_ccir656_interlace_mode()	\
do {						\
    REG_CIM_CFG &= ~CIM_CFG_DSM_MASK; 		\
    REG_CIM_CFG |= CIM_CFG_DSM_CIM; 		\
} while (0)

#define __cim_enable_gated_clock_mode()		\
do {						\
    REG_CIM_CFG &= ~CIM_CFG_DSM_MASK; 		\
    REG_CIM_CFG |= CIM_CFG_DSM_GCM; 		\
} while (0)

#define __cim_enable_nongated_clock_mode()	\
do {						\
    REG_CIM_CFG &= ~CIM_CFG_DSM_MASK; 		\
    REG_CIM_CFG |= CIM_CFG_DSM_NGCM; 		\
} while (0)

/* sclk:system bus clock
 * mclk: CIM master clock
 */
#define __cim_set_master_clk(sclk, mclk)			\
do {								\
    REG_CIM_CTRL &= ~CIM_CTRL_MCLKDIV_MASK;			\
    REG_CIM_CTRL |= (((sclk)/(mclk) - 1) << CIM_CTRL_MCLKDIV_BIT);	\
} while (0)

#define __cim_enable_sof_intr() \
  ( REG_CIM_CTRL |= CIM_CTRL_DMA_SOFM )
#define __cim_disable_sof_intr() \
  ( REG_CIM_CTRL &= ~CIM_CTRL_DMA_SOFM )

#define __cim_enable_eof_intr() \
  ( REG_CIM_CTRL |= CIM_CTRL_DMA_EOFM )
#define __cim_disable_eof_intr() \
  ( REG_CIM_CTRL &= ~CIM_CTRL_DMA_EOFM )

#define __cim_enable_stop_intr() \
  ( REG_CIM_CTRL |= CIM_CTRL_DMA_STOPM )
#define __cim_disable_stop_intr() \
  ( REG_CIM_CTRL &= ~CIM_CTRL_DMA_STOPM )

#define __cim_enable_trig_intr() \
  ( REG_CIM_CTRL |= CIM_CTRL_RXF_TRIGM )
#define __cim_disable_trig_intr() \
  ( REG_CIM_CTRL &= ~CIM_CTRL_RXF_TRIGM )

#define __cim_enable_rxfifo_overflow_intr() \
  ( REG_CIM_CTRL |= CIM_CTRL_RXF_OFM )
#define __cim_disable_rxfifo_overflow_intr() \
  ( REG_CIM_CTRL &= ~CIM_CTRL_RXF_OFM )

/* n=1-16 */
#define __cim_set_frame_rate(n) 		\
do {						\
    REG_CIM_CTRL &= ~CIM_CTRL_FRC_MASK; 	\
    REG_CIM_CTRL |= CIM_CTRL_FRC_##n; 		\
} while (0)

#define __cim_enable_dma()   ( REG_CIM_CTRL |= CIM_CTRL_DMA_EN )
#define __cim_disable_dma()  ( REG_CIM_CTRL &= ~CIM_CTRL_DMA_EN )

#define __cim_reset_rxfifo() ( REG_CIM_CTRL |= CIM_CTRL_RXF_RST )
#define __cim_unreset_rxfifo() ( REG_CIM_CTRL &= ~CIM_CTRL_RXF_RST )

/* n=4,8,12,16,20,24,28,32 */
#define __cim_set_rxfifo_trigger(n) 		\
do {						\
    REG_CIM_CTRL &= ~CIM_CTRL_RXF_TRIG_MASK; 	\
    REG_CIM_CTRL |= CIM_CTRL_RXF_TRIG_##n; 	\
} while (0)

#define __cim_clear_state()   	     ( REG_CIM_STATE = 0 )

#define __cim_disable_done()   	     ( REG_CIM_STATE & CIM_STATE_VDD )
#define __cim_rxfifo_empty()   	     ( REG_CIM_STATE & CIM_STATE_RXF_EMPTY )
#define __cim_rxfifo_reach_trigger() ( REG_CIM_STATE & CIM_STATE_RXF_TRIG )
#define __cim_rxfifo_overflow()      ( REG_CIM_STATE & CIM_STATE_RXF_OF )
#define __cim_clear_rxfifo_overflow() ( REG_CIM_STATE &= ~CIM_STATE_RXF_OF )
#define __cim_dma_stop()   	     ( REG_CIM_STATE & CIM_STATE_DMA_STOP )
#define __cim_dma_eof()   	     ( REG_CIM_STATE & CIM_STATE_DMA_EOF )
#define __cim_dma_sof()   	     ( REG_CIM_STATE & CIM_STATE_DMA_SOF )

#define __cim_get_iid()   	     ( REG_CIM_IID )
#define __cim_get_image_data()       ( REG_CIM_RXFIFO )
#define __cim_get_dam_cmd()          ( REG_CIM_CMD )

#define __cim_set_da(a)              ( REG_CIM_DA = (a) )

/***************************************************************************
 * PWM
 ***************************************************************************/

/* n is the pwm channel (0,1,..) */
#define __pwm_enable_module(n)		( REG_PWM_CTR(n) |= PWM_CTR_EN )
#define __pwm_disable_module(n)		( REG_PWM_CTR(n) &= ~PWM_CTR_EN )
#define __pwm_graceful_shutdown_mode(n)	( REG_PWM_CTR(n) &= ~PWM_CTR_SD )
#define __pwm_abrupt_shutdown_mode(n)	( REG_PWM_CTR(n) |= PWM_CTR_SD )
#define __pwm_set_full_duty(n)		( REG_PWM_DUT(n) |= PWM_DUT_FDUTY )

#define __pwm_set_prescale(n, p) \
  ( REG_PWM_CTR(n) = ((REG_PWM_CTR(n) & ~PWM_CTR_PRESCALE_MASK) | (p) ) )
#define __pwm_set_period(n, p) \
  ( REG_PWM_PER(n) = ( (REG_PWM_PER(n) & ~PWM_PER_PERIOD_MASK) | (p) ) )
#define __pwm_set_duty(n, d) \
  ( REG_PWM_DUT(n) = ( (REG_PWM_DUT(n) & ~PWM_DUT_FDUTY) | (d) ) )

/***************************************************************************
 * EMC
 ***************************************************************************/

#define __emc_enable_split() ( REG_EMC_BCR = EMC_BCR_BRE )
#define __emc_disable_split() ( REG_EMC_BCR = 0 )

#define __emc_smem_bus_width(n) /* 8, 16 or 32*/		\
	( REG_EMC_SMCR = (REG_EMC_SMCR & EMC_SMCR_BW_MASK) |	\
			 EMC_SMCR_BW_##n##BIT )
#define __emc_smem_byte_control() \
	( REG_EMC_SMCR = (REG_EMC_SMCR | EMC_SMCR_BCM )
#define __emc_normal_smem() \
	( REG_EMC_SMCR = (REG_EMC_SMCR & ~EMC_SMCR_SMT )
#define __emc_burst_smem() \
	( REG_EMC_SMCR = (REG_EMC_SMCR | EMC_SMCR_SMT )
#define __emc_smem_burstlen(n) /* 4, 8, 16 or 32 */ \
	( REG_EMC_SMCR = (REG_EMC_SMCR & EMC_SMCR_BL_MASK) | (EMC_SMCR_BL_##n )

/***************************************************************************
 * GPIO
 ***************************************************************************/

/* p is the port number (0,1,2,3)
 * o is the pin offset (0-31) inside the port
 * n is the absolute number of a pin (0-124), regardless of the port
 * m is the interrupt manner (low/high/falling/rising)
 */

#define __gpio_port_data(p)	( REG_GPIO_GPDR(p) )

#define __gpio_port_as_output(p, o)		\
do {						\
    unsigned int tmp;				\
    REG_GPIO_GPIER(p) &= ~(1 << (o));		\
    REG_GPIO_GPDIR(p) |= (1 << (o));		\
    if (o < 16) {				\
	tmp = REG_GPIO_GPALR(p);		\
	tmp &= ~(3 << ((o) << 1));		\
	REG_GPIO_GPALR(p) = tmp;		\
    } else {					\
	tmp = REG_GPIO_GPAUR(p);		\
	tmp &= ~(3 << (((o) - 16)<< 1));	\
	REG_GPIO_GPAUR(p) = tmp;		\
    }						\
} while (0)

#define __gpio_port_as_input(p, o)		\
do {						\
    unsigned int tmp;				\
    REG_GPIO_GPIER(p) &= ~(1 << (o));		\
    REG_GPIO_GPDIR(p) &= ~(1 << (o));		\
    if (o < 16) {				\
	tmp = REG_GPIO_GPALR(p);		\
	tmp &= ~(3 << ((o) << 1));		\
	REG_GPIO_GPALR(p) = tmp;		\
    } else {					\
	tmp = REG_GPIO_GPAUR(p);		\
	tmp &= ~(3 << (((o) - 16)<< 1));	\
	REG_GPIO_GPAUR(p) = tmp;		\
    }						\
} while (0)

#define __gpio_as_output(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	__gpio_port_as_output(p, o);		\
} while (0)

#define __gpio_as_input(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	__gpio_port_as_input(p, o);		\
} while (0)

#define __gpio_set_pin(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	__gpio_port_data(p) |= (1 << o);	\
} while (0)

#define __gpio_clear_pin(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	__gpio_port_data(p) &= ~(1 << o);	\
} while (0)

static __inline__ unsigned int __gpio_get_pin(unsigned int n)
{
	unsigned int p, o;
	p = (n) / 32;
	o = (n) % 32;
	if (__gpio_port_data(p) & (1 << o))
		return 1;
	else
		return 0;
}


#define __gpio_set_irq_detect_manner(p, o, m)	\
do {						\
    unsigned int tmp;				\
    if (o < 16) {				\
	tmp = REG_GPIO_GPIDLR(p);		\
	tmp &= ~(3 << ((o) << 1));		\
	tmp |= ((m) << ((o) << 1));		\
	REG_GPIO_GPIDLR(p) = tmp;		\
    } else {					\
	o -= 16;				\
	tmp = REG_GPIO_GPIDUR(p);		\
	tmp &= ~(3 << ((o) << 1));		\
	tmp |= ((m) << ((o) << 1));		\
	REG_GPIO_GPIDUR(p) = tmp;		\
    }						\
} while (0)

#define __gpio_port_as_irq(p, o, m)		\
do {						\
    __gpio_set_irq_detect_manner(p, o, m);  	\
    __gpio_port_as_input(p, o);			\
    REG_GPIO_GPIER(p) |= (1 << o);		\
} while (0)

#define __gpio_as_irq(n, m)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
        __gpio_port_as_irq(p, o, m);  		\
} while (0)


#define __gpio_as_irq_high_level(n)	__gpio_as_irq(n, GPIO_IRQ_HILEVEL)
#define __gpio_as_irq_low_level(n)	__gpio_as_irq(n, GPIO_IRQ_LOLEVEL)
#define __gpio_as_irq_fall_edge(n)	__gpio_as_irq(n, GPIO_IRQ_FALLEDG)
#define __gpio_as_irq_rise_edge(n)	__gpio_as_irq(n, GPIO_IRQ_RAISEDG)


#define __gpio_mask_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_GPIER(p) &= ~(1 << o);		\
} while (0)

#define __gpio_unmask_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_GPIER(n) |= (1 << o);		\
} while (0)

#define __gpio_ack_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_GPFR(p) |= (1 << o);		\
} while (0)


static __inline__ unsigned int __gpio_get_irq(void)
{
	unsigned int tmp, i;

	tmp = REG_GPIO_GPFR(3);
	for (i=0; i<32; i++)
		if (tmp & (1 << i))
			return 0x60 + i;
	tmp = REG_GPIO_GPFR(2);
	for (i=0; i<32; i++)
		if (tmp & (1 << i))
			return 0x40 + i;
	tmp = REG_GPIO_GPFR(1);
	for (i=0; i<32; i++)
		if (tmp & (1 << i))
			return 0x20 + i;
	tmp = REG_GPIO_GPFR(0);
	for (i=0; i<32; i++)
		if (tmp & (1 << i))
			return i;
	return 0;
}

#define __gpio_group_irq(n)			\
({						\
	register int tmp, i;			\
	tmp = REG_GPIO_GPFR((n));		\
	for (i=31;i>=0;i--)			\
		if (tmp & (1 << i))		\
			break;			\
	i;					\
})

#define __gpio_enable_pull(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_GPPUR(p) |= (1 << o);		\
} while (0)

#define __gpio_disable_pull(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_GPPUR(p) &= ~(1 << o);		\
} while (0)

/* Init the alternate function pins */


#define __gpio_as_ssi()				\
do {						\
	REG_GPIO_GPALR(2) &= 0xFC00FFFF;	\
	REG_GPIO_GPALR(2) |= 0x01550000;	\
} while (0)

#define __gpio_as_uart3()			\
do {						\
	REG_GPIO_GPAUR(0) &= 0xFFFF0000;	\
	REG_GPIO_GPAUR(0) |= 0x00005555;	\
} while (0)

#define __gpio_as_uart2()			\
do {						\
	REG_GPIO_GPALR(3) &= 0x3FFFFFFF;	\
	REG_GPIO_GPALR(3) |= 0x40000000;	\
	REG_GPIO_GPAUR(3) &= 0xF3FFFFFF;	\
	REG_GPIO_GPAUR(3) |= 0x04000000;	\
} while (0)

#define __gpio_as_uart1()			\
do {						\
	REG_GPIO_GPAUR(0) &= 0xFFF0FFFF;	\
	REG_GPIO_GPAUR(0) |= 0x00050000;	\
} while (0)

#define __gpio_as_uart0()			\
do {						\
	REG_GPIO_GPAUR(3) &= 0x0FFFFFFF;	\
	REG_GPIO_GPAUR(3) |= 0x50000000;	\
} while (0)


#define __gpio_as_scc0()			\
do {						\
	REG_GPIO_GPALR(2) &= 0xFFFFFFCC;	\
	REG_GPIO_GPALR(2) |= 0x00000011;	\
} while (0)

#define __gpio_as_scc1()			\
do {						\
	REG_GPIO_GPALR(2) &= 0xFFFFFF33;	\
	REG_GPIO_GPALR(2) |= 0x00000044;	\
} while (0)

#define __gpio_as_scc()				\
do {						\
	__gpio_as_scc0();			\
	__gpio_as_scc1();			\
} while (0)

#define __gpio_as_dma()				\
do {						\
	REG_GPIO_GPALR(0) &= 0x00FFFFFF;	\
	REG_GPIO_GPALR(0) |= 0x55000000;	\
	REG_GPIO_GPAUR(0) &= 0xFF0FFFFF;	\
	REG_GPIO_GPAUR(0) |= 0x00500000;	\
} while (0)

#define __gpio_as_msc()				\
do {						\
	REG_GPIO_GPALR(1) &= 0xFFFF000F;	\
	REG_GPIO_GPALR(1) |= 0x00005550;	\
} while (0)

#define __gpio_as_pcmcia()			\
do {						\
	REG_GPIO_GPAUR(2) &= 0xF000FFFF;	\
	REG_GPIO_GPAUR(2) |= 0x05550000;	\
} while (0)

#define __gpio_as_emc()				\
do {						\
	REG_GPIO_GPALR(2) &= 0x3FFFFFFF;	\
	REG_GPIO_GPALR(2) |= 0x40000000;	\
	REG_GPIO_GPAUR(2) &= 0xFFFF0000;	\
	REG_GPIO_GPAUR(2) |= 0x00005555;	\
} while (0)

#define __gpio_as_lcd_slave()			\
do {						\
	REG_GPIO_GPALR(1) &= 0x0000FFFF;	\
	REG_GPIO_GPALR(1) |= 0x55550000;	\
	REG_GPIO_GPAUR(1) &= 0x00000000;	\
	REG_GPIO_GPAUR(1) |= 0x55555555;	\
} while (0)

#define __gpio_as_lcd_master()			\
do {						\
	REG_GPIO_GPALR(1) &= 0x0000FFFF;	\
	REG_GPIO_GPALR(1) |= 0x55550000;	\
	REG_GPIO_GPAUR(1) &= 0x00000000;	\
	REG_GPIO_GPAUR(1) |= 0x556A5555;	\
} while (0)

#define __gpio_as_usb()				\
do {						\
	REG_GPIO_GPAUR(0) &= 0x00FFFFFF;	\
	REG_GPIO_GPAUR(0) |= 0x55000000;	\
} while (0)

#define __gpio_as_ac97()			\
do {						\
	REG_GPIO_GPALR(2) &= 0xC3FF03FF;	\
	REG_GPIO_GPALR(2) |= 0x24005400;	\
} while (0)

#define __gpio_as_i2s_slave()			\
do {						\
	REG_GPIO_GPALR(2) &= 0xC3FF0CFF;	\
	REG_GPIO_GPALR(2) |= 0x14005100;	\
} while (0)

#define __gpio_as_i2s_master()			\
do {						\
	REG_GPIO_GPALR(2) &= 0xC3FF0CFF;	\
	REG_GPIO_GPALR(2) |= 0x28005100;	\
} while (0)

#define __gpio_as_eth()				\
do {						\
	REG_GPIO_GPAUR(3) &= 0xFC000000;	\
	REG_GPIO_GPAUR(3) |= 0x01555555;	\
} while (0)

#define __gpio_as_pwm()				\
do {						\
	REG_GPIO_GPAUR(2) &= 0x0FFFFFFF;	\
	REG_GPIO_GPAUR(2) |= 0x50000000;	\
} while (0)

#define __gpio_as_ps2()				\
do {						\
	REG_GPIO_GPALR(1) &= 0xFFFFFFF0;	\
	REG_GPIO_GPALR(1) |= 0x00000005;	\
} while (0)

#define __gpio_as_uprt()			\
do {						\
	REG_GPIO_GPALR(1) &= 0x0000000F;	\
	REG_GPIO_GPALR(1) |= 0x55555550;	\
	REG_GPIO_GPALR(3) &= 0xC0000000;	\
	REG_GPIO_GPALR(3) |= 0x15555555;	\
} while (0)

#define __gpio_as_cim()				\
do {						\
	REG_GPIO_GPALR(0) &= 0xFF000000;	\
	REG_GPIO_GPALR(0) |= 0x00555555;	\
} while (0)

/***************************************************************************
 * HARB
 ***************************************************************************/

#define __harb_usb0_udc()			\
do {						\
  REG_HARB_HAPOR &= ~HARB_HAPOR_UCHSEL; 	\
} while (0)

#define __harb_usb0_uhc()			\
do {						\
  REG_HARB_HAPOR |= HARB_HAPOR_UCHSEL; 		\
} while (0)

#define __harb_set_priority(n)			\
do {						\
  REG_HARB_HAPOR = ((REG_HARB_HAPOR & ~HARB_HAPOR_PRIO_MASK) | n);	\
} while (0)

/***************************************************************************
 * I2C
 ***************************************************************************/

#define __i2c_enable()		( REG_I2C_CR |= I2C_CR_I2CE )
#define __i2c_disable()		( REG_I2C_CR &= ~I2C_CR_I2CE )

#define __i2c_send_start()	( REG_I2C_CR |= I2C_CR_STA )
#define __i2c_send_stop()	( REG_I2C_CR |= I2C_CR_STO )
#define __i2c_send_ack()	( REG_I2C_CR &= ~I2C_CR_AC )
#define __i2c_send_nack()	( REG_I2C_CR |= I2C_CR_AC )

#define __i2c_set_drf()		( REG_I2C_SR |= I2C_SR_DRF )
#define __i2c_clear_drf()	( REG_I2C_SR &= ~I2C_SR_DRF )
#define __i2c_check_drf()	( REG_I2C_SR & I2C_SR_DRF )

#define __i2c_received_ack()	( !(REG_I2C_SR & I2C_SR_ACKF) )
#define __i2c_is_busy()		( REG_I2C_SR & I2C_SR_BUSY )
#define __i2c_transmit_ended()	( REG_I2C_SR & I2C_SR_TEND )

#define __i2c_set_clk(dev_clk, i2c_clk) \
  ( REG_I2C_GR = (dev_clk) / (16*(i2c_clk)) - 1 )

#define __i2c_read()		( REG_I2C_DR )
#define __i2c_write(val)	( REG_I2C_DR = (val) )

/***************************************************************************
 * UDC
 ***************************************************************************/

#define __udc_set_16bit_phy()		( REG_UDC_DevCFGR |= UDC_DevCFGR_PI )
#define __udc_set_8bit_phy()		( REG_UDC_DevCFGR &= ~UDC_DevCFGR_PI )

#define __udc_enable_sync_frame()	( REG_UDC_DevCFGR |= UDC_DevCFGR_SS )
#define __udc_disable_sync_frame()	( REG_UDC_DevCFGR &= ~UDC_DevCFGR_SS )

#define __udc_self_powered()		( REG_UDC_DevCFGR |= UDC_DevCFGR_SP )
#define __udc_bus_powered()		( REG_UDC_DevCFGR &= ~UDC_DevCFGR_SP )

#define __udc_enable_remote_wakeup()	( REG_UDC_DevCFGR |= UDC_DevCFGR_RW )
#define __udc_disable_remote_wakeup()	( REG_UDC_DevCFGR &= ~UDC_DevCFGR_RW )

#define __udc_set_speed_high()				\
do {							\
	REG_UDC_DevCFGR &= ~UDC_DevCFGR_SPD_MASK;	\
	REG_UDC_DevCFGR |= UDC_DevCFGR_SPD_HS;		\
} while (0)

#define __udc_set_speed_full()				\
do {							\
	REG_UDC_DevCFGR &= ~UDC_DevCFGR_SPD_MASK;	\
	REG_UDC_DevCFGR |= UDC_DevCFGR_SPD_FS;		\
} while (0)

#define __udc_set_speed_low()				\
do {							\
	REG_UDC_DevCFGR &= ~UDC_DevCFGR_SPD_MASK;	\
	REG_UDC_DevCFGR |= UDC_DevCFGR_SPD_LS;		\
} while (0)


#define __udc_set_dma_mode()		( REG_UDC_DevCR |= UDC_DevCR_DM )
#define __udc_set_slave_mode()		( REG_UDC_DevCR &= ~UDC_DevCR_DM )
#define __udc_set_big_endian()		( REG_UDC_DevCR |= UDC_DevCR_BE )
#define __udc_set_little_endian()	( REG_UDC_DevCR &= ~UDC_DevCR_BE )
#define __udc_generate_resume()		( REG_UDC_DevCR |= UDC_DevCR_RES )
#define __udc_clear_resume()		( REG_UDC_DevCR &= ~UDC_DevCR_RES )


#define __udc_get_enumarated_speed()	( REG_UDC_DevSR & UDC_DevSR_ENUMSPD_MASK )
#define __udc_suspend_detected()	( REG_UDC_DevSR & UDC_DevSR_SUSP )
#define __udc_get_alternate_setting()	( (REG_UDC_DevSR & UDC_DevSR_ALT_MASK) >> UDC_DevSR_ALT_BIT )
#define __udc_get_interface_number()	( (REG_UDC_DevSR & UDC_DevSR_INTF_MASK) >> UDC_DevSR_INTF_BIT )
#define __udc_get_config_number()	( (REG_UDC_DevSR & UDC_DevSR_CFG_MASK) >> UDC_DevSR_CFG_BIT )


#define __udc_sof_detected(r)		( (r) & UDC_DevIntR_SOF )
#define __udc_usb_suspend_detected(r)	( (r) & UDC_DevIntR_US )
#define __udc_usb_reset_detected(r)	( (r) & UDC_DevIntR_UR )
#define __udc_set_interface_detected(r)	( (r) & UDC_DevIntR_SI )
#define __udc_set_config_detected(r)	( (r) & UDC_DevIntR_SC )

#define __udc_clear_sof()		( REG_UDC_DevIntR |= UDC_DevIntR_SOF )
#define __udc_clear_usb_suspend()	( REG_UDC_DevIntR |= UDC_DevIntR_US )
#define __udc_clear_usb_reset()		( REG_UDC_DevIntR |= UDC_DevIntR_UR )
#define __udc_clear_set_interface()	( REG_UDC_DevIntR |= UDC_DevIntR_SI )
#define __udc_clear_set_config()	( REG_UDC_DevIntR |= UDC_DevIntR_SC )

#define __udc_mask_sof()		( REG_UDC_DevIntMR |= UDC_DevIntR_SOF )
#define __udc_mask_usb_suspend()	( REG_UDC_DevIntMR |= UDC_DevIntR_US )
#define __udc_mask_usb_reset()		( REG_UDC_DevIntMR |= UDC_DevIntR_UR )
#define __udc_mask_set_interface()	( REG_UDC_DevIntMR |= UDC_DevIntR_SI )
#define __udc_mask_set_config()		( REG_UDC_DevIntMR |= UDC_DevIntR_SC )
#define __udc_mask_all_dev_intrs() \
  ( REG_UDC_DevIntMR = UDC_DevIntR_SOF | UDC_DevIntR_US | \
      UDC_DevIntR_UR | UDC_DevIntR_SI | UDC_DevIntR_SC )

#define __udc_unmask_sof()		( REG_UDC_DevIntMR &= ~UDC_DevIntR_SOF )
#define __udc_unmask_usb_suspend()	( REG_UDC_DevIntMR &= ~UDC_DevIntR_US )
#define __udc_unmask_usb_reset()	( REG_UDC_DevIntMR &= ~UDC_DevIntR_UR )
#define __udc_unmask_set_interface()	( REG_UDC_DevIntMR &= ~UDC_DevIntR_SI )
#define __udc_unmask_set_config()	( REG_UDC_DevIntMR &= ~UDC_DevIntR_SC )
#if 0
#define __udc_unmask_all_dev_intrs() \
  ( REG_UDC_DevIntMR = ~(UDC_DevIntR_SOF | UDC_DevIntR_US | \
      UDC_DevIntR_UR | UDC_DevIntR_SI | UDC_DevIntR_SC) )
#else
#define __udc_unmask_all_dev_intrs() \
  ( REG_UDC_DevIntMR = 0x00000000 )
#endif


#define __udc_ep0out_irq_detected(epintr) \
  ( (((epintr) & UDC_EPIntR_OUTEP_MASK) >> (UDC_EPIntR_OUTEP_BIT + 0)) & 0x1 )
#define __udc_ep5out_irq_detected(epintr) \
  ( (((epintr) & UDC_EPIntR_OUTEP_MASK) >> (UDC_EPIntR_OUTEP_BIT + 5)) & 0x1 )
#define __udc_ep6out_irq_detected(epintr) \
  ( (((epintr) & UDC_EPIntR_OUTEP_MASK) >> (UDC_EPIntR_OUTEP_BIT + 6)) & 0x1 )
#define __udc_ep7out_irq_detected(epintr) \
  ( (((epintr) & UDC_EPIntR_OUTEP_MASK) >> (UDC_EPIntR_OUTEP_BIT + 7)) & 0x1 )

#define __udc_ep0in_irq_detected(epintr) \
  ( (((epintr) & UDC_EPIntR_INEP_MASK) >> (UDC_EPIntR_INEP_BIT + 0)) & 0x1 )
#define __udc_ep1in_irq_detected(epintr) \
  ( (((epintr) & UDC_EPIntR_INEP_MASK) >> (UDC_EPIntR_INEP_BIT + 1)) & 0x1 )
#define __udc_ep2in_irq_detected(epintr) \
  ( (((epintr) & UDC_EPIntR_INEP_MASK) >> (UDC_EPIntR_INEP_BIT + 2)) & 0x1 )
#define __udc_ep3in_irq_detected(epintr) \
  ( (((epintr) & UDC_EPIntR_INEP_MASK) >> (UDC_EPIntR_INEP_BIT + 3)) & 0x1 )
#define __udc_ep4in_irq_detected(epintr) \
  ( (((epintr) & UDC_EPIntR_INEP_MASK) >> (UDC_EPIntR_INEP_BIT + 4)) & 0x1 )


#define __udc_mask_ep0out_irq() \
  ( REG_UDC_EPIntMR |= (1 << (UDC_EPIntMR_OUTEP_BIT + 0)) )
#define __udc_mask_ep5out_irq() \
  ( REG_UDC_EPIntMR |= (1 << (UDC_EPIntMR_OUTEP_BIT + 5)) )
#define __udc_mask_ep6out_irq() \
  ( REG_UDC_EPIntMR |= (1 << (UDC_EPIntMR_OUTEP_BIT + 6)) )
#define __udc_mask_ep7out_irq() \
  ( REG_UDC_EPIntMR |= (1 << (UDC_EPIntMR_OUTEP_BIT + 7)) )

#define __udc_unmask_ep0out_irq() \
  ( REG_UDC_EPIntMR &= ~(1 << (UDC_EPIntMR_OUTEP_BIT + 0)) )
#define __udc_unmask_ep5out_irq() \
  ( REG_UDC_EPIntMR &= ~(1 << (UDC_EPIntMR_OUTEP_BIT + 5)) )
#define __udc_unmask_ep6out_irq() \
  ( REG_UDC_EPIntMR &= ~(1 << (UDC_EPIntMR_OUTEP_BIT + 6)) )
#define __udc_unmask_ep7out_irq() \
  ( REG_UDC_EPIntMR &= ~(1 << (UDC_EPIntMR_OUTEP_BIT + 7)) )

#define __udc_mask_ep0in_irq() \
  ( REG_UDC_EPIntMR |= (1 << (UDC_EPIntMR_INEP_BIT + 0)) )
#define __udc_mask_ep1in_irq() \
  ( REG_UDC_EPIntMR |= (1 << (UDC_EPIntMR_INEP_BIT + 1)) )
#define __udc_mask_ep2in_irq() \
  ( REG_UDC_EPIntMR |= (1 << (UDC_EPIntMR_INEP_BIT + 2)) )
#define __udc_mask_ep3in_irq() \
  ( REG_UDC_EPIntMR |= (1 << (UDC_EPIntMR_INEP_BIT + 3)) )
#define __udc_mask_ep4in_irq() \
  ( REG_UDC_EPIntMR |= (1 << (UDC_EPIntMR_INEP_BIT + 4)) )

#define __udc_unmask_ep0in_irq() \
  ( REG_UDC_EPIntMR &= ~(1 << (UDC_EPIntMR_INEP_BIT + 0)) )
#define __udc_unmask_ep1in_irq() \
  ( REG_UDC_EPIntMR &= ~(1 << (UDC_EPIntMR_INEP_BIT + 1)) )
#define __udc_unmask_ep2in_irq() \
  ( REG_UDC_EPIntMR &= ~(1 << (UDC_EPIntMR_INEP_BIT + 2)) )
#define __udc_unmask_ep3in_irq() \
  ( REG_UDC_EPIntMR &= ~(1 << (UDC_EPIntMR_INEP_BIT + 3)) )
#define __udc_unmask_ep4in_irq() \
  ( REG_UDC_EPIntMR &= ~(1 << (UDC_EPIntMR_INEP_BIT + 4)) )

#define __udc_mask_all_ep_intrs() \
  ( REG_UDC_EPIntMR = 0xffffffff )
#define __udc_unmask_all_ep_intrs() \
  ( REG_UDC_EPIntMR = 0x00000000 )


/* ep0 only CTRL, ep1 only INTR, ep2/3/5/6 only BULK, ep4/7 only ISO */
#define __udc_config_endpoint_type()						\
do {										\
  REG_UDC_EP0InCR = (REG_UDC_EP0InCR & ~UDC_EPCR_ET_MASK) | UDC_EPCR_ET_CTRL;	\
  REG_UDC_EP0OutCR = (REG_UDC_EP0OutCR & ~UDC_EPCR_ET_MASK) | UDC_EPCR_ET_CTRL;	\
  REG_UDC_EP1InCR = (REG_UDC_EP1InCR & ~UDC_EPCR_ET_MASK) | UDC_EPCR_ET_INTR;	\
  REG_UDC_EP2InCR = (REG_UDC_EP2InCR & ~UDC_EPCR_ET_MASK) | UDC_EPCR_ET_BULK;	\
  REG_UDC_EP3InCR = (REG_UDC_EP3InCR & ~UDC_EPCR_ET_MASK) | UDC_EPCR_ET_BULK;	\
  REG_UDC_EP4InCR = (REG_UDC_EP4InCR & ~UDC_EPCR_ET_MASK) | UDC_EPCR_ET_ISO;	\
  REG_UDC_EP5OutCR = (REG_UDC_EP5OutCR & ~UDC_EPCR_ET_MASK) | UDC_EPCR_ET_BULK;	\
  REG_UDC_EP6OutCR = (REG_UDC_EP6OutCR & ~UDC_EPCR_ET_MASK) | UDC_EPCR_ET_BULK;	\
  REG_UDC_EP7OutCR = (REG_UDC_EP7OutCR & ~UDC_EPCR_ET_MASK) | UDC_EPCR_ET_ISO;	\
} while (0)

#define __udc_enable_ep0out_snoop_mode()  ( REG_UDC_EP0OutCR |= UDC_EPCR_SN )
#define __udc_enable_ep5out_snoop_mode()  ( REG_UDC_EP5OutCR |= UDC_EPCR_SN )
#define __udc_enable_ep6out_snoop_mode()  ( REG_UDC_EP6OutCR |= UDC_EPCR_SN )
#define __udc_enable_ep7out_snoop_mode()  ( REG_UDC_EP7OutCR |= UDC_EPCR_SN )

#define __udc_disable_ep0out_snoop_mode() ( REG_UDC_EP0OutCR &= ~UDC_EPCR_SN )
#define __udc_disable_ep5out_snoop_mode() ( REG_UDC_EP5OutCR &= ~UDC_EPCR_SN )
#define __udc_disable_ep6out_snoop_mode() ( REG_UDC_EP6OutCR &= ~UDC_EPCR_SN )
#define __udc_disable_ep7out_snoop_mode() ( REG_UDC_EP7OutCR &= ~UDC_EPCR_SN )

#define __udc_flush_ep0in_fifo()  ( REG_UDC_EP0InCR |= UDC_EPCR_F )
#define __udc_flush_ep1in_fifo()  ( REG_UDC_EP1InCR |= UDC_EPCR_F )
#define __udc_flush_ep2in_fifo()  ( REG_UDC_EP2InCR |= UDC_EPCR_F )
#define __udc_flush_ep3in_fifo()  ( REG_UDC_EP3InCR |= UDC_EPCR_F )
#define __udc_flush_ep4in_fifo()  ( REG_UDC_EP4InCR |= UDC_EPCR_F )

#define __udc_unflush_ep0in_fifo()  ( REG_UDC_EP0InCR &= ~UDC_EPCR_F )
#define __udc_unflush_ep1in_fifo()  ( REG_UDC_EP1InCR &= ~UDC_EPCR_F )
#define __udc_unflush_ep2in_fifo()  ( REG_UDC_EP2InCR &= ~UDC_EPCR_F )
#define __udc_unflush_ep3in_fifo()  ( REG_UDC_EP3InCR &= ~UDC_EPCR_F )
#define __udc_unflush_ep4in_fifo()  ( REG_UDC_EP4InCR &= ~UDC_EPCR_F )

#define __udc_enable_ep0in_stall()  ( REG_UDC_EP0InCR |= UDC_EPCR_S )
#define __udc_enable_ep0out_stall() ( REG_UDC_EP0OutCR |= UDC_EPCR_S )
#define __udc_enable_ep1in_stall()  ( REG_UDC_EP1InCR |= UDC_EPCR_S )
#define __udc_enable_ep2in_stall()  ( REG_UDC_EP2InCR |= UDC_EPCR_S )
#define __udc_enable_ep3in_stall()  ( REG_UDC_EP3InCR |= UDC_EPCR_S )
#define __udc_enable_ep4in_stall()  ( REG_UDC_EP4InCR |= UDC_EPCR_S )
#define __udc_enable_ep5out_stall() ( REG_UDC_EP5OutCR |= UDC_EPCR_S )
#define __udc_enable_ep6out_stall() ( REG_UDC_EP6OutCR |= UDC_EPCR_S )
#define __udc_enable_ep7out_stall() ( REG_UDC_EP7OutCR |= UDC_EPCR_S )

#define __udc_disable_ep0in_stall()  ( REG_UDC_EP0InCR &= ~UDC_EPCR_S )
#define __udc_disable_ep0out_stall() ( REG_UDC_EP0OutCR &= ~UDC_EPCR_S )
#define __udc_disable_ep1in_stall()  ( REG_UDC_EP1InCR &= ~UDC_EPCR_S )
#define __udc_disable_ep2in_stall()  ( REG_UDC_EP2InCR &= ~UDC_EPCR_S )
#define __udc_disable_ep3in_stall()  ( REG_UDC_EP3InCR &= ~UDC_EPCR_S )
#define __udc_disable_ep4in_stall()  ( REG_UDC_EP4InCR &= ~UDC_EPCR_S )
#define __udc_disable_ep5out_stall() ( REG_UDC_EP5OutCR &= ~UDC_EPCR_S )
#define __udc_disable_ep6out_stall() ( REG_UDC_EP6OutCR &= ~UDC_EPCR_S )
#define __udc_disable_ep7out_stall() ( REG_UDC_EP7OutCR &= ~UDC_EPCR_S )


#define __udc_ep0out_packet_size() \
  ( (REG_UDC_EP0OutSR & UDC_EPSR_RXPKTSIZE_MASK) >> UDC_EPSR_RXPKTSIZE_BIT )
#define __udc_ep5out_packet_size() \
  ( (REG_UDC_EP5OutSR & UDC_EPSR_RXPKTSIZE_MASK) >> UDC_EPSR_RXPKTSIZE_BIT )
#define __udc_ep6out_packet_size() \
  ( (REG_UDC_EP6OutSR & UDC_EPSR_RXPKTSIZE_MASK) >> UDC_EPSR_RXPKTSIZE_BIT )
#define __udc_ep7out_packet_size() \
  ( (REG_UDC_EP7OutSR & UDC_EPSR_RXPKTSIZE_MASK) >> UDC_EPSR_RXPKTSIZE_BIT )

#define __udc_ep0in_received_intoken()   ( (REG_UDC_EP0InSR & UDC_EPSR_IN) )
#define __udc_ep1in_received_intoken()   ( (REG_UDC_EP1InSR & UDC_EPSR_IN) )
#define __udc_ep2in_received_intoken()   ( (REG_UDC_EP2InSR & UDC_EPSR_IN) )
#define __udc_ep3in_received_intoken()   ( (REG_UDC_EP3InSR & UDC_EPSR_IN) )
#define __udc_ep4in_received_intoken()   ( (REG_UDC_EP4InSR & UDC_EPSR_IN) )

#define __udc_ep0out_received_none() \
  ( (REG_UDC_EP0OutSR & UDC_EPSR_OUT_MASK) == UDC_EPSR_OUT_NONE )
#define __udc_ep0out_received_data() \
  ( (REG_UDC_EP0OutSR & UDC_EPSR_OUT_MASK) == UDC_EPSR_OUT_RCVDATA )
#define __udc_ep0out_received_setup() \
  ( (REG_UDC_EP0OutSR & UDC_EPSR_OUT_MASK) == UDC_EPSR_OUT_RCVSETUP )

#define __udc_ep5out_received_none() \
  ( (REG_UDC_EP5OutSR & UDC_EPSR_OUT_MASK) == UDC_EPSR_OUT_NONE )
#define __udc_ep5out_received_data() \
  ( (REG_UDC_EP5OutSR & UDC_EPSR_OUT_MASK) == UDC_EPSR_OUT_RCVDATA )
#define __udc_ep5out_received_setup() \
  ( (REG_UDC_EP5OutSR & UDC_EPSR_OUT_MASK) == UDC_EPSR_OUT_RCVSETUP )

#define __udc_ep6out_received_none() \
  ( (REG_UDC_EP6OutSR & UDC_EPSR_OUT_MASK) == UDC_EPSR_OUT_NONE )
#define __udc_ep6out_received_data() \
  ( (REG_UDC_EP6OutSR & UDC_EPSR_OUT_MASK) == UDC_EPSR_OUT_RCVDATA )
#define __udc_ep6out_received_setup() \
  ( (REG_UDC_EP6OutSR & UDC_EPSR_OUT_MASK) == UDC_EPSR_OUT_RCVSETUP )

#define __udc_ep7out_received_none() \
  ( (REG_UDC_EP7OutSR & UDC_EPSR_OUT_MASK) == UDC_EPSR_OUT_NONE )
#define __udc_ep7out_received_data() \
  ( (REG_UDC_EP7OutSR & UDC_EPSR_OUT_MASK) == UDC_EPSR_OUT_RCVDATA )
#define __udc_ep7out_received_setup() \
  ( (REG_UDC_EP7OutSR & UDC_EPSR_OUT_MASK) == UDC_EPSR_OUT_RCVSETUP )

/* ep7out ISO only */
#define __udc_ep7out_get_pid() \
  ( (REG_UDC_EP7OutSR & UDC_EPSR_PID_MASK) >> UDC_EPSR_PID_BIT )


#define __udc_ep0in_set_buffer_size(n) ( REG_UDC_EP0InBSR = (n) )
#define __udc_ep1in_set_buffer_size(n) ( REG_UDC_EP1InBSR = (n) )
#define __udc_ep2in_set_buffer_size(n) ( REG_UDC_EP2InBSR = (n) )
#define __udc_ep3in_set_buffer_size(n) ( REG_UDC_EP3InBSR = (n) )
#define __udc_ep4in_set_buffer_size(n) ( REG_UDC_EP4InBSR = (n) )

#define __udc_ep0out_get_frame_number(n) ( UDC_EP0OutPFNR )
#define __udc_ep5out_get_frame_number(n) ( UDC_EP5OutPFNR )
#define __udc_ep6out_get_frame_number(n) ( UDC_EP6OutPFNR )
#define __udc_ep7out_get_frame_number(n) ( UDC_EP7OutPFNR )


#define __udc_ep0in_set_max_packet_size(n)  ( REG_UDC_EP0InMPSR = (n) )
#define __udc_ep0out_set_max_packet_size(n) ( REG_UDC_EP0OutMPSR = (n) )
#define __udc_ep1in_set_max_packet_size(n)  ( REG_UDC_EP1InMPSR = (n) )
#define __udc_ep2in_set_max_packet_size(n)  ( REG_UDC_EP2InMPSR = (n) )
#define __udc_ep3in_set_max_packet_size(n)  ( REG_UDC_EP3InMPSR = (n) )
#define __udc_ep4in_set_max_packet_size(n)  ( REG_UDC_EP4InMPSR = (n) )
#define __udc_ep5out_set_max_packet_size(n) ( REG_UDC_EP5OutMPSR = (n) )
#define __udc_ep6out_set_max_packet_size(n) ( REG_UDC_EP6OutMPSR = (n) )
#define __udc_ep7out_set_max_packet_size(n) ( REG_UDC_EP7OutMPSR = (n) )

/* set to 0xFFFF for UDC */
#define __udc_set_setup_command_address(n)  ( REG_UDC_STCMAR = (n) )

/* Init and configure EPxInfR(x=0,1,2,3,4,5,6,7)
 * c: Configuration number to which this endpoint belongs
 * i: Interface number to which this endpoint belongs
 * a: Alternate setting to which this endpoint belongs
 * p: max Packet size of this endpoint
 */

#define __udc_ep0info_init(c,i,a,p) 			\
do { 							\
  REG_UDC_EP0InfR &= ~UDC_EPInfR_MPS_MASK; 		\
  REG_UDC_EP0InfR |= ((p) << UDC_EPInfR_MPS_BIT); 	\
  REG_UDC_EP0InfR &= ~UDC_EPInfR_ALTS_MASK; 		\
  REG_UDC_EP0InfR |= ((a) << UDC_EPInfR_ALTS_BIT); 	\
  REG_UDC_EP0InfR &= ~UDC_EPInfR_IFN_MASK; 		\
  REG_UDC_EP0InfR |= ((i) << UDC_EPInfR_IFN_BIT); 	\
  REG_UDC_EP0InfR &= ~UDC_EPInfR_CGN_MASK; 		\
  REG_UDC_EP0InfR |= ((c) << UDC_EPInfR_CGN_BIT); 	\
  REG_UDC_EP0InfR &= ~UDC_EPInfR_EPT_MASK; 		\
  REG_UDC_EP0InfR |= UDC_EPInfR_EPT_CTRL; 		\
  REG_UDC_EP0InfR &= ~UDC_EPInfR_EPD; 			\
  REG_UDC_EP0InfR |= UDC_EPInfR_EPD_OUT; 		\
  REG_UDC_EP0InfR &= ~UDC_EPInfR_EPN_MASK;		\
  REG_UDC_EP0InfR |= (0 << UDC_EPInfR_EPN_BIT);		\
} while (0)

#define __udc_ep1info_init(c,i,a,p) 			\
do { 							\
  REG_UDC_EP1InfR &= ~UDC_EPInfR_MPS_MASK; 		\
  REG_UDC_EP1InfR |= ((p) << UDC_EPInfR_MPS_BIT); 	\
  REG_UDC_EP1InfR &= ~UDC_EPInfR_ALTS_MASK; 		\
  REG_UDC_EP1InfR |= ((a) << UDC_EPInfR_ALTS_BIT); 	\
  REG_UDC_EP1InfR &= ~UDC_EPInfR_IFN_MASK; 		\
  REG_UDC_EP1InfR |= ((i) << UDC_EPInfR_IFN_BIT); 	\
  REG_UDC_EP1InfR &= ~UDC_EPInfR_CGN_MASK; 		\
  REG_UDC_EP1InfR |= ((c) << UDC_EPInfR_CGN_BIT); 	\
  REG_UDC_EP1InfR &= ~UDC_EPInfR_EPT_MASK; 		\
  REG_UDC_EP1InfR |= UDC_EPInfR_EPT_INTR; 		\
  REG_UDC_EP1InfR &= ~UDC_EPInfR_EPD; 			\
  REG_UDC_EP1InfR |= UDC_EPInfR_EPD_IN; 		\
  REG_UDC_EP1InfR &= ~UDC_EPInfR_EPN_MASK;		\
  REG_UDC_EP1InfR |= (1 << UDC_EPInfR_EPN_BIT);		\
} while (0)

#define __udc_ep2info_init(c,i,a,p) 			\
do { 							\
  REG_UDC_EP2InfR &= ~UDC_EPInfR_MPS_MASK; 		\
  REG_UDC_EP2InfR |= ((p) << UDC_EPInfR_MPS_BIT); 	\
  REG_UDC_EP2InfR &= ~UDC_EPInfR_ALTS_MASK; 		\
  REG_UDC_EP2InfR |= ((a) << UDC_EPInfR_ALTS_BIT); 	\
  REG_UDC_EP2InfR &= ~UDC_EPInfR_IFN_MASK; 		\
  REG_UDC_EP2InfR |= ((i) << UDC_EPInfR_IFN_BIT); 	\
  REG_UDC_EP2InfR &= ~UDC_EPInfR_CGN_MASK; 		\
  REG_UDC_EP2InfR |= ((c) << UDC_EPInfR_CGN_BIT); 	\
  REG_UDC_EP2InfR &= ~UDC_EPInfR_EPT_MASK; 		\
  REG_UDC_EP2InfR |= UDC_EPInfR_EPT_BULK; 		\
  REG_UDC_EP2InfR &= ~UDC_EPInfR_EPD; 			\
  REG_UDC_EP2InfR |= UDC_EPInfR_EPD_IN; 		\
  REG_UDC_EP2InfR &= ~UDC_EPInfR_EPN_MASK;		\
  REG_UDC_EP2InfR |= (2 << UDC_EPInfR_EPN_BIT);		\
} while (0)

#define __udc_ep3info_init(c,i,a,p) 			\
do { 							\
  REG_UDC_EP3InfR &= ~UDC_EPInfR_MPS_MASK; 		\
  REG_UDC_EP3InfR |= ((p) << UDC_EPInfR_MPS_BIT); 	\
  REG_UDC_EP3InfR &= ~UDC_EPInfR_ALTS_MASK; 		\
  REG_UDC_EP3InfR |= ((a) << UDC_EPInfR_ALTS_BIT); 	\
  REG_UDC_EP3InfR &= ~UDC_EPInfR_IFN_MASK; 		\
  REG_UDC_EP3InfR |= ((i) << UDC_EPInfR_IFN_BIT); 	\
  REG_UDC_EP3InfR &= ~UDC_EPInfR_CGN_MASK; 		\
  REG_UDC_EP3InfR |= ((c) << UDC_EPInfR_CGN_BIT); 	\
  REG_UDC_EP3InfR &= ~UDC_EPInfR_EPT_MASK; 		\
  REG_UDC_EP3InfR |= UDC_EPInfR_EPT_BULK; 		\
  REG_UDC_EP3InfR &= ~UDC_EPInfR_EPD; 			\
  REG_UDC_EP3InfR |= UDC_EPInfR_EPD_IN; 		\
  REG_UDC_EP3InfR &= ~UDC_EPInfR_EPN_MASK;		\
  REG_UDC_EP3InfR |= (3 << UDC_EPInfR_EPN_BIT);		\
} while (0)

#define __udc_ep4info_init(c,i,a,p) 			\
do { 							\
  REG_UDC_EP4InfR &= ~UDC_EPInfR_MPS_MASK; 		\
  REG_UDC_EP4InfR |= ((p) << UDC_EPInfR_MPS_BIT); 	\
  REG_UDC_EP4InfR &= ~UDC_EPInfR_ALTS_MASK; 		\
  REG_UDC_EP4InfR |= ((a) << UDC_EPInfR_ALTS_BIT); 	\
  REG_UDC_EP4InfR &= ~UDC_EPInfR_IFN_MASK; 		\
  REG_UDC_EP4InfR |= ((i) << UDC_EPInfR_IFN_BIT); 	\
  REG_UDC_EP4InfR &= ~UDC_EPInfR_CGN_MASK; 		\
  REG_UDC_EP4InfR |= ((c) << UDC_EPInfR_CGN_BIT); 	\
  REG_UDC_EP4InfR &= ~UDC_EPInfR_EPT_MASK; 		\
  REG_UDC_EP4InfR |= UDC_EPInfR_EPT_ISO; 		\
  REG_UDC_EP4InfR &= ~UDC_EPInfR_EPD; 			\
  REG_UDC_EP4InfR |= UDC_EPInfR_EPD_IN; 		\
  REG_UDC_EP4InfR &= ~UDC_EPInfR_EPN_MASK;		\
  REG_UDC_EP4InfR |= (4 << UDC_EPInfR_EPN_BIT);		\
} while (0)

#define __udc_ep5info_init(c,i,a,p) 			\
do { 							\
  REG_UDC_EP5InfR &= ~UDC_EPInfR_MPS_MASK; 		\
  REG_UDC_EP5InfR |= ((p) << UDC_EPInfR_MPS_BIT); 	\
  REG_UDC_EP5InfR &= ~UDC_EPInfR_ALTS_MASK; 		\
  REG_UDC_EP5InfR |= ((a) << UDC_EPInfR_ALTS_BIT); 	\
  REG_UDC_EP5InfR &= ~UDC_EPInfR_IFN_MASK; 		\
  REG_UDC_EP5InfR |= ((i) << UDC_EPInfR_IFN_BIT); 	\
  REG_UDC_EP5InfR &= ~UDC_EPInfR_CGN_MASK; 		\
  REG_UDC_EP5InfR |= ((c) << UDC_EPInfR_CGN_BIT); 	\
  REG_UDC_EP5InfR &= ~UDC_EPInfR_EPT_MASK; 		\
  REG_UDC_EP5InfR |= UDC_EPInfR_EPT_BULK; 		\
  REG_UDC_EP5InfR &= ~UDC_EPInfR_EPD; 			\
  REG_UDC_EP5InfR |= UDC_EPInfR_EPD_OUT; 		\
  REG_UDC_EP5InfR &= ~UDC_EPInfR_EPN_MASK;		\
  REG_UDC_EP5InfR |= (5 << UDC_EPInfR_EPN_BIT);		\
} while (0)

#define __udc_ep6info_init(c,i,a,p) 			\
do { 							\
  REG_UDC_EP6InfR &= ~UDC_EPInfR_MPS_MASK; 		\
  REG_UDC_EP6InfR |= ((p) << UDC_EPInfR_MPS_BIT); 	\
  REG_UDC_EP6InfR &= ~UDC_EPInfR_ALTS_MASK; 		\
  REG_UDC_EP6InfR |= ((a) << UDC_EPInfR_ALTS_BIT); 	\
  REG_UDC_EP6InfR &= ~UDC_EPInfR_IFN_MASK; 		\
  REG_UDC_EP6InfR |= ((i) << UDC_EPInfR_IFN_BIT); 	\
  REG_UDC_EP6InfR &= ~UDC_EPInfR_CGN_MASK; 		\
  REG_UDC_EP6InfR |= ((c) << UDC_EPInfR_CGN_BIT); 	\
  REG_UDC_EP6InfR &= ~UDC_EPInfR_EPT_MASK; 		\
  REG_UDC_EP6InfR |= UDC_EPInfR_EPT_BULK; 		\
  REG_UDC_EP6InfR &= ~UDC_EPInfR_EPD; 			\
  REG_UDC_EP6InfR |= UDC_EPInfR_EPD_OUT; 		\
  REG_UDC_EP6InfR &= ~UDC_EPInfR_EPN_MASK;		\
  REG_UDC_EP6InfR |= (6 << UDC_EPInfR_EPN_BIT);		\
} while (0)

#define __udc_ep7info_init(c,i,a,p) 			\
do { 							\
  REG_UDC_EP7InfR &= ~UDC_EPInfR_MPS_MASK; 		\
  REG_UDC_EP7InfR |= ((p) << UDC_EPInfR_MPS_BIT); 	\
  REG_UDC_EP7InfR &= ~UDC_EPInfR_ALTS_MASK; 		\
  REG_UDC_EP7InfR |= ((a) << UDC_EPInfR_ALTS_BIT); 	\
  REG_UDC_EP7InfR &= ~UDC_EPInfR_IFN_MASK; 		\
  REG_UDC_EP7InfR |= ((i) << UDC_EPInfR_IFN_BIT); 	\
  REG_UDC_EP7InfR &= ~UDC_EPInfR_CGN_MASK; 		\
  REG_UDC_EP7InfR |= ((c) << UDC_EPInfR_CGN_BIT); 	\
  REG_UDC_EP7InfR &= ~UDC_EPInfR_EPT_MASK; 		\
  REG_UDC_EP7InfR |= UDC_EPInfR_EPT_ISO; 		\
  REG_UDC_EP7InfR &= ~UDC_EPInfR_EPD; 			\
  REG_UDC_EP7InfR |= UDC_EPInfR_EPD_OUT; 		\
  REG_UDC_EP7InfR &= ~UDC_EPInfR_EPN_MASK;		\
  REG_UDC_EP7InfR |= (7 << UDC_EPInfR_EPN_BIT);		\
} while (0)


/***************************************************************************
 * DMAC
 ***************************************************************************/

/* n is the DMA channel (0 - 7) */

#define __dmac_enable_all_channels() \
  ( REG_DMAC_DMACR |= DMAC_DMACR_DME | DMAC_DMACR_PR_ROUNDROBIN )
#define __dmac_disable_all_channels() \
  ( REG_DMAC_DMACR &= ~DMAC_DMACR_DME )

/* p=0,1,2,3 */
#define __dmac_set_priority(p) 				\
do {							\
	REG_DMAC_DMACR &= ~DMAC_DMACR_PR_MASK;		\
	REG_DMAC_DMACR |= ((p) << DMAC_DMACR_PR_BIT);	\
} while (0)

#define __dmac_test_halt_error() ( REG_DMAC_DMACR & DMAC_DMACR_HTR )
#define __dmac_test_addr_error() ( REG_DMAC_DMACR & DMAC_DMACR_AER )

#define __dmac_enable_channel(n) \
  ( REG_DMAC_DCCSR(n) |= DMAC_DCCSR_CHDE )
#define __dmac_disable_channel(n) \
  ( REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_CHDE )
#define __dmac_channel_enabled(n) \
  ( REG_DMAC_DCCSR(n) & DMAC_DCCSR_CHDE )

#define __dmac_channel_enable_irq(n) \
  ( REG_DMAC_DCCSR(n) |= DMAC_DCCSR_TCIE )
#define __dmac_channel_disable_irq(n) \
  ( REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_TCIE )

#define __dmac_channel_transmit_halt_detected(n) \
  (  REG_DMAC_DCCSR(n) & DMAC_DCCSR_HLT )
#define __dmac_channel_transmit_end_detected(n) \
  (  REG_DMAC_DCCSR(n) & DMAC_DCCSR_TC )
#define __dmac_channel_address_error_detected(n) \
  (  REG_DMAC_DCCSR(n) & DMAC_DCCSR_AR )

#define __dmac_channel_clear_transmit_halt(n) \
  (  REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_HLT )
#define __dmac_channel_clear_transmit_end(n) \
  (  REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_TC )
#define __dmac_channel_clear_address_error(n) \
  (  REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_AR )

#define __dmac_channel_set_single_mode(n) \
  (  REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_TM )
#define __dmac_channel_set_block_mode(n) \
  (  REG_DMAC_DCCSR(n) |= DMAC_DCCSR_TM )

#define __dmac_channel_set_transfer_unit_32bit(n)	\
do {							\
	REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_DS_MASK;	\
	REG_DMAC_DCCSR(n) |= DMAC_DCCSR_DS_32b;		\
} while (0)

#define __dmac_channel_set_transfer_unit_16bit(n)	\
do {							\
	REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_DS_MASK;	\
	REG_DMAC_DCCSR(n) |= DMAC_DCCSR_DS_16b;		\
} while (0)

#define __dmac_channel_set_transfer_unit_8bit(n)	\
do {							\
	REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_DS_MASK;	\
	REG_DMAC_DCCSR(n) |= DMAC_DCCSR_DS_8b;		\
} while (0)

#define __dmac_channel_set_transfer_unit_16byte(n)	\
do {							\
	REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_DS_MASK;	\
	REG_DMAC_DCCSR(n) |= DMAC_DCCSR_DS_16B;		\
} while (0)

#define __dmac_channel_set_transfer_unit_32byte(n)	\
do {							\
	REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_DS_MASK;	\
	REG_DMAC_DCCSR(n) |= DMAC_DCCSR_DS_32B;		\
} while (0)

/* w=8,16,32 */
#define __dmac_channel_set_dest_port_width(n,w)		\
do {							\
	REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_DWDH_MASK;	\
	REG_DMAC_DCCSR(n) |= DMAC_DCCSR_DWDH_##w;	\
} while (0)

/* w=8,16,32 */
#define __dmac_channel_set_src_port_width(n,w)		\
do {							\
	REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_SWDH_MASK;	\
	REG_DMAC_DCCSR(n) |= DMAC_DCCSR_SWDH_##w;	\
} while (0)

/* v=0-15 */
#define __dmac_channel_set_rdil(n,v)				\
do {								\
	REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_RDIL_MASK;		\
	REG_DMAC_DCCSR(n) |= ((v) << DMAC_DCCSR_RDIL_BIT);	\
} while (0)

#define __dmac_channel_dest_addr_fixed(n) \
  (  REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_DAM )
#define __dmac_channel_dest_addr_increment(n) \
  (  REG_DMAC_DCCSR(n) |= DMAC_DCCSR_DAM )

#define __dmac_channel_src_addr_fixed(n) \
  (  REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_SAM )
#define __dmac_channel_src_addr_increment(n) \
  (  REG_DMAC_DCCSR(n) |= DMAC_DCCSR_SAM )

#define __dmac_channel_set_eop_high(n) \
  (  REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_EOPM )
#define __dmac_channel_set_eop_low(n) \
  (  REG_DMAC_DCCSR(n) |= DMAC_DCCSR_EOPM )

#define __dmac_channel_set_erdm(n,m)				\
do {								\
	REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_SWDH_MASK;		\
	REG_DMAC_DCCSR(n) |= ((m) << DMAC_DCCSR_ERDM_BIT);	\
} while (0)

#define __dmac_channel_set_eackm(n) \
  ( REG_DMAC_DCCSR(n) |= DMAC_DCCSR_EACKM )
#define __dmac_channel_clear_eackm(n) \
  ( REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_EACKM )

#define __dmac_channel_set_eacks(n) \
  ( REG_DMAC_DCCSR(n) |= DMAC_DCCSR_EACKS )
#define __dmac_channel_clear_eacks(n) \
  ( REG_DMAC_DCCSR(n) &= ~DMAC_DCCSR_EACKS )


#define __dmac_channel_irq_detected(n) \
  ( REG_DMAC_DCCSR(n) & (DMAC_DCCSR_TC | DMAC_DCCSR_AR) )

static __inline__ int __dmac_get_irq(void)
{
	int i;
	for (i=0;i<NUM_DMA;i++)
		if (__dmac_channel_irq_detected(i))
			return i;
	return -1;
}

/***************************************************************************
 * AIC (AC'97 & I2S Controller)
 ***************************************************************************/

#define __aic_enable()		( REG_AIC_FR |= AIC_FR_ENB )
#define __aic_disable()		( REG_AIC_FR &= ~AIC_FR_ENB )
#define __aic_reset()		( REG_AIC_FR |= AIC_FR_RST )
#define __aic_select_ac97()	( REG_AIC_FR &= ~AIC_FR_AUSEL )
#define __aic_select_i2s()	( REG_AIC_FR |= AIC_FR_AUSEL )

#define __i2s_as_master()	( REG_AIC_FR |= AIC_FR_BCKD | AIC_FR_SYNCD )
#define __i2s_as_slave()	( REG_AIC_FR &= ~(AIC_FR_BCKD | AIC_FR_SYNCD) )

#define __aic_set_transmit_trigger(n) 			\
do {							\
	REG_AIC_FR &= ~AIC_FR_TFTH_MASK;		\
	REG_AIC_FR |= ((n) << AIC_FR_TFTH_BIT);		\
} while(0)

#define __aic_set_receive_trigger(n) 			\
do {							\
	REG_AIC_FR &= ~AIC_FR_RFTH_MASK;		\
	REG_AIC_FR |= ((n) << AIC_FR_RFTH_BIT);		\
} while(0)

#define __aic_enable_record()	( REG_AIC_CR |= AIC_CR_EREC )
#define __aic_disable_record()	( REG_AIC_CR &= ~AIC_CR_EREC )
#define __aic_enable_replay()	( REG_AIC_CR |= AIC_CR_ERPL )
#define __aic_disable_replay()	( REG_AIC_CR &= ~AIC_CR_ERPL )
#define __aic_enable_loopback()	( REG_AIC_CR |= AIC_CR_ENLBF )
#define __aic_disable_loopback() ( REG_AIC_CR &= ~AIC_CR_ENLBF )

#define __aic_flush_fifo()	( REG_AIC_CR |= AIC_CR_FLUSH )
#define __aic_unflush_fifo()	( REG_AIC_CR &= ~AIC_CR_FLUSH )

#define __aic_enable_transmit_intr() \
  ( REG_AIC_CR |= (AIC_CR_ETFS | AIC_CR_ETUR) )
#define __aic_disable_transmit_intr() \
  ( REG_AIC_CR &= ~(AIC_CR_ETFS | AIC_CR_ETUR) )
#define __aic_enable_receive_intr() \
  ( REG_AIC_CR |= (AIC_CR_ERFS | AIC_CR_EROR) )
#define __aic_disable_receive_intr() \
  ( REG_AIC_CR &= ~(AIC_CR_ERFS | AIC_CR_EROR) )

#define __aic_enable_transmit_dma()  ( REG_AIC_CR |= AIC_CR_TDMS )
#define __aic_disable_transmit_dma() ( REG_AIC_CR &= ~AIC_CR_TDMS )
#define __aic_enable_receive_dma()   ( REG_AIC_CR |= AIC_CR_RDMS )
#define __aic_disable_receive_dma()  ( REG_AIC_CR &= ~AIC_CR_RDMS )

#define AC97_PCM_XS_L_FRONT   	AIC_ACCR1_XS_SLOT3
#define AC97_PCM_XS_R_FRONT   	AIC_ACCR1_XS_SLOT4
#define AC97_PCM_XS_CENTER    	AIC_ACCR1_XS_SLOT6
#define AC97_PCM_XS_L_SURR    	AIC_ACCR1_XS_SLOT7
#define AC97_PCM_XS_R_SURR    	AIC_ACCR1_XS_SLOT8
#define AC97_PCM_XS_LFE       	AIC_ACCR1_XS_SLOT9

#define AC97_PCM_RS_L_FRONT   	AIC_ACCR1_RS_SLOT3
#define AC97_PCM_RS_R_FRONT   	AIC_ACCR1_RS_SLOT4
#define AC97_PCM_RS_CENTER    	AIC_ACCR1_RS_SLOT6
#define AC97_PCM_RS_L_SURR    	AIC_ACCR1_RS_SLOT7
#define AC97_PCM_RS_R_SURR    	AIC_ACCR1_RS_SLOT8
#define AC97_PCM_RS_LFE       	AIC_ACCR1_RS_SLOT9

#define __ac97_set_xs_none()	( REG_AIC_ACCR1 &= ~AIC_ACCR1_XS_MASK )
#define __ac97_set_xs_mono() 						\
do {									\
	REG_AIC_ACCR1 &= ~AIC_ACCR1_XS_MASK;				\
	REG_AIC_ACCR1 |= AC97_PCM_XS_R_FRONT;				\
} while(0)
#define __ac97_set_xs_stereo() 						\
do {									\
	REG_AIC_ACCR1 &= ~AIC_ACCR1_XS_MASK;				\
	REG_AIC_ACCR1 |= AC97_PCM_XS_L_FRONT | AC97_PCM_XS_R_FRONT;	\
} while(0)

/* In fact, only stereo is support now. */ 
#define __ac97_set_rs_none()	( REG_AIC_ACCR1 &= ~AIC_ACCR1_RS_MASK )
#define __ac97_set_rs_mono() 						\
do {									\
	REG_AIC_ACCR1 &= ~AIC_ACCR1_RS_MASK;				\
	REG_AIC_ACCR1 |= AC97_PCM_RS_R_FRONT;				\
} while(0)
#define __ac97_set_rs_stereo() 						\
do {									\
	REG_AIC_ACCR1 &= ~AIC_ACCR1_RS_MASK;				\
	REG_AIC_ACCR1 |= AC97_PCM_RS_L_FRONT | AC97_PCM_RS_R_FRONT;	\
} while(0)

#define __ac97_warm_reset_codec()		\
 do {						\
	REG_AIC_ACCR2 |= AIC_ACCR2_SA;		\
	REG_AIC_ACCR2 |= AIC_ACCR2_SS;		\
	udelay(1);				\
	REG_AIC_ACCR2 &= ~AIC_ACCR2_SS;		\
	REG_AIC_ACCR2 &= ~AIC_ACCR2_SA;		\
 } while (0)

//#define Jz_AC97_RESET_BUG 1
#ifndef Jz_AC97_RESET_BUG
#define __ac97_cold_reset_codec()		\
 do {						\
	REG_AIC_ACCR2 |= AIC_ACCR2_SA;		\
	REG_AIC_ACCR2 &= ~AIC_ACCR2_SS;		\
	REG_AIC_ACCR2 |=  AIC_ACCR2_SR;		\
	udelay(1);				\
	REG_AIC_ACCR2 &= ~AIC_ACCR2_SR;		\
	REG_AIC_ACCR2 &= ~AIC_ACCR2_SA;		\
 } while (0)
#else
#define __ac97_cold_reset_codec()		\
 do {						\
        __gpio_as_output(111); /* SDATA_OUT */	\
        __gpio_as_output(110); /* SDATA_IN */	\
        __gpio_as_output(112); /* SYNC */	\
        __gpio_as_output(114); /* RESET# */	\
	__gpio_clear_pin(111);			\
	__gpio_clear_pin(110);			\
	__gpio_clear_pin(112);			\
	__gpio_clear_pin(114);			\
	udelay(2);				\
	__gpio_set_pin(114);			\
	udelay(1);				\
	__gpio_as_ac97();			\
 } while (0)
#endif

/* n=8,16,18,20 */
#define __ac97_set_iass(n) \
 ( REG_AIC_ACCR2 = (REG_AIC_ACCR2 & ~AIC_ACCR2_IASS_MASK) | AIC_ACCR2_IASS_##n##BIT )
#define __ac97_set_oass(n) \
 ( REG_AIC_ACCR2 = (REG_AIC_ACCR2 & ~AIC_ACCR2_OASS_MASK) | AIC_ACCR2_OASS_##n##BIT )

#define __i2s_select_i2s()            ( REG_AIC_I2SCR &= ~AIC_I2SCR_AMSL )
#define __i2s_select_left_justified() ( REG_AIC_I2SCR |= AIC_I2SCR_AMSL )

/* n=8,16,18,20,24 */
#define __i2s_set_sample_size(n) \
 ( REG_AIC_I2SCR |= (REG_AIC_I2SCR & ~AIC_I2SCR_WL_MASK) | AIC_I2SCR_WL_##n##BIT )

#define __i2s_stop_clock()   ( REG_AIC_I2SCR |= AIC_I2SCR_STPBK )
#define __i2s_start_clock()  ( REG_AIC_I2SCR &= ~AIC_I2SCR_STPBK )

#define __aic_transmit_request()  ( REG_AIC_SR & AIC_SR_TFS )
#define __aic_receive_request()   ( REG_AIC_SR & AIC_SR_RFS )
#define __aic_transmit_underrun() ( REG_AIC_SR & AIC_SR_TUR )
#define __aic_receive_overrun()   ( REG_AIC_SR & AIC_SR_ROR )

#define __aic_clear_errors()      ( REG_AIC_SR &= ~(AIC_SR_TUR | AIC_SR_ROR) )

#define __aic_get_transmit_resident() \
  ( (REG_AIC_SR & AIC_SR_TFL_MASK) >> AIC_SR_TFL_BIT )
#define __aic_get_receive_count() \
  ( (REG_AIC_SR & AIC_SR_RFL_MASK) >> AIC_SR_RFL_BIT )

#define __ac97_command_transmitted()     ( REG_AIC_ACSR & AIC_ACSR_CADT )
#define __ac97_status_received()         ( REG_AIC_ACSR & AIC_ACSR_SADR )
#define __ac97_status_receive_timeout()  ( REG_AIC_ACSR & AIC_ACSR_RSTO )
#define __ac97_codec_is_low_power_mode() ( REG_AIC_ACSR & AIC_ACSR_CLPM )
#define __ac97_codec_is_ready()          ( REG_AIC_ACSR & AIC_ACSR_CRDY )

#define __i2s_is_busy() ( REG_AIC_I2SSR & AIC_I2SSR_BSY )

#define CODEC_READ_CMD	        (1 << 19)
#define CODEC_WRITE_CMD	        (0 << 19)
#define CODEC_REG_INDEX_BIT     12
#define CODEC_REG_INDEX_MASK	(0x7f << CODEC_REG_INDEX_BIT)	/* 18:12 */
#define CODEC_REG_DATA_BIT      4
#define CODEC_REG_DATA_MASK	(0x0ffff << 4)	/* 19:4 */

#define __ac97_out_rcmd_addr(reg) 					\
do { 									\
    REG_AIC_ACCAR = CODEC_READ_CMD | ((reg) << CODEC_REG_INDEX_BIT); 	\
} while (0)

#define __ac97_out_wcmd_addr(reg) 					\
do { 									\
    REG_AIC_ACCAR = CODEC_WRITE_CMD | ((reg) << CODEC_REG_INDEX_BIT); 	\
} while (0)

#define __ac97_out_data(value) 						\
do { 									\
    REG_AIC_ACCDR = ((value) << CODEC_REG_DATA_BIT); 			\
} while (0)

#define __ac97_in_data() \
 ( (REG_AIC_ACSDR & CODEC_REG_DATA_MASK) >> CODEC_REG_DATA_BIT )

#define __ac97_in_status_addr() \
 ( (REG_AIC_ACSAR & CODEC_REG_INDEX_MASK) >> CODEC_REG_INDEX_BIT )

#define __i2s_set_sample_rate(i2sclk, sync) \
  ( REG_AIC_I2SDIV = ((i2sclk) / (4*64)) / (sync) )

#define __aic_write_tfifo(v)  ( REG_AIC_DR = (v) )
#define __aic_read_rfifo()    ( REG_AIC_DR )

//
// Define next ops for AC97 compatible
//

#define AC97_ACSR	AIC_ACSR

#define __ac97_enable()		__aic_enable(); __aic_select_ac97()
#define __ac97_disable()	__aic_disable()
#define __ac97_reset()		__aic_reset()

#define __ac97_set_transmit_trigger(n)	__aic_set_transmit_trigger(n)
#define __ac97_set_receive_trigger(n)	__aic_set_receive_trigger(n)

#define __ac97_enable_record()		__aic_enable_record()
#define __ac97_disable_record()		__aic_disable_record()
#define __ac97_enable_replay()		__aic_enable_replay()
#define __ac97_disable_replay()		__aic_disable_replay()
#define __ac97_enable_loopback()	__aic_enable_loopback()
#define __ac97_disable_loopback()	__aic_disable_loopback()

#define __ac97_enable_transmit_dma()	__aic_enable_transmit_dma()
#define __ac97_disable_transmit_dma()	__aic_disable_transmit_dma()
#define __ac97_enable_receive_dma()	__aic_enable_receive_dma()
#define __ac97_disable_receive_dma()	__aic_disable_receive_dma()

#define __ac97_transmit_request()	__aic_transmit_request()
#define __ac97_receive_request()	__aic_receive_request()
#define __ac97_transmit_underrun()	__aic_transmit_underrun()
#define __ac97_receive_overrun()	__aic_receive_overrun()

#define __ac97_clear_errors()		__aic_clear_errors()

#define __ac97_get_transmit_resident()	__aic_get_transmit_resident()
#define __ac97_get_receive_count()	__aic_get_receive_count()

#define __ac97_enable_transmit_intr()	__aic_enable_transmit_intr()
#define __ac97_disable_transmit_intr()	__aic_disable_transmit_intr()
#define __ac97_enable_receive_intr()	__aic_enable_receive_intr()
#define __ac97_disable_receive_intr()	__aic_disable_receive_intr()

#define __ac97_write_tfifo(v)		__aic_write_tfifo(v)
#define __ac97_read_rfifo()		__aic_read_rfifo()

//
// Define next ops for I2S compatible
//

#define I2S_ACSR	AIC_I2SSR

#define __i2s_enable()		 __aic_enable(); __aic_select_i2s()
#define __i2s_disable()		__aic_disable()
#define __i2s_reset()		__aic_reset()

#define __i2s_set_transmit_trigger(n)	__aic_set_transmit_trigger(n)
#define __i2s_set_receive_trigger(n)	__aic_set_receive_trigger(n)

#define __i2s_enable_record()		__aic_enable_record()
#define __i2s_disable_record()		__aic_disable_record()
#define __i2s_enable_replay()		__aic_enable_replay()
#define __i2s_disable_replay()		__aic_disable_replay()
#define __i2s_enable_loopback()		__aic_enable_loopback()
#define __i2s_disable_loopback()	__aic_disable_loopback()

#define __i2s_enable_transmit_dma()	__aic_enable_transmit_dma()
#define __i2s_disable_transmit_dma()	__aic_disable_transmit_dma()
#define __i2s_enable_receive_dma()	__aic_enable_receive_dma()
#define __i2s_disable_receive_dma()	__aic_disable_receive_dma()

#define __i2s_transmit_request()	__aic_transmit_request()
#define __i2s_receive_request()		__aic_receive_request()
#define __i2s_transmit_underrun()	__aic_transmit_underrun()
#define __i2s_receive_overrun()		__aic_receive_overrun()

#define __i2s_clear_errors()		__aic_clear_errors()

#define __i2s_get_transmit_resident()	__aic_get_transmit_resident()
#define __i2s_get_receive_count()	__aic_get_receive_count()

#define __i2s_enable_transmit_intr()	__aic_enable_transmit_intr()
#define __i2s_disable_transmit_intr()	__aic_disable_transmit_intr()
#define __i2s_enable_receive_intr()	__aic_enable_receive_intr()
#define __i2s_disable_receive_intr()	__aic_disable_receive_intr()

#define __i2s_write_tfifo(v)		__aic_write_tfifo(v)
#define __i2s_read_rfifo()		__aic_read_rfifo()

#define __i2s_reset_codec()			\
 do {						\
        __gpio_as_output(111); /* SDATA_OUT */	\
        __gpio_as_input(110); /* SDATA_IN */	\
        __gpio_as_output(112); /* SYNC */	\
        __gpio_as_output(114); /* RESET# */	\
	__gpio_clear_pin(111);			\
	__gpio_clear_pin(110);			\
	__gpio_clear_pin(112);			\
	__gpio_clear_pin(114);			\
        __gpio_as_i2s_master();			\
 } while (0)


/***************************************************************************
 * LCD
 ***************************************************************************/

#define __lcd_set_dis()			( REG_LCD_CTRL |= LCD_CTRL_DIS )
#define __lcd_clr_dis()			( REG_LCD_CTRL &= ~LCD_CTRL_DIS )

#define __lcd_set_ena()			( REG_LCD_CTRL |= LCD_CTRL_ENA )
#define __lcd_clr_ena()			( REG_LCD_CTRL &= ~LCD_CTRL_ENA )

/* n=1,2,4,8,16 */
#define __lcd_set_bpp(n) \
  ( REG_LCD_CTRL = (REG_LCD_CTRL & ~LCD_CTRL_BPP_MASK) | LCD_CTRL_BPP_##n )

/* n=4,8,16 */
#define __lcd_set_burst_length(n) 		\
do {						\
	REG_LCD_CTRL &= ~LCD_CTRL_BST_MASK;	\
	REG_LCD_CTRL |= LCD_CTRL_BST_n##;	\
} while (0)

#define __lcd_select_rgb565()		( REG_LCD_CTRL &= ~LCD_CTRL_RGB555 )
#define __lcd_select_rgb555()		( REG_LCD_CTRL |= LCD_CTRL_RGB555 )

#define __lcd_set_ofup()		( REG_LCD_CTRL |= LCD_CTRL_OFUP )
#define __lcd_clr_ofup()		( REG_LCD_CTRL &= ~LCD_CTRL_OFUP )

/* n=2,4,16 */
#define __lcd_set_stn_frc(n) 			\
do {						\
	REG_LCD_CTRL &= ~LCD_CTRL_FRC_MASK;	\
	REG_LCD_CTRL |= LCD_CTRL_FRC_n##;	\
} while (0)


#define __lcd_pixel_endian_little()	( REG_LCD_CTRL |= LCD_CTRL_PEDN )
#define __lcd_pixel_endian_big()	( REG_LCD_CTRL &= ~LCD_CTRL_PEDN )

#define __lcd_reverse_byte_endian()	( REG_LCD_CTRL |= LCD_CTRL_BEDN )
#define __lcd_normal_byte_endian()	( REG_LCD_CTRL &= ~LCD_CTRL_BEDN )

#define __lcd_enable_eof_intr()		( REG_LCD_CTRL |= LCD_CTRL_EOFM )
#define __lcd_disable_eof_intr()	( REG_LCD_CTRL &= ~LCD_CTRL_EOFM )

#define __lcd_enable_sof_intr()		( REG_LCD_CTRL |= LCD_CTRL_SOFM )
#define __lcd_disable_sof_intr()	( REG_LCD_CTRL &= ~LCD_CTRL_SOFM )

#define __lcd_enable_ofu_intr()		( REG_LCD_CTRL |= LCD_CTRL_OFUM )
#define __lcd_disable_ofu_intr()	( REG_LCD_CTRL &= ~LCD_CTRL_OFUM )

#define __lcd_enable_ifu0_intr()	( REG_LCD_CTRL |= LCD_CTRL_IFUM0 )
#define __lcd_disable_ifu0_intr()	( REG_LCD_CTRL &= ~LCD_CTRL_IFUM0 )

#define __lcd_enable_ifu1_intr()	( REG_LCD_CTRL |= LCD_CTRL_IFUM1 )
#define __lcd_disable_ifu1_intr()	( REG_LCD_CTRL &= ~LCD_CTRL_IFUM1 )

#define __lcd_enable_ldd_intr()		( REG_LCD_CTRL |= LCD_CTRL_LDDM )
#define __lcd_disable_ldd_intr()	( REG_LCD_CTRL &= ~LCD_CTRL_LDDM )

#define __lcd_enable_qd_intr()		( REG_LCD_CTRL |= LCD_CTRL_QDM )
#define __lcd_disable_qd_intr()		( REG_LCD_CTRL &= ~LCD_CTRL_QDM )


/* LCD status register indication */

#define __lcd_quick_disable_done()	( REG_LCD_STATE & LCD_STATE_QD )
#define __lcd_disable_done()		( REG_LCD_STATE & LCD_STATE_LDD )
#define __lcd_infifo0_underrun()	( REG_LCD_STATE & LCD_STATE_IFU0 )
#define __lcd_infifo1_underrun()	( REG_LCD_STATE & LCD_STATE_IFU1 )
#define __lcd_outfifo_underrun()	( REG_LCD_STATE & LCD_STATE_OFU )
#define __lcd_start_of_frame()		( REG_LCD_STATE & LCD_STATE_SOF )
#define __lcd_end_of_frame()		( REG_LCD_STATE & LCD_STATE_EOF )

#define __lcd_clr_outfifounderrun()	( REG_LCD_STATE &= ~LCD_STATE_OFU )
#define __lcd_clr_sof()			( REG_LCD_STATE &= ~LCD_STATE_SOF )
#define __lcd_clr_eof()			( REG_LCD_STATE &= ~LCD_STATE_EOF )

#define __lcd_panel_white()		( REG_LCD_DEV |= LCD_DEV_WHITE )
#define __lcd_panel_black()		( REG_LCD_DEV &= ~LCD_DEV_WHITE )

/* n=1,2,4,8 for single mono-STN 
 * n=4,8 for dual mono-STN
 */
#define __lcd_set_panel_datawidth(n) 		\
do { 						\
	REG_LCD_DEV &= ~LCD_DEV_PDW_MASK; 	\
	REG_LCD_DEV |= LCD_DEV_PDW_n##;		\
} while (0)

/* m=LCD_DEV_MODE_GENERUIC_TFT_xxx */
#define __lcd_set_panel_mode(m) 		\
do {						\
	REG_LCD_DEV &= ~LCD_DEV_MODE_MASK;	\
	REG_LCD_DEV |= (m);			\
} while(0)

/* n = 0-255 */
#define __lcd_disable_ac_bias()		( REG_LCD_IO = 0xff )
#define __lcd_set_ac_bias(n) 			\
do {						\
	REG_LCD_IO &= ~LCD_IO_ACB_MASK;		\
	REG_LCD_IO |= ((n) << LCD_IO_ACB_BIT);	\
} while(0)

#define __lcd_io_set_dir()		( REG_LCD_IO |= LCD_IO_DIR )
#define __lcd_io_clr_dir()		( REG_LCD_IO &= ~LCD_IO_DIR )

#define __lcd_io_set_dep()		( REG_LCD_IO |= LCD_IO_DEP )
#define __lcd_io_clr_dep()		( REG_LCD_IO &= ~LCD_IO_DEP )

#define __lcd_io_set_vsp()		( REG_LCD_IO |= LCD_IO_VSP )
#define __lcd_io_clr_vsp()		( REG_LCD_IO &= ~LCD_IO_VSP )

#define __lcd_io_set_hsp()		( REG_LCD_IO |= LCD_IO_HSP )
#define __lcd_io_clr_hsp()		( REG_LCD_IO &= ~LCD_IO_HSP )

#define __lcd_io_set_pcp()		( REG_LCD_IO |= LCD_IO_PCP )
#define __lcd_io_clr_pcp()		( REG_LCD_IO &= ~LCD_IO_PCP )

#define __lcd_vsync_get_vps() \
  ( (REG_LCD_VSYNC & LCD_VSYNC_VPS_MASK) >> LCD_VSYNC_VPS_BIT )

#define __lcd_vsync_get_vpe() \
  ( (REG_LCD_VSYNC & LCD_VSYNC_VPE_MASK) >> LCD_VSYNC_VPE_BIT )
#define __lcd_vsync_set_vpe(n) 				\
do {							\
	REG_LCD_VSYNC &= ~LCD_VSYNC_VPE_MASK;		\
	REG_LCD_VSYNC |= (n) << LCD_VSYNC_VPE_BIT;	\
} while (0)

#define __lcd_hsync_get_hps() \
  ( (REG_LCD_HSYNC & LCD_HSYNC_HPS_MASK) >> LCD_HSYNC_HPS_BIT )
#define __lcd_hsync_set_hps(n) 				\
do {							\
	REG_LCD_HSYNC &= ~LCD_HSYNC_HPS_MASK;		\
	REG_LCD_HSYNC |= (n) << LCD_HSYNC_HPS_BIT;	\
} while (0)

#define __lcd_hsync_get_hpe() \
  ( (REG_LCD_HSYNC & LCD_HSYNC_HPE_MASK) >> LCD_VSYNC_HPE_BIT )
#define __lcd_hsync_set_hpe(n) 				\
do {							\
	REG_LCD_HSYNC &= ~LCD_HSYNC_HPE_MASK;		\
	REG_LCD_HSYNC |= (n) << LCD_HSYNC_HPE_BIT;	\
} while (0)

#define __lcd_vat_get_ht() \
  ( (REG_LCD_VAT & LCD_VAT_HT_MASK) >> LCD_VAT_HT_BIT )
#define __lcd_vat_set_ht(n) 				\
do {							\
	REG_LCD_VAT &= ~LCD_VAT_HT_MASK;		\
	REG_LCD_VAT |= (n) << LCD_VAT_HT_BIT;		\
} while (0)

#define __lcd_vat_get_vt() \
  ( (REG_LCD_VAT & LCD_VAT_VT_MASK) >> LCD_VAT_VT_BIT )
#define __lcd_vat_set_vt(n) 				\
do {							\
	REG_LCD_VAT &= ~LCD_VAT_VT_MASK;		\
	REG_LCD_VAT |= (n) << LCD_VAT_VT_BIT;		\
} while (0)

#define __lcd_dah_get_hds() \
  ( (REG_LCD_DAH & LCD_DAH_HDS_MASK) >> LCD_DAH_HDS_BIT )
#define __lcd_dah_set_hds(n) 				\
do {							\
	REG_LCD_DAH &= ~LCD_DAH_HDS_MASK;		\
	REG_LCD_DAH |= (n) << LCD_DAH_HDS_BIT;		\
} while (0)

#define __lcd_dah_get_hde() \
  ( (REG_LCD_DAH & LCD_DAH_HDE_MASK) >> LCD_DAH_HDE_BIT )
#define __lcd_dah_set_hde(n) 				\
do {							\
	REG_LCD_DAH &= ~LCD_DAH_HDE_MASK;		\
	REG_LCD_DAH |= (n) << LCD_DAH_HDE_BIT;		\
} while (0)

#define __lcd_dav_get_vds() \
  ( (REG_LCD_DAV & LCD_DAV_VDS_MASK) >> LCD_DAV_VDS_BIT )
#define __lcd_dav_set_vds(n) 				\
do {							\
	REG_LCD_DAV &= ~LCD_DAV_VDS_MASK;		\
	REG_LCD_DAV |= (n) << LCD_DAV_VDS_BIT;		\
} while (0)

#define __lcd_dav_get_vde() \
  ( (REG_LCD_DAV & LCD_DAV_VDE_MASK) >> LCD_DAV_VDE_BIT )
#define __lcd_dav_set_vde(n) 				\
do {							\
	REG_LCD_DAV &= ~LCD_DAV_VDE_MASK;		\
	REG_LCD_DAV |= (n) << LCD_DAV_VDE_BIT;		\
} while (0)

#define __lcd_cmd0_set_sofint()		( REG_LCD_CMD0 |= LCD_CMD_SOFINT )
#define __lcd_cmd0_clr_sofint()		( REG_LCD_CMD0 &= ~LCD_CMD_SOFINT )
#define __lcd_cmd1_set_sofint()		( REG_LCD_CMD1 |= LCD_CMD_SOFINT )
#define __lcd_cmd1_clr_sofint()		( REG_LCD_CMD1 &= ~LCD_CMD_SOFINT )

#define __lcd_cmd0_set_eofint()		( REG_LCD_CMD0 |= LCD_CMD_EOFINT )
#define __lcd_cmd0_clr_eofint()		( REG_LCD_CMD0 &= ~LCD_CMD_EOFINT )
#define __lcd_cmd1_set_eofint()		( REG_LCD_CMD1 |= LCD_CMD_EOFINT )
#define __lcd_cmd1_clr_eofint()		( REG_LCD_CMD1 &= ~LCD_CMD_EOFINT )

#define __lcd_cmd0_set_pal()		( REG_LCD_CMD0 |= LCD_CMD_PAL )
#define __lcd_cmd0_clr_pal()		( REG_LCD_CMD0 &= ~LCD_CMD_PAL )

#define __lcd_cmd0_get_len() \
  ( (REG_LCD_CMD0 & LCD_CMD_LEN_MASK) >> LCD_CMD_LEN_BIT )
#define __lcd_cmd1_get_len() \
  ( (REG_LCD_CMD1 & LCD_CMD_LEN_MASK) >> LCD_CMD_LEN_BIT )



/***************************************************************************
 * DES
 ***************************************************************************/


/***************************************************************************
 * CPM
 ***************************************************************************/
#define __cpm_plcr1_fd() \
	((REG_CPM_PLCR1 & CPM_PLCR1_PLL1FD_MASK) >> CPM_PLCR1_PLL1FD_BIT)
#define __cpm_plcr1_rd() \
	((REG_CPM_PLCR1 & CPM_PLCR1_PLL1RD_MASK) >> CPM_PLCR1_PLL1RD_BIT)
#define __cpm_plcr1_od() \
	((REG_CPM_PLCR1 & CPM_PLCR1_PLL1OD_MASK) >> CPM_PLCR1_PLL1OD_BIT)
#define __cpm_cfcr_mfr() \
	((REG_CPM_CFCR & CPM_CFCR_MFR_MASK) >> CPM_CFCR_MFR_BIT)
#define __cpm_cfcr_pfr() \
	((REG_CPM_CFCR & CPM_CFCR_PFR_MASK) >> CPM_CFCR_PFR_BIT)
#define __cpm_cfcr_sfr() \
	((REG_CPM_CFCR & CPM_CFCR_SFR_MASK) >> CPM_CFCR_SFR_BIT)
#define __cpm_cfcr_ifr() \
	((REG_CPM_CFCR & CPM_CFCR_IFR_MASK) >> CPM_CFCR_IFR_BIT)

static __inline__ unsigned int __cpm_divisor_encode(unsigned int n)
{
	unsigned int encode[10] = {1,2,3,4,6,8,12,16,24,32};
	int i;
	for (i=0;i<10;i++)
		if (n < encode[i])
			break;
	return i;
}

#define __cpm_set_mclk_div(n) \
do { \
	REG_CPM_CFCR = (REG_CPM_CFCR & ~CPM_CFCR_MFR_MASK) | \
		       ((n) << (CPM_CFCR_MFR_BIT)); \
} while (0)

#define __cpm_set_pclk_div(n) \
do { \
	REG_CPM_CFCR = (REG_CPM_CFCR & ~CPM_CFCR_PFR_MASK) | \
		       ((n) << (CPM_CFCR_PFR_BIT)); \
} while (0)

#define __cpm_set_sclk_div(n) \
do { \
	REG_CPM_CFCR = (REG_CPM_CFCR & ~CPM_CFCR_SFR_MASK) | \
		       ((n) << (CPM_CFCR_SFR_BIT)); \
} while (0)

#define __cpm_set_iclk_div(n) \
do { \
	REG_CPM_CFCR = (REG_CPM_CFCR & ~CPM_CFCR_IFR_MASK) | \
		       ((n) << (CPM_CFCR_IFR_BIT)); \
} while (0)

#define __cpm_set_lcdclk_div(n) \
do { \
	REG_CPM_CFCR = (REG_CPM_CFCR & ~CPM_CFCR_LFR_MASK) | \
		       ((n) << (CPM_CFCR_LFR_BIT)); \
} while (0)

#define __cpm_enable_cko1()  (REG_CPM_CFCR |= CPM_CFCR_CKOEN1)
#define __cpm_enable_cko2()  (REG_CPM_CFCR |= CPM_CFCR_CKOEN2)
#define __cpm_disable_cko1()  (REG_CPM_CFCR &= ~CPM_CFCR_CKOEN1)
#define __cpm_disable_cko2()  (REG_CPM_CFCR &= ~CPM_CFCR_CKOEN2)

#define __cpm_idle_mode()					\
	(REG_CPM_LPCR = (REG_CPM_LPCR & ~CPM_LPCR_LPM_MASK) |	\
			CPM_LPCR_LPM_IDLE)
#define __cpm_sleep_mode()					\
	(REG_CPM_LPCR = (REG_CPM_LPCR & ~CPM_LPCR_LPM_MASK) |	\
			CPM_LPCR_LPM_SLEEP)
#define __cpm_hibernate_mode()					\
	(REG_CPM_LPCR = (REG_CPM_LPCR & ~CPM_LPCR_LPM_MASK) |	\
			CPM_LPCR_LPM_HIBERNATE)

#define __cpm_start_uart0() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_UART0))
#define __cpm_start_uart1() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_UART1))
#define __cpm_start_uart2() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_UART2))
#define __cpm_start_uart3() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_UART3))
#define __cpm_start_ost() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_OST))
#define __cpm_start_dmac() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_DMAC))
#define __cpm_start_uhc() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_UHC))
#define __cpm_start_lcd() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_LCD))
#define __cpm_start_i2c() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_I2C))
#define __cpm_start_aic_pclk() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_AICPCLK))
#define __cpm_start_aic_bitclk() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_AICBCLK))
#define __cpm_start_pwm0() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_PWM0))
#define __cpm_start_pwm1() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_PWM1))
#define __cpm_start_ssi() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_SSI))
#define __cpm_start_msc() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_MSC))
#define __cpm_start_scc() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_SCC))
#define __cpm_start_eth() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_ETH))
#define __cpm_start_kbc() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_KBC))
#define __cpm_start_cim() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_CIM))
#define __cpm_start_udc() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_UDC))
#define __cpm_start_uprt() \
	(REG_CPM_MSCR &= ~(1 << CPM_MSCR_MSTP_UPRT))
#define __cpm_start_all() (REG_CPM_MSCR = 0)

#define __cpm_stop_uart0() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_UART0))
#define __cpm_stop_uart1() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_UART1))
#define __cpm_stop_uart2() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_UART2))
#define __cpm_stop_uart3() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_UART3))
#define __cpm_stop_ost() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_OST))
#define __cpm_stop_dmac() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_DMAC))
#define __cpm_stop_uhc() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_UHC))
#define __cpm_stop_lcd() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_LCD))
#define __cpm_stop_i2c() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_I2C))
#define __cpm_stop_aic_pclk() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_AICPCLK))
#define __cpm_stop_aic_bitclk() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_AICBCLK))
#define __cpm_stop_pwm0() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_PWM0))
#define __cpm_stop_pwm1() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_PWM1))
#define __cpm_stop_ssi() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_SSI))
#define __cpm_stop_msc() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_MSC))
#define __cpm_stop_scc() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_SCC))
#define __cpm_stop_eth() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_ETH))
#define __cpm_stop_kbc() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_KBC))
#define __cpm_stop_cim() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_CIM))
#define __cpm_stop_udc() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_UDC))
#define __cpm_stop_uprt() \
	(REG_CPM_MSCR |= (1 << CPM_MSCR_MSTP_UPRT))
#define __cpm_stop_all() (REG_CPM_MSCR = 0xffffffff)

#define __cpm_set_pin(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	if (p == 0)				\
		REG_CPM_GSR0 |= (1 << o);	\
	else if (p == 1)			\
		REG_CPM_GSR1 |= (1 << o);	\
	else if (p == 2)			\
		REG_CPM_GSR2 |= (1 << o);	\
	else if (p == 3)			\
		REG_CPM_GSR3 |= (1 << o);	\
} while (0)

#define __cpm_clear_pin(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	if (p == 0)				\
		REG_CPM_GSR0 &= ~(1 << o);	\
	else if (p == 1)			\
		REG_CPM_GSR1 &= ~(1 << o);	\
	else if (p == 2)			\
		REG_CPM_GSR2 &= ~(1 << o);	\
	else if (p == 3)			\
		REG_CPM_GSR3 &= ~(1 << o);	\
} while (0)


#define __cpm_select_msc_clk(type) \
do {                               \
  if (type == 0)                   \
    REG_CPM_CFCR &= ~CPM_CFCR_MSC; \
  else                             \
    REG_CPM_CFCR |= CPM_CFCR_MSC;  \
  REG_CPM_CFCR |= CPM_CFCR_UPE;    \
} while(0)


/***************************************************************************
 * SSI
 ***************************************************************************/

#define __ssi_enable()    ( REG_SSI_CR0 |= SSI_CR0_SSIE )
#define __ssi_disable()   ( REG_SSI_CR0 &= ~SSI_CR0_SSIE )
#define __ssi_select_ce() ( REG_SSI_CR0 &= ~SSI_CR0_FSEL )

#define __ssi_normal_mode() ( REG_SSI_ITR &= ~SSI_ITR_IVLTM_MASK )

#define __ssi_select_ce2() 		\
do { 					\
	REG_SSI_CR0 |= SSI_CR0_FSEL; 	\
	REG_SSI_CR1 &= ~SSI_CR1_MULTS; 	\
} while (0)

#define __ssi_select_gpc() 		\
do { 					\
	REG_SSI_CR0 &= ~SSI_CR0_FSEL; 	\
	REG_SSI_CR1 |= SSI_CR1_MULTS; 	\
} while (0)

#define __ssi_enable_tx_intr() 	\
  ( REG_SSI_CR0 |= SSI_CR0_TIE | SSI_CR0_TEIE )

#define __ssi_disable_tx_intr() \
  ( REG_SSI_CR0 &= ~(SSI_CR0_TIE | SSI_CR0_TEIE) )

#define __ssi_enable_rx_intr() 	\
  ( REG_SSI_CR0 |= SSI_CR0_RIE | SSI_CR0_REIE )

#define __ssi_disable_rx_intr() \
  ( REG_SSI_CR0 &= ~(SSI_CR0_RIE | SSI_CR0_REIE) )

#define __ssi_enable_loopback()  ( REG_SSI_CR0 |= SSI_CR0_LOOP )
#define __ssi_disable_loopback() ( REG_SSI_CR0 &= ~SSI_CR0_LOOP )

#define __ssi_enable_receive()   ( REG_SSI_CR0 &= ~SSI_CR0_DISREV )
#define __ssi_disable_receive()  ( REG_SSI_CR0 |= SSI_CR0_DISREV )

#define __ssi_finish_receive() 	\
  ( REG_SSI_CR0 |= (SSI_CR0_RFINE | SSI_CR0_RFINC) )

#define __ssi_disable_recvfinish() \
  ( REG_SSI_CR0 &= ~(SSI_CR0_RFINE | SSI_CR0_RFINC) )

#define __ssi_flush_txfifo()   ( REG_SSI_CR0 |= SSI_CR0_TFLUSH )
#define __ssi_flush_rxfifo()   ( REG_SSI_CR0 |= SSI_CR0_RFLUSH )

#define __ssi_flush_fifo() \
  ( REG_SSI_CR0 |= SSI_CR0_TFLUSH | SSI_CR0_RFLUSH )

#define __ssi_finish_transmit() ( REG_SSI_CR1 &= ~SSI_CR1_UNFIN )

/* Motorola's SPI format, set 1 delay */
#define __ssi_spi_format() 					\
do { 								\
	REG_SSI_CR1 &= ~SSI_CR1_FMAT_MASK; 			\
	REG_SSI_CR1 |= SSI_CR1_FMAT_SPI; 			\
	REG_SSI_CR1 &= ~(SSI_CR1_TFVCK_MASK|SSI_CR1_TCKFI_MASK);\
	REG_SSI_CR1 |= (SSI_CR1_TFVCK_1 | SSI_CR1_TCKFI_1);	\
} while (0)

/* TI's SSP format, must clear SSI_CR1.UNFIN */
#define __ssi_ssp_format() 					\
do { 								\
	REG_SSI_CR1 &= ~(SSI_CR1_FMAT_MASK | SSI_CR1_UNFIN); 	\
	REG_SSI_CR1 |= SSI_CR1_FMAT_SSP; 			\
} while (0)

/* National's Microwire format, must clear SSI_CR0.RFINE, and set max delay */
#define __ssi_microwire_format() 				\
do { 								\
	REG_SSI_CR1 &= ~SSI_CR1_FMAT_MASK; 			\
	REG_SSI_CR1 |= SSI_CR1_FMAT_MW1; 			\
	REG_SSI_CR1 &= ~(SSI_CR1_TFVCK_MASK|SSI_CR1_TCKFI_MASK);\
	REG_SSI_CR1 |= (SSI_CR1_TFVCK_3 | SSI_CR1_TCKFI_3);	\
	REG_SSI_CR0 &= ~SSI_CR0_RFINE; 				\
} while (0)

/* CE# level (FRMHL), CE# in interval time (ITFRM),
   clock phase and polarity (PHA POL),
   interval time (SSIITR), interval characters/frame (SSIICR) */

 /* frmhl,endian,mcom,flen,pha,pol MASK */
#define SSICR1_MISC_MASK 					\
	( SSI_CR1_FRMHL_MASK | SSI_CR1_LFST | SSI_CR1_MCOM_MASK	\
	| SSI_CR1_FLEN_MASK | SSI_CR1_PHA | SSI_CR1_POL )	\

#define __ssi_spi_set_misc(frmhl,endian,flen,mcom,pha,pol)	\
do { 								\
	REG_SSI_CR1 &= ~SSICR1_MISC_MASK; 			\
	REG_SSI_CR1 |= ((frmhl) << 30) | ((endian) << 25) | 	\
		 (((mcom) - 1) << 12) | (((flen) - 2) << 4) | 	\
	         ((pha) << 1) | (pol); 				\
} while(0)

/* Transfer with MSB or LSB first */
#define __ssi_set_msb() ( REG_SSI_CR1 &= ~SSI_CR1_LFST )
#define __ssi_set_lsb() ( REG_SSI_CR1 |= SSI_CR1_LFST )

/* n = 2 - 17 */
#define __ssi_set_frame_length(n) \
    ( REG_SSI_CR1 = ((REG_SSI_CR1 & ~SSI_CR1_FLEN_MASK) | SSI_CR1_FLEN_##n##BIT) )

/* n = 1 - 16 */
#define __ssi_set_microwire_command_length(n) \
    ( REG_SSI_CR1 = ((REG_SSI_CR1 & ~SSI_CR1_MCOM_MASK) | SSI_CR1_MCOM_##n##BIT) )

/* Set the clock phase for SPI */
#define __ssi_set_spi_clock_phase(n) \
    ( REG_SSI_CR1 = ((REG_SSI_CR1 & ~SSI_CR1_PHA) | (n&0x1)) )

/* Set the clock polarity for SPI */
#define __ssi_set_spi_clock_polarity(n) \
    ( REG_SSI_CR1 = ((REG_SSI_CR1 & ~SSI_CR1_POL) | (n&0x1)) )

/* n = 1,4,8,14 */
#define __ssi_set_tx_trigger(n) 		\
do { 						\
	REG_SSI_CR1 &= ~SSI_CR1_TTRG_MASK; 	\
	REG_SSI_CR1 |= SSI_CR1_TTRG_##n; 	\
} while (0)

/* n = 1,4,8,14 */
#define __ssi_set_rx_trigger(n) 		\
do { 						\
	REG_SSI_CR1 &= ~SSI_CR1_RTRG_MASK; 	\
	REG_SSI_CR1 |= SSI_CR1_RTRG_##n; 	\
} while (0)

#define __ssi_get_txfifo_count() \
    ( (REG_SSI_SR & SSI_SR_TFIFONUM_MASK) >> SSI_SR_TFIFONUM_BIT )

#define __ssi_get_rxfifo_count() \
    ( (REG_SSI_SR & SSI_SR_RFIFONUM_MASK) >> SSI_SR_RFIFONUM_BIT )

#define __ssi_clear_errors() \
    ( REG_SSI_SR &= ~(SSI_SR_UNDR | SSI_SR_OVER) )

#define __ssi_transfer_end()	( REG_SSI_SR & SSI_SR_END )
#define __ssi_is_busy()		( REG_SSI_SR & SSI_SR_BUSY )

#define __ssi_txfifo_full()	( REG_SSI_SR & SSI_SR_TFF )
#define __ssi_rxfifo_empty()	( REG_SSI_SR & SSI_SR_RFE )
#define __ssi_rxfifo_noempty()	( REG_SSI_SR & SSI_SR_RFHF )

#define __ssi_set_clk(dev_clk, ssi_clk) \
  ( REG_SSI_GR = (dev_clk) / (2*(ssi_clk)) - 1 )

#define __ssi_receive_data()    REG_SSI_DR
#define __ssi_transmit_data(v)  ( REG_SSI_DR = (v) )

/***************************************************************************
 * WDT
 ***************************************************************************/

#define __wdt_set_count(count) ( REG_WDT_WTCNT = (count) )
#define __wdt_start()          ( REG_WDT_WTCSR |= WDT_WTCSR_START )
#define __wdt_stop()           ( REG_WDT_WTCSR &= ~WDT_WTCSR_START )


/***************************************************************************
 ***************************************************************************/

/* 
 * CPU clocks
 */
#ifdef CFG_EXTAL
#define JZ_EXTAL		CFG_EXTAL
#else
#define JZ_EXTAL		3686400
#endif
#define JZ_EXTAL2		32768 /* RTC clock */

static __inline__ unsigned int __cpm_get_pllout(void)
{
	unsigned int nf, nr, no, pllout;
	unsigned long plcr = REG_CPM_PLCR1;
	unsigned long od[4] = {1, 2, 2, 4};
	if (plcr & CPM_PLCR1_PLL1EN) {
		nf = (plcr & CPM_PLCR1_PLL1FD_MASK) >> CPM_PLCR1_PLL1FD_BIT;
		nr = (plcr & CPM_PLCR1_PLL1RD_MASK) >> CPM_PLCR1_PLL1RD_BIT;
		no = od[((plcr & CPM_PLCR1_PLL1OD_MASK) >> CPM_PLCR1_PLL1OD_BIT)];
		pllout = (JZ_EXTAL) / ((nr+2) * no) * (nf+2);
	} else
		pllout = JZ_EXTAL;
	return pllout;
}

static __inline__ unsigned int __cpm_get_iclk(void)
{
	unsigned int iclk;
	int div[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};
	unsigned long cfcr = REG_CPM_CFCR;
	unsigned long plcr = REG_CPM_PLCR1;
	if (plcr & CPM_PLCR1_PLL1EN)
		iclk = __cpm_get_pllout() /
		       div[(cfcr & CPM_CFCR_IFR_MASK) >> CPM_CFCR_IFR_BIT];
	else
		iclk = JZ_EXTAL;
	return iclk;
}

static __inline__ unsigned int __cpm_get_sclk(void)
{
	unsigned int sclk;
	int div[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};
	unsigned long cfcr = REG_CPM_CFCR;
	unsigned long plcr = REG_CPM_PLCR1;
	if (plcr & CPM_PLCR1_PLL1EN)
		sclk = __cpm_get_pllout() /
		       div[(cfcr & CPM_CFCR_SFR_MASK) >> CPM_CFCR_SFR_BIT];
	else
		sclk = JZ_EXTAL;
	return sclk;
}

static __inline__ unsigned int __cpm_get_mclk(void)
{
	unsigned int mclk;
	int div[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};
	unsigned long cfcr = REG_CPM_CFCR;
	unsigned long plcr = REG_CPM_PLCR1;
	if (plcr & CPM_PLCR1_PLL1EN)
		mclk = __cpm_get_pllout() /
		       div[(cfcr & CPM_CFCR_MFR_MASK) >> CPM_CFCR_MFR_BIT];
	else
		mclk = JZ_EXTAL;
	return mclk;
}

static __inline__ unsigned int __cpm_get_pclk(void)
{
	unsigned int devclk;
	int div[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};
	unsigned long cfcr = REG_CPM_CFCR;
	unsigned long plcr = REG_CPM_PLCR1;
	if (plcr & CPM_PLCR1_PLL1EN)
		devclk = __cpm_get_pllout() /
			 div[(cfcr & CPM_CFCR_PFR_MASK) >> CPM_CFCR_PFR_BIT];
	else
		devclk = JZ_EXTAL;
	return devclk;
}

static __inline__ unsigned int __cpm_get_devclk(void)
{
	unsigned int devclk;
	int div[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};
	unsigned long cfcr = REG_CPM_CFCR;
	unsigned long plcr = REG_CPM_PLCR1;
	if (plcr & CPM_PLCR1_PLL1EN)
		devclk = __cpm_get_pllout() /
			 div[(cfcr & CPM_CFCR_PFR_MASK) >> CPM_CFCR_PFR_BIT];
	else
		devclk = JZ_EXTAL;
	return devclk;
}

#endif /* !__ASSEMBLY__ */

#endif /* __JZ4730_H__ */
