/* 
 * SPDX-License-Identifier: GPL-2.0+
 * 
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Berg Xing <bergxing@allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Sunxi platform dram register definition.
 */

#pragma once

#include <PiPei.h>

#define SUNXI_DRAMC_BASE 0x01c01000

#define DRAMC_CCR (SUNXI_DRAMC_BASE + 0x000)               /* 0x00 controller configuration register */
#define DRAMC_DCR (SUNXI_DRAMC_BASE + 0x004)               /* 0x04 dram configuration register */
#define DRAMC_IOCR (SUNXI_DRAMC_BASE + 0x008)              /* 0x08 i/o configuration register */
#define DRAMC_CSR (SUNXI_DRAMC_BASE + 0x00c)               /* 0x0c controller status register */
#define DRAMC_DRR (SUNXI_DRAMC_BASE + 0x010)               /* 0x10 dram refresh register */
#define DRAMC_TPR0 (SUNXI_DRAMC_BASE + 0x014)              /* 0x14 dram timing parameters register 0 */
#define DRAMC_TPR1 (SUNXI_DRAMC_BASE + 0x018)              /* 0x18 dram timing parameters register 1 */
#define DRAMC_TPR2 (SUNXI_DRAMC_BASE + 0x01c)              /* 0x1c dram timing parameters register 2 */
#define DRAMC_GDLLCR (SUNXI_DRAMC_BASE + 0x020)            /* 0x20 global dll control register */
#define DRAMC_RSLR0 (SUNXI_DRAMC_BASE + 0x04c)             /* 0x4c rank system latency register */
#define DRAMC_RSLR1 (SUNXI_DRAMC_BASE + 0x050)             /* 0x50 rank system latency register */
#define DRAMC_RDGR0 (SUNXI_DRAMC_BASE + 0x05c)             /* 0x5c rank dqs gating register */
#define DRAMC_RDGR1 (SUNXI_DRAMC_BASE + 0x060)             /* 0x60 rank dqs gating register */
#define DRAMC_ODTCR (SUNXI_DRAMC_BASE + 0x098)             /* 0x98 odt configuration register */
#define DRAMC_DTR0 (SUNXI_DRAMC_BASE + 0x09c)              /* 0x9c data training register 0 */
#define DRAMC_DTR1 (SUNXI_DRAMC_BASE + 0x0a0)              /* 0xa0 data training register 1 */
#define DRAMC_DTAR (SUNXI_DRAMC_BASE + 0x0a4)              /* 0xa4 data training address register */
#define DRAMC_ZQCR0 (SUNXI_DRAMC_BASE + 0x0a8)             /* 0xa8 zq control register 0 */
#define DRAMC_ZQCR1 (SUNXI_DRAMC_BASE + 0x0ac)             /* 0xac zq control register 1 */
#define DRAMC_ZQSR (SUNXI_DRAMC_BASE + 0x0b0)              /* 0xb0 zq status register */
#define DRAMC_IDCR (SUNXI_DRAMC_BASE + 0x0b4)              /* 0xb4 initializaton delay configure reg */
#define DRAMC_MR (SUNXI_DRAMC_BASE + 0x1f0)                /* 0x1f0 mode register */
#define DRAMC_EMR (SUNXI_DRAMC_BASE + 0x1f4)               /* 0x1f4 extended mode register */
#define DRAMC_EMR2 (SUNXI_DRAMC_BASE + 0x1f8)              /* 0x1f8 extended mode register */
#define DRAMC_EMR3 (SUNXI_DRAMC_BASE + 0x1fc)              /* 0x1fc extended mode register */
#define DRAMC_DLLCTR (SUNXI_DRAMC_BASE + 0x200)            /* 0x200 dll control register */
#define DRAMC_DLLCR0 (SUNXI_DRAMC_BASE + 0x204)            /* 0x204 dll control register 0(byte 0) */
#define DRAMC_DLLCR1 (SUNXI_DRAMC_BASE + 0x208)            /* 0x208 dll control register 1(byte 1) */
#define DRAMC_DLLCR2 (SUNXI_DRAMC_BASE + 0x20c)            /* 0x20c dll control register 2(byte 2) */
#define DRAMC_DLLCR3 (SUNXI_DRAMC_BASE + 0x210)            /* 0x210 dll control register 3(byte 3) */
#define DRAMC_DLLCR4 (SUNXI_DRAMC_BASE + 0x214)            /* 0x214 dll control register 4(byte 4) */
#define DRAMC_DQTR0 (SUNXI_DRAMC_BASE + 0x218)             /* 0x218 dq timing register */
#define DRAMC_DQTR1 (SUNXI_DRAMC_BASE + 0x21c)             /* 0x21c dq timing register */
#define DRAMC_DQTR2 (SUNXI_DRAMC_BASE + 0x220)             /* 0x220 dq timing register */
#define DRAMC_DQTR3 (SUNXI_DRAMC_BASE + 0x224)             /* 0x224 dq timing register */
#define DRAMC_DQSTR (SUNXI_DRAMC_BASE + 0x228)             /* 0x228 dqs timing register */
#define DRAMC_DQSBTR (SUNXI_DRAMC_BASE + 0x22c)            /* 0x22c dqsb timing register */
#define DRAMC_MCR (SUNXI_DRAMC_BASE + 0x230)               /* 0x230 mode configure register */
#define DRAMC_PPWRSCTL (SUNXI_DRAMC_BASE + 0x23c)          /* 0x23c pad power save control */
#define DRAMC_APR (SUNXI_DRAMC_BASE + 0x240)               /* 0x240 arbiter period register */
#define DRAMC_PLDTR (SUNXI_DRAMC_BASE + 0x244)             /* 0x244 priority level data threshold reg */
#define DRAMC_HPCR(x) (SUNXI_DRAMC_BASE + 0x250 + 4 * (x)) /* 0x250 host port configure register */
#define DRAMC_CSEL (SUNXI_DRAMC_BASE + 0x2e0)              /* 0x2e0 controller select register */

