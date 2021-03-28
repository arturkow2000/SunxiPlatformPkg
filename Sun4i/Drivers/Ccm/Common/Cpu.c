/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (c) Artur Kowalski. <arturkow2000@gmail.com>
**/

#include "Driver.h"
#include "Pll.h"

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Sunxi/HW/ClockSun4i.h>

EFI_STATUS
EFIAPI
CcmGetCpuFrequency(
  IN  CONST CCM_PLL_DESC *Desc,
  OUT UINT32 *Hz
  )
{
  EFI_STATUS Status;
  UINT32 Reg;
  UINT32 Src;
  UINT32 InputFreq;

  if (Hz == NULL)
    return EFI_INVALID_PARAMETER;

  Reg = MmioRead32(CCM_BASE + CCM_OFFSET_CPU_AHB_APB0_CFG);
  Src = (Reg & CPU_CLK_SRC_MASK) >> CPU_CLK_SRC_SHIFT;

  switch (Src) {
  case CPU_CLK_SRC_OSC24M:
    InputFreq = 24000000;
    break;
  case CPU_CLK_SRC_PLL1:
    Status = CcmPll1GetFrequency(NULL, &InputFreq);
    if (EFI_ERROR(Status))
      return Status;
    break;
  default:
    ASSERT(0);
    return EFI_DEVICE_ERROR;
  }

  *Hz = InputFreq;
  return EFI_SUCCESS;
}
