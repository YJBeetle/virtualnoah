/*
 * QEMU JZ Soc emulation
 *
 * Copyright (c) 2009 yajin (yajin@vm-kernel.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


/*
 * The emulation target is pavo demo board.
 *  http://www.ingenic.cn/eng/productServ/kfyd/Hardware/pffaqQuestionContent.aspx?Category=2&Question=3
 *
 */

#include "hw.h"
#include "mips.h"
#include "sysemu.h"
#include "qemu-timer.h"
#include "qemu-char.h"
#include "flash.h"
#include "soc_dma.h"
#include "audio/audio.h"
#include "pc.h"
#include "osdep.h"
#include "mips_jz.h"
#include "console.h"

uint32_t jz4740_badwidth_read8(void *opaque, target_phys_addr_t addr)
{
    uint8_t ret;

    JZ4740_8B_REG(addr);
    cpu_physical_memory_read(addr, (void *) &ret, 1);
    return ret;
}

void jz4740_badwidth_write8(void *opaque, target_phys_addr_t addr,
                            uint32_t value)
{
    uint8_t val8 = value;

    JZ4740_8B_REG(addr);
    cpu_physical_memory_write(addr, (void *) &val8, 1);
}

uint32_t jz4740_badwidth_read16(void *opaque, target_phys_addr_t addr)
{
    uint16_t ret;
    JZ4740_16B_REG(addr);
    cpu_physical_memory_read(addr, (void *) &ret, 2);
    return ret;
}

void jz4740_badwidth_write16(void *opaque, target_phys_addr_t addr,
                             uint32_t value)
{
    uint16_t val16 = value;

    JZ4740_16B_REG(addr);
    cpu_physical_memory_write(addr, (void *) &val16, 2);
}

uint32_t jz4740_badwidth_read32(void *opaque, target_phys_addr_t addr)
{
    uint32_t ret;

    JZ4740_32B_REG(addr);
    cpu_physical_memory_read(addr, (void *) &ret, 4);
    return ret;
}

void jz4740_badwidth_write32(void *opaque, target_phys_addr_t addr,
                             uint32_t value)
{
    JZ4740_32B_REG(addr);
    cpu_physical_memory_write(addr, (void *) &value, 4);
}


/*clock reset and power control*/
struct jz4740_cpm_s
{
    target_phys_addr_t base;
    struct jz_state_s *soc;

    uint32_t cpccr;
    uint32_t cppcr;
    uint32_t i2scdr;
    uint32_t lpcdr;
    uint32_t msccdr;
    uint32_t uhccdr;
    uint32_t uhctst;
    uint32_t ssicdr;

    uint32_t lcr;
    uint32_t clkgr;
    uint32_t scr;
};

static void jz4740_dump_clocks(jz_clk parent)
{
    jz_clk i = parent;

    for (i = i->child1; i; i = i->sibling)
        jz4740_dump_clocks(i);
}

static inline void jz4740_cpccr_update(struct jz4740_cpm_s *s,
                                       uint32_t new_value)
{
    uint32_t ldiv, mdiv, pdiv, hdiv, cdiv, udiv;
    uint32_t div_table[10] = {
        1, 2, 3, 4, 6, 8, 12, 16, 24, 32
    };

    if (unlikely(new_value == s->cpccr))
        return;

    if (new_value & CPM_CPCCR_PCS)
        jz_clk_setrate(jz_findclk(s->soc, "pll_divider"), 1, 1);
    else
        jz_clk_setrate(jz_findclk(s->soc, "pll_divider"), 2, 1);


    ldiv = (new_value & CPM_CPCCR_LDIV_MASK) >> CPM_CPCCR_LDIV_BIT;
    ldiv++;

    mdiv = div_table[(new_value & CPM_CPCCR_MDIV_MASK) >> CPM_CPCCR_MDIV_BIT];
    pdiv = div_table[(new_value & CPM_CPCCR_PDIV_MASK) >> CPM_CPCCR_PDIV_BIT];
    hdiv = div_table[(new_value & CPM_CPCCR_HDIV_MASK) >> CPM_CPCCR_HDIV_BIT];
    cdiv = div_table[(new_value & CPM_CPCCR_CDIV_MASK) >> CPM_CPCCR_CDIV_BIT];
    udiv = div_table[(new_value & CPM_CPCCR_UDIV_MASK) >> CPM_CPCCR_UDIV_BIT];

    jz_clk_setrate(jz_findclk(s->soc, "ldclk"), ldiv, 1);
    jz_clk_setrate(jz_findclk(s->soc, "mclk"), mdiv, 1);
    jz_clk_setrate(jz_findclk(s->soc, "pclk"), pdiv, 1);
    jz_clk_setrate(jz_findclk(s->soc, "hclk"), hdiv, 1);
    jz_clk_setrate(jz_findclk(s->soc, "cclk"), cdiv, 1);
    jz_clk_setrate(jz_findclk(s->soc, "usbclk"), udiv, 1);

    if (new_value & CPM_CPCCR_UCS)
        jz_clk_reparent(jz_findclk(s->soc, "usbclk"),
                        jz_findclk(s->soc, "pll_divider"));
    else
        jz_clk_reparent(jz_findclk(s->soc, "usbclk"),
                        jz_findclk(s->soc, "osc_extal"));

    if (new_value & CPM_CPCCR_I2CS)
        jz_clk_reparent(jz_findclk(s->soc, "i2sclk"),
                        jz_findclk(s->soc, "pll_divider"));
    else
        jz_clk_reparent(jz_findclk(s->soc, "i2sclk"),
                        jz_findclk(s->soc, "osc_extal"));

    s->cpccr = new_value;

    jz4740_dump_clocks(jz_findclk(s->soc, "osc_extal"));

}

static inline void jz4740_cppcr_update(struct jz4740_cpm_s *s,
                                       uint32_t new_value)
{
    uint32_t pllm, plln, pllod, pllbp, pllen;
    uint32_t pll0[4] = {
        1, 2, 2, 4
    };


    pllen = new_value & CPM_CPPCR_PLLEN;
    pllbp = new_value & CPM_CPPCR_PLLBP;
    if ((!pllen) || (pllen && pllbp))
    {
        jz_clk_setrate(jz_findclk(s->soc, "pll_output"), 1, 1);
        s->cppcr = new_value | CPM_CPPCR_PLLS;
        return;
    }


    pllm = (new_value & CPM_CPPCR_PLLM_MASK) >> CPM_CPPCR_PLLM_BIT;
    plln = (new_value & CPM_CPPCR_PLLN_MASK) >> CPM_CPPCR_PLLN_BIT;
    pllod = (new_value & CPM_CPPCR_PLLOD_MASK) >> CPM_CPPCR_PLLOD_BIT;
    jz_clk_setrate(jz_findclk(s->soc, "pll_output"), (plln + 2) * pll0[pllod],
                   pllm + 2);

    s->cppcr = new_value;

    jz4740_dump_clocks(jz_findclk(s->soc, "osc_extal"));

}

static inline void jz4740_i2scdr_update(struct jz4740_cpm_s *s,
                                        uint32_t new_value)
{
    uint32_t i2scdr;

    i2scdr = new_value & CPM_I2SCDR_I2SDIV_MASK;
    if (unlikely(i2scdr == s->i2scdr))
        return;


    jz_clk_setrate(jz_findclk(s->soc, "i2sclk"), i2scdr + 1, 1);

    s->i2scdr = i2scdr;

    jz4740_dump_clocks(jz_findclk(s->soc, "osc_extal"));

}

static inline void jz4740_lpcdr_update(struct jz4740_cpm_s *s,
                                       uint32_t new_value)
{
    uint32_t ipcdr;

    ipcdr = new_value & CPM_LPCDR_PIXDIV_MASK;
    /*TODO: */
    s->lpcdr = ipcdr;
}

static inline void jz4740_msccdr_update(struct jz4740_cpm_s *s,
                                        uint32_t new_value)
{
    uint32_t msccdr;

    msccdr = new_value & CPM_MSCCDR_MSCDIV_MASK;

    if (unlikely(msccdr == s->msccdr))
        return;


    jz_clk_setrate(jz_findclk(s->soc, "mscclk"), msccdr + 1, 1);

    s->msccdr = msccdr;

    jz4740_dump_clocks(jz_findclk(s->soc, "osc_extal"));

}

static inline void jz4740_uhccdr_update(struct jz4740_cpm_s *s,
                                        uint32_t new_value)
{
    uint32_t uhccdr;

    uhccdr = new_value & 0xf;
    /*TODO: */
    s->uhccdr = uhccdr;
}

static void jz4740_cpm_write(void *opaque, target_phys_addr_t addr,
                             uint32_t value)
{
    struct jz4740_cpm_s *s = (struct jz4740_cpm_s *) opaque;

    switch (addr)
    {
    case 0x0:
        jz4740_cpccr_update(s, value);
        break;
    case 0x4:
        s->lcr = value & 0xff;
        break;
    case 0x20:
        s->clkgr = value & 0xffff;
        break;
    case 0x24:
        s->scr = value & 0xffff;
        break;
    case 0x10:
        jz4740_cppcr_update(s, value);
        break;
    case 0x60:
        jz4740_i2scdr_update(s, value);
        break;
    case 0x64:
        jz4740_lpcdr_update(s, value);
        break;
    case 0x68:
        jz4740_msccdr_update(s, value);
        break;
    case 0x6c:
        jz4740_uhccdr_update(s, value);
        break;
    case 0x70:
        s->uhctst = value & 0x3f;
        break;
    case 0x74:
        s->ssicdr = value & 0xf;
        break;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_cpm_write undefined addr " JZ_FMT_plx
                  "  value %x \n", addr, value);
    }

}


static uint32_t jz474_cpm_read(void *opaque, target_phys_addr_t addr)
{
    struct jz4740_cpm_s *s = (struct jz4740_cpm_s *) opaque;

    switch (addr)
    {
    case 0x0:
        return s->cpccr;
    case 0x4:
        return s->lcr;
    case 0x20:
        return s->clkgr;
    case 0x24:
        return s->scr;
    case 0x10:
        return s->cppcr;
    case 0x60:
        return s->i2scdr;
    case 0x64:
        return s->lpcdr;
    case 0x68:
        return s->msccdr;
    case 0x6c:
        return s->uhccdr;
    case 0x70:
        return s->uhctst;
    case 0x74:
        return s->ssicdr;
    default:
        cpu_abort(s->soc->env,
                  "jz474_cpm_read undefined addr " JZ_FMT_plx "  \n", addr);
    }

}