typedef struct _SUN4I_DRAM_PARA
{
  UINT32 Clock;
  UINT32 MbusClock;
  UINT32 Type;
  UINT32 RankNum;
  UINT32 Density;
  UINT32 IoWidth;
  UINT32 BusWidth;
  UINT32 Cas;
  UINT32 Zq;
  UINT32 OdtEnable;
  UINT32 Tpr0;
  UINT32 Tpr1;
  UINT32 Tpr2;
  UINT32 Tpr3;
  UINT32 Tpr4;
  UINT32 Tpr5;
  UINT32 Emr1;
  UINT32 Emr2;
  UINT32 Emr3;
  UINT32 DqsGatingDelay;
  UINT32 ActiveWindowing;
} SUN4I_DRAM_PARA;

#define DRAM_CCR_COMMAND_RATE_1T (0x1 << 5)
#define DRAM_CCR_DQS_GATE (0x1 << 14)
#define DRAM_CCR_DQS_DRIFT_COMP (0x1 << 17)
#define DRAM_CCR_ITM_OFF (0x1 << 28)
#define DRAM_CCR_DATA_TRAINING (0x1 << 30)
#define DRAM_CCR_INIT (0x1 << 31)

#define DRAM_MEMORY_TYPE_DDR1 1
#define DRAM_MEMORY_TYPE_DDR2 2
#define DRAM_MEMORY_TYPE_DDR3 3
#define DRAM_MEMORY_TYPE_LPDDR2 4
#define DRAM_MEMORY_TYPE_LPDDR 5
#define DRAM_DCR_TYPE (0x1 << 0)
#define DRAM_DCR_TYPE_DDR2 0x0
#define DRAM_DCR_TYPE_DDR3 0x1
#define DRAM_DCR_IO_WIDTH(n) (((n)&0x3) << 1)
#define DRAM_DCR_IO_WIDTH_MASK DRAM_DCR_IO_WIDTH(0x3)
#define DRAM_DCR_IO_WIDTH_8BIT 0x0
#define DRAM_DCR_IO_WIDTH_16BIT 0x1
#define DRAM_DCR_CHIP_DENSITY(n) (((n)&0x7) << 3)
#define DRAM_DCR_CHIP_DENSITY_MASK DRAM_DCR_CHIP_DENSITY(0x7)
#define DRAM_DCR_CHIP_DENSITY_256M 0x0
#define DRAM_DCR_CHIP_DENSITY_512M 0x1
#define DRAM_DCR_CHIP_DENSITY_1024M 0x2
#define DRAM_DCR_CHIP_DENSITY_2048M 0x3
#define DRAM_DCR_CHIP_DENSITY_4096M 0x4
#define DRAM_DCR_CHIP_DENSITY_8192M 0x5
#define DRAM_DCR_BUS_WIDTH(n) (((n)&0x7) << 6)
#define DRAM_DCR_BUS_WIDTH_MASK DRAM_DCR_BUS_WIDTH(0x7)
#define DRAM_DCR_BUS_WIDTH_32BIT 0x3
#define DRAM_DCR_BUS_WIDTH_16BIT 0x1
#define DRAM_DCR_BUS_WIDTH_8BIT 0x0
#define DRAM_DCR_RANK_SEL(n) (((n)&0x3) << 10)
#define DRAM_DCR_RANK_SEL_MASK DRAM_DCR_CMD_RANK(0x3)
#define DRAM_DCR_CMD_RANK_ALL (0x1 << 12)
#define DRAM_DCR_MODE(n) (((n)&0x3) << 13)
#define DRAM_DCR_MODE_MASK DRAM_DCR_MODE(0x3)
#define DRAM_DCR_MODE_SEQ 0x0
#define DRAM_DCR_MODE_INTERLEAVE 0x1

