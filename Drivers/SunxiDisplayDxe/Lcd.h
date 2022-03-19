/**
 SPDX-License-Identifier: GPL-2.0+
 
 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on U-Boot driver
 (C) Copyright 2013-2014 Luc Verhaegen <libv@skynet.be>
 (C) Copyright 2014-2015 Hans de Goede <hdegoede@redhat.com>

**/

#pragma once

#define SUNXI_LCD_CTRL                0x00  /* 32 bit */
#define SUNXI_LCD_INT0                0x04  /* 32 bit */
#define SUNXI_LCD_INT1                0x08  /* 32 bit */
#define SUNXI_LCD_TCON0_FRM_CTRL      0x10  /* 32 bit */
#define SUNXI_LCD_TCON0_FRM_SEED_0    0x14  /* 32 bit */
#define SUNXI_LCD_TCON0_FRM_SEED_1    0x18  /* 32 bit */
#define SUNXI_LCD_TCON0_FRM_SEED_2    0x1C  /* 32 bit */
#define SUNXI_LCD_TCON0_FRM_SEED_3    0x20  /* 32 bit */
#define SUNXI_LCD_TCON0_FRM_SEED_4    0x24  /* 32 bit */
#define SUNXI_LCD_TCON0_FRM_SEED_5    0x28  /* 32 bit */
#define SUNXI_LCD_TCON0_FRM_TABLE_0   0x2c  /* 32 bit */
#define SUNXI_LCD_TCON0_FRM_TABLE_1   0x30  /* 32 bit */
#define SUNXI_LCD_TCON0_FRM_TABLE_2   0x34  /* 32 bit */
#define SUNXI_LCD_TCON0_FRM_TABLE_3   0x38  /* 32 bit */
#define SUNXI_LCD_TCON0_CTRL          0x40  /* 32 bit */
#define SUNXI_LCD_TCON0_DCLK          0x44  /* 32 bit */
#define SUNXI_LCD_TCON0_TIMING_ACTIVE 0x48  /* 32 bit */
#define SUNXI_LCD_TCON0_TIMING_H      0x4c  /* 32 bit */
#define SUNXI_LCD_TCON0_TIMING_V      0x50  /* 32 bit */
#define SUNXI_LCD_TCON0_TIMING_SYNC   0x54  /* 32 bit */
#define SUNXI_LCD_TCON0_HV_INTF       0x58  /* 32 bit */
#define SUNXI_LCD_TCON0_CPU_INTF      0x60  /* 32 bit */
#define SUNXI_LCD_TCON0_CPU_WR_DAT    0x64  /* 32 bit */
#define SUNXI_LCD_TCON0_CPU_RD_DAT0   0x68  /* 32 bit */
#define SUNXI_LCD_TCON0_CPU_RD_DAT1   0x6c  /* 32 bit */
#define SUNXI_LCD_TCON0_TTL_TIMING_0  0x70  /* 32 bit */
#define SUNXI_LCD_TCON0_TTL_TIMING_1  0x74  /* 32 bit */
#define SUNXI_LCD_TCON0_TTL_TIMING_2  0x78  /* 32 bit */
#define SUNXI_LCD_TCON0_TTL_TIMING_3  0x7c  /* 32 bit */
#define SUNXI_LCD_TCON0_TTL_TIMING_4  0x80  /* 32 bit */
#define SUNXI_LCD_TCON0_LVDS_INTF     0x84  /* 32 bit */
#define SUNXI_LCD_TCON0_IO_POLARITY   0x88  /* 32 bit */
#define SUNXI_LCD_TCON0_IO_TRISTATE   0x8c  /* 32 bit */
#define SUNXI_LCD_TCON1_CTRL          0x90  /* 32 bit */
#define SUNXI_LCD_TCON1_TIMING_SOURCE 0x94  /* 32 bit */
#define SUNXI_LCD_TCON1_TIMING_SCALE  0x98  /* 32 bit */
#define SUNXI_LCD_TCON1_TIMING_OUT    0x9c  /* 32 bit */
#define SUNXI_LCD_TCON1_TIMING_H      0xa0  /* 32 bit */
#define SUNXI_LCD_TCON1_TIMING_V      0xa4  /* 32 bit */
#define SUNXI_LCD_TCON1_TIMING_SYNC   0xa8  /* 32 bit */
#define SUNXI_LCD_TCON1_IO_POLARITY   0xf0  /* 32 bit */
#define SUNXI_LCD_TCON1_IO_TRISTATE   0xf4  /* 32 bit */
#define SUNXI_LCD_MUX_CTRL            0x200 /* 32 bit */
#define SUNXI_LCD_LVDS_ANA_0          0x220 /* 32 bit */
#define SUNXI_LCD_LVDS_ANA_1          0x224 /* 32 bit */

