#include <AsmMacroIoLib.h>

#include "../Sun4i/Ccm.h"

#
# SoC Specific hardware initialization
#
ASM_FUNC(SplHWInitSoc)
  ldr r0, =CCM_BASE

  # TODO: ensure external oscillator is enabled before switching CPU clock source

  # Switch CPU to 24Mhz external oscillator
  ldr r1, =0x00010010
  str r1, [r0, #CCM_OFFSET_CPU_AXI_AHB_APB0_CFG]

  # Default PLL1 config same as in U-Boot
  ldr r1, =0xa1005000
  str r1, [r0, #CCM_OFFSET_PLL1_CFG]
  # Wait for PLL1 to stabilize
  ldr r2, =200
  1:
  subs r2, r2, #1
  bne 1b
  # Switch CPU clock source to PLL1
  ldr r1, =0x20010
  str r1, [r0, #CCM_OFFSET_CPU_AXI_AHB_APB0_CFG]

  ldr r1, =0xa1009911
  str r1, [r0, #CCM_OFFSET_PLL6_CFG]

  # Prepare for UART, set APB1 Frequency to 24Mhz
  ldr r1, =0
  str r1, [r0, #CCM_OFFSET_APB1_CLK_DIV_CFG]

  # Enable GPIO clock
  ldr r1, [r0, #CCM_OFFSET_APB0_GATING]
  orr r1, r1, #(1 << CCM_APB0_GATE_PIO)
  str r1, [r0, #CCM_OFFSET_APB0_GATING]

  b SplHWInitBoard