static CPUReadMemoryFunc *jz4740_cpm_readfn[] = {
    jz4740_badwidth_read32, 
    jz4740_badwidth_read32, 
    jz474_cpm_read,
};

static CPUWriteMemoryFunc *jz4740_cpm_writefn[] = {
    jz4740_badwidth_write32, 
    jz4740_badwidth_write32, 
    jz4740_cpm_write,
};

static void jz4740_cpm_reset(struct jz4740_cpm_s *s)
{
    s->cpccr = 0x42040000;
    s->cppcr = 0x28080011;
    s->i2scdr = 0x00000004;
    s->lpcdr = 0x00000004;
    s->msccdr = 0x00000004;
    s->uhccdr = 0x00000004;
    s->uhctst = 0x0;
    s->ssicdr = 0x00000004;

    s->lcr = 0xf8;
    s->clkgr = 0x0;
    s->scr = 0x1500;
}

static struct jz4740_cpm_s *jz4740_cpm_init(struct jz_state_s *soc)
{
    int iomemtype;
    struct jz4740_cpm_s *s = (struct jz4740_cpm_s *) qemu_mallocz(sizeof(*s));
    s->base = JZ4740_PHYS_BASE(JZ4740_CPM_BASE);
    s->soc = soc;

    jz4740_cpm_reset(s);

    iomemtype =
        cpu_register_io_memory(0, jz4740_cpm_readfn, jz4740_cpm_writefn, s);
    cpu_register_physical_memory(s->base, 0x00001000, iomemtype);
    return s;
}


/* JZ4740 interrupt controller
  * It issues INT2 to MIPS
  */
struct jz4740_intc_s
{
    qemu_irq parent_irq;

    target_phys_addr_t base;
    struct jz_state_s *soc;

    uint32_t icsr;
    uint32_t icmr;
    uint32_t icmsr;
    uint32_t icmcr;
    uint32_t icpr;
};

static uint32_t jz4740_intc_read(void *opaque, target_phys_addr_t addr)
{
    struct jz4740_intc_s *s = (struct jz4740_intc_s *) opaque;

    switch (addr)
    {
    case 0x8:
    case 0xc:
        JZ4740_WO_REG(addr);
        break;
    case 0x0:
        return s->icsr;
    case 0x4:
        return s->icmr;
    case 0x10:
        return s->icpr;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_intc_read undefined addr " JZ_FMT_plx "  \n", addr);

    }
    return (0);
}

static void jz4740_intc_write(void *opaque, target_phys_addr_t addr,
                              uint32_t value)
{
    struct jz4740_intc_s *s = (struct jz4740_intc_s *) opaque;

    switch (addr)
    {
    case 0x0:
        JZ4740_RO_REG(addr);
        break;
    case 0x4:
        s->icmr = value;
        break;
    case 0x8:
        s->icmr |= value;
        break;
    case 0xc:
        s->icmr &= ~value;
        break;
    case 0x10:
        s->icpr &= ~value;
        qemu_set_irq(s->parent_irq, 0);
        break;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_intc_write undefined addr " JZ_FMT_plx
                  "  value %x \n", addr, value);
    }
}


static CPUReadMemoryFunc *jz4740_intc_readfn[] = {
    jz4740_badwidth_read32, 
    jz4740_badwidth_read32, 
    jz4740_intc_read,
};

static CPUWriteMemoryFunc *jz4740_intc_writefn[] = {
    jz4740_badwidth_write32, 
    jz4740_badwidth_write32, 
    jz4740_intc_write,
};

static void jz4740_intc_reset(struct jz4740_intc_s *s)
{
    s->icsr = 0x0;
    s->icmr = 0xffffffff;
    s->icpr = 0x0;
}

static void jz4740_set_irq(void *opaque, int irq, int level)
{
    struct jz4740_intc_s *s = (struct jz4740_intc_s *) opaque;
    uint32_t irq_mask = 1 << irq;


    if (level)
    {
        s->icsr |= irq_mask;
        s->icpr &= ~irq_mask;
        if (!(s->icmr & irq_mask))
        {
            s->icpr |= irq_mask;
            qemu_set_irq(s->parent_irq, 1);
        }
    }

}

static qemu_irq *jz4740_intc_init(struct jz_state_s *soc, qemu_irq parent_irq)
{
    int iomemtype;
    struct jz4740_intc_s *s = (struct jz4740_intc_s *) qemu_mallocz(sizeof(*s));
    s->base = JZ4740_PHYS_BASE(JZ4740_INTC_BASE);
    s->parent_irq = parent_irq;
    s->soc = soc;

    jz4740_intc_reset(s);

    iomemtype =
        cpu_register_io_memory(0, jz4740_intc_readfn, jz4740_intc_writefn, s);
    cpu_register_physical_memory(s->base, 0x00001000, iomemtype);
    return qemu_allocate_irqs(jz4740_set_irq, s, 32);
}

/*external memory controller*/
struct jz4740_emc_s
{
    qemu_irq irq;
    target_phys_addr_t base;
    struct jz_state_s *soc;

    uint32_t bcr;
    uint32_t smcr1;             /*0x13010014 */
    uint32_t smcr2;             /*0x13010018 */
    uint32_t smcr3;             /*0x1301001c */
    uint32_t smcr4;             /*0x13010020 */
    uint32_t sacr1;             /*0x13010034 */
    uint32_t sacr2;             /*0x13010038 */
    uint32_t sacr3;             /*0x1301003c */
    uint32_t sacr4;             /*0x13010040 */

    uint32_t nfcsr;             /*0x13010050 */
    uint32_t nfeccr;            /*0x13010100 */
    uint32_t nfecc;             /*0x13010104 */
    uint32_t nfpar0;            /*0x13010108 */
    uint32_t nfpar1;            /*0x1301010c */
    uint32_t nfpar2;            /*0x13010110 */
    uint32_t nfints;            /*0x13010114 */
    uint32_t nfinte;            /*0x13010118 */
    uint32_t nferr0;            /*0x1301011c */
    uint32_t nferr1;            /*0x13010120 */
    uint32_t nferr2;            /*0x13010124 */
    uint32_t nferr3;            /*0x13010128 */

    uint32_t dmcr;              /*0x13010080 */
    uint32_t rtcsr;             /*0x13010084 */
    uint32_t rtcnt;             /*0x13010088 */
    uint32_t rtcor;             /*0x1301008c */
    uint32_t dmar;              /*0x13010090 */
    uint32_t sdmr;              /*0x1301a000 */

};


static void jz4740_emc_reset(struct jz4740_emc_s *s)
{
    s->smcr1 = 0xfff7700;
    s->smcr2 = 0xfff7700;
    s->smcr3 = 0xfff7700;
    s->smcr4 = 0xfff7700;
    s->sacr1 = 0x18fc;
    s->sacr2 = 0x16fe;
    s->sacr3 = 0x14fe;
    s->sacr4 = 0xcfc;

    s->nfcsr = 0x0;
    s->nfeccr = 0x0;
    s->nfecc = 0x0;
    s->nfpar0 = 0x0;
    s->nfpar1 = 0x0;
    s->nfpar2 = 0x0;
    s->nfints = 0x0;
    s->nfinte = 0x0;
    s->nferr0 = 0x0;
    s->nferr1 = 0x0;
    s->nferr2 = 0x0;
    s->nferr3 = 0x0;

    s->dmcr = 0x0;
    s->rtcsr = 0x0;
    s->rtcnt = 0x0;
    s->rtcor = 0x0;
    s->dmar = 0x20f8;
    s->sdmr = 0x0;

}

static uint32_t jz4740_emc_read8(void *opaque, target_phys_addr_t addr)
{
    struct jz4740_emc_s *s = (struct jz4740_emc_s *) opaque;

    switch (addr)
    {
    case 0x108:
    case 0x109:
    case 0x10a:
    case 0x10b:
        return (s->nfpar0 >> ((addr - 0x108) * 8)) & 0xff;
    case 0x10c:
    case 0x10d:
    case 0x10e:
    case 0x10f:
        return (s->nfpar1 >> ((addr - 0x10c) * 8)) & 0xff;
    case 0x110:
    case 0x111:
    case 0x112:
    case 0x113:
        return (s->nfpar2 >> ((addr - 0x110) * 8)) & 0xff;
    case 0xa000:
    case 0xa001:
    case 0xa002:
    case 0xa003:
        return (s->sdmr >> ((addr - 0xa000) * 8)) & 0xff;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_emc_read8 undefined addr " JZ_FMT_plx " \n", addr);


    }
    return (0);
}

static uint32_t jz4740_emc_read16(void *opaque, target_phys_addr_t addr)
{
    struct jz4740_emc_s *s = (struct jz4740_emc_s *) opaque;

    switch (addr)
    {
    case 0x108:
    case 0x10a:
        return (s->nfpar0 >> ((addr - 0x108) * 8)) & 0xffff;
    case 0x10c:
    case 0x10e:
        return (s->nfpar1 >> ((addr - 0x10c) * 8)) & 0xffff;
    case 0x110:
    case 0x112:
        return (s->nfpar2 >> ((addr - 0x110) * 8)) & 0xffff;
    case 0x11c:
    case 0x11e:
        return (s->nferr0 >> ((addr - 0x11c) * 8)) & 0xffff;
    case 0x120:
    case 0x122:
        return (s->nferr1 >> ((addr - 0x120) * 8)) & 0xffff;
    case 0x124:
    case 0x126:
        return (s->nferr2 >> ((addr - 0x124) * 8)) & 0xffff;
    case 0x128:
    case 0x12a:
        return (s->nferr3 >> ((addr - 0x128) * 8)) & 0xffff;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_emc_read16 undefined addr " JZ_FMT_plx " \n", addr);
    }
    return (0);
}

static uint32_t jz4740_emc_read32(void *opaque, target_phys_addr_t addr)
{
    struct jz4740_emc_s *s = (struct jz4740_emc_s *) opaque;

    switch (addr)
    {
    case 0x0:
        return s->bcr;
    case 0x14:
        return s->smcr1;
    case 0x18:
        return s->smcr2;
    case 0x1c:
        return s->smcr3;
    case 0x20:
        return s->smcr4;
    case 0x34:
        return s->sacr1;
    case 0x38:
        return s->sacr2;
    case 0x3c:
        return s->sacr3;
    case 0x40:
        return s->sacr4;
    case 0x50:
        return s->nfcsr;
    case 0x100:
        return s->nfeccr;
    case 0x104:
        return s->nfecc;
    case 0x108:
        return s->nfpar0;
    case 0x10c:
        return s->nfpar1;
    case 0x110:
        return s->nfpar2;
    case 0x114:
        return s->nfints;
    case 0x118:
        return s->nfinte;
    case 0x11c:
        return s->nferr0;
    case 0x120:
        return s->nferr1;
    case 0x124:
        return s->nferr2;
    case 0x128:
        return s->nferr3;
    case 0x80:
        return s->dmcr;
    case 0x90:
        return s->dmar;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_emc_read32 undefined addr " JZ_FMT_plx " \n", addr);
    }
    return (0);
}

