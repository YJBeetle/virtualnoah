/*
 * QEMU JZ Soc emulation glue code
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


 
#ifdef TCU_INDEX
static inline void glue(jz4740_tcu_time_sync,
                        TCU_INDEX) (struct jz4740_tcu_s * s)
{
    int64_t distance;
    int64_t temp;

    if ((!(s->tsr & (1 << TCU_INDEX))) && (s->ter & (1 << TCU_INDEX))
        && (s->freq[TCU_INDEX] != 0))
    {
        /*first enable timer */
        if (s->time[TCU_INDEX] == 0)
        {
            s->time[TCU_INDEX] = qemu_get_clock(vm_clock);
            s->tcnt[TCU_INDEX] = 0;
            return;
        }
        distance = qemu_get_clock(vm_clock) - s->time[TCU_INDEX];
        //temp = muldiv64(distance,(s->freq[TCU_INDEX]/s->prescale[TCU_INDEX]),ticks_per_sec);
        temp = muldiv64(distance, 46875, ticks_per_sec);
        if (temp != 0)
        {
            /*distance is too short */
            s->tcnt[TCU_INDEX] += temp;
            s->time[TCU_INDEX] = qemu_get_clock(vm_clock);
        }
        else
        {
            /*distance is too short.
             * Do not sync timer this timer.
             */
        }

        //printf("%lld distance %lld \n",muldiv64(distance,(s->freq[TCU_INDEX]/s->prescale[TCU_INDEX]),ticks_per_sec),distance);

        if (s->tcnt[TCU_INDEX] >= 0x10000)
            s->tcnt[TCU_INDEX] = 0x0;


    }
}

static inline void glue(jz4740_tcu_start_half,
                        TCU_INDEX) (struct jz4740_tcu_s * s)
{
    int64_t next = qemu_get_clock(vm_clock);
    int64_t count;

    /*The timer has not beed initialized */
    if (!s->half_timer[TCU_INDEX])
        return;
	
    if ((!(s->tsr & (1 << (TCU_INDEX+16)))) && (s->ter & (1 << (TCU_INDEX+16)))
        && (s->freq[TCU_INDEX] != 0))
    {
        glue(jz4740_tcu_time_sync, TCU_INDEX) (s);
        /*calculate next fire time */
        count =
            (s->tdhr[TCU_INDEX] - s->tcnt[TCU_INDEX]) * s->prescale[TCU_INDEX];
        next += muldiv64(count, ticks_per_sec, s->freq[TCU_INDEX]);
        qemu_mod_timer(s->half_timer[TCU_INDEX], next);

    }
    else
        qemu_del_timer(s->half_timer[TCU_INDEX]);

}

static inline void glue(jz4740_tcu_start_full,
                        TCU_INDEX) (struct jz4740_tcu_s * s)
{
    int64_t next = qemu_get_clock(vm_clock);
    int64_t count;

    /*The timer has not beed initialized */
    if (!s->full_timer[TCU_INDEX])
        return;
    if ((!(s->tsr & (1 << TCU_INDEX))) && (s->ter & (1 << TCU_INDEX))
        && (s->freq[TCU_INDEX] != 0))
    {
        glue(jz4740_tcu_time_sync, TCU_INDEX) (s);
        /*calculate next fire time */
        count =
            (s->tdfr[TCU_INDEX] - s->tcnt[TCU_INDEX]) * s->prescale[TCU_INDEX];
        next += muldiv64(count, ticks_per_sec, s->freq[TCU_INDEX]);
        qemu_mod_timer(s->full_timer[TCU_INDEX], next);
    }
    else
        qemu_del_timer(s->full_timer[TCU_INDEX]);
}


/* 
 *     TCNT will reset to 0 if it reach to TDFR.
 *       So for the half compare, the next fire count is (TDFR-TDHR) + TDHR
 *                                                                                            
 */
