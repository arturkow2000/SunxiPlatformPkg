#pragma once

#define CCM_BASE FixedPcdGet32(CcmBase)

#define CCM_OFFSET_PLL1_CFG 0x00
#define CCM_OFFSET_PLL5_CFG 0x20
#define CCM_OFFSET_PLL5_TUN 0x24
#define CCM_OFFSET_PLL6_CFG 0x28
#define CCM_OFFSET_CPU_AXI_AHB_APB0_CFG 0x054
#define CCM_OFFSET_APB1_CLK_DIV_CFG 0x058
#define CCM_OFFSET_AHB_GATING0 0x060
#define CCM_OFFSET_AHB_GATING1 0x064
#define CCM_OFFSET_APB0_GATING 0x068
#define CCM_OFFSET_APB1_GATING 0x06c
#define CCM_OFFSET_MBUS_CLK_CFG 0x15c

#define CCM_AHB_GATE_SDRAM 14
#define CCM_AHB_GATE_DLL 15

#define CCM_APB0_GATE_PIO 5

#define CCM_APB1_GATE_UART0 16
#define CCM_APB1_GATE_UART1 17
#define CCM_APB1_GATE_UART2 18
#define CCM_APB1_GATE_UART3 19

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

UINT32 CcmGetPll5pFreq(VOID);
UINT32 CcmGetPll6Freq(VOID);
