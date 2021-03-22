#include "../Spl.h"
#include "Ccm.h"

UINT32 CcmGetPll5pFreq(VOID)
{
  UINT32 Reg;
  INT32 N;
  INT32 K;
  INT32 P;

  Reg = MmioRead32(CCM_BASE + CCM_OFFSET_PLL5_CFG);
  N = ((Reg & CCM_PLL5_CTRL_N_MASK) >> CCM_PLL5_CTRL_N_SHIFT);
  K = ((Reg & CCM_PLL5_CTRL_K_MASK) >> CCM_PLL5_CTRL_K_SHIFT) + 1;
  P = ((Reg & CCM_PLL5_CTRL_P_MASK) >> CCM_PLL5_CTRL_P_SHIFT);

  return (24000000 * N * K) >> P;
}

UINT32 CcmGetPll6Freq(VOID)
{
  UINT32 Reg;
  INT32 N;
  INT32 K;

  Reg = MmioRead32(CCM_BASE + CCM_OFFSET_PLL6_CFG);
  N = ((Reg & CCM_PLL6_CTRL_N_MASK) >> CCM_PLL6_CTRL_N_SHIFT);
  K = ((Reg & CCM_PLL6_CTRL_K_MASK) >> CCM_PLL6_CTRL_K_SHIFT) + 1;

  return 24000000 * N * K / 2;
}