static void glue(jz4740_tcu_half_cb, TCU_INDEX) (void *opaque)
{
    struct jz4740_tcu_s *s = (struct jz4740_tcu_s *) opaque;
    int64_t next = qemu_get_clock(vm_clock);
    int64_t count;

    if ((!(s->tsr & (1 << TCU_INDEX))) && (s->ter & (1 << TCU_INDEX))
        && (s->freq[TCU_INDEX] != 0))
    {
        count = s->tdfr[TCU_INDEX] * s->prescale[TCU_INDEX];
        next += muldiv64(count, ticks_per_sec, s->freq[TCU_INDEX]);
        qemu_mod_timer(s->half_timer[TCU_INDEX], next);
        s->tfr |= 1 << (16 + TCU_INDEX);
        jz4740_tcu_update_interrupt(s);
        s->tcnt[TCU_INDEX] = s->tdhr[TCU_INDEX];
        s->time[TCU_INDEX] = qemu_get_clock(vm_clock);
    }
    else
        qemu_del_timer(s->half_timer[TCU_INDEX]);
}


static void glue(jz4740_tcu_full_cb, TCU_INDEX) (void *opaque)
{
    struct jz4740_tcu_s *s = (struct jz4740_tcu_s *) opaque;
    int64_t next = qemu_get_clock(vm_clock);
    int64_t count;

    if ((!(s->tsr & (1 << TCU_INDEX))) && (s->ter & (1 << TCU_INDEX))
        && (s->freq[TCU_INDEX] != 0))
    {
        count = s->tdfr[TCU_INDEX] * s->prescale[TCU_INDEX];
        next += muldiv64(count, ticks_per_sec, s->freq[TCU_INDEX]);
        qemu_mod_timer(s->full_timer[TCU_INDEX], next);
        s->tfr |= 1 << TCU_INDEX;
        jz4740_tcu_update_interrupt(s);
        s->tcnt[TCU_INDEX] = 0;
        s->time[TCU_INDEX] = qemu_get_clock(vm_clock);
    }
    else
        qemu_del_timer(s->full_timer[TCU_INDEX]);
}

static uint32_t glue(jz4740_tcu_read, TCU_INDEX) (void *opaque,
                                                  target_phys_addr_t addr)
{
    struct jz4740_tcu_s *s = (struct jz4740_tcu_s *) opaque;

    addr -= 0x40 + TCU_INDEX * 0x10;

    switch (addr)
    {
    case 0x0:
        return s->tdfr[TCU_INDEX];
    case 0x4:
        return s->tdhr[TCU_INDEX];
    case 0x8:
        glue(jz4740_tcu_time_sync, TCU_INDEX) (s);
        return s->tcnt[TCU_INDEX];
    case 0xc:
        return s->tcsr[TCU_INDEX];
    default:
        cpu_abort(s->soc->env,
                  "jz4740_tcu_read undefined addr "JZ_FMT_plx" timer %x \n", addr,
                  TCU_INDEX);
    }
    return (0);
}

static void glue(jz4740_tcu_write, TCU_INDEX) (void *opaque,
                                               target_phys_addr_t addr,
                                               uint32_t value)
{
    struct jz4740_tcu_s *s = (struct jz4740_tcu_s *) opaque;

    addr -= 0x40 + TCU_INDEX * 0x10;

    switch (addr)
    {
    case 0x0:
         /*TDFR*/ 
         s->tdfr[TCU_INDEX] = value & 0xffff;
        glue(jz4740_tcu_start_full, TCU_INDEX) (s);
        break;
    case 0x4:
         /*TDHR*/ s->tdhr[TCU_INDEX] = value & 0xffff;
        glue(jz4740_tcu_start_half, TCU_INDEX) (s);
        break;
    case 0x8:
         /*TCNT*/ s->tcnt[TCU_INDEX] = value & 0xffff;
        s->time[TCU_INDEX] = qemu_get_clock(vm_clock);
        glue(jz4740_tcu_start_half, TCU_INDEX) (s);
        glue(jz4740_tcu_start_full, TCU_INDEX) (s);
        break;
    case 0xc:
        s->tcsr[TCU_INDEX] = value & 0x3bf;
        switch (value & 0x7)
        {
        case 0x1:
            s->freq[TCU_INDEX] = jz_clk_getrate(jz_findclk(s->soc, "pclk"));
            break;
        case 0x2:
            s->freq[TCU_INDEX] = jz_clk_getrate(jz_findclk(s->soc, "osc_32K"));
            break;
        case 0x4:
            s->freq[TCU_INDEX] =
                jz_clk_getrate(jz_findclk(s->soc, "osc_extal"));
            break;
        default:
            s->freq[TCU_INDEX] = 0x0;
            break;
        }
        s->prescale[TCU_INDEX] = 1 << (((value & 0x38) >> 3) * 2);
        glue(jz4740_tcu_start_half, TCU_INDEX) (s);
        glue(jz4740_tcu_start_full, TCU_INDEX) (s);
        break;
    default:
        cpu_abort(s->soc->env,
                  "jz4740_tcu_write undefined addr "JZ_FMT_plx" timer %x \n", addr,
                  TCU_INDEX);

    }
}


