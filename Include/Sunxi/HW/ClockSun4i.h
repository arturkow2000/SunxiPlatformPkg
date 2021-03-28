/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * sun4i, sun5i and sun7i clock register definitions
 *
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 */

#ifndef _SUNXI_CLOCK_SUN4I_H
#define _SUNXI_CLOCK_SUN4I_H

#define CCM_BASE FixedPcdGet32(CcmBase)

#define CCM_OFFSET_PLL1_CFG 0x000         /* 0x00 pll1 control */
#define CCM_OFFSET_PLL1_TUN 0x004         /* 0x04 pll1 tuning */
#define CCM_OFFSET_PLL2_CFG 0x008         /* 0x08 pll2 control */
#define CCM_OFFSET_PLL2_TUN 0x00c         /* 0x0c pll2 tuning */
#define CCM_OFFSET_PLL3_CFG 0x010         /* 0x10 pll3 control */
#define CCM_OFFSET_PLL4_CFG 0x018         /* 0x18 pll4 control */
#define CCM_OFFSET_PLL5_CFG 0x020         /* 0x20 pll5 control */
#define CCM_OFFSET_PLL5_TUN 0x024         /* 0x24 pll5 tuning */
#define CCM_OFFSET_PLL6_CFG 0x028         /* 0x28 pll6 control */
#define CCM_OFFSET_PLL6_TUN 0x02c         /* 0x2c pll6 tuning */
#define CCM_OFFSET_PLL7_CFG 0x030         /* 0x30 pll7 control */
#define CCM_OFFSET_PLL1_TUN2 0x034        /* 0x34 pll5 tuning2 */
#define CCM_OFFSET_PLL5_TUN2 0x03c        /* 0x3c pll5 tuning2 */
#define CCM_OFFSET_PLL_LOCK_DBG 0x04c     /* 0x4c pll lock time debug */
#define CCM_OFFSET_OSC24M_CFG 0x050       /* 0x50 osc24m control */
#define CCM_OFFSET_CPU_AHB_APB0_CFG 0x054 /* 0x54 cpu,ahb and apb0 divide ratio */
#define CCM_OFFSET_APB1_CLK_DIV_CFG 0x058 /* 0x58 apb1 clock dividor */
#define CCM_OFFSET_AXI_GATE 0x05c         /* 0x5c axi module clock gating */
#define CCM_OFFSET_AHB_GATE0 0x060        /* 0x60 ahb module clock gating 0 */
#define CCM_OFFSET_AHB_GATE1 0x064        /* 0x64 ahb module clock gating 1 */
#define CCM_OFFSET_APB0_GATE 0x068        /* 0x68 apb0 module clock gating */
#define CCM_OFFSET_APB1_GATE 0x06c        /* 0x6c apb1 module clock gating */
#define CCM_OFFSET_NAND0_CLK__CFG 0x080   /* 0x80 nand sub clock control */
#define CCM_OFFSET_MS_SCLK_CFG 0x084      /* 0x84 memory stick sub clock control */
#define CCM_OFFSET_SD0_CLK_CFG 0x088      /* 0x88 sd0 clock control */
#define CCM_OFFSET_SD1_CLK_CFG 0x08c      /* 0x8c sd1 clock control */
#define CCM_OFFSET_SD2_CLK_CFG 0x090      /* 0x90 sd2 clock control */
#define CCM_OFFSET_SD3_CLK_CFG 0x094      /* 0x94 sd3 clock control */
#define CCM_OFFSET_TS_CLK_CFG 0x098       /* 0x98 transport stream clock control */
#define CCM_OFFSET_SS_CLK_CFG 0x09c       /* 0x9c */
#define CCM_OFFSET_SPI0_CLK_CFG 0x0a0     /* 0xa0 */
#define CCM_OFFSET_SPI1_CLK_CFG 0x0a4     /* 0xa4 */
#define CCM_OFFSET_SPI2_CLK_CFG 0x0a8     /* 0xa8 */
#define CCM_OFFSET_IR0_CLK_CFG 0x0b0      /* 0xac */
#define CCM_OFFSET_IR1_CLK_CFG 0x0b4      /* 0xb4 */
#define CCM_OFFSET_IIS_CLK_CFG 0x0b8      /* 0xb8 */
#define CCM_OFFSET_AC97_CLK_CFG 0x0bc     /* 0xbc */
#define CCM_OFFSET_SPDIF_CLK_CFG 0x0c0    /* 0xc0 */
#define CCM_OFFSET_KEYPAD_CLK_CFG 0x0c4   /* 0xc4 */
#define CCM_OFFSET_SATA_CLK_CFG 0x0c8     /* 0xc8 */
#define CCM_OFFSET_USB_CLK_CFG 0x0cc      /* 0xcc */
#define CCM_OFFSET_GPS_CLK_CFG 0x0d0      /* 0xd0 */
#define CCM_OFFSET_SPI3_CLK_CFG 0x0d4     /* 0xd4 */

