/*
 * Include file for Ingenic Semiconductor's JZ5730 CPU.
 */
#ifndef __JZ5730_H__
#define __JZ5730_H__

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
#define PCIC_BASE	0xB3070000
#define	ETH_BASE	0xB3100000
#define	NBM_BASE	0xB3F00000

#define	CPM_BASE	0xB0000000
#define	INTC_BASE	0xB0001000
#define	OST_BASE	0xB0002000
#define	RTC_BASE	0xB0003000
#define	WDT_BASE	0xB0004000
#define	GPIO_BASE	0xB0010000
#define	AIC_BASE	0xB0020000
#define	UART0_BASE	0xB0030000
#define	UART1_BASE	0xB0031000
#define	SCC_BASE	0xB0041000
#define	I2C_BASE	0xB0042000
#define	SSI_BASE	0xB0043000
#define	PWM0_BASE	0xB0050000
#define	PWM1_BASE	0xB0051000
#define	DES_BASE	0xB0060000

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
 * PCIC
 *************************************************************************/
#define PCIC_CFGAR		(PCIC_BASE + 0x2c)
#define PCIC_CFGDR		(PCIC_BASE + 0x30)
#define PCIC_SR			(PCIC_BASE + 0x34)
#define PCIC_IER		(PCIC_BASE + 0x38)
#define PCIC_PTA_MEMATR		(PCIC_BASE + 0x40)
#define PCIC_ATP_MTR0		(PCIC_BASE + 0x44)
#define PCIC_ATP_MTR1		(PCIC_BASE + 0x48)
#define PCIC_ATP_MTR2		(PCIC_BASE + 0x4c)
#define PCIC_ATP_IOTR		(PCIC_BASE + 0x50)
#define PCIC_INTACK		(PCIC_BASE + 0x54)

#define PCIC_CFG_COMMAND	(PCIC_BASE + 0x100 + 0x04)
#define PCIC_CFG_STATUS		(PCIC_BASE + 0x100 + 0x06)
#define PCIC_CFG_MEMBASE0	(PCIC_BASE + 0x100 + 0x10)

#define REG_PCIC_CFGAR		REG32(PCIC_CFGAR)
#define REG_PCIC_CFGDR		REG32(PCIC_CFGDR)
#define REG_PCIC_SR		REG32(PCIC_SR)
#define REG_PCIC_IER		REG32(PCIC_IER)
#define REG_PCIC_PTA_MEMATR	REG32(PCIC_PTA_MEMATR)
#define REG_PCIC_ATP_MTR0	REG32(PCIC_ATP_MTR0)
#define REG_PCIC_ATP_MTR1	REG32(PCIC_ATP_MTR1)
#define REG_PCIC_ATP_MTR2	REG32(PCIC_ATP_MTR2)
#define REG_PCIC_ATP_IOTR	REG32(PCIC_ATP_IOTR)
#define REG_PCIC_INTACK		REG32(PCIC_INTACK)

#define REG_PCIC_CFG_COMMAND	REG16(PCIC_CFG_COMMAND)
#define REG_PCIC_CFG_STATUS	REG16(PCIC_CFG_STATUS)
#define REG_PCIC_CFG_MEMBASE0	REG32(PCIC_CFG_MEMBASE0)

#define PCIC_SR_INTD		(1 << 15)
#define PCIC_SR_INTC		(1 << 14)
#define PCIC_SR_INTB		(1 << 13)
#define PCIC_SR_INTA		(1 << 12)
#define PCIC_SR_APER		(1 << 11)	/* Addr parity error */
#define PCIC_SR_DPER		(1 << 10)	/* Data parity error */
#define PCIC_SR_MA		(1 << 9)	/* Master abort */
#define PCIC_SR_TA		(1 << 8)	/* Target abort */

#define PCIC_IER_SPLIT		(1 << 31)	/* AHB split enable */
#define PCIC_IER_PCIA		(1 << 30)	/* PCI extern arbiter */
#define PCIC_IER_INTD		(1 << 6)	/* INTD enable */
#define PCIC_IER_INTC		(1 << 5)	/* INTC enable */
#define PCIC_IER_INTB		(1 << 4)	/* INTB enable */
#define PCIC_IER_INTA		(1 << 3)	/* INTA enable */
#define PCIC_IER_MIE		(1 << 2)	/* mailbox intr en */
#define PCIC_IER_IE		(1 << 1)	/* Interrupt en */

#define PCIC_CFG_COMMAND_MASTER	(1 << 2)
#define PCIC_CFG_COMMAND_MEM	(1 << 1)
#define PCIC_CFG_COMMAND_IO	(1 << 0)

#define IRQ_PCIC_0	40
#define NUM_PCID	4

