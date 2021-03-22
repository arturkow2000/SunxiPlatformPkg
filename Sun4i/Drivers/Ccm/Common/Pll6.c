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
CcmGetPll6GetFrequency(
  IN  CONST CCM_PLL_DESC *Desc,
  OUT UINT32 *Hz
  )
{
  UINT32 v;
  UINT32 n;
  UINT32 k;

  if (Hz == NULL)
    return EFI_INVALID_PARAMETER;

  v = MmioRead32(CCM_BASE + CCM_OFFSET_PLL6_CFG);
  n = (v & CCM_PLL6_CTRL_N_MASK) >> CCM_PLL6_CTRL_N_SHIFT;
  k = ((v & CCM_PLL6_CTRL_K_MASK) >> CCM_PLL6_CTRL_K_SHIFT) + 1;

  *Hz = 24000000 * n * k / 2;

  return EFI_SUCCESS;
}