static void jz4740_emc_write8(void *opaque, target_phys_addr_t addr,
                              uint32_t value)
{
    struct jz4740_emc_s *s = (struct jz4740_emc_s *) opaque;

    switch (addr)
    {
    case 0x108:
    case 0x109:
    case 0x10a:
    case 0x10b:
        s->nfpar0 |= (value & 0xff) << ((addr - 0x108) * 8);
        break;
    case 0x10c:
    case 0x10d:
    case 0x10e:
    case 0x10f:
        s->nfpar1 |= (value & 0xff) << ((addr - 0x10c) * 8);
        break;
    case 0x110:
    case 0x111:
    case 0x112:
    case 0x113:
        s->nfpar2 |= (value & 0xff) << ((addr - 0x110) * 8);
        break;
    case 0xa000 ... 0xa3ff:
        break;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_emc_write8 undefined addr " JZ_FMT_plx
                  "  value %x \n", addr, value);
    }
}
static void jz4740_emc_write16(void *opaque, target_phys_addr_t addr,
                               uint32_t value)
{
    struct jz4740_emc_s *s = (struct jz4740_emc_s *) opaque;

    switch (addr)
    {
    case 0x108:
    case 0x10a:
        s->nfpar0 |= (value & 0xffff) << ((addr - 0x108) * 8);
        break;
    case 0x10c:
    case 0x10e:
        s->nfpar1 |= (value & 0xffff) << ((addr - 0x10c) * 8);
        break;
    case 0x110:
    case 0x112:
        s->nfpar2 |= (value & 0xffff) << ((addr - 0x110) * 8);
        break;
    case 0x84:
    case 0x86:
        s->rtcsr |= (value & 0xffff) << ((addr - 0x84) * 8);
        break;
    case 0x88:
    case 0x8a:
        s->rtcnt |= (value & 0xffff) << ((addr - 0x88) * 8);
        break;
    case 0x8c:
        s->rtcor |= (value & 0xffff) << ((addr - 0x8c) * 8);
        break;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_emc_write16 undefined addr " JZ_FMT_plx
                  "  value %x \n", addr, value);
    }
}

static void jz4740_emc_upate_interrupt(struct jz4740_emc_s *s)
{
    qemu_set_irq(s->irq, s->nfints & s->nfinte);
}

static void jz4740_emc_write32(void *opaque, target_phys_addr_t addr,
                               uint32_t value)
{
    struct jz4740_emc_s *s = (struct jz4740_emc_s *) opaque;

    switch (addr)
    {
    case 0x104:
    case 0x11c:
    case 0x120:
    case 0x124:
    case 0x128:
        JZ4740_RO_REG(addr);
        break;
    case 0x0:
        s->bcr = value;
        break;
    case 0x14:
        s->smcr1 = value & 0xfff77cf;
        break;
    case 0x18:
        s->smcr2 = value & 0xfff77cf;
        break;
    case 0x1c:
        s->smcr3 = value & 0xfff77cf;
        break;
    case 0x20:
        s->smcr4 = value & 0xfff77cf;
        break;
    case 0x34:
        s->sacr1 = value & 0xffff;
        break;
    case 0x38:
        s->sacr2 = value & 0xffff;
        break;
    case 0x3c:
        s->sacr3 = value & 0xffff;
        break;
    case 0x40:
        s->sacr4 = value & 0xffff;
        break;
    case 0x50:
        s->nfcsr = value & 0xffff;
        break;
    case 0x100:
        s->nfeccr = value & 0x1f;
        if (s->nfeccr & 0x2)
        {
            s->nfecc = 0x0;
            s->nfpar0 = 0x0;
            s->nfpar1 = 0x0;
            s->nfpar2 = 0x0;
            s->nfints = 0x0;
            s->nfinte = 0x0;
            s->nferr0 = 0x0;
            s->nferr1 = 0x0;
            s->nferr2 = 0x0;
            s->nferr3 = 0x0;
        }
         /*RS*/
            /*TODO: Real RS error correction */
            if (s->nfeccr & 0x4)
        {
            if ((s->nfeccr & 0x10) && (!(s->nfeccr & 0x8)))
            {
                /*decode */
                s->nfints = 0x8;
                s->nferr0 = 0x0;
                s->nferr1 = 0x0;
                s->nferr2 = 0x0;
            }
            if (s->nfeccr & 0x8)
            {
                /*encoding */
                s->nfints = 0x4;
                s->nfpar0 = 0xffffffff; /*fake value. for debug */
                s->nfpar1 = 0xffffffff; /*fake value */
                s->nfpar2 = 0xff;       /*fake value */
            }
        }
        else
        {
            s->nfecc = 0xffffff;
        }
        jz4740_emc_upate_interrupt(s);
        break;
    case 0x108:
        s->nfpar0 = value;
        break;
    case 0x10c:
        s->nfpar1 = value;
        break;
    case 0x110:
        s->nfpar2 = value & 0xff;
        break;
    case 0x114:
        s->nfints = value & 0x1fffffff;
        jz4740_emc_upate_interrupt(s);
        break;
    case 0x118:
        s->nfinte = value & 0x1f;
        jz4740_emc_upate_interrupt(s);
        break;
    case 0x080:
        s->dmcr = value & 0x9fbeff7f;
        break;
    case 0x90:
        s->dmar = value & 0xffff;
        break;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_emc_write32 undefined addr " JZ_FMT_plx
                  "  value %x \n", addr, value);

    }

}

static CPUReadMemoryFunc *jz4740_emc_readfn[] = {
    jz4740_emc_read8, 
    jz4740_emc_read16, 
    jz4740_emc_read32,
};

static CPUWriteMemoryFunc *jz4740_emc_writefn[] = {
    jz4740_emc_write8, 
    jz4740_emc_write16, 
    jz4740_emc_write32,
};


static struct jz4740_emc_s *jz4740_emc_init(struct jz_state_s *soc,
                                            qemu_irq irq)
{
    int iomemtype;
    struct jz4740_emc_s *s = (struct jz4740_emc_s *) qemu_mallocz(sizeof(*s));
    s->base = JZ4740_PHYS_BASE(JZ4740_EMC_BASE);
    s->soc = soc;
    s->irq = irq;

    jz4740_emc_reset(s);

    iomemtype =
        cpu_register_io_memory(0, jz4740_emc_readfn, jz4740_emc_writefn, s);
    cpu_register_physical_memory(s->base, 0x00010000, iomemtype);
    return s;

}

struct jz4740_gpio_s
{
    qemu_irq irq;
    target_phys_addr_t base;
    struct jz_state_s *soc;

    uint32_t papin[4];
    uint32_t padat[4];
    uint32_t paim[4];
    uint32_t pape[4];
    uint32_t pafun[4];
    uint32_t pasel[4];
    uint32_t padir[4];
    uint32_t patrg[4];
    uint32_t paflg[4];
};

static void jz4740_gpio_reset(struct jz4740_gpio_s *s)
{
    memset(s->papin, 0x0, sizeof(s->papin));
    memset(s->padat, 0x0, sizeof(s->padat));
    memset(s->paim, 0xffffffff, sizeof(s->paim));
    memset(s->pape, 0x0, sizeof(s->pape));
    memset(s->pafun, 0x0, sizeof(s->pafun));
    memset(s->pasel, 0x0, sizeof(s->pasel));
    memset(s->padir, 0x0, sizeof(s->padir));
    memset(s->patrg, 0x0, sizeof(s->patrg));
    memset(s->paflg, 0x0, sizeof(s->paflg));
}

static uint32_t jz4740_gpio_read(void *opaque, target_phys_addr_t addr)
{
    struct jz4740_gpio_s *s = (struct jz4740_gpio_s *) opaque;
    uint32_t group;

    switch (addr)
    {
    case 0x14:
    case 0x114:
    case 0x214:
    case 0x314:
    case 0x18:
    case 0x118:
    case 0x218:
    case 0x318:
    case 0x24:
    case 0x124:
    case 0x224:
    case 0x324:
    case 0x28:
    case 0x128:
    case 0x228:
    case 0x328:
    case 0x34:
    case 0x134:
    case 0x234:
    case 0x334:
    case 0x38:
    case 0x138:
    case 0x238:
    case 0x338:
    case 0x44:
    case 0x144:
    case 0x244:
    case 0x344:
    case 0x48:
    case 0x148:
    case 0x248:
    case 0x348:
    case 0x54:
    case 0x154:
    case 0x254:
    case 0x354:
    case 0x58:
    case 0x158:
    case 0x258:
    case 0x358:
    case 0x64:
    case 0x164:
    case 0x264:
    case 0x364:
    case 0x68:
    case 0x168:
    case 0x268:
    case 0x368:
    case 0x74:
    case 0x174:
    case 0x274:
    case 0x374:
    case 0x78:
    case 0x178:
    case 0x278:
    case 0x378:
    case 0x84:
    case 0x184:
    case 0x284:
    case 0x384:
        JZ4740_WO_REG(addr);
        break;

    case 0x0:
    case 0x100:
    case 0x200:
    case 0x300:
        group = (addr - 0x0) / 0x100;
        if (addr == 0x200)
        {
            /*GPIO(C) PIN 30  -> NAND FLASH R/B. */
            /*FOR NAND FLASH.PIN 30 ----|_____|------ */
            s->papin[2] &= 0x40000000;
            if (s->papin[2])
                s->papin[2] &= ~0x40000000;
            else
                s->papin[2] |= 0x40000000;
        }
        return s->papin[group];
    case 0x10:
    case 0x110:
    case 0x210:
    case 0x310:
        group = (addr - 0x10) / 0x100;
        return s->padat[group];
    case 0x20:
    case 0x120:
    case 0x220:
    case 0x320:
        group = (addr - 0x20) / 0x100;
        return s->paim[group];
    case 0x30:
    case 0x130:
    case 0x230:
    case 0x330:
        group = (addr - 0x30) / 0x100;
        return s->pape[group];
    case 0x40:
    case 0x140:
    case 0x240:
    case 0x340:
        group = (addr - 0x40) / 0x100;
        return s->pafun[group];
    case 0x50:
    case 0x150:
    case 0x250:
    case 0x350:
        group = (addr - 0x50) / 0x100;
        return s->pasel[group];
    case 0x60:
    case 0x160:
    case 0x260:
    case 0x360:
        group = (addr - 0x60) / 0x100;
        return s->padir[group];
    case 0x70:
    case 0x170:
    case 0x270:
    case 0x370:
        group = (addr - 0x70) / 0x100;
        return s->patrg[group];
    case 0x80:
    case 0x180:
    case 0x280:
    case 0x380:
        group = (addr - 0x80) / 0x100;
        return s->paflg[group];
    default:
        cpu_abort(s->soc->env,
                  "jz4740_gpio_read undefined addr " JZ_FMT_plx " \n", addr);
    }
    return (0);
}