#define JzSOC_PCI_IO_START	0xA0000000
#define JzSOC_PCI_IO_END	0xA7FFFFFF
#define JzSOC_PCI_MEM_START	0xA8000000
#define JzSOC_PCI_MEM_END	0xAFFFFFFF

#define JzSOC_PCI_VGA_TEXT_BASE	0xB0000000
#define JzSOC_PCI_VGA_FB_BASE	0xB8000000

#define JzSOC_PCI_CORE_START	0x10000000


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

/*
 * Define macros for REMRCR
 */
#define REMRCR_EN	(1 << 31)	/* Remote output receive enable */


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
#define IRQ_UART1	8
#define IRQ_UART0	9
#define IRQ_SCC		10
#define IRQ_UHC		13
#define IRQ_RTC		15
#define IRQ_SSI		17
#define IRQ_PCIC	18
#define IRQ_ETH		19
#define IRQ_AIC		20
#define IRQ_DMAC	21
#define IRQ_OST2	22
#define IRQ_OST1	23
#define IRQ_OST0	24
#define IRQ_GPIO2	26
#define IRQ_GPIO1	27
#define IRQ_GPIO0	28



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
#define GPIO_GPPUR(n)	(GPIO_BASE + (0x0c + (n)*0x30))
#define GPIO_GPSLR(n)	(GPIO_BASE + (0x10 + (n)*0x30))
#define GPIO_GPSUR(n)	(GPIO_BASE + (0x14 + (n)*0x30))
#define GPIO_GPFLR(n)	(GPIO_BASE + (0x18 + (n)*0x30))
#define GPIO_GPFUR(n)	(GPIO_BASE + (0x1c + (n)*0x30))
#define GPIO_GPIMR(n)	(GPIO_BASE + (0x24 + (n)*0x30))
#define GPIO_GPFR(n)	(GPIO_BASE + (0x28 + (n)*0x30))

#define REG_GPIO_GPDR(n)	REG32(GPIO_GPDR((n)))
#define REG_GPIO_GPPUR(n)	REG32(GPIO_GPPUR((n)))
#define REG_GPIO_GPSLR(n)	REG32(GPIO_GPSLR((n)))
#define REG_GPIO_GPSUR(n)	REG32(GPIO_GPSUR((n)))
#define REG_GPIO_GPFLR(n)	REG32(GPIO_GPFLR((n)))
#define REG_GPIO_GPFUR(n)	REG32(GPIO_GPFUR((n)))
#define REG_GPIO_GPIMR(n)	REG32(GPIO_GPIMR((n)))
#define REG_GPIO_GPFR(n)	REG32(GPIO_GPFR((n)))

#define IRQ_GPIO_0	48
#define NUM_GPIO	70

#define GPIO_GPDR0	REG_GPIO_GPDR(0)
#define GPIO_GPDR1	REG_GPIO_GPDR(1)
#define GPIO_GPDR2	REG_GPIO_GPDR(2)
#define GPIO_GPPUR0	REG_GPIO_GPPUR(0)
#define GPIO_GPPUR1	REG_GPIO_GPPUR(1)
#define GPIO_GPPUR2	REG_GPIO_GPPUR(2)
#define GPIO_GPSLR0	REG_GPIO_GPSLR(0)
#define GPIO_GPSLR1	REG_GPIO_GPSLR(1)
#define GPIO_GPSLR2	REG_GPIO_GPSLR(2)
#define GPIO_GPSUR0	REG_GPIO_GPSUR(0)
#define GPIO_GPSUR1	REG_GPIO_GPSUR(1)
#define GPIO_GPSUR2	REG_GPIO_GPSUR(2)
#define GPIO_GPFLR0	REG_GPIO_GPFLR(0)
#define GPIO_GPFLR1	REG_GPIO_GPFLR(1)
#define GPIO_GPFLR2	REG_GPIO_GPFLR(2)
#define GPIO_GPFUR0	REG_GPIO_GPFUR(0)
#define GPIO_GPFUR1	REG_GPIO_GPFUR(1)
#define GPIO_GPFUR2	REG_GPIO_GPFUR(2)
#define GPIO_GPIMR0	REG_GPIO_GPIMR(0)
#define GPIO_GPIMR1	REG_GPIO_GPIMR(1)
#define GPIO_GPIMR2	REG_GPIO_GPIMR(2)
#define GPIO_GPFR0	REG_GPIO_GPFR(0)
#define GPIO_GPFR1	REG_GPIO_GPFR(1)
#define GPIO_GPFR2	REG_GPIO_GPFR(2)


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

