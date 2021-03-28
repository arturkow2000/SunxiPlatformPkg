/**
 SPDX-License-Identifier: GPL-2.0-only

 Copyright (c) Artur Kowalski. <arturkow2000@gmail.com>

 Based on code from U-Boot:
 (C) Copyright 2007-2012
 Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 Tom Cubie <tangliang@allwinnertech.com>
 
 (C) Copyright 2013 Luke Kenneth Casson Leighton <lkcl@lkcl.net>
**/

#include "Driver.h"
#include "Pll.h"

#include <Library/IoLib.h>
#include <Sunxi/HW/ClockSun4i.h>

EFI_STATUS
EFIAPI
CcmPll1GetFrequency(
  IN  CONST CCM_PLL_DESC *Desc,
  OUT UINT32 *Hz
  )
{
  UINT32 Reg;
  UINT32 n;
  UINT32 k;
  UINT32 m;
  UINT32 p;

  if (!Hz)
    return EFI_INVALID_PARAMETER;

  Reg = MmioRead32(CCM_BASE + CCM_OFFSET_PLL1_CFG);
  n = (Reg & CCM_PLL1_CFG_FACTOR_N_MASK) >> CCM_PLL1_CFG_FACTOR_N_SHIFT;
  k = ((Reg & CCM_PLL1_CFG_FACTOR_K_MASK) >> CCM_PLL1_CFG_FACTOR_K_SHIFT) + 1;
  m = ((Reg & CCM_PLL1_CFG_FACTOR_M_MASK) >> CCM_PLL1_CFG_FACTOR_M_SHIFT) + 1;
  p = (Reg & CCM_PLL1_CFG_DIVP_MASK) >> CCM_PLL1_CFG_DIVP_SHIFT;

  *Hz = (24000000 * n * k) / (m * p);

  return EFI_SUCCESS;
}