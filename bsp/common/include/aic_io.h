/*
 * Copyright (c) 2022, Artinchip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __AIC_IO_H__
#define __AIC_IO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "aic_common.h"

#ifndef BIT
#define BIT(s) (1U << (s))
#define BIT_UL(s) (1UL << (s))
#endif

#define GENMASK(h, l)       (((~(0U)) - ((1U) << (l)) + 1) & \
                             (~(0U) >> (BITS_PER_LONG - 1 - (h))))
#define GENMASK_UL(h, l)    (((~(0UL)) - ((1UL) << (l)) + 1) & \
                             (~(0UL) >> (BITS_PER_LONG - 1 - (h))))

static inline void __raw_writeb(u8 val, volatile void *addr)
{
    asm volatile("sb %0, 0(%1)" : : "r" (val), "r" (addr));
}

static inline u8 __raw_readb(const volatile void *addr)
{
    u8 val;

    asm volatile("lb %0, 0(%1)" : "=r" (val) : "r" (addr));
    return val;
}

static inline void __raw_writel(u32 val, volatile void *addr)
{
    asm volatile("sw %0, 0(%1)" : : "r"(val), "r"(addr));
}

static inline u32 __raw_readl(const volatile void *addr)
{
    u32 val;

    asm volatile("lw %0, 0(%1)" : "=r"(val) : "r"(addr));
    return val;
}

#define mmiowb_set_pending() \
    do { \
    } while (0)
#define cpu_to_le32(val) (val)
#define le32_to_cpu(val) (val)

#define readb_cpu(c)    ({ u8  __r = __raw_readb(c); __r; })
#define readl_cpu(c)    ({ u32 __r = le32_to_cpu(__raw_readl(c)); __r; })

#define writeb_cpu(v, c)    ((void)__raw_writeb((v), (c)))
#define writel_cpu(v, c)    ((void)__raw_writel((u32)cpu_to_le32(v), (c)))

#define __io_br()       do { } while (0)
#define __io_ar(v)      __asm__ __volatile__("fence i,r" : : : "memory");
#define __io_bw()       __asm__ __volatile__("fence w,o" : : : "memory");
#define __io_aw()       mmiowb_set_pending()

#define readb(c) \
    ({ \
        u8  __v; \
        __io_br(); \
        __v = readb_cpu((volatile void *)c); \
        __io_ar(__v); __v; \
    })
#define readl(c) \
    ({ \
        u32 __v; \
        __io_br(); \
        __v = readl_cpu((volatile void *)c); \
        __io_ar(__v); \
        __v; \
    })

#define writeb(v, c) \
    ({ \
        __io_bw(); \
        writeb_cpu((v), (volatile void *)(c)); \
        __io_aw(); \
    })
#define writel(v, c) \
    ({ \
        __io_bw(); \
        writel_cpu((v), ((volatile void *)c)); \
        __io_aw(); \
    })

/* Some common bit operation of a (long type) register */
#define writel_clrbits(mask, addr)       writel(readl(addr) & ~(mask), addr)
#define writel_clrbit(bit, addr)         writel_clrbits(bit, addr)
#define writel_bits(val, mask, shift, addr) \
    ({ \
        if (val) \
            writel((readl(addr) & ~(mask)) | ((val) << (shift)), addr); \
        else \
            writel_clrbits(mask, addr); \
    })
#define writel_bit(bit, addr)           writel(readl(addr) | bit, addr)
#define readl_bits(mask, shift, addr)   ((readl(addr) & (mask)) >> (shift))
#define readl_bit(bit, addr)            ((readl(addr) & bit) ? 1 : 0)

/* Some common bit operation of a variable */
#define clrbits(mask, cur)              (cur &= ~(mask))
#define clrbit(bit, cur)                clrbits(bit, cur)
#define setbits(val, mask, shift, cur) \
    ({ \
        if (val) \
            (cur = (cur & ~(mask)) | ((val) << (shift))); \
        else \
            clrbits(mask, cur); \
    })
#define setbit(bit, cur)                (cur |= bit)
#define getbits(mask, shift, cur)       ((cur & (mask)) >> (shift))
#define getbit(bit, cur)                (cur & bit ? 1 : 0)

#define bitrev8(x)   \
({                  \
    u8 ___x = x;            \
    ___x = (___x >> 4) | (___x << 4);   \
    ___x = ((___x & (u8)0xCCU) >> 2) | ((___x & (u8)0x33U) << 2);   \
    ___x = ((___x & (u8)0xAAU) >> 1) | ((___x & (u8)0x55U) << 1);   \
    ___x;                               \
})

#ifdef __cplusplus
}
#endif

#endif /* __AIC_IO_H__ */