#define AIC_CR_ASVTSU		(1 << 9)
#define AIC_CR_ENDSW		(1 << 10)
#define AIC_CR_M2S		(1 << 11)
#define AIC_CR_OSS_BIT		19
#define AIC_CR_OSS_MASK		(7 << AIC_CR_OSS_BIT)
  #define AIC_CR_OSS_8BIT	(0 << AIC_CR_OSS_BIT)
  #define AIC_CR_OSS_16BIT	(1 << AIC_CR_OSS_BIT)
  #define AIC_CR_OSS_18BIT	(2 << AIC_CR_OSS_BIT)
  #define AIC_CR_OSS_20BIT	(3 << AIC_CR_OSS_BIT)
  #define AIC_CR_OSS_24BIT	(4 << AIC_CR_OSS_BIT)
#define AIC_CR_ISS_BIT		16
#define AIC_CR_ISS_MASK		(7 << AIC_CR_ISS_BIT)
  #define AIC_CR_ISS_8BIT	(0 << AIC_CR_ISS_BIT)
  #define AIC_CR_ISS_16BIT	(1 << AIC_CR_ISS_BIT)
  #define AIC_CR_ISS_18BIT	(2 << AIC_CR_ISS_BIT)
  #define AIC_CR_ISS_20BIT	(3 << AIC_CR_ISS_BIT)
  #define AIC_CR_ISS_24BIT	(4 << AIC_CR_ISS_BIT)

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
#define	AIC_SR_RFL_MASK		(0x3f << AIC_SR_RFL_BIT)
#define	AIC_SR_TFL_BIT		8
#define	AIC_SR_TFL_MASK		(0x3f << AIC_SR_TFL_BIT)
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
#define CPM_SPR		(CPM_BASE+0x44)

#define REG_CPM_CFCR	REG32(CPM_CFCR)
#define REG_CPM_PLCR1	REG32(CPM_PLCR1)
#define REG_CPM_OCR	REG32(CPM_OCR)
#define REG_CPM_CFCR2	REG32(CPM_CFCR2)
#define REG_CPM_LPCR	REG32(CPM_LPCR)
#define REG_CPM_RSTR	REG32(CPM_RSTR)
#define REG_CPM_MSCR	REG32(CPM_MSCR)
#define REG_CPM_SCR	REG32(CPM_SCR)
#define REG_CPM_SPR     REG32(CPM_SPR)


#define CPM_CFCR_SSI		(1 << 31)
#define CPM_CFCR_CKOEN		(1 << 30)
//#define CPM_CFCR_LFR_BIT	12
#define CPM_CFCR_UCS		(1 << 29)
#define CPM_CFCR_UFR_BIT	25
#define CPM_CFCR_UFR_MASK	(0x07 << CPM_CFCR_UFR_BIT)
#define CPM_CFCR_UPE		(1 << 24)
#define CPM_CFCR_SSIFR_BIT	16
#define CPM_CFCR_SSIFR_MASK	(0xff << CPM_CFCR_SSIFR_BIT)
#define CPM_CFCR_MFR_BIT	12
#define CPM_CFCR_MFR_MASK	(0x0f << CPM_CFCR_MFR_BIT)
#define CPM_CFCR_PFR_BIT	8
#define CPM_CFCR_PFR_MASK	(0x0f << CPM_CFCR_PFR_BIT)
#define CPM_CFCR_SFR_BIT	4
#define CPM_CFCR_SFR_MASK	(0x0f << CPM_CFCR_SFR_BIT)
#define CPM_CFCR_IFR_BIT	0
#define CPM_CFCR_IFR_MASK	(0x0f << CPM_CFCR_IFR_BIT)

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

#define CPM_CFCR2_I2SFR_BIT	0
#define CPM_CFCR2_I2SFR_MASK	(0x1ff << CPM_CFCR2_PXFR_BIT)

#define CPM_LPCR_DUTY_BIT	3
#define CPM_LPCR_DUTY_MASK	(0x1f << CPM_LPCR_DUTY_BIT)
#define CPM_LPCR_DOZE		(1 << 2)
#define CPM_LPCR_LPM_BIT	0
#define CPM_LPCR_LPM_MASK	(0x03 << CPM_LPCR_LPM_BIT)
  #define CPM_LPCR_LPM_IDLE		(0 << CPM_LPCR_LPM_BIT)
  #define CPM_LPCR_LPM_SLEEP		(1 << CPM_LPCR_LPM_BIT)
  #define CPM_LPCR_LPM_HIBERNATE	(2 << CPM_LPCR_LPM_BIT)

#define CPM_RSTR_PR		(1 << 2)
#define CPM_RSTR_WR		(1 << 1)
#define CPM_RSTR_HR		(1 << 0)