static CPUReadMemoryFunc *glue(jz4740_tcu_readfn, TCU_INDEX)[] =
{
jz4740_badwidth_read16,
        glue(jz4740_tcu_read, TCU_INDEX), jz4740_badwidth_read16,};

static CPUWriteMemoryFunc *glue(jz4740_tcu_writefn, TCU_INDEX)[] =
{
jz4740_badwidth_write16,
        glue(jz4740_tcu_write, TCU_INDEX), jz4740_badwidth_write16,};

static void glue(jz4740_tcu_init, TCU_INDEX) (struct jz_state_s * soc,
                                              struct jz4740_tcu_s * s)
{
    int iomemtype;

    s->half_timer[TCU_INDEX] =
        qemu_new_timer(vm_clock, glue(jz4740_tcu_half_cb, TCU_INDEX), s);
    s->full_timer[TCU_INDEX] =
        qemu_new_timer(vm_clock, glue(jz4740_tcu_full_cb, TCU_INDEX), s);

    iomemtype =
        cpu_register_io_memory(0, glue(jz4740_tcu_readfn, TCU_INDEX),
                               glue(jz4740_tcu_writefn, TCU_INDEX), s);
    cpu_register_physical_memory(s->base + 0x00000040 + TCU_INDEX * 0x10,
                                 0x00000010, iomemtype);
}

#undef TCU_INDEX
#endif



#ifdef JZ4740_LCD_PANEL

#if DEPTH == 8
# define BPP 1
# define PIXEL_TYPE uint8_t
#elif DEPTH == 15 || DEPTH == 16
# define BPP 2
# define PIXEL_TYPE uint16_t
#elif DEPTH == 24
# define BPP 3
# define PIXEL_TYPE uint32_t
#elif DEPTH == 32
# define BPP 4
# define PIXEL_TYPE uint32_t
#else
# error unsupport depth
#endif

/*
 * 2-bit colour
 */
static void glue(draw_line2_, DEPTH) (uint8_t * d, const uint8_t * s, int width,
                                      const uint16_t * pal)
{
    uint8_t v, r, g, b;

    do
    {
        v = ldub_raw((void *) s);
        r = (pal[v & 3] >> 4) & 0xf0;
        g = pal[v & 3] & 0xf0;
        b = (pal[v & 3] << 4) & 0xf0;
        ((PIXEL_TYPE *) d)[0] = glue(rgb_to_pixel, DEPTH) (r, g, b);
        d += BPP;
        v >>= 2;
        r = (pal[v & 3] >> 4) & 0xf0;
        g = pal[v & 3] & 0xf0;
        b = (pal[v & 3] << 4) & 0xf0;
        ((PIXEL_TYPE *) d)[0] = glue(rgb_to_pixel, DEPTH) (r, g, b);
        d += BPP;
        v >>= 2;
        r = (pal[v & 3] >> 4) & 0xf0;
        g = pal[v & 3] & 0xf0;
        b = (pal[v & 3] << 4) & 0xf0;
        ((PIXEL_TYPE *) d)[0] = glue(rgb_to_pixel, DEPTH) (r, g, b);
        d += BPP;
        v >>= 2;
        r = (pal[v & 3] >> 4) & 0xf0;
        g = pal[v & 3] & 0xf0;
        b = (pal[v & 3] << 4) & 0xf0;
        ((PIXEL_TYPE *) d)[0] = glue(rgb_to_pixel, DEPTH) (r, g, b);
        d += BPP;
        s++;
        width -= 4;
    }
    while (width > 0);
}

/*
 * 4-bit colour
 */