#if 0
struct sunxi_ccm_reg {
        u32 dram_clk_gate;      /* 0x100 */
        u32 be0_clk_cfg;        /* 0x104 */
        u32 be1_clk_cfg;        /* 0x108 */
        u32 fe0_clk_cfg;        /* 0x10c */
        u32 fe1_clk_cfg;        /* 0x110 */
        u32 mp_clk_cfg;         /* 0x114 */
        u32 lcd0_ch0_clk_cfg;   /* 0x118 */
        u32 lcd1_ch0_clk_cfg;   /* 0x11c */
        u32 csi_isp_clk_cfg;    /* 0x120 */
        u8 res6[0x4];
        u32 tvd_clk_reg;        /* 0x128 */
        u32 lcd0_ch1_clk_cfg;   /* 0x12c */
        u32 lcd1_ch1_clk_cfg;   /* 0x130 */
        u32 csi0_clk_cfg;       /* 0x134 */
        u32 csi1_clk_cfg;       /* 0x138 */
        u32 ve_clk_cfg;         /* 0x13c */
        u32 audio_codec_clk_cfg;        /* 0x140 */
        u32 avs_clk_cfg;        /* 0x144 */
        u32 ace_clk_cfg;        /* 0x148 */
        u32 lvds_clk_cfg;       /* 0x14c */
        u32 hdmi_clk_cfg;       /* 0x150 */
        u32 mali_clk_cfg;       /* 0x154 */
        u8 res7[0x4];
        u32 mbus_clk_cfg;       /* 0x15c */
        u8 res8[0x4];
        u32 gmac_clk_cfg;       /* 0x164 */
};
#endif

/* apb1 bit field */
#define APB1_CLK_SRC_SHIFT 24
#define APB1_CLK_SRC_MASK (0x3 << APB1_CLK_SRC_SHIFT)
#define APB1_CLK_SRC_OSC24M 0
#define APB1_CLK_SRC_PLL6 1
#define APB1_CLK_SRC_LOSC 2
#define APB1_CLK_RATE_N_SHIFT (16)
#define APB1_CLK_RATE_N_MASK (3 << APB1_CLK_RATE_N_SHIFT)
#define APB1_CLK_RATE_N_1 0
#define APB1_CLK_RATE_N_2 1
#define APB1_CLK_RATE_N_4 2
#define APB1_CLK_RATE_N_8 3
#define APB1_CLK_RATE_M(m) (((m)-1) << 0)
#define APB1_CLK_RATE_M_MASK (0x1f << 0)

/* apb1 gate field */
#define APB1_GATE_UART_SHIFT (16)
#define APB1_GATE_UART_MASK (0xff << APB1_GATE_UART_SHIFT)
#define APB1_GATE_TWI_SHIFT (0)
#define APB1_GATE_TWI_MASK (0xf << APB1_GATE_TWI_SHIFT)