static void jz4740_gpio_write(void *opaque, target_phys_addr_t addr,
                              uint32_t value)
{
    struct jz4740_gpio_s *s = (struct jz4740_gpio_s *) opaque;
    uint32_t group;

    switch (addr)
    {
    case 0x0:
    case 0x100:
    case 0x200:
    case 0x300:
    case 0x10:
    case 0x110:
    case 0x210:
    case 0x310:
    case 0x20:
    case 0x120:
    case 0x220:
    case 0x320:
    case 0x30:
    case 0x130:
    case 0x230:
    case 0x330:
    case 0x40:
    case 0x140:
    case 0x240:
    case 0x340:
    case 0x50:
    case 0x150:
    case 0x250:
    case 0x350:
    case 0x60:
    case 0x160:
    case 0x260:
    case 0x360:
    case 0x70:
    case 0x170:
    case 0x270:
    case 0x370:
    case 0x80:
    case 0x180:
    case 0x280:
    case 0x380:
        JZ4740_RO_REG(addr);
        break;
    case 0x14:
    case 0x114:
    case 0x214:
    case 0x314:
        group = (addr - 0x14) / 0x100;
        s->padat[group] = value;
        break;
    case 0x18:
    case 0x118:
    case 0x218:
    case 0x318:
        group = (addr - 0x18) / 0x100;
        s->padat[group] &= ~value;
        break;
    case 0x24:
    case 0x124:
    case 0x224:
    case 0x324:
        group = (addr - 0x24) / 0x100;
        s->paim[group] = value;
        break;
    case 0x28:
    case 0x128:
    case 0x228:
    case 0x328:
        group = (addr - 0x28) / 0x100;
        s->paim[group] &= ~value;
        break;
    case 0x34:
    case 0x134:
    case 0x234:
    case 0x334:
        group = (addr - 0x34) / 0x100;
        s->pape[group] = value;
        break;
    case 0x38:
    case 0x138:
    case 0x238:
    case 0x338:
        group = (addr - 0x38) / 0x100;
        s->pape[group] &= ~value;
        break;
    case 0x44:
    case 0x144:
    case 0x244:
    case 0x344:
        group = (addr - 0x44) / 0x100;
        s->pafun[group] = value;
        break;
    case 0x48:
    case 0x148:
    case 0x248:
    case 0x348:
        group = (addr - 0x48) / 0x100;
        s->pafun[group] &= ~value;
        break;
    case 0x54:
    case 0x154:
    case 0x254:
    case 0x354:
        group = (addr - 0x54) / 0x100;
        s->pasel[group] = value;
        break;
    case 0x58:
    case 0x158:
    case 0x258:
    case 0x358:
        group = (addr - 0x58) / 0x100;
        s->pasel[group] &= ~value;
        break;
    case 0x64:
    case 0x164:
    case 0x264:
    case 0x364:
        group = (addr - 0x64) / 0x100;
        s->padir[group] = value;
        break;
    case 0x68:
    case 0x168:
    case 0x268:
    case 0x368:
        group = (addr - 0x68) / 0x100;
        s->padir[group] &= ~value;
        break;
    case 0x74:
    case 0x174:
    case 0x274:
    case 0x374:
        group = (addr - 0x74) / 0x100;
        s->patrg[group] = value;
        break;
    case 0x78:
    case 0x178:
    case 0x278:
    case 0x378:
        group = (addr - 0x78) / 0x100;
        s->patrg[group] &= ~value;
        break;
    case 0x84:
    case 0x184:
    case 0x284:
    case 0x384:
        group = (addr - 0x74) / 0x100;
        s->paflg[group] &= ~value;
        break;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_gpio_write undefined addr " JZ_FMT_plx
                  "  value %x \n", addr, value);

    }


}


static CPUReadMemoryFunc *jz4740_gpio_readfn[] = {
    jz4740_badwidth_read32, 
    jz4740_badwidth_read32, 
    jz4740_gpio_read,
};

static CPUWriteMemoryFunc *jz4740_gpio_writefn[] = {
    jz4740_badwidth_write32, 
    jz4740_badwidth_write32, 
    jz4740_gpio_write,
};

static struct jz4740_gpio_s *jz4740_gpio_init(struct jz_state_s *soc,
                                              qemu_irq irq)
{
    int iomemtype;
    struct jz4740_gpio_s *s = (struct jz4740_gpio_s *) qemu_mallocz(sizeof(*s));
    s->base = JZ4740_PHYS_BASE(JZ4740_GPIO_BASE);
    s->soc = soc;
    s->irq = irq;

    jz4740_gpio_reset(s);

    iomemtype =
        cpu_register_io_memory(0, jz4740_gpio_readfn, jz4740_gpio_writefn, s);
    cpu_register_physical_memory(s->base, 0x00010000, iomemtype);
    return s;

}

struct jz4740_rtc_s
{
    qemu_irq irq;
    target_phys_addr_t base;
    struct jz_state_s *soc;

    QEMUTimer *hz_tm;
    struct tm tm;
    //int sec_offset;
    int64_t next;

    uint32_t rtccr;
    uint32_t rtcsr;
    uint32_t rtcsar;
    uint32_t rtcgr;

    uint32_t hcr;
    uint32_t hwfcr;
    uint32_t hrcr;
    uint32_t hwcr;
    uint32_t hwrsr;
    uint32_t hspr;

};


static void jz4740_rtc_update_interrupt(struct jz4740_rtc_s *s)
{
	if (!s->rtcsr&0x1)
		return;
	
    if (((s->rtccr & 0x40) && (s->rtccr & 0x20))
        || ((s->rtccr & 0x10) && (s->rtccr & 0x8)))
    {
    	qemu_set_irq(s->irq, 1);
    }
        

}

static inline void jz4740_rtc_start(struct jz4740_rtc_s *s)
{
    s->next = +qemu_get_clock(rt_clock);
    qemu_mod_timer(s->hz_tm, s->next);
}

static inline void jz4740_rtc_stop(struct jz4740_rtc_s *s)
{
    qemu_del_timer(s->hz_tm);
    s->next = -qemu_get_clock(rt_clock);
    if (s->next < 1)
        s->next = 1;
}

static void jz4740_rtc_hz(void *opaque)
{
    struct jz4740_rtc_s *s = (struct jz4740_rtc_s *) opaque;

    s->next += 1000;
    qemu_mod_timer(s->hz_tm, s->next);
    if (s->rtccr & 0x1)
    {
        s->rtcsr++;
        s->rtccr |= 0x40;
        if (s->rtcsr & 0x4)
        {
            if (s->rtcsr == s->rtcsar)
                s->rtccr |= 0x10;
        }
        jz4740_rtc_update_interrupt(s);
    }
}

static void jz4740_rtc_reset(struct jz4740_rtc_s *s)
{
	
    s->rtccr = 0x81;
    s->next = 1000;

    /*Maybe rtcsr need to be saved to file */
    s->rtcsr = time(NULL);
    jz4740_rtc_start(s);

}

static uint32_t jz4740_rtc_read(void *opaque, target_phys_addr_t addr)
{
    struct jz4740_rtc_s *s = (struct jz4740_rtc_s *) opaque;

    switch (addr)
    {
    case 0x0:
        return s->rtccr | 0x80;
    case 0x4:
        return s->rtcsr;
    case 0x8:
        return s->rtcsar;
    case 0xc:
        return s->rtcgr;
    case 0x20:
        return s->hcr;
    case 0x24:
        return s->hwfcr;
    case 0x28:
        return s->hrcr;
    case 0x2c:
        return s->hwcr;
    case 0x30:
        return s->hwrsr;
    case 0x34:
        return s->hspr;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_rtc_read undefined addr " JZ_FMT_plx "\n", addr);
    }

    return (0);
}

static void jz4740_rtc_write(void *opaque, target_phys_addr_t addr,
                             uint32_t value)
{
    struct jz4740_rtc_s *s = (struct jz4740_rtc_s *) opaque;

    switch (addr)
    {
    case 0x0:
        s->rtccr = value & 0x7d;
        if (!value & 0x40)
            s->rtccr &= ~0x40;
        if (!value & 0x10)
            s->rtccr &= ~0x10;
        if (s->rtccr & 0x1)
        {
            jz4740_rtc_start(s);
        }
        else
        	jz4740_rtc_stop(s);
        break;
    case 0x4:
        s->rtcsr = value;
        break;
    case 0x8:
        s->rtcsar = value;
        break;
    case 0xc:
        s->rtcgr = value & 0x13ffffff;
        break;
    case 0x20:
        s->hcr = value & 0x1;
        break;
    case 0x24:
        s->hwfcr = value & 0xffe0;
        break;
    case 0x28:
        s->hrcr = value & 0xfe0;
        break;
    case 0x2c:
        s->hwcr = value & 0x1;
        break;
    case 0x30:
        s->hwrsr = value & 0x33;
        break;
    case 0x34:
        s->hspr = value;
        break;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_rtc_write undefined addr " JZ_FMT_plx
                  "  value %x \n", addr, value);
    }

}

static CPUReadMemoryFunc *jz4740_rtc_readfn[] = {
    jz4740_badwidth_read32, 
    jz4740_badwidth_read32, 
    jz4740_rtc_read,
};

static CPUWriteMemoryFunc *jz4740_rtc_writefn[] = {
    jz4740_badwidth_write32, 
    jz4740_badwidth_write32, 
    jz4740_rtc_write,
};

