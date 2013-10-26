/*****************************************************************************
 *
 * JZC External DMA Channel for Data Transfer
 *
* $Id: rv_mdma_hw.h,v 1.5 2009/01/16 06:20:56 yzhai Exp $
 *
 ****************************************************************************/
#ifndef __RV_MDMA_HW__
#define __RV_MDMA_HW__

/*DDMA_GP base*/
#ifdef AUX_VADDR_SPACE
# define DDMA_GP0_VBASE 0x13A00000
# define DDMA_GP1_VBASE 0x13900000
#else
# define DDMA_GP0_VBASE 0xB3A00000
# define DDMA_GP1_VBASE 0xB3900000
#endif

/*DDMA_GP REG OFFSET*/
#define DDMA_GP_DHA 0x0
#define DDMA_GP_DCS 0x4
#  define DDMA_GP_DCS_BTN_SFT 0x10
#  define DDMA_GP_DCS_BTN_MSK 0xFFFF
#  define DDMA_GP_DCS_END_SFT 0x2
#  define DDMA_GP_DCS_END 0x1
#  define DDMA_GP_DCS_RST_SFT 0x1
#  define DDMA_GP_DCS_RST 0x1
#  define DDMA_GP_DCS_SUP_SFT 0x0
#  define DDMA_GP_DCS_SUP 0x1
#if 0
#define write_MDMAreg(reg, val) \
({ i_sw((val), (reg), 0);     \
})
#define read_MDMAreg(reg, off)  \
({ i_lw((reg), (off));        \
})
#else
#define write_MDMAreg(reg, val) \
({ *((volatile unsigned int *)(reg))=val;     \
})
#define read_MDMAreg(reg, off)  \
	({ *((volatile unsigned int *)(reg+off));    \
})
#endif
#define MDMA0_RUN()					\
({ write_MDMAreg((DDMA_GP0_VBASE+DDMA_GP_DCS), 0x1);	\
})

#define POLLING_MDMA0_END()					\
({ while(!(read_MDMAreg(DDMA_GP0_VBASE,DDMA_GP_DCS) & 0x4));	\
})

#define SET_MDMA0_DHA(mdma_dha)					\
({ write_MDMAreg((DDMA_GP0_VBASE+DDMA_GP_DHA), mdma_dha);	\
})

#define MDMA1_RUN()					\
({ write_MDMAreg((DDMA_GP1_VBASE+DDMA_GP_DCS), 0x1);	\
})

#define POLLING_MDMA1_END()					\
({ while(!(read_MDMAreg(DDMA_GP1_VBASE,DDMA_GP_DCS) & 0x4));	\
})

#define SET_MDMA1_DHA(mdma_dha)					\
({ write_MDMAreg((DDMA_GP1_VBASE+DDMA_GP_DHA), mdma_dha);	\
})

/*DDMA_GP Descriptor-Node*/
/*
     | bit31 | bit30......bit16 | bit15......bit0 |
     +--------------------------------------------+
   0 |<--------          TSA       -------------->|
     +--------------------------------------------+
   4 |<--------          TDA       -------------->|
     +--------------------------------------------+
   8 |<-  TST                 ->|<-  TDT   ------>|
     +--------------------------------------------+
  12 |<-TAG->|<-    TRN       ->|<-  NUM        ->|
     +--------------------------------------------+         
  
  Note:
    TSA: source address
    TDA: destination address
    TST: source stride
    TDT: destination stride
    TAG: Current node Link or Not
    TRN: ROW width for DMA(unit: byte)
    NUM: Total for DMA(unit: byte)   
 */
#define U32 unsigned int
typedef struct RV_MDMA_DesNode{
  U32  TSA;
  U32  TDA;
  U32  STRD;   /*{TST,TDT}*/
  U32  UNIT;   /*{TAG,TRN,NUM}*/
}RV_MDMA_DesNode;

#define MDMA_STRD(TST,TDT)         ( (((TST) & 0xFFFF)<<16) | ((TDT) & 0xFFFF))
#define MDMA_UNIT(TAG,TRN,NUM)     ( (((TAG) & 0x1)<<31) | (((TRN) & 0x7FFF)<<16) | ((NUM) & 0xFFFF))

#endif /*__RV_MDMA_HW__*/