/* clock divide */
#define AXI_DIV_SHIFT (0)
#define AXI_DIV_MASK (3 << AXI_DIV_SHIFT)
#define AXI_DIV_1 0
#define AXI_DIV_2 1
#define AXI_DIV_3 2
#define AXI_DIV_4 3
#define AHB_DIV_SHIFT (4)
#define AHB_DIV_MASK (3 << AHB_DIV_SHIFT)
#define AHB_DIV_1 0
#define AHB_DIV_2 1
#define AHB_DIV_4 2
#define AHB_DIV_8 3
#define APB0_DIV_SHIFT (8)
#define APB0_DIV_MASK (3 << APB0_DIV_SHIFT)
#define APB0_DIV_1 0
#define APB0_DIV_2 1
#define APB0_DIV_4 2
#define APB0_DIV_8 3
#define CPU_CLK_SRC_SHIFT (16)
#define CPU_CLK_SRC_MASK (3 << CPU_CLK_SRC_SHIFT)
#define CPU_CLK_SRC_OSC24M 1
#define CPU_CLK_SRC_PLL1 2

#define CCM_PLL1_CFG_ENABLE_SHIFT 31
#define CCM_PLL1_CFG_VCO_RST_SHIFT 30
#define CCM_PLL1_CFG_VCO_BIAS_SHIFT 26
#define CCM_PLL1_CFG_PLL4_EXCH_SHIFT 25
#define CCM_PLL1_CFG_BIAS_CUR_SHIFT 20
#define CCM_PLL1_CFG_DIVP_SHIFT 16
#define CCM_PLL1_CFG_DIVP_MASK 0xF0000
#define CCM_PLL1_CFG_LCK_TMR_SHIFT 13
#define CCM_PLL1_CFG_FACTOR_N_SHIFT 8
#define CCM_PLL1_CFG_FACTOR_N_MASK 0x1F00
#define CCM_PLL1_CFG_FACTOR_K_SHIFT 4
#define CCM_PLL1_CFG_FACTOR_K_MASK 0xf0
#define CCM_PLL1_CFG_SIG_DELT_PAT_IN_SHIFT 3
#define CCM_PLL1_CFG_SIG_DELT_PAT_EN_SHIFT 2
#define CCM_PLL1_CFG_FACTOR_M_SHIFT 0
#define CCM_PLL1_CFG_FACTOR_M_MASK 0x3

#define PLL1_CFG_DEFAULT 0xa1005000

#if defined CONFIG_OLD_SUNXI_KERNEL_COMPAT && defined CONFIG_MACH_SUN5I
/*
 * Older linux-sunxi-3.4 kernels override our PLL6 setting with 300 MHz,
 * halving the mbus frequency, so set it to 300 MHz ourselves and base the
 * mbus divider on that.
 */
#define PLL6_CFG_DEFAULT 0xa1009900
#else
#define PLL6_CFG_DEFAULT 0xa1009911
#endif

/* nand clock */
#define NAND_CLK_SRC_OSC24 0
#define NAND_CLK_DIV_N 0
#define NAND_CLK_DIV_M 0

/* gps clock */
#define GPS_SCLK_GATING_OFF 0
#define GPS_RESET 0

/* ahb clock gate bit offset */
#define AHB_GATE_OFFSET_GPS 26
#define AHB_GATE_OFFSET_SATA 25
#define AHB_GATE_OFFSET_PATA 24
#define AHB_GATE_OFFSET_SPI3 23
#define AHB_GATE_OFFSET_SPI2 22
#define AHB_GATE_OFFSET_SPI1 21
#define AHB_GATE_OFFSET_SPI0 20
#define AHB_GATE_OFFSET_TS0 18
#define AHB_GATE_OFFSET_EMAC 17
#define AHB_GATE_OFFSET_ACE 16
#define AHB_GATE_OFFSET_DLL 15
#define AHB_GATE_OFFSET_SDRAM 14
#define AHB_GATE_OFFSET_NAND0 13
#define AHB_GATE_OFFSET_MS 12
#define AHB_GATE_OFFSET_MMC3 11
#define AHB_GATE_OFFSET_MMC2 10
#define AHB_GATE_OFFSET_MMC1 9
#define AHB_GATE_OFFSET_MMC0 8
#define AHB_GATE_OFFSET_MMC(n) (AHB_GATE_OFFSET_MMC0 + (n))
#define AHB_GATE_OFFSET_BIST 7
#define AHB_GATE_OFFSET_DMA 6
#define AHB_GATE_OFFSET_SS 5
#define AHB_GATE_OFFSET_USB_OHCI1 4
#define AHB_GATE_OFFSET_USB_EHCI1 3
#define AHB_GATE_OFFSET_USB_OHCI0 2
#define AHB_GATE_OFFSET_USB_EHCI0 1
#define AHB_GATE_OFFSET_USB0 0