static struct jz4740_rtc_s *jz4740_rtc_init(struct jz_state_s *soc,
                                            qemu_irq irq)
{
    int iomemtype;
    struct jz4740_rtc_s *s = (struct jz4740_rtc_s *) qemu_mallocz(sizeof(*s));
    s->base = JZ4740_PHYS_BASE(JZ4740_RTC_BASE);
    s->soc = soc;
    s->irq = irq;

    s->hz_tm = qemu_new_timer(rt_clock, jz4740_rtc_hz, s);

    jz4740_rtc_reset(s);

    iomemtype =
        cpu_register_io_memory(0, jz4740_rtc_readfn, jz4740_rtc_writefn, s);
    cpu_register_physical_memory(s->base, 0x00001000, iomemtype);
    return s;
}

struct jz4740_tcu_s
{
    qemu_irq tcu_irq0;
    qemu_irq tcu_irq1;
    qemu_irq tcu_irq2;

    target_phys_addr_t base;
    struct jz_state_s *soc;


    QEMUTimer *half_timer[8];
    QEMUTimer *full_timer[8];
    int64_t time[8];

    uint32_t tsr;
    uint32_t ter;
    uint32_t tfr;
    uint32_t tmr;

    uint32_t tdfr[8];
    uint32_t tdhr[8];
    uint32_t tcnt[8];
    uint32_t tcsr[8];

    uint32_t prescale[8];
    uint32_t freq[8];
};

static void jz4740_tcu_update_interrupt(struct jz4740_tcu_s *s)
{
    //printf("s->tfr %x s->tmr %x \n",s->tfr,s->tmr);
    if (((s->tfr & 0x1) & (~(s->tmr & 0x1)))
        || ((s->tfr & 0x10000) & (~(s->tmr & 0x10000))))
    {
        qemu_set_irq(s->tcu_irq0, 1);
    }
    else
        qemu_set_irq(s->tcu_irq0, 0);

    if (((s->tfr & 0x2) & (~(s->tmr & 0x2)))
        || ((s->tfr & 0x20000) & (~(s->tmr & 0x20000))))
    {
        qemu_set_irq(s->tcu_irq1, 1);
    }
    else
        qemu_set_irq(s->tcu_irq1, 0);

    if (((s->tfr & 0xfc) & (~(s->tmr & 0xfc)))
        || ((s->tfr & 0xfc0000) & (~(s->tmr & 0xfc0000))))
    {
        qemu_set_irq(s->tcu_irq2, 1);
    }
    else
        qemu_set_irq(s->tcu_irq2, 0);

}

#undef TCU_INDEX
#define TCU_INDEX   0
#include "mips_jz_glue.h"
#define TCU_INDEX   1
#include "mips_jz_glue.h"
#define TCU_INDEX   2
#include "mips_jz_glue.h"
#define TCU_INDEX   3
#include "mips_jz_glue.h"
#define TCU_INDEX   4
#include "mips_jz_glue.h"
#define TCU_INDEX   5
#include "mips_jz_glue.h"
#define TCU_INDEX   6
#include "mips_jz_glue.h"
#define TCU_INDEX   7
#include "mips_jz_glue.h"
#undef TCU_INDEX

#define  jz4740_tcu_start(s) do {        \
	jz4740_tcu_start_half0(s);              \
	jz4740_tcu_start_full0(s);    \
	jz4740_tcu_start_half1(s); \
	jz4740_tcu_start_full1(s); \
	jz4740_tcu_start_half2(s); \
	jz4740_tcu_start_full2(s); \
	jz4740_tcu_start_half3(s); \
	jz4740_tcu_start_full3(s); \
	jz4740_tcu_start_half4(s); \
	jz4740_tcu_start_full4(s); \
	jz4740_tcu_start_half5(s); \
	jz4740_tcu_start_full5(s); \
	jz4740_tcu_start_half6(s); \
	jz4740_tcu_start_full6(s); \
	jz4740_tcu_start_half7(s); \
	jz4740_tcu_start_full7(s); \
}while (0)

static void jz4740_tcu_if_reset(struct jz4740_tcu_s *s)
{
    int i;

    s->tsr = 0x0;
    s->ter = 0x0;
    s->tfr = 0x0;
    s->tmr = 0x0;
    for (i = 0; i < 8; i++)
    {
        s->tdfr[i] = 0xffff;
        s->tdhr[i] = 0x8000;
        s->tcnt[i] = 0x0;
        s->tcsr[i] = 0x0;
        s->half_timer[i] = NULL;
        s->full_timer[i] = NULL;
    }
}

static void jz4740_tcu_if_write8(void *opaque, target_phys_addr_t addr,
                                 uint32_t value)
{
    struct jz4740_tcu_s *s = (struct jz4740_tcu_s *) opaque;

    switch (addr)
    {
    case 0x14:
        s->ter |= (value & 0xff);
        jz4740_tcu_start(s);
        break;
    case 0x18:
        s->ter &= ~(value & 0xff);
        jz4740_tcu_start(s);
        break;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_tcu_if_write8 undefined addr " JZ_FMT_plx
                  " value %x \n", addr, value);
    }

}

static void jz4740_tcu_if_write32(void *opaque, target_phys_addr_t addr,
                                  uint32_t value)
{
    struct jz4740_tcu_s *s = (struct jz4740_tcu_s *) opaque;

    switch (addr)
    {
    case 0x2c:
        s->tsr |= (value & 0x100ff);
        jz4740_tcu_start(s);
        break;
    case 0x3c:
        s->tsr &= ~(value & 0x100ff);
        jz4740_tcu_start(s);
        break;
    case 0x24:
        s->tfr |= (value & 0xff00ff);
        break;
    case 0x28:
        s->tfr &= ~(value & 0xff00ff);
        break;
    case 0x34:
        s->tmr |= (value & 0xff00ff);
        jz4740_tcu_update_interrupt(s);
        break;
    case 0x38:
        s->tmr &= ~(value & 0xff00ff);
        jz4740_tcu_update_interrupt(s);
        break;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_tcu_if_write32 undefined addr " JZ_FMT_plx
                  " value %x \n", addr, value);

    }
}

static uint32_t jz4740_tcu_if_read8(void *opaque, target_phys_addr_t addr)
{
    struct jz4740_tcu_s *s = (struct jz4740_tcu_s *) opaque;

    switch (addr)
    {
    case 0x10:
        return s->ter;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_tcu_if_read8 undefined addr " JZ_FMT_plx "\n", addr);
    }
    return (0);
}

static uint32_t jz4740_tcu_if_read32(void *opaque, target_phys_addr_t addr)
{
    struct jz4740_tcu_s *s = (struct jz4740_tcu_s *) opaque;

    switch (addr)
    {
    case 0x1c:
        return s->tsr;
    case 0x20:
        return s->tfr;
    case 0x30:
        return s->tmr;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_tcu_if_read32 undefined addr " JZ_FMT_plx "\n", addr);

    }
    return (0);

}

static CPUReadMemoryFunc *jz4740_tcu_if_readfn[] = {
    jz4740_tcu_if_read8,
    jz4740_badwidth_read32, 
    jz4740_tcu_if_read32,
};

static CPUWriteMemoryFunc *jz4740_tcu_if_writefn[] = {
    jz4740_tcu_if_write8, 
    jz4740_badwidth_write32, 
    jz4740_tcu_if_write32,
};

static struct jz4740_tcu_s *jz4740_tcu_if_init(struct jz_state_s *soc,
                                               qemu_irq tcu_irq0,
                                               qemu_irq tcu_irq1,
                                               qemu_irq tcu_irq2)
{
    int iomemtype;
    //int i;

    struct jz4740_tcu_s *s = (struct jz4740_tcu_s *) qemu_mallocz(sizeof(*s));
    s->base = JZ4740_PHYS_BASE(JZ4740_TCU_BASE);
    s->soc = soc;
    s->tcu_irq0 = tcu_irq0;
    s->tcu_irq1 = tcu_irq1;
    s->tcu_irq2 = tcu_irq2;

    jz4740_tcu_if_reset(s);

    iomemtype =
        cpu_register_io_memory(0, jz4740_tcu_if_readfn, jz4740_tcu_if_writefn,
                               s);
    cpu_register_physical_memory(s->base, 0x00000040, iomemtype);
    return s;

}

static void jz4740_tcu_init(struct jz_state_s *soc,
                            struct jz4740_tcu_s *s, int timer_index)
{
    switch (timer_index)
    {
    case 0x0:
        jz4740_tcu_init0(soc, s);
        break;
    case 0x1:
        jz4740_tcu_init1(soc, s);
        break;
    case 0x2:
        jz4740_tcu_init2(soc, s);
        break;
    case 0x3:
        jz4740_tcu_init3(soc, s);
        break;
    case 0x4:
        jz4740_tcu_init4(soc, s);
        break;
    case 0x5:
        jz4740_tcu_init5(soc, s);
        break;
    case 0x6:
        jz4740_tcu_init6(soc, s);
        break;
    case 0x7:
        jz4740_tcu_init7(soc, s);
        break;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_tcu_init undefined timer %x \n", timer_index);
    }
}

typedef void (*jz4740_lcd_fn_t) (uint8_t * d, const uint8_t * s, int width,
                                 const uint16_t * pal);
struct jz_fb_descriptor
{
    uint32_t fdadr;             /* Frame descriptor address register */
    uint32_t fsadr;             /* Frame source address register */
    uint32_t fidr;              /* Frame ID register */
    uint32_t ldcmd;             /* Command register */
};

struct jz4740_lcdc_s
{
    qemu_irq irq;

    target_phys_addr_t base;
    struct jz_state_s *soc;

    DisplayState *state;
    QEMUConsole *console;
    jz4740_lcd_fn_t *line_fn_tab;
    jz4740_lcd_fn_t line_fn;


    uint32_t lcdcfg;
    uint32_t lcdvsync;
    uint32_t lcdhsync;
    uint32_t lcdvat;
    uint32_t lcddah;
    uint32_t lcddav;
    uint32_t lcdps;
    uint32_t lcdcls;
    uint32_t lcdspl;
    uint32_t lcdrev;
    uint32_t lcdctrl;
    uint32_t lcdstate;
    uint32_t lcdiid;
    uint32_t lcdda0;
    uint32_t lcdsa0;
    uint32_t lcdfid0;
    uint32_t lcdcmd0;
    uint32_t lcdda1;
    uint32_t lcdsa1;
    uint32_t lcdfid1;
    uint32_t lcdcmd1;

    uint32_t ena;
    uint32_t dis;
    uint32_t width;
    uint32_t height;
    uint32_t bpp;               /*bit per second */
    uint16_t palette[256];
    uint32_t invalidate;

};

/*bit per pixel*/
static const int jz4740_lcd_bpp[0x6] = {
    1, 2, 4, 8, 16, 32          /*4740 uses 32 bit for 24bpp */
};