#define CPM_MSCR_MSTP_BIT	0
#define CPM_MSCR_MSTP_MASK	(0xffff << CPM_MSCR_MSTP_BIT)
  #define CPM_MSCR_MSTP_UART0	0
  #define CPM_MSCR_MSTP_UART1	1
  #define CPM_MSCR_MSTP_OST	2
  #define CPM_MSCR_MSTP_RTC	3
  #define CPM_MSCR_MSTP_DMAC	4
  #define CPM_MSCR_MSTP_UHC	5
  #define CPM_MSCR_MSTP_I2C	6
  #define CPM_MSCR_MSTP_AIC1	7
  #define CPM_MSCR_MSTP_PWM0	8
  #define CPM_MSCR_MSTP_PWM1	9
  #define CPM_MSCR_MSTP_SSI	10
  #define CPM_MSCR_MSTP_SCC	11
  #define CPM_MSCR_MSTP_AIC2	12
  #define CPM_MSCR_MSTP_DES	13
  #define CPM_MSCR_MSTP_ETH	14
  #define CPM_MSCR_MSTP_PCI	15

#define CPM_SCR_SUSPEND1	(1 << 7)
#define CPM_SCR_SUSPEND0	(1 << 6)
#define CPM_SCR_O2SE		(1 << 5)
#define CPM_SCR_O1SE		(1 << 4)
#define CPM_SCR_HGP             (1 << 3)
#define CPM_SCR_HZP		(1 << 2)
#define CPM_SCR_HZM		(1 << 1)



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
 * PCIC
 ***************************************************************************/
#define __pcic_enable_irq(irq)			\
do {						\
	REG_PCIC_IER |= (1 << (irq + 3));	\
} while (0)

#define __pcic_disable_irq(irq)			\
do {						\
	REG_PCIC_IER &= ~(1 << (irq + 3));	\
} while (0)

#define __pcic_get_irq()			\
({	unsigned int __t__, __i__;		\
	__t__ = (REG_PCIC_SR >> 12) & 0x0f;	\
	for (__i__ = 0; __i__ < 4; __i__ ++) {	\
		if (__t__ & 0x01)		\
			break;			\
		__t__ >>= 1;			\
	}					\
	__i__;					\
})

#define __pcic_bus_errors_detected()		\
	((REG_PCIC_CFG_STATUS & 0xf000) != 0)

#define __pcic_clear_bus_errors()		\
do {						\
	REG_PCIC_CFG_STATUS = 0xf000;		\
} while (0)
 
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
  ( REG8(UART_BASE + UART_OFF*(n) + OFF_FCR) |= UARTFCR_UUE | UARTFCR_FE )
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
  ( REG8(UART_BASE + UART_OFF*(n) + OFF_LSR) &= ~(UARTLSR_ORER | UARTLSR_BRK | UARTLSR_FER | UARTLSR_PER | UARTLSR_RFER) )

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
 * n is the absolute number of a pin (0-70), regardless of the port
 * m is the interrupt manner (low/high/falling/rising)
 */

#define __gpio_port_data(p)	( REG_GPIO_GPDR(p) )

#define __gpio_port_as_gpiofn(p, o, fn)			\
do {							\
	unsigned int tmp;				\
							\
	if (o < 16) {					\
		tmp = REG_GPIO_GPSLR(p);		\
		tmp &= ~(3 << ((o) << 1));		\
		REG_GPIO_GPSLR(p) = tmp;		\
		tmp = REG_GPIO_GPFLR(p);		\
		tmp &= ~(3 << ((o) << 1));		\
		tmp |= fn << ((o) << 1);		\
		REG_GPIO_GPFLR(p) = tmp;		\
	} else {					\
		tmp = REG_GPIO_GPSUR(p);		\
		tmp &= ~(3 << (((o) - 16) << 1));	\
		REG_GPIO_GPSUR(p) = tmp;		\
		tmp = REG_GPIO_GPFUR(p);		\
		tmp &= ~(3 << (((o) - 16) << 1));	\
		tmp |= fn << (((o) - 16) << 1);		\
		REG_GPIO_GPFUR(p) = tmp;		\
	}						\
							\
} while (0)

#define __gpio_port_as_output(p, o)			\
do {							\
	__gpio_port_as_gpiofn((p), (o), 1);		\
} while (0)

#define __gpio_port_as_input(p, o)			\
do {							\
	__gpio_port_as_gpiofn((p), (o), 0);		\
} while (0)