#define SUNXI_LCDC_X(x)                         (((x) - 1) << 16)
#define SUNXI_LCDC_Y(y)                         (((y) - 1) << 0)
#define SUNXI_LCDC_TCON_VSYNC_MASK              (1 << 24)
#define SUNXI_LCDC_TCON_HSYNC_MASK              (1 << 25)
#define SUNXI_LCDC_CTRL_IO_MAP_MASK             (1 << 0)
#define SUNXI_LCDC_CTRL_IO_MAP_TCON0            (0 << 0)
#define SUNXI_LCDC_CTRL_IO_MAP_TCON1            (1 << 0)
#define SUNXI_LCDC_CTRL_TCON_ENABLE             (1 << 31)
#define SUNXI_LCDC_TCON0_FRM_CTRL_RGB666        ((1 << 31) | (0 << 4))
#define SUNXI_LCDC_TCON0_FRM_CTRL_RGB565        ((1 << 31) | (5 << 4))
#define SUNXI_LCDC_TCON0_FRM_SEED               0x11111111
#define SUNXI_LCDC_TCON0_FRM_TAB0               0x01010000
#define SUNXI_LCDC_TCON0_FRM_TAB1               0x15151111
#define SUNXI_LCDC_TCON0_FRM_TAB2               0x57575555
#define SUNXI_LCDC_TCON0_FRM_TAB3               0x7f7f7777
#define SUNXI_LCDC_TCON0_CTRL_CLK_DELAY(n)      (((n) & 0x1f) << 4)
#define SUNXI_LCDC_TCON0_CTRL_ENABLE            (1 << 31)
#define SUNXI_LCDC_TCON0_DCLK_DIV(n)            ((n) << 0)
#define SUNXI_LCDC_TCON0_DCLK_ENABLE            (0xf << 28)
#define SUNXI_LCDC_TCON0_TIMING_H_BP(n)         (((n) - 1) << 0)
#define SUNXI_LCDC_TCON0_TIMING_H_TOTAL(n)      (((n) - 1) << 16)
#define SUNXI_LCDC_TCON0_TIMING_V_BP(n)         (((n) - 1) << 0)
#define SUNXI_LCDC_TCON0_TIMING_V_TOTAL(n)      (((n) * 2) << 16)
#ifdef CONFIG_SUNXI_GEN_SUN6I
#define SUNXI_LCDC_TCON0_LVDS_CLK_SEL_TCON0     (1 << 20)
#else
#define SUNXI_LCDC_TCON0_LVDS_CLK_SEL_TCON0     0 /* NA */
#endif
#define SUNXI_LCDC_TCON0_LVDS_INTF_BITWIDTH(n)  ((n) << 26)
#define SUNXI_LCDC_TCON0_LVDS_INTF_ENABLE       (1 << 31)
#define SUNXI_LCDC_TCON0_IO_POL_DCLK_PHASE(x)   ((x) << 28)
#define SUNXI_LCDC_TCON1_CTRL_CLK_DELAY(n)      (((n) & 0x1f) << 4)
#define SUNXI_LCDC_TCON1_CTRL_INTERLACE_ENABLE  (1 << 20)
#define SUNXI_LCDC_TCON1_CTRL_ENABLE            (1 << 31)
#define SUNXI_LCDC_TCON1_TIMING_H_BP(n)         (((n) - 1) << 0)
#define SUNXI_LCDC_TCON1_TIMING_H_TOTAL(n)      (((n) - 1) << 16)
#define SUNXI_LCDC_TCON1_TIMING_V_BP(n)         (((n) - 1) << 0)
#define SUNXI_LCDC_TCON1_TIMING_V_TOTAL(n)      ((n) << 16)
#define SUNXI_LCDC_MUX_CTRL_SRC0_MASK           (0xf << 0)
#define SUNXI_LCDC_MUX_CTRL_SRC0(x)             ((x) << 0)
#define SUNXI_LCDC_MUX_CTRL_SRC1_MASK           (0xf << 4)
#define SUNXI_LCDC_MUX_CTRL_SRC1(x)             ((x) << 4)
#ifdef CONFIG_SUNXI_GEN_SUN6I
#define SUNXI_LCDC_LVDS_ANA0                    0x40040320
#define SUNXI_LCDC_LVDS_ANA0_EN_MB              (1 << 31)
#define SUNXI_LCDC_LVDS_ANA0_DRVC               (1 << 24)
#define SUNXI_LCDC_LVDS_ANA0_DRVD(x)            ((x) << 20)
#else
#define SUNXI_LCDC_LVDS_ANA0                    0x3f310000
#define SUNXI_LCDC_LVDS_ANA0_UPDATE             (1 << 22)
#endif
#define SUNXI_LCDC_LVDS_ANA1_INIT1              (0x1f << 26 | 0x1f << 10)
#define SUNXI_LCDC_LVDS_ANA1_INIT2              (0x1f << 16 | 0x1f << 00)