static void jz4740_lcdc_reset(struct jz4740_lcdc_s *s)
{

}

static uint32_t jz4740_lcdc_read(void *opaque, target_phys_addr_t addr)
{
    struct jz4740_lcdc_s *s = (struct jz4740_lcdc_s *) opaque;

    switch (addr)
    {
    case 0x0:
        return s->lcdcfg;
    case 0x4:
        return s->lcdvsync;
    case 0x8:
        return s->lcdhsync;
    case 0xc:
        return s->lcdvat;
    case 0x10:
        return s->lcddah;
    case 0x14:
        return s->lcddav;
    case 0x18:
        return s->lcdps;
    case 0x1c:
        return s->lcdcls;
    case 0x20:
        return s->lcdspl;
    case 0x24:
        return s->lcdrev;
    case 0x30:
        return s->lcdctrl;
    case 0x34:
        return s->lcdstate;
    case 0x38:
        return s->lcdiid;
    case 0x40:
        return s->lcdda0;
    case 0x44:
        return s->lcdsa0;
    case 0x48:
        return s->lcdfid0;
    case 0x4c:
        return s->lcdcmd0;
    case 0x50:
        return s->lcdda1;
    case 0x54:
        return s->lcdsa1;
    case 0x58:
        return s->lcdfid1;
    case 0x5c:
        return s->lcdcmd1;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_lcdc_read undefined addr " JZ_FMT_plx " \n", addr);

    }

}

static void jz4740_lcdc_write(void *opaque, target_phys_addr_t addr,
                              uint32_t value)
{
    struct jz4740_lcdc_s *s = (struct jz4740_lcdc_s *) opaque;

    switch (addr)
    {
    case 0x44:
    case 0x48:
    case 0x4c:
        JZ4740_RO_REG(addr);
        break;
    case 0x0:
        s->lcdcfg = value & 0x80ffffbf;
        break;
    case 0x4:
        s->lcdvsync = value & 0x7ff07ff;
        break;
    case 0x8:
        s->lcdhsync = value & 0x7ff07ff;
        break;
    case 0xc:
        s->lcdvat = value & 0x7ff07ff;
        break;
    case 0x10:
        s->lcddah = value & 0x7ff07ff;
        s->width = (value & 0x7ff) - ((value >> 16) & 0x7ff);
        break;
    case 0x14:
        s->height = (value & 0x7ff) - ((value >> 16) & 0x7ff);
        s->lcddav = value & 0x7ff07ff;
        break;
    case 0x18:
        s->lcdps = value & 0x7ff07ff;
        break;
    case 0x1c:
        s->lcdcls = value & 0x7ff07ff;
        break;
    case 0x20:
        s->lcdspl = value & 0x7ff07ff;
        break;
    case 0x24:
        s->lcdrev = value & 0x7ff0000;
        break;
    case 0x30:
        s->lcdctrl = value & 0x3fff3fff;
        s->ena = (value & 0x8) >> 3;
        s->dis = (value & 0x10) >> 4;
        s->bpp = jz4740_lcd_bpp[value & 0x7];
        if ((s->bpp == 1))
        {
            fprintf(stderr, "bpp =1 is not supported\n");
            exit(-1);
        }
        s->line_fn = s->line_fn_tab[value & 0x7];
        break;
    case 0x34:
        s->lcdstate = value & 0xbf;
        break;
    case 0x38:
        s->lcdiid = value;
        break;
    case 0x40:
        s->lcdda0 = value;
        break;
    case 0x50:
        s->lcdda1 = value;
        break;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_lcdc_write undefined addr " JZ_FMT_plx " value %x \n",
                  addr, value);
    }

}

static CPUReadMemoryFunc *jz4740_lcdc_readfn[] = {
    jz4740_badwidth_read32, 
    jz4740_badwidth_read32, 
    jz4740_lcdc_read,
};

static CPUWriteMemoryFunc *jz4740_lcdc_writefn[] = {
    jz4740_badwidth_write32,
    jz4740_badwidth_write32, 
    jz4740_lcdc_write,
};

#include "pixel_ops.h"
#define JZ4740_LCD_PANEL
#define DEPTH 8
#include "mips_jz_glue.h"
#define DEPTH 15
#include "mips_jz_glue.h"
#define DEPTH 16
#include "mips_jz_glue.h"
#define DEPTH 24
#include "mips_jz_glue.h"
#define DEPTH 32
#include "mips_jz_glue.h"
#undef JZ4740_LCD_PANEL

static void *jz4740_lcd_get_buffer(struct jz4740_lcdc_s *s,
                                   target_phys_addr_t addr)
{
    uint32_t pd;

    pd = cpu_get_physical_page_desc(addr);
    if ((pd & ~TARGET_PAGE_MASK) != IO_MEM_RAM)
        /* TODO */
        cpu_abort(cpu_single_env, "%s: framebuffer outside RAM!\n",
                  __FUNCTION__);
    else
        return phys_ram_base +
            (pd & TARGET_PAGE_MASK) + (addr & ~TARGET_PAGE_MASK);
}

static void jz4740_lcd_update_display(void *opaque)
{
    struct jz4740_lcdc_s *s = (struct jz4740_lcdc_s *) opaque;

    uint8_t *src, *dest;
    struct jz_fb_descriptor *fb_des;

    int step, linesize;
    int y;


    if (!s->ena)
        return;
    if (s->dis)
        return;

    if (!s->lcdda0)
        return;

    fb_des = (struct jz_fb_descriptor *) jz4740_lcd_get_buffer(s, s->lcdda0);
    s->lcdda0 = fb_des->fdadr;
    s->lcdsa0 = fb_des->fsadr;
    s->lcdfid0 = fb_des->fidr;
    s->lcdcmd0 = fb_des->ldcmd;

    src = (uint8_t *) jz4740_lcd_get_buffer(s, fb_des->fsadr);
    if (s->lcdcmd0 & (0x1 << 28))
    {
        /*palette */
        memcpy(s->palette, src, sizeof(s->palette));
        return;
    }

    /*frame buffer */

    if (s->width != ds_get_width(s->state) ||
        s->height != ds_get_height(s->state))
    {
        qemu_console_resize(s->console, s->width, s->height);
        s->invalidate = 1;
    }

    step = (s->width * s->bpp) >> 3;
    dest = ds_get_data(s->state);
    linesize = ds_get_linesize(s->state);

    //printf("s->width %d s->height  %d s->bpp %d linesize %d \n",s->width,s->height ,s->bpp,linesize);

    for (y = 0; y < s->height; y++)
    {
        s->line_fn(dest, src, s->width, s->palette);
        //memcpy(dest,src,step);
        src += step;
        dest += linesize;
    }


    dpy_update(s->state, 0, 0, s->width, s->height);
    s->lcdstate |= 0x20;
    if ((s->lcdcmd0 & 0x40000000) && (!(s->lcdctrl & 0x2000)))
        qemu_set_irq(s->irq, 1);
}

static inline void jz4740_lcd_invalidate_display(void *opaque)
{
    struct jz4740_lcdc_s *s = (struct jz4740_lcdc_s *) opaque;
    s->invalidate = 1;
}

static struct jz4740_lcdc_s *jz4740_lcdc_init(struct jz_state_s *soc,
                                              qemu_irq irq, DisplayState * ds)
{
    int iomemtype;

    struct jz4740_lcdc_s *s = (struct jz4740_lcdc_s *) qemu_mallocz(sizeof(*s));
    s->base = JZ4740_PHYS_BASE(JZ4740_LCD_BASE);
    s->soc = soc;
    s->irq = irq;
    s->state = ds;


    jz4740_lcdc_reset(s);

    iomemtype =
        cpu_register_io_memory(0, jz4740_lcdc_readfn, jz4740_lcdc_writefn, s);
    cpu_register_physical_memory(s->base, 0x10000, iomemtype);

    s->console = graphic_console_init(s->state, jz4740_lcd_update_display,
                                      jz4740_lcd_invalidate_display,
                                      NULL, NULL, s);
    switch (ds_get_bits_per_pixel(s->state))
    {
    case 0x0:
        s->line_fn_tab = qemu_mallocz(sizeof(jz4740_lcd_fn_t) * 6);
        break;
    case 8:
        s->line_fn_tab = jz4740_lcd_draw_fn_8;
        break;
    case 15:
        s->line_fn_tab = jz4740_lcd_draw_fn_15;
        break;
    case 16:
        s->line_fn_tab = jz4740_lcd_draw_fn_16;
        break;
    case 24:
        s->line_fn_tab = jz4740_lcd_draw_fn_24;
        break;
    case 32:
        s->line_fn_tab = jz4740_lcd_draw_fn_32;
        break;
    default:
        fprintf(stderr, "%s: Bad color depth\n", __FUNCTION__);
        exit(1);
    }

    return s;

}

#define JZ4740_DMA_NUM         6
struct jz4740_dma_s
{
    qemu_irq irq;

    target_phys_addr_t base;
    struct jz_state_s *soc;

    uint32_t dmac;
    uint32_t dirqp;
    uint32_t ddr;
    uint32_t ddrs;

    uint32_t dsa[JZ4740_DMA_NUM];
    uint32_t dta[JZ4740_DMA_NUM];
    uint32_t dtc[JZ4740_DMA_NUM];
    uint32_t drs[JZ4740_DMA_NUM];
    uint32_t dcs[JZ4740_DMA_NUM];
    uint32_t dcm[JZ4740_DMA_NUM];
    uint32_t dda[JZ4740_DMA_NUM];

};

struct jz4740_desc_s
{
    uint32_t dcmd;              /* DCMD value for the current transfer */
    uint32_t dsadr;             /* DSAR value for the current transfer */
    uint32_t dtadr;             /* DTAR value for the current transfer */
    uint32_t ddadr;             /* Points to the next descriptor + transfer count */
};

static inline void jz4740_dma_transfer(struct jz4740_dma_s *s,
                                       target_phys_addr_t src,
                                       target_phys_addr_t dest, uint32_t len)
{
    uint32_t pd_src, pd_dest;
    uint8_t *sr, *de;

    pd_src = cpu_get_physical_page_desc(src);
    if ((pd_src & ~TARGET_PAGE_MASK) != IO_MEM_RAM)
        /* TODO */
        cpu_abort(cpu_single_env, "%s: DMA source address "JZ_FMT_plx" outside RAM!\n",
                  __FUNCTION__, src);
    else
        sr = phys_ram_base +
            (pd_src & TARGET_PAGE_MASK) + (src & ~TARGET_PAGE_MASK);

    pd_dest = cpu_get_physical_page_desc(dest);
    if ((pd_dest & ~TARGET_PAGE_MASK) != IO_MEM_RAM)
        /* TODO */
        cpu_abort(cpu_single_env,
                  "%s: DMA destination address "JZ_FMT_plx" outside RAM!\n",
                  __FUNCTION__, dest);
    else
        de = phys_ram_base +
            (pd_dest & TARGET_PAGE_MASK) + (dest & ~TARGET_PAGE_MASK);

    memcpy(de, sr, len);
}