#define __gpio_port_as_output_opendrain(p, o)		\
do {							\
	__gpio_port_as_gpiofn((p), (o), 2);		\
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

#define __gpio_as_irq(n, m)				\
do {							\
	unsigned int p, o, tmp;				\
	p = (n) / 32;					\
	o = (n) % 32;					\
	if (o < 16) {					\
		tmp = REG_GPIO_GPSLR(p);		\
		tmp &= ~(3 << ((o) << 1));		\
		tmp |= 2 << ((o) << 1);			\
		REG_GPIO_GPSLR(p) = tmp;		\
		tmp = REG_GPIO_GPFLR(p);		\
		tmp &= ~(3 << ((o) << 1));		\
		tmp |= (m) << ((o) << 1);		\
		REG_GPIO_GPFLR(p) = tmp;		\
	} else {					\
		tmp = REG_GPIO_GPSUR(p);		\
		tmp &= ~(3 << (((o) - 16) << 1));	\
		tmp |= 2 << (((o) - 16) << 1);		\
		REG_GPIO_GPSUR(p) = tmp;		\
		tmp = REG_GPIO_GPFUR(p);		\
		tmp &= ~(3 << (((o) - 16) << 1));	\
		tmp |= (m) << (((o) - 16) << 1);	\
		REG_GPIO_GPFUR(p) = tmp;		\
	}						\
} while (0)


#define __gpio_as_irq_low_level(n)	__gpio_as_irq(n, 0)
#define __gpio_as_irq_high_level(n)	__gpio_as_irq(n, 1)
#define __gpio_as_irq_fall_edge(n)	__gpio_as_irq(n, 2)
#define __gpio_as_irq_rise_edge(n)	__gpio_as_irq(n, 3)

#define __gpio_mask_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_GPIMR(p) &= ~(1 << o);		\
} while (0)

#define __gpio_unmask_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_GPIMR(p) |= (1 << o);		\
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

/* Init the alternate function pins */


#define __gpio_as_ssi()						\
do {								\
	GPIO_GPSUR1 = (GPIO_GPSUR1 & 0xfc00ffff) | 0x01550000;	\
	GPIO_GPFUR1 = (GPIO_GPFUR1 & 0xfc00ffff);		\
} while (0)

#define __gpio_as_uart1()					\
do {								\
	GPIO_GPSLR0 = (GPIO_GPSLR0 & 0xfff0ffff) | 0x00050000;	\
	GPIO_GPFLR0 = (GPIO_GPFLR0 & 0xfff0ffff);		\
	GPIO_GPSUR0 = (GPIO_GPSUR0 & 0xfffffff0) | 0x00000005;	\
	GPIO_GPFUR0 = (GPIO_GPFUR0 & 0xfffffff0);		\
} while (0)

#define __gpio_as_uart0()					\
do {								\
	GPIO_GPSLR2 = (GPIO_GPSLR2 & 0xfffffff0) | 0x00000005;	\
	GPIO_GPFLR2 = (GPIO_GPFLR2 & 0xfffffff0);		\
} while (0)


#define __gpio_as_scc0()					\
do {								\
	GPIO_GPSUR1 = (GPIO_GPSUR1 & 0xc3ffffff) | 0x14000000;	\
	GPIO_GPFUR1 = (GPIO_GPFUR1 & 0xc3ffffff);		\
} while (0)

#define __gpio_as_scc() __gpio_as_scc0();

#define __gpio_as_dma()						\
do {								\
	GPIO_GPSLR0 = (GPIO_GPSLR0 & 0x000fffff) | 0x55500000;	\
	GPIO_GPFLR0 = (GPIO_GPFLR0 & 0x000fffff);		\
} while (0)

#define __gpio_as_pcmcia()					\
do {								\
	GPIO_GPSUR0 = (GPIO_GPSUR0 & 0xf000ffff) | 0x05550000;	\
	GPIO_GPFUR0 = (GPIO_GPFUR0 & 0xf000ffff);		\
} while (0)

#define __gpio_as_emc(csmask)					\
do {								\
	GPIO_GPSLR1 = (GPIO_GPSLR1 & 0xfff00003) | 0x00055554;	\
	GPIO_GPFLR1 = (GPIO_GPFLR1 & 0xfff00003);		\
} while (0)

#define __gpio_as_usbclk()					\
do {								\
	GPIO_GPSLR1 = (GPIO_GPSLR1 & 0xfffffffc) | 0x00000001;	\
	GPIO_GPFLR1 = (GPIO_GPFLR1 & 0xfffffffc);		\
} while (0)

#define __gpio_as_ac97()					\
do {								\
	GPIO_GPSUR1 = (GPIO_GPSUR1 & 0xffff030f) | 0x00005450;	\
	GPIO_GPFUR1 = (GPIO_GPFUR1 & 0xffff030f) | 0x00000040;	\
} while (0)

#define __gpio_as_i2s_slave()					\
do {								\
	GPIO_GPSUR1 = (GPIO_GPSUR1 & 0xffff0c0f) | 0x00005150;	\
	GPIO_GPFUR1 = (GPIO_GPFUR1 & 0xffff0c0f);		\
} while (0)