/* ahb clock gate bit offset (second register) */
#define AHB_GATE_OFFSET_GMAC 17
#define AHB_GATE_OFFSET_DE_FE0 14
#define AHB_GATE_OFFSET_DE_BE0 12
#define AHB_GATE_OFFSET_HDMI 11
#define AHB_GATE_OFFSET_LCD1 5
#define AHB_GATE_OFFSET_LCD0 4
#define AHB_GATE_OFFSET_TVE1 3
#define AHB_GATE_OFFSET_TVE0 2

#define CCM_AHB_GATE_GPS (0x1 << 26)
#define CCM_AHB_GATE_SDRAM (0x1 << 14)
#define CCM_AHB_GATE_DLL (0x1 << 15)
#define CCM_AHB_GATE_ACE (0x1 << 16)

#define CCM_PLL3_CTRL_M_SHIFT 0
#define CCM_PLL3_CTRL_M_MASK (0x7f << CCM_PLL3_CTRL_M_SHIFT)
#define CCM_PLL3_CTRL_M(n) (((n)&0x7f) << 0)
#define CCM_PLL3_CTRL_INTEGER_MODE (0x1 << 15)
#define CCM_PLL3_CTRL_EN (0x1 << 31)

#define CCM_PLL5_CTRL_M(n) (((n)&0x3) << 0)
#define CCM_PLL5_CTRL_M_MASK CCM_PLL5_CTRL_M(0x3)
#define CCM_PLL5_CTRL_M_X(n) ((n)-1)
#define CCM_PLL5_CTRL_M1(n) (((n)&0x3) << 2)
#define CCM_PLL5_CTRL_M1_MASK CCM_PLL5_CTRL_M1(0x3)
#define CCM_PLL5_CTRL_M1_X(n) ((n)-1)
#define CCM_PLL5_CTRL_K(n) (((n)&0x3) << 4)
#define CCM_PLL5_CTRL_K_SHIFT 4
#define CCM_PLL5_CTRL_K_MASK CCM_PLL5_CTRL_K(0x3)
#define CCM_PLL5_CTRL_K_X(n) ((n)-1)
#define CCM_PLL5_CTRL_LDO (0x1 << 7)
#define CCM_PLL5_CTRL_N(n) (((n)&0x1f) << 8)
#define CCM_PLL5_CTRL_N_SHIFT 8
#define CCM_PLL5_CTRL_N_MASK CCM_PLL5_CTRL_N(0x1f)
#define CCM_PLL5_CTRL_N_X(n) (n)
#define CCM_PLL5_CTRL_P(n) (((n)&0x3) << 16)
#define CCM_PLL5_CTRL_P_SHIFT 16
#define CCM_PLL5_CTRL_P_MASK CCM_PLL5_CTRL_P(0x3)
#define CCM_PLL5_CTRL_P_X(n) ((n)-1)
#define CCM_PLL5_CTRL_BW (0x1 << 18)
#define CCM_PLL5_CTRL_VCO_GAIN (0x1 << 19)
#define CCM_PLL5_CTRL_BIAS(n) (((n)&0x1f) << 20)
#define CCM_PLL5_CTRL_BIAS_MASK CCM_PLL5_CTRL_BIAS(0x1f)
#define CCM_PLL5_CTRL_BIAS_X(n) ((n)-1)
#define CCM_PLL5_CTRL_VCO_BIAS (0x1 << 25)
#define CCM_PLL5_CTRL_DDR_CLK (0x1 << 29)
#define CCM_PLL5_CTRL_BYPASS (0x1 << 30)
#define CCM_PLL5_CTRL_EN (0x1 << 31)