static void glue(draw_line4_, DEPTH) (uint8_t * d, const uint8_t * s, int width,
                                      const uint16_t * pal)
{
    uint8_t v, r, g, b;

    do
    {
        v = ldub_raw((void *) s);
        r = (pal[v & 0xf] >> 4) & 0xf0;
        g = pal[v & 0xf] & 0xf0;
        b = (pal[v & 0xf] << 4) & 0xf0;
        ((PIXEL_TYPE *) d)[0] = glue(rgb_to_pixel, DEPTH) (r, g, b);
        d += BPP;
        v >>= 4;
        r = (pal[v & 0xf] >> 4) & 0xf0;
        g = pal[v & 0xf] & 0xf0;
        b = (pal[v & 0xf] << 4) & 0xf0;
        ((PIXEL_TYPE *) d)[0] = glue(rgb_to_pixel, DEPTH) (r, g, b);
        d += BPP;
        s++;
        width -= 2;
    }
    while (width > 0);
}

/*
 * 8-bit colour
 */
static void glue(draw_line8_, DEPTH) (uint8_t * d, const uint8_t * s, int width,
                                      const uint16_t * pal)
{
    uint8_t v, r, g, b;

    do
    {
        v = ldub_raw((void *) s);
        r = (pal[v] >> 4) & 0xf0;
        g = pal[v] & 0xf0;
        b = (pal[v] << 4) & 0xf0;
        ((PIXEL_TYPE *) d)[0] = glue(rgb_to_pixel, DEPTH) (r, g, b);
        s++;
        d += BPP;
    }
    while (--width != 0);
}


/*
 * 16-bit colour
 */
static void glue(draw_line16_, DEPTH) (uint8_t * d, const uint8_t * s,
                                       int width, const uint16_t * pal)
{
#if DEPTH == 16 && defined(WORDS_BIGENDIAN) == defined(TARGET_WORDS_BIGENDIAN)
    memcpy(d, s, width * 2);
#else
    uint16_t v;
    uint8_t r, g, b;

    do
    {
        v = lduw_raw((void *) s);
        r = (v >> 8) & 0xf8;
        g = (v >> 3) & 0xfc;
        b = (v << 3) & 0xf8;
        ((PIXEL_TYPE *) d)[0] = glue(rgb_to_pixel, DEPTH) (r, g, b);
        s += 2;
        d += BPP;
    }
    while (--width != 0);
#endif
}

/*
 * 24-bit colour.  JZ4740 uses 4 bytes to store 18/24 bit color.
 */
static void glue(draw_line24_, DEPTH) (uint8_t * d, const uint8_t * s,
                                       int width, const uint16_t * pal)
{
#if DEPTH == 32 && defined(WORDS_BIGENDIAN) == defined(TARGET_WORDS_BIGENDIAN)
    memcpy(d, s, width * 4);
#else
    uint32_t v;
    uint8_t r, g, b;

    do
    {
        v = ldl_raw((void *) s);
        r = (v >> 16) & 0xff;
        g = (v >> 8) & 0xff;
        b = (v >> 0) & 0xff;
        ((PIXEL_TYPE *) d)[0] = glue(rgb_to_pixel, DEPTH) (r, g, b);
        s += 3;
        d += BPP;
    }
    while (--width != 0);
#endif
}

static jz4740_lcd_fn_t glue(jz4740_lcd_draw_fn_, DEPTH)[6] =
{
    NULL,                       /*0x0 *//*1 bit per pixel */
        (jz4740_lcd_fn_t) glue(draw_line2_, DEPTH),     /*0x1 *//*2 bit per pixel */
        (jz4740_lcd_fn_t) glue(draw_line4_, DEPTH),     /*0x2 *//*4 bit per pixel */
        (jz4740_lcd_fn_t) glue(draw_line8_, DEPTH),     /*0x3 *//*8 bit per pixel */
        (jz4740_lcd_fn_t) glue(draw_line16_, DEPTH),    /*0x4 *//*15/16 bit per pixel */
        (jz4740_lcd_fn_t) glue(draw_line24_, DEPTH),    /*0x5 *//*18/24 bit per pixel */
};


#undef DEPTH
#undef BPP
#undef PIXEL_TYPE

#endif