#define DRAM_CSR_DTERR (0x1 << 20)
#define DRAM_CSR_DTIERR (0x1 << 21)
#define DRAM_CSR_FAILED (DRAM_CSR_DTERR | DRAM_CSR_DTIERR)

#define DRAM_DRR_TRFC(n) ((n)&0xff)
#define DRAM_DRR_TREFI(n) (((n)&0xffff) << 8)
#define DRAM_DRR_BURST(n) ((((n)-1) & 0xf) << 24)

#define DRAM_MCR_MODE_NORM(n) (((n)&0x3) << 0)
#define DRAM_MCR_MODE_NORM_MASK DRAM_MCR_MOD_NORM(0x3)
#define DRAM_MCR_MODE_DQ_OUT(n) (((n)&0x3) << 2)
#define DRAM_MCR_MODE_DQ_OUT_MASK DRAM_MCR_MODE_DQ_OUT(0x3)
#define DRAM_MCR_MODE_ADDR_OUT(n) (((n)&0x3) << 4)
#define DRAM_MCR_MODE_ADDR_OUT_MASK DRAM_MCR_MODE_ADDR_OUT(0x3)
#define DRAM_MCR_MODE_DQ_IN_OUT(n) (((n)&0x3) << 6)
#define DRAM_MCR_MODE_DQ_IN_OUT_MASK DRAM_MCR_MODE_DQ_IN_OUT(0x3)
#define DRAM_MCR_MODE_DQ_TURNON_DELAY(n) (((n)&0x7) << 8)
#define DRAM_MCR_MODE_DQ_TURNON_DELAY_MASK DRAM_MCR_MODE_DQ_TURNON_DELAY(0x7)
#define DRAM_MCR_MODE_ADDR_IN (0x1 << 11)
#define DRAM_MCR_RESET (0x1 << 12)
#define DRAM_MCR_MODE_EN(n) (((n)&0x3) << 13)
#define DRAM_MCR_MODE_EN_MASK DRAM_MCR_MOD_EN(0x3)
#define DRAM_MCR_DCLK_OUT (0x1 << 16)

#define DRAM_DLLCR_NRESET (0x1 << 30)
#define DRAM_DLLCR_DISABLE (0x1 << 31)

#define DRAM_ZQCR0_IMP_DIV(n) (((n)&0xff) << 20)
#define DRAM_ZQCR0_IMP_DIV_MASK DRAM_ZQCR0_IMP_DIV(0xff)
#define DRAM_ZQCR0_ZCAL (1 << 31) /* Starts ZQ calibration when set to 1 */
#define DRAM_ZQCR0_ZDEN (1 << 28) /* Uses ZDATA instead of doing calibration */

#define DRAM_ZQSR_ZDONE (1 << 31) /* ZQ calibration completion flag */

#define DRAM_IOCR_ODT_EN ((3 << 30) | (3 << 0))

#define DRAM_MR_BURST_LENGTH(n) (((n)&0x7) << 0)
#define DRAM_MR_BURST_LENGTH_MASK DRAM_MR_BURST_LENGTH(0x7)
#define DRAM_MR_CAS_LAT(n) (((n)&0x7) << 4)
#define DRAM_MR_CAS_LAT_MASK DRAM_MR_CAS_LAT(0x7)
#define DRAM_MR_WRITE_RECOVERY(n) (((n)&0x7) << 9)
#define DRAM_MR_WRITE_RECOVERY_MASK DRAM_MR_WRITE_RECOVERY(0x7)
#define DRAM_MR_POWER_DOWN (0x1 << 12)

#define DRAM_CSEL_MAGIC 0x16237495

UINT32 DramInit(SUN4I_DRAM_PARA *Para);

UINT64 DramDetect(UINT64 *Base, UINT64 MaxSize);