static inline uint32_t jz4740_dma_unit_size(struct jz4740_dma_s *s,
                                            uint32_t cmd)
{
    switch ((cmd & 0x700) >> 8)
    {
    case 0x0:
        return 4;
    case 0x1:
        return 1;
    case 0x2:
        return 2;
    case 0x3:
        return 16;
    case 0x4:
        return 32;
    }
    return (0);
}


/*No-descriptor transfer*/
static inline void jz4740_dma_ndrun(struct jz4740_dma_s *s, int channel)
{
    uint32_t len;

    len = jz4740_dma_unit_size(s, s->dcs[channel]) * s->dtc[channel];

    jz4740_dma_transfer(s, s->dsa[channel], s->dta[channel], len);

    /*finish dma transfer */
    s->dtc[channel] = 0;
    /*set DIR QP */
    s->dirqp |= 1 << channel;

    /*some cleanup work */
    /*clean AR TT GLOBAL AR */
    s->dcs[channel] &= 0xffffffe7;
    s->dmac &= 0xfffffffb;

    if (s->dcm[channel] & 0x2)
        qemu_set_irq(s->irq, 1);
}

        /*descriptor transfer */
static inline void jz4740_dma_drun(struct jz4740_dma_s *s, int channel)
{
    struct jz4740_desc_s *desc;
    target_phys_addr_t desc_phy;
    uint32_t pd;

    desc_phy = s->dda[channel];
    if (desc_phy & 0xf)
        cpu_abort(s->soc->env,
                  "jz4740_dma_drun descriptor address " JZ_FMT_plx
                  " must be 4 bytes aligned \n", desc_phy);

    pd = cpu_get_physical_page_desc(desc_phy);
    if ((pd & ~TARGET_PAGE_MASK) != IO_MEM_RAM)
        cpu_abort(cpu_single_env,
                  "%s: DMA descriptor address " JZ_FMT_plx " outside RAM!\n",
                  __FUNCTION__, desc_phy);
    else
        desc = (struct jz4740_desc_s *) (phys_ram_base +
                                         (pd & TARGET_PAGE_MASK) +
                                         (desc_phy & ~TARGET_PAGE_MASK));

    if (!desc)
        cpu_abort(cpu_single_env,
                  "%s: DMA descriptor %x is NULL!\n", __FUNCTION__,
                  (uint32_t)desc);

    while (1)
    {
        if ((desc->dcmd & 0x8) && (!(desc->dcmd & 0x10)))
        {
            /*Stop DMA and set DCSN.INV=1 */
            s->dcs[channel] |= 1 << 6;
            return;
        }
        jz4740_dma_transfer(s, desc->dtadr, desc->dsadr,
                            (desc->ddadr & 0xffffff) *
                            jz4740_dma_unit_size(s, desc->dcmd));

        if ((desc->dcmd) & (1 << 3))
            /*clear v */
            desc->dcmd &= ~(1 << 4);
        if (desc->dcmd & 0x1)
            /*set DCSN.CT=1 */
            s->dcs[channel] |= 0x2;
        else
            /*set DCSN.TT=1 */
            s->dcs[channel] |= 0x8;

        if (desc->dcmd & 0x2)
            qemu_set_irq(s->irq, 1);

        if ((desc->dcmd) & 0x1)
        {
            /*fetch next descriptor */
            desc_phy = s->dda[channel] & 0xfffff000;
            desc_phy += (desc->dtadr & 0xff000000) >> 24;
            pd = cpu_get_physical_page_desc(desc_phy);
            if ((pd & ~TARGET_PAGE_MASK) != IO_MEM_RAM)
                cpu_abort(cpu_single_env,
                          "%s: DMA descriptor address "JZ_FMT_plx" outside RAM!\n",
                          __FUNCTION__, desc_phy);
            else
                desc = (struct jz4740_desc_s *) (phys_ram_base +
                                                 (pd & TARGET_PAGE_MASK)
                                                 +
                                                 (desc_phy &
                                                  ~TARGET_PAGE_MASK));
            if (!desc)
                cpu_abort(cpu_single_env,
                          "%s: DMA descriptor %x is NULL!\n",
                          __FUNCTION__, (uint32_t) desc);
        }
        else
            break;
    }
}

static void jz4740_dma_en_channel(struct jz4740_dma_s *s, int channel)
{
    if (s->dmac & 0x1)
    {
        if (s->dcs[channel] & (1 << 31))
        {
            /*NON DESCRIPTOR */
            jz4740_dma_ndrun(s, channel);
        }
    }
}

static inline void jz4740_dma_en_global(struct jz4740_dma_s *s)
{
    int channel;
    for (channel = 0; channel < JZ4740_DMA_NUM; channel++)
    {
        jz4740_dma_en_channel(s, channel);
    }
}

static inline void jz4740_dma_en_dbn(struct jz4740_dma_s *s, int channel)
{
    if ((s->dmac & 0x1) && (s->dcs[channel] & (1 << 31)))
    {
        jz4740_dma_drun(s, channel);
    }
}

static void jz4740_dma_reset(struct jz4740_dma_s *s)
{

}

static uint32_t jz4740_dma_read(void *opaque, target_phys_addr_t addr)
{
    struct jz4740_dma_s *s = (struct jz4740_dma_s *) opaque;
    int channel;

    switch (addr)
    {
    case 0x300:
        return s->dmac;
    case 0x304:
        return s->dirqp;
    case 0x308:
        return s->ddr;
    case 0x0:
    case 0x20:
    case 0x40:
    case 0x60:
    case 0x80:
    case 0xa0:
        channel = (addr - 0x0) / 0x20;
        return s->dsa[channel];
    case 0x4:
    case 0x24:
    case 0x44:
    case 0x64:
    case 0x84:
    case 0xa4:
        channel = (addr - 0x4) / 0x20;
        return s->dta[channel];
    case 0x8:
    case 0x28:
    case 0x48:
    case 0x68:
    case 0x88:
    case 0xa8:
        channel = (addr - 0x8) / 0x20;
        return s->dtc[channel];
    case 0xc:
    case 0x2c:
    case 0x4c:
    case 0x6c:
    case 0x8c:
    case 0xac:
        channel = (addr - 0xc) / 0x20;
        return s->drs[channel];
    case 0x10:
    case 0x30:
    case 0x50:
    case 0x70:
    case 0x90:
    case 0xb0:
        channel = (addr - 0x10) / 0x20;
        return s->dcs[channel];
    case 0x14:
    case 0x34:
    case 0x54:
    case 0x74:
    case 0x94:
    case 0xb4:
        channel = (addr - 0x14) / 0x20;
        return s->dcm[channel];
    case 0x18:
    case 0x38:
    case 0x58:
    case 0x78:
    case 0x98:
    case 0xb8:
        channel = (addr - 0x18) / 0x20;
        return s->dda[channel];
    default:
        cpu_abort(s->soc->env,
                  "jz4740_dma_read undefined addr " JZ_FMT_plx "  \n", addr);
    }
    return (0);
}

static void jz4740_dma_write(void *opaque, target_phys_addr_t addr,
                             uint32_t value)
{
    struct jz4740_dma_s *s = (struct jz4740_dma_s *) opaque;
    int channel;

    switch (addr)
    {
    case 0x304:
        JZ4740_RO_REG(addr);
        break;
    case 0x300:
        s->dmac = value & 0x30d;
        if (s->dmac & 0x1)
            jz4740_dma_en_global(s);
        break;
    case 0x308:
    case 0x30c:
        s->ddr = value & 0xff;
        for (channel = 0; channel < JZ4740_DMA_NUM; channel++)
        {
            if (s->ddr & (1 << channel))
            {
                jz4740_dma_en_dbn(s, channel);
                break;
            }
        }
        break;
    case 0x0:
    case 0x20:
    case 0x40:
    case 0x60:
    case 0x80:
    case 0xa0:
        channel = (addr - 0x0) / 0x20;
        s->dsa[channel] = value;
        break;
    case 0x4:
    case 0x24:
    case 0x44:
    case 0x64:
    case 0x84:
    case 0xa4:
        channel = (addr - 0x4) / 0x20;
        s->dta[channel] = value;
        break;
    case 0x8:
    case 0x28:
    case 0x48:
    case 0x68:
    case 0x88:
    case 0xa8:
        channel = (addr - 0x8) / 0x20;
        s->dtc[channel] = value;
        break;
    case 0xc:
    case 0x2c:
    case 0x4c:
    case 0x6c:
    case 0x8c:
    case 0xac:
        channel = (addr - 0xc) / 0x20;
        s->drs[channel] = value & 0x10;
        if (s->drs[channel] != 0x8)
        {
            fprintf(stderr, "Only auto request is supproted \n");
        }
        break;
    case 0x10:
    case 0x30:
    case 0x50:
    case 0x70:
    case 0x90:
    case 0xb0:
        channel = (addr - 0x10) / 0x20;
        s->dcs[channel] = value & 0x80ff005f;
        if (s->dcs[channel] & 0x1)
            jz4740_dma_en_channel(s, channel);
        break;
    case 0x14:
    case 0x34:
    case 0x54:
    case 0x74:
    case 0x94:
    case 0xb4:
        channel = (addr - 0x14) / 0x20;
        s->dcm[channel] = value & 0xcff79f;
        break;
    case 0x18:
    case 0x38:
    case 0x58:
    case 0x78:
    case 0x98:
    case 0xb8:
        channel = (addr - 0x18) / 0x20;
        s->dda[channel] = 0xfffffff0;
        break;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_dma_read undefined addr " JZ_FMT_plx "  \n", addr);
    }

}

static CPUReadMemoryFunc *jz4740_dma_readfn[] = {
    jz4740_badwidth_read32, 
    jz4740_badwidth_read32, 
    jz4740_dma_read,
};

static CPUWriteMemoryFunc *jz4740_dma_writefn[] = {
    jz4740_badwidth_write32, 
    jz4740_badwidth_write32, 
    jz4740_dma_write,
};