#define __gpio_as_i2s_master()					\
do {								\
	GPIO_GPSUR1 = (GPIO_GPSUR1 & 0xffff0c0f) | 0x00005150;	\
	GPIO_GPFUR1 = (GPIO_GPFUR1 & 0xffff0c0f) | 0x00000050;	\
} while (0)

#define __gpio_as_pci()						\
do {								\
	GPIO_GPSLR1 = (GPIO_GPSLR1 & 0x000fffff) | 0x55500000;	\
	GPIO_GPFLR1 = (GPIO_GPFLR1 & 0x000fffff);		\
	GPIO_GPSUR1 = (GPIO_GPSUR1 & 0xfffffff0) | 0x00000005;	\
	GPIO_GPFUR1 = (GPIO_GPFUR1 & 0xfffffff0);		\
} while (0)

#define __gpio_as_eth()						\
do {								\
	GPIO_GPSLR0 = (GPIO_GPSLR0 & 0xffff0003) | 0x00005554;	\
	GPIO_GPFLR0 = (GPIO_GPFLR0 & 0xffff0003);		\
	GPIO_GPSUR0 = (GPIO_GPSUR0 & 0xffff000f) | 0x00005550;	\
	GPIO_GPFUR0 = (GPIO_GPFUR0 & 0xffff000f);		\
} while (0)