#define CCM_PLL6_CTRL_EN 31
#define CCM_PLL6_CTRL_BYPASS_EN 30
#define CCM_PLL6_CTRL_SATA_EN_SHIFT 14
#define CCM_PLL6_CTRL_N_SHIFT 8
#define CCM_PLL6_CTRL_N_MASK (0x1f << CCM_PLL6_CTRL_N_SHIFT)
#define CCM_PLL6_CTRL_K_SHIFT 4
#define CCM_PLL6_CTRL_K_MASK (0x3 << CCM_PLL6_CTRL_K_SHIFT)

#define CCM_GPS_CTRL_RESET (0x1 << 0)
#define CCM_GPS_CTRL_GATE (0x1 << 1)

#define CCM_DRAM_CTRL_DCLK_OUT (0x1 << 15)

#define CCM_MBUS_CTRL_M(n) (((n)&0xf) << 0)
#define CCM_MBUS_CTRL_M_MASK CCM_MBUS_CTRL_M(0xf)
#define CCM_MBUS_CTRL_M_X(n) ((n)-1)
#define CCM_MBUS_CTRL_N(n) (((n)&0xf) << 16)
#define CCM_MBUS_CTRL_N_MASK CCM_MBUS_CTRL_N(0xf)
#define CCM_MBUS_CTRL_N_X(n) (((n) >> 3) ? 3 : (((n) >> 2) ? 2 : (((n) >> 1) ? 1 : 0)))
#define CCM_MBUS_CTRL_CLK_SRC(n) (((n)&0x3) << 24)
#define CCM_MBUS_CTRL_CLK_SRC_MASK CCM_MBUS_CTRL_CLK_SRC(0x3)
#define CCM_MBUS_CTRL_CLK_SRC_HOSC 0x0
#define CCM_MBUS_CTRL_CLK_SRC_PLL6 0x1
#define CCM_MBUS_CTRL_CLK_SRC_PLL5 0x2
#define CCM_MBUS_CTRL_GATE (0x1 << 31)

#define CCM_NAND_CTRL_M(x) ((x)-1)
#define CCM_NAND_CTRL_N(x) ((x) << 16)
#define CCM_NAND_CTRL_OSCM24 (0x0 << 24)
#define CCM_NAND_CTRL_PLL6 (0x1 << 24)
#define CCM_NAND_CTRL_PLL5 (0x2 << 24)
#define CCM_NAND_CTRL_ENABLE (0x1 << 31)

#define CCM_MMC_CTRL_M(x) ((x)-1)
#define CCM_MMC_CTRL_OCLK_DLY(x) ((x) << 8)
#define CCM_MMC_CTRL_N(x) ((x) << 16)
#define CCM_MMC_CTRL_SCLK_DLY(x) ((x) << 20)
#define CCM_MMC_CTRL_OSCM24 (0x0 << 24)
#define CCM_MMC_CTRL_PLL6 (0x1 << 24)
#define CCM_MMC_CTRL_PLL5 (0x2 << 24)
#define CCM_MMC_CTRL_ENABLE (0x1 << 31)

#define CCM_DRAM_GATE_OFFSET_DE_FE1 24 /* Note the order of FE1 and */
#define CCM_DRAM_GATE_OFFSET_DE_FE0 25 /* FE0 is swapped ! */
#define CCM_DRAM_GATE_OFFSET_DE_BE0 26
#define CCM_DRAM_GATE_OFFSET_DE_BE1 27