static struct jz4740_dma_s *jz4740_dma_init(struct jz_state_s *soc,
                                            qemu_irq irq)
{
    int iomemtype;
    struct jz4740_dma_s *s = (struct jz4740_dma_s *) qemu_mallocz(sizeof(*s));
    s->base = JZ4740_PHYS_BASE(JZ4740_DMAC_BASE);
    s->soc = soc;
    s->irq = irq;

    jz4740_dma_reset(s);

    iomemtype =
        cpu_register_io_memory(0, jz4740_dma_readfn, jz4740_dma_writefn, s);
    cpu_register_physical_memory(s->base, 0x00010000, iomemtype);
    return s;

}







































//===========================================jz_ts=========================================


#define PEN_DOWN 1
#define PEN_UP       0
struct jz4740_sadc_s
{
    qemu_irq irq;

    target_phys_addr_t base;
    struct jz_state_s *soc;

    uint32_t adena;
    uint32_t adcfg;
    uint32_t adctrl;
    uint32_t adstate;
    uint32_t adsame;
    uint32_t adwait;
    uint32_t adtch;
    uint32_t adbdat;
    uint32_t adsdat;
    uint32_t addpin;

	uint8_t tchen;
    uint8_t ex_in;
    uint8_t xyz;
    uint8_t snum;

    uint16_t x;
    uint16_t y;

    uint16_t pen_state;
    uint8_t read_index;
    
    
};

static void jz4740_touchscreen_interrupt(struct jz4740_sadc_s *s)
{
    if (!s->tchen)
    	return;

    if ((s->adctrl)&(s->adstate))
    {
    	qemu_set_irq(s->irq,1);
    }
    	
}

static void jz4740_touchscreen_event(void *opaque, int x, int y, int z, int buttons_state)
{
  if( x>0 && y>0 && x<32767 && y<32767){
    struct jz4740_sadc_s *s = opaque;

    if (!s->tchen)
    	return;

    s->x = (x*4096)/0x7FFF;
    s->y = (y*4096)/0x7FFF;

    if ((s->pen_state == PEN_UP)&&(buttons_state==PEN_DOWN))
    {
        s->adstate |= 0x14;
        jz4740_touchscreen_interrupt(s);
    }
    else if ((s->pen_state == PEN_DOWN)&&(buttons_state==PEN_UP))
    {
	s->adstate |= 0xc;
	jz4740_touchscreen_interrupt(s);
    }
    
    s->pen_state = buttons_state;
  }
}

static uint32_t jz4740_sadc_read8(void *opaque, target_phys_addr_t addr)
{
    struct jz4740_sadc_s *s = (struct jz4740_sadc_s *) opaque;

    switch (addr)
    {
    	case 0x0:
    		return s->adena;
    	case 0x8:
    		return s->adctrl;
    	case 0xc:
    		return s->adstate;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_sadc_read8 undefined addr " JZ_FMT_plx "  \n", addr);
    }
    return (0);
}

static uint32_t jz4740_sdac_read16(void *opaque, target_phys_addr_t addr)
{
    struct jz4740_sadc_s *s = (struct jz4740_sadc_s *) opaque;
    
    switch (addr)
    {
    	case 0x10:
    		return s->adsame;
    	case 0x14:
    		return s->adwait;
    	case 0x1c:
    		return s->adbdat;
    	case 0x20:
    		return s->adsdat;
    	default:
        cpu_abort(s->soc->env,
                  "jz4740_sdac_read16 undefined addr " JZ_FMT_plx "  \n", addr);
    }
    return (0);
}

static uint32_t jz4740_sdac_read32(void *opaque, target_phys_addr_t addr)
{
    struct jz4740_sadc_s *s = (struct jz4740_sadc_s *) opaque;
    switch (addr)
    {
    	case 0x4:
    		return s->adcfg;
    	case 0x18:
    		/*TODO: Other type format*/
    		if (s->read_index==0)
    		{
    			s->read_index ++;
    			return (((s->x) & 0x7fff) | ((s->y & 0x7ffff) << 16));
    		}
    		else
    		{
    			s->read_index = 0;
    			return (0x3fff);
    		}
    	default:
        cpu_abort(s->soc->env,
                  "jz4740_sdac_read32 undefined addr " JZ_FMT_plx "  \n", addr);
    }
    return (0);
}

static void jz4740_sadc_write8(void *opaque, target_phys_addr_t addr,
                             uint32_t value)
{
    struct jz4740_sadc_s *s = (struct jz4740_sadc_s *) opaque;

    switch (addr)
    {
    	case 0x0:
    		s->adena = value & 0x7;
    		s->tchen = value & 0x4;
    		break;
    	case 0x8:
    		s->adctrl = value & 0x1f;
    		break;
    	case 0xc:
    		s->adstate &= ~(value & 0x1f);
    		break;
    	default:
        cpu_abort(s->soc->env,
                  "jz4740_sadc_write8 undefined addr " JZ_FMT_plx "  value %x \n", addr,value);
    }
}

static void jz4740_sadc_write16(void *opaque, target_phys_addr_t addr,
                             uint32_t value)
{
    struct jz4740_sadc_s *s = (struct jz4740_sadc_s *) opaque;

    switch (addr)
    {
    	case 0x10:
    		s->adsame = value & 0xffff;
    		break;
    	case 0x14:
    		s->adsdat = value & 0xffff;
    		break;
    	case 0x1c:
    		s->adbdat = 0x0;
    	case 0x20:
    		s->adsdat = 0x0;
    	default:
          cpu_abort(s->soc->env,
                  "jz4740_sadc_write16 undefined addr " JZ_FMT_plx "  value %x \n", addr,value);
    }
}

static void jz4740_sadc_write32(void *opaque, target_phys_addr_t addr,
                             uint32_t value)
{
    struct jz4740_sadc_s *s = (struct jz4740_sadc_s *) opaque;

    switch (addr)
    {
    	case 0x4:
    		s->adcfg = value & 0xc007ffff;
    		s->ex_in = (value & 0x40000000)>>30;
    		s->xyz = (value & 0x6fff)>>13;
    		s->snum = ((value & 0x1cff)>>10)+1;
    		break;
    	case 18:
    		s->adtch = value & 0x8fff8fff;
    		break;
    	default:
          cpu_abort(s->soc->env,
                  "jz4740_sadc_write32 undefined addr " JZ_FMT_plx "  value %x \n", addr,value);
    }
}

static void jz4740_sadc_reset(struct jz4740_sadc_s *s)
{
	s->adcfg = 0x0002002c;
	s->tchen = 0;
	s->snum = 1;
	s->xyz = 0;
	s->ex_in = 0;
}

static CPUReadMemoryFunc *jz4740_sadc_readfn[] = {
    jz4740_sadc_read8, 
    jz4740_sdac_read16, 
    jz4740_sdac_read32,
};

static CPUWriteMemoryFunc *jz4740_sadc_writefn[] = {
    jz4740_sadc_write8, 
    jz4740_sadc_write16, 
    jz4740_sadc_write32,
};

static struct jz4740_sadc_s *jz4740_sadc_init(struct jz_state_s *soc,
                                            qemu_irq irq)
{   
	int iomemtype;
	struct jz4740_sadc_s *s;

    s = (struct jz4740_sadc_s *)
            qemu_mallocz(sizeof(struct jz4740_sadc_s));
    s->base = JZ4740_PHYS_BASE(JZ4740_SADC_BASE);
    s->irq = irq;
    s->soc = soc;

    qemu_add_mouse_event_handler(jz4740_touchscreen_event, s, 1, "QEMU JZ4740 Touchscreen");

    jz4740_sadc_reset(s);

    iomemtype =
        cpu_register_io_memory(0, jz4740_sadc_readfn, jz4740_sadc_writefn, s);
    cpu_register_physical_memory(s->base, 0x00001000, iomemtype);
    return s;
}





















static void jz4740_cpu_reset(void *opaque)
{
    fprintf(stderr, "%s: UNIMPLEMENTED!", __FUNCTION__);
}

struct jz_state_s *jz4740_init(unsigned long sdram_size,
                               uint32_t osc_extal_freq, DisplayState * ds)
{
    struct jz_state_s *s = (struct jz_state_s *)
        qemu_mallocz(sizeof(struct jz_state_s));
    ram_addr_t sram_base, sdram_base;
    qemu_irq *intc;

    s->mpu_model = jz4740;
    s->env = cpu_init("jz4740");

    if (!s->env)
    {
        fprintf(stderr, "Unable to find CPU definition\n");
        exit(1);
    }

    qemu_register_reset(jz4740_cpu_reset, s->env);

    s->sdram_size = sdram_size;
    s->sram_size = JZ4740_SRAM_SIZE;

    /* Clocks */
    jz_clk_init(s, osc_extal_freq);

    /*map sram to 0x80000000 and sdram to 0x80004000 */
    sram_base = qemu_ram_alloc(s->sram_size);
    cpu_register_physical_memory(0x0, s->sram_size, (sram_base | IO_MEM_RAM));
    sdram_base = qemu_ram_alloc(s->sdram_size);
    cpu_register_physical_memory(JZ4740_SRAM_SIZE, s->sdram_size,
                                 (sdram_base | IO_MEM_RAM));

    /* Init internal devices */
    cpu_mips_irq_init_cpu(s->env);
    cpu_mips_clock_init(s->env);


    /* Clocks */
    jz_clk_init(s, osc_extal_freq);

    intc = jz4740_intc_init(s, s->env->irq[2]);
    s->cpm = jz4740_cpm_init(s);
    s->emc = jz4740_emc_init(s, intc[2]);
    s->gpio = jz4740_gpio_init(s, intc[25]);
    s->rtc = jz4740_rtc_init(s, intc[15]);
    s->tcu = jz4740_tcu_if_init(s, intc[23], intc[22], intc[21]);
    jz4740_tcu_init(s, s->tcu, 0);
    s->lcdc = jz4740_lcdc_init(s, intc[30], ds);
    s->dma = jz4740_dma_init(s, intc[20]);
//////////////////////////jz_ts////////////////////////////////
    s->sadc = jz4740_sadc_init(s,intc[12]);
//////////////////////////jz_ts!///////////////////////////////

////////////////////////jz_mouse///////////////////////////////
//    s->sadc = jz4740_sadc2_init(s,intc[12]);
////////////////////////jz_mouse!//////////////////////////////

    if (serial_hds[0])
        serial_mm_init(0x10030000, 2, intc[9], 57600, serial_hds[0], 1);

    return s;
}