#define __gpio_as_pwm()						\
do {								\
	GPIO_GPSUR0 = (GPIO_GPSUR0 & 0x0fffffff) | 0x50000000;	\
	GPIO_GPFUR0 = (GPIO_GPFUR0 & 0x0fffffff);		\
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

#define __aic_enable_mono2stereo()	(REG_AIC_CR |= AIC_CR_M2S)
#define __aic_disable_mono2stereo()	(REG_AIC_CR &= ~AIC_CR_M2S)
#define __aic_enable_byteswap()		(REG_AIC_CR |= AIC_CR_ENDSW)
#define __aic_disable_byteswap()	(REG_AIC_CR &= ~AIC_CR_ENDSW)
#define __aic_enable_unsignadj()	(REG_AIC_CR |= AIC_CR_ASVTSU)
#define __aic_disable_unsignadj()	(REG_AIC_CR &= ~AIC_CR_ASVTSU)

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

#define Jz_AC97_RESET_BUG 1

#ifndef Jz_AC97_RESET_BUG
#define __ac97_cold_reset_codec()		\
 do {						\
	REG_AIC_ACCR2 |= AIC_ACCR2_SA;		\
	REG_AIC_ACCR2 &= ~AIC_ACCR2_SS;		\
	REG_AIC_ACCR2 &= ~AIC_ACCR2_SO;		\
	REG_AIC_ACCR2 |=  AIC_ACCR2_SR;		\
	udelay(1);				\
	REG_AIC_ACCR2 &= ~AIC_ACCR2_SR;		\
	REG_AIC_ACCR2 &= ~AIC_ACCR2_SA;		\
 } while (0)
#else
#define __ac97_cold_reset_codec()		\
 do {						\
	__gpio_as_output(54); /* SDATA_OUT */	\
	__gpio_as_output(55); /* SDATA_IN */	\
	__gpio_as_output(51); /* SYNC */	\
	__gpio_as_output(53); /* RESET# */	\
	__gpio_clear_pin(54);			\
	__gpio_clear_pin(55);			\
	__gpio_clear_pin(51);			\
	__gpio_clear_pin(53);			\
	udelay(10);				\
	__gpio_set_pin(53);			\
	udelay(1);				\
	__gpio_as_ac97();			\
 } while (0)
#endif

/* n=8,16,18,20 */
#define __ac97_set_iass(n) \
 ( REG_AIC_CR = (REG_AIC_CR & ~AIC_CR_ISS_MASK) | AIC_CR_ISS_##n##BIT )
#define __ac97_set_oass(n) \
 ( REG_AIC_CR = (REG_AIC_CR & ~AIC_CR_OSS_MASK) | AIC_CR_OSS_##n##BIT )

#define __i2s_select_i2s()            ( REG_AIC_I2SCR &= ~AIC_I2SCR_AMSL )
#define __i2s_select_left_justified() ( REG_AIC_I2SCR |= AIC_I2SCR_AMSL )

/* n=8,16,18,20,24 */
#define __i2s_set_sample_size(n) \
do { \
	__ac97_set_iass(n); \
	__ac97_set_oass(n); \
} while (0)

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

#if 0
#define __i2s_set_sample_rate(i2sclk, sync) \
  ( REG_AIC_I2SDIV = ((i2sclk) / (4*64)) / (sync) )
#else
#define __i2s_set_sample_rate(i2sclk, sync) do { } while(0)
#endif

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
        __gpio_as_output(55); /* SDATA_OUT */	\
        __gpio_as_input(54);  /* SDATA_IN */	\
        __gpio_as_output(51); /* SYNC */	\
        __gpio_as_output(53); /* RESET# */	\
	__gpio_clear_pin(55);			\
	__gpio_clear_pin(54);			\
	__gpio_clear_pin(51);			\
	__gpio_clear_pin(53);			\
        __gpio_as_i2s_master();			\
 } while (0)


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

#define __cpm_enable_cko()  (REG_CPM_CFCR |= CPM_CFCR_CKOEN)
#define __cpm_disable_cko()  (REG_CPM_CFCR &= ~CPM_CFCR_CKOEN)

#define __cpm_idle_mode()					\
	(REG_CPM_LPCR = (REG_CPM_LPCR & ~CPM_LPCR_LPM_MASK) |	\
			CPM_LPCR_LPM_IDLE)
#define __cpm_sleep_mode()					\
	(REG_CPM_LPCR = (REG_CPM_LPCR & ~CPM_LPCR_LPM_MASK) |	\
			CPM_LPCR_LPM_SLEEP)
#define __cpm_hibernate_mode()					\
	(REG_CPM_LPCR = (REG_CPM_LPCR & ~CPM_LPCR_LPM_MASK) |	\
			CPM_LPCR_LPM_HIBERNATE)

#define __cpm_stop_uart(n) \
	(REG_CPM_MSCR |= (CPM_MSCR_MSTP_UART##n << CPM_MSCR_MSTP_BIT))
#define __cpm_stop_pwm(n) \
	(REG_CPM_MSCR |= (CPM_MSCR_MSTP_PWM##n << CPM_MSCR_MSTP_BIT))
#define __cpm_stop_aic(n) \
	(REG_CPM_MSCR |= (CPM_MSCR_MSTP_AIC##n << CPM_MSCR_MSTP_BIT))
#define __cpm_stop_ost() \
	(REG_CPM_MSCR |= (CPM_MSCR_MSTP_OST << CPM_MSCR_MSTP_BIT))
#define __cpm_stop_rtc() \
	(REG_CPM_MSCR |= (CPM_MSCR_MSTP_RTC << CPM_MSCR_MSTP_BIT))
#define __cpm_stop_dmac() \
	(REG_CPM_MSCR |= (CPM_MSCR_MSTP_DMAC << CPM_MSCR_MSTP_BIT))
#define __cpm_stop_uhc() \
	(REG_CPM_MSCR |= (CPM_MSCR_MSTP_UHC << CPM_MSCR_MSTP_BIT))
#define __cpm_stop_lcd() \
	(REG_CPM_MSCR |= (CPM_MSCR_MSTP_LCD << CPM_MSCR_MSTP_BIT))
#define __cpm_stop_i2c() \
	(REG_CPM_MSCR |= (CPM_MSCR_MSTP_I2C << CPM_MSCR_MSTP_BIT))
#define __cpm_stop_ssi() \
	(REG_CPM_MSCR |= (CPM_MSCR_MSTP_SSI << CPM_MSCR_MSTP_BIT))
#define __cpm_stop_msc() \
	(REG_CPM_MSCR |= (CPM_MSCR_MSTP_MSC << CPM_MSCR_MSTP_BIT))
#define __cpm_stop_scc() \
	(REG_CPM_MSCR |= (CPM_MSCR_MSTP_SCC << CPM_MSCR_MSTP_BIT))
#define __cpm_stop_fir() \
	(REG_CPM_MSCR |= (CPM_MSCR_MSTP_FIR << CPM_MSCR_MSTP_BIT))
#define __cpm_stop_des() \
	(REG_CPM_MSCR |= (CPM_MSCR_MSTP_DES << CPM_MSCR_MSTP_BIT))
#define __cpm_stop_eth() \
	(REG_CPM_MSCR |= (CPM_MSCR_MSTP_ETH << CPM_MSCR_MSTP_BIT))
#define __cpm_stop_ps2() \
	(REG_CPM_MSCR |= (CPM_MSCR_MSTP_PS2 << CPM_MSCR_MSTP_BIT))
#define __cpm_stop_cim() \
	(REG_CPM_MSCR |= (CPM_MSCR_MSTP_CIM << CPM_MSCR_MSTP_BIT))
#define __cpm_stop_udc() \
	(REG_CPM_MSCR |= (CPM_MSCR_MSTP_UDC << CPM_MSCR_MSTP_BIT))
#define __cpm_stop_all() (REG_CPM_MSCR = 0xffffffff)

#define __cpm_start_uart(n) \
	(REG_CPM_MSCR &= ~(CPM_MSCR_MSTP_UART##n << CPM_MSCR_MSTP_BIT))
#define __cpm_start_pwm(n) \
	(REG_CPM_MSCR &= ~(CPM_MSCR_MSTP_PWM##n << CPM_MSCR_MSTP_BIT))
#define __cpm_start_aic(n) \
	(REG_CPM_MSCR &= ~(CPM_MSCR_MSTP_AIC##n << CPM_MSCR_MSTP_BIT))
#define __cpm_start_ost() \
	(REG_CPM_MSCR &= ~(CPM_MSCR_MSTP_OST << CPM_MSCR_MSTP_BIT))
#define __cpm_start_rtc() \
	(REG_CPM_MSCR &= ~(CPM_MSCR_MSTP_RTC << CPM_MSCR_MSTP_BIT))
#define __cpm_start_dmac() \
	(REG_CPM_MSCR &= ~(CPM_MSCR_MSTP_DMAC << CPM_MSCR_MSTP_BIT))
#define __cpm_start_uhc() \
	(REG_CPM_MSCR &= ~(CPM_MSCR_MSTP_UHC << CPM_MSCR_MSTP_BIT))
#define __cpm_start_lcd() \
	(REG_CPM_MSCR &= ~(CPM_MSCR_MSTP_LCD << CPM_MSCR_MSTP_BIT))
#define __cpm_start_i2c() \
	(REG_CPM_MSCR &= ~(CPM_MSCR_MSTP_I2C << CPM_MSCR_MSTP_BIT))
#define __cpm_start_ssi() \
	(REG_CPM_MSCR &= ~(CPM_MSCR_MSTP_SSI << CPM_MSCR_MSTP_BIT))
#define __cpm_start_msc() \
	(REG_CPM_MSCR &= ~(CPM_MSCR_MSTP_MSC << CPM_MSCR_MSTP_BIT))
#define __cpm_start_scc() \
	(REG_CPM_MSCR &= ~(CPM_MSCR_MSTP_SCC << CPM_MSCR_MSTP_BIT))
#define __cpm_start_fir() \
	(REG_CPM_MSCR &= ~(CPM_MSCR_MSTP_FIR << CPM_MSCR_MSTP_BIT))
#define __cpm_start_des() \
	(REG_CPM_MSCR &= ~(CPM_MSCR_MSTP_DES << CPM_MSCR_MSTP_BIT))
#define __cpm_start_eth() \
	(REG_CPM_MSCR &= ~(CPM_MSCR_MSTP_ETH << CPM_MSCR_MSTP_BIT))
#define __cpm_start_ps2() \
	(REG_CPM_MSCR &= ~(CPM_MSCR_MSTP_PS2 << CPM_MSCR_MSTP_BIT))
#define __cpm_start_cim() \
	(REG_CPM_MSCR &= ~(CPM_MSCR_MSTP_CIM << CPM_MSCR_MSTP_BIT))
#define __cpm_start_udc() \
	(REG_CPM_MSCR &= ~(CPM_MSCR_MSTP_UDC << CPM_MSCR_MSTP_BIT))
#define __cpm_start_all() (REG_CPM_MSCR = 0x00000000)


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

#define __ssi_set_frame_length(n) \
    REG_SSI_CR1 = (REG_SSI_CR1 & ~SSI_CR1_FLEN_MASK) | (((n) - 2) << 4) 

/* n = 1 - 16 */
#define __ssi_set_microwire_command_length(n) \
    ( REG_SSI_CR1 = ((REG_SSI_CR1 & ~SSI_CR1_MCOM_MASK) | SSI_CR1_MCOM_##n##BIT) )

/* Set the clock phase for SPI */
#define __ssi_set_spi_clock_phase(n) \
    ( REG_SSI_CR1 = ((REG_SSI_CR1 & ~SSI_CR1_PHA) | (n&0x1)) )

/* Set the clock polarity for SPI */
#define __ssi_set_spi_clock_polarity(n) \
    ( REG_SSI_CR1 = ((REG_SSI_CR1 & ~SSI_CR1_POL) | (n&0x1)) )

/* n = i*8 (i=0,1,2...,15) */
#define __ssi_set_tx_trigger(n) 		\
do { 						\
	REG_SSI_CR1 &= ~SSI_CR1_TTRG_MASK; 	\
	REG_SSI_CR1 |= (n>>3) << SSI_CR1_TTRG_BIT; 	\
} while (0)

/* n = i*8 (i=0,1,2...,15) */
#define __ssi_set_rx_trigger(n) 		\
do { 						\
	REG_SSI_CR1 &= ~SSI_CR1_RTRG_MASK; 	\
	REG_SSI_CR1 |= (n>>3) << SSI_CR1_RTRG_BIT; 	\
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

#endif /* __JZ5730_H__ */