#define CCM_LCD_CH0_CTRL_PLL3 (0 << 24)
#define CCM_LCD_CH0_CTRL_PLL7 (1 << 24)
#define CCM_LCD_CH0_CTRL_PLL3_2X (2 << 24)
#define CCM_LCD_CH0_CTRL_PLL7_2X (3 << 24)
#define CCM_LCD_CH0_CTRL_MIPI_PLL 0 /* No mipi pll on sun4i/5i/7i */
#ifdef CONFIG_MACH_SUN5I
#define CCM_LCD_CH0_CTRL_TVE_RST (0x1 << 29)
#else
#define CCM_LCD_CH0_CTRL_TVE_RST 0 /* No separate tve-rst on sun4i/7i */
#endif
#define CCM_LCD_CH0_CTRL_RST (0x1 << 30)
#define CCM_LCD_CH0_CTRL_GATE (0x1 << 31)

#define CCM_LCD_CH1_CTRL_M(n) ((((n)-1) & 0xf) << 0)
#define CCM_LCD_CH1_CTRL_HALF_SCLK1 (1 << 11)
#define CCM_LCD_CH1_CTRL_PLL3 (0 << 24)
#define CCM_LCD_CH1_CTRL_PLL7 (1 << 24)
#define CCM_LCD_CH1_CTRL_PLL3_2X (2 << 24)
#define CCM_LCD_CH1_CTRL_PLL7_2X (3 << 24)
/* Enable / disable both ch1 sclk1 and sclk2 at the same time */
#define CCM_LCD_CH1_CTRL_GATE (0x1 << 31 | 0x1 << 15)

#define CCM_LVDS_CTRL_RST (1 << 0)

#define CCM_HDMI_CTRL_M(n) ((((n)-1) & 0xf) << 0)
#define CCM_HDMI_CTRL_PLL_MASK (3 << 24)
#define CCM_HDMI_CTRL_PLL3 (0 << 24)
#define CCM_HDMI_CTRL_PLL7 (1 << 24)
#define CCM_HDMI_CTRL_PLL3_2X (2 << 24)
#define CCM_HDMI_CTRL_PLL7_2X (3 << 24)
/* No separate ddc gate on sun4i, sun5i and sun7i */
#define CCM_HDMI_CTRL_DDC_GATE 0
#define CCM_HDMI_CTRL_GATE (0x1 << 31)

#define CCM_GMAC_CTRL_TX_CLK_SRC_MII 0x0
#define CCM_GMAC_CTRL_TX_CLK_SRC_EXT_RGMII 0x1
#define CCM_GMAC_CTRL_TX_CLK_SRC_INT_RGMII 0x2
#define CCM_GMAC_CTRL_GPIT_MII (0x0 << 2)
#define CCM_GMAC_CTRL_GPIT_RGMII (0x1 << 2)
#define CCM_GMAC_CTRL_RX_CLK_DELAY(x) ((x) << 5)
#define CCM_GMAC_CTRL_TX_CLK_DELAY(x) ((x) << 10)

#define CCM_USB_CTRL_PHY0_RST (0x1 << 0)
#define CCM_USB_CTRL_PHY1_RST (0x1 << 1)
#define CCM_USB_CTRL_PHY2_RST (0x1 << 2)
#define CCM_USB_CTRL_OHCI0_CLK (0x1 << 6)
#define CCM_USB_CTRL_OHCI1_CLK (0x1 << 7)
#define CCM_USB_CTRL_PHYGATE (0x1 << 8)
/* These 3 are sun6i only, define them as 0 on sun4i */
#define CCM_USB_CTRL_PHY0_CLK 0
#define CCM_USB_CTRL_PHY1_CLK 0
#define CCM_USB_CTRL_PHY2_CLK 0

/* CCM bits common to all Display Engine (and IEP) clock ctrl regs */
#define CCM_DE_CTRL_M(n) ((((n)-1) & 0xf) << 0)
#define CCM_DE_CTRL_PLL_MASK (3 << 24)
#define CCM_DE_CTRL_PLL3 (0 << 24)
#define CCM_DE_CTRL_PLL7 (1 << 24)
#define CCM_DE_CTRL_PLL5P (2 << 24)
#define CCM_DE_CTRL_RST (1 << 30)
#define CCM_DE_CTRL_GATE (1 << 31)

#endif /* _SUNXI_CLOCK_SUN4I_H */