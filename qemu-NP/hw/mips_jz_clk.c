/*
 * JZ Soc clocks.
 *
 * Copyright (C) 2009 yajin <yajin@vm-kernel.org>
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include "hw.h"
#include "mips_jz.h"



#define ALWAYS_ENABLED	 (1 << 0)
#define CLOCK_IN_JZ4730	 (1 << 10)
#define CLOCK_IN_JZ4740	 (1 << 11)
#define CLOCK_IN_JZ4750	 (1 << 12)



static struct clk osc_extal = {
    .name = "osc_extal",
    .rate = 12000000,
    .flags = ALWAYS_ENABLED | CLOCK_IN_JZ4740,
};

static struct clk osc_32K = {
    .name = "osc_32K",
    .rate = 32768,
    .flags = ALWAYS_ENABLED | CLOCK_IN_JZ4740,
};

static struct clk lcd_pclk = {
    .name = "lcd_pclk",
    //.rate     = ??,
    //.flags    = ALWAYS_ENABLED | CLOCK_IN_JZ4740,
};

static struct clk pll_output = {
    .name = "pll_output",
    .parent = &osc_extal,
    .flags = ALWAYS_ENABLED | CLOCK_IN_JZ4740,
};

static struct clk pll_divider = {
    .name = "pll_divider",
    .parent = &pll_output,
    .flags = ALWAYS_ENABLED | CLOCK_IN_JZ4740,
};

static struct clk cclk = {
    .name = "cclk",
    .parent = &pll_output,
    .flags = ALWAYS_ENABLED | CLOCK_IN_JZ4740,
};

static struct clk pclk = {
    .name = "pclk",
    .parent = &pll_output,
    .flags = ALWAYS_ENABLED | CLOCK_IN_JZ4740,
};

static struct clk hclk = {
    .name = "hclk",
    .parent = &pll_output,
    .flags = ALWAYS_ENABLED | CLOCK_IN_JZ4740,
};

static struct clk mclk = {
    .name = "mclk",
    .parent = &pll_output,
    .flags = ALWAYS_ENABLED | CLOCK_IN_JZ4740,
};

static struct clk ldclk = {
    .name = "ldclk",
    .parent = &pll_divider,
    .flags = ALWAYS_ENABLED | CLOCK_IN_JZ4740,
};

static struct clk lpclk = {
    .name = "lpclk",
    .parent = &pll_divider,
    .flags = ALWAYS_ENABLED | CLOCK_IN_JZ4740,
};

static struct clk i2sclk = {
    .name = "i2sclk",
    .parent = &pll_divider,
    .flags = ALWAYS_ENABLED | CLOCK_IN_JZ4740,
};

static struct clk mscclk = {
    .name = "mscclk",
    .parent = &pll_divider,
    .flags = ALWAYS_ENABLED | CLOCK_IN_JZ4740,
};

static struct clk usbclk = {
    .name = "usbclk",
    .parent = &pll_divider,
    .flags = ALWAYS_ENABLED | CLOCK_IN_JZ4740,
};

static struct clk ssiclk = {
    .name = "ssiclk",
    .parent = &pll_divider,
    .flags = ALWAYS_ENABLED | CLOCK_IN_JZ4740,
};


static struct clk *onchip_clks[] = {
    &osc_extal,
    &lcd_pclk,
    &osc_32K,
    &pll_output,
    &pll_divider,
    &cclk,
    &pclk,
    &hclk,
    &mclk,
    &ldclk,
    &lpclk,
    &i2sclk,
    &mscclk,
    &usbclk,
    &ssiclk,
};

struct clk *jz_findclk(struct jz_state_s *cpu, const char *name)
{
    struct clk *i;

    for (i = cpu->clks; i->name; i++)
        if (!strcmp(i->name, name) || (i->alias && !strcmp(i->alias, name)))
            return i;
    cpu_abort(cpu->env, "%s: %s not found\n", __FUNCTION__, name);
}

static void jz_clk_update(struct clk *clk)
{
    int parent, running;
    //qemu_irq *user;
    struct clk *i;

    if (clk->parent)
        parent = clk->parent->running;
    else
        parent = 1;

    running = parent && (clk->enabled ||
                         ((clk->flags & ALWAYS_ENABLED)));
    if (clk->running != running)
    {
        clk->running = running;
        //for (user = clk->users; *user; user ++)
        //    qemu_set_irq(*user, running);
        for (i = clk->child1; i; i = i->sibling)
            jz_clk_update(i);
    }
}

static void jz_clk_rate_update_full(struct clk *clk, unsigned long int rate,
                                    unsigned long int div,
                                    unsigned long int mult)
{
    struct clk *i;
    //qemu_irq *user;

    clk->rate = muldiv64(rate, mult, div);
    if (clk->running)
        //for (user = clk->users; *user; user ++)
        //    qemu_irq_raise(*user);
        for (i = clk->child1; i; i = i->sibling)
            jz_clk_rate_update_full(i, rate,
                                    div * i->divisor, mult * i->multiplier);
}

static void jz_clk_rate_update(struct clk *clk)
{
    struct clk *i;
    unsigned long int div, mult = div = 1;

    for (i = clk; i->parent; i = i->parent)
    {
        div *= i->divisor;
        mult *= i->multiplier;
    }

    jz_clk_rate_update_full(clk, i->rate, div, mult);
}

void jz_clk_reparent(struct clk *clk, struct clk *parent)
{
    struct clk **p;

    if (clk->parent)
    {
        for (p = &clk->parent->child1; *p != clk; p = &(*p)->sibling);
        *p = clk->sibling;
    }

    clk->parent = parent;
    if (parent)
    {
        clk->sibling = parent->child1;
        parent->child1 = clk;
        jz_clk_update(clk);
        jz_clk_rate_update(clk);
    }
    else
        clk->sibling = 0;
}

void jz_clk_onoff(struct clk *clk, int on)
{
    clk->enabled = on;
    jz_clk_update(clk);
}

void jz_clk_setrate(struct clk *clk, int divide, int multiply)
{
    clk->divisor = divide;
    clk->multiplier = multiply;
    jz_clk_rate_update(clk);
}

int64_t jz_clk_getrate(struct clk *clk)
{
    return clk->rate;
}

void jz_clk_init(struct jz_state_s *mpu, uint32_t osc_extal_freq)
{
    struct clk **i, *j, *k;
    int count;
    int flag;

    if (cpu_is_jz4730(mpu))
        flag = CLOCK_IN_JZ4730;
    else if (cpu_is_jz4740(mpu))
        flag = CLOCK_IN_JZ4740;
    else if (cpu_is_jz4750(mpu))
        flag = CLOCK_IN_JZ4750;
    else
        return;

    osc_extal.rate = osc_extal_freq;

    for (i = onchip_clks, count = 0; *i; i++)
        if ((*i)->flags & flag)
            count++;
    mpu->clks = (struct clk *) qemu_mallocz(sizeof(struct clk) * (count + 1));
    for (i = onchip_clks, j = mpu->clks; *i; i++)
        if ((*i)->flags & flag)
        {
            memcpy(j, *i, sizeof(struct clk));
            for (k = mpu->clks; k < j; k++)
                if (j->parent && !strcmp(j->parent->name, k->name))
                {
                    j->parent = k;
                    j->sibling = k->child1;
                    k->child1 = j;
                }
                else if (k->parent && !strcmp(k->parent->name, j->name))
                {
                    k->parent = j;
                    k->sibling = j->child1;
                    j->child1 = k;
                }
            j->divisor = j->divisor ? : 1;
            j->multiplier = j->multiplier ? : 1;
            j++;
        }
    for (j = mpu->clks; count--; j++)
    {
        jz_clk_update(j);
        jz_clk_rate_update(j);
    }
}
