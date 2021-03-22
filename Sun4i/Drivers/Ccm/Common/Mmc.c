/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (c) Artur Kowalski. <arturkow2000@gmail.com>

 Based on code from U-Boot:
 (C) Copyright 2007-2012
 Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 Tom Cubie <tangliang@allwinnertech.com>
**/

#include "Driver.h"
#include "Pll.h"

#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Sunxi/HW/ClockSun4i.h>

EFI_STATUS
EFIAPI
CcmMmcGetFrequency(
  IN  CONST CCM_PLL_DESC *Desc,
  OUT UINT32 *Hz
  )
{
  EFI_STATUS Status;
  UINT32 Offset;
  UINT32 Reg;
  UINT32 Input;
  UINT32 InputHz;
  UINT32 m;
  UINT32 n;

  if (Desc == NULL) {
    ASSERT(0);
    return EFI_DEVICE_ERROR;
  }

  if (Hz == NULL)
    return EFI_INVALID_PARAMETER;

  Offset = CCM_OFFSET_SD0_CLK_CFG + Desc->Data * 4;
  Reg = MmioRead32(CCM_BASE + Offset);

  if (!(Reg & CCM_MMC_CTRL_ENABLE)) {
    *Hz = 0;
    return EFI_SUCCESS;
  }

  // extract PLL input
  Input = Reg & (3 << 24);
  switch (Input) {
  case CCM_MMC_CTRL_OSCM24:
    InputHz = 24000000;
    break;
  case CCM_MMC_CTRL_PLL6:
    Status = CcmGetPll6GetFrequency(NULL, &InputHz);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "Failed to get PLL6 frequency: %r\n", Status));
      ASSERT(0);
      return Status;
    }
    break;
  default:
    DEBUG((EFI_D_ERROR, "Unsupported input: 0x%x\n", Input));
    break;
  }

  if (InputHz == 0) {
    *Hz = 0;
    return EFI_SUCCESS;
  }

  m = (Reg & 0xff) + 1;
  n = (Reg >> 16) & 0x0f;

  *Hz = InputHz / (1 << n) / m;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
CcmMmcSetFrequency(
  IN  CONST CCM_PLL_DESC *Desc,
  IN  UINT32 Hz
  )
{
  UINT32 Input;
  UINT32 InputHz;
  UINT32 m;
  UINT32 n;
  UINT32 OclkDly;
  UINT32 SclkDly;
  EFI_STATUS Status;

  if (Desc == NULL) {
    ASSERT(0);
    return EFI_DEVICE_ERROR;
  }

  Status = EFI_SUCCESS;

  if (Hz == 0) {
    MmioAnd32(CCM_BASE + CCM_OFFSET_SD0_CLK_CFG + Desc->Data * 4, ~CCM_MMC_CTRL_ENABLE);
    goto Exit;
  }

  if (Hz <= 24000000) {
    Input = CCM_MMC_CTRL_OSCM24;
    InputHz = 24000000;
  } else {
    Input = CCM_MMC_CTRL_PLL6;
    Status = CcmGetPll6GetFrequency(NULL, &InputHz);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "Failed to get PLL6 frequency: %r\n", Status));
      ASSERT(0);
      return Status;
    }
  }

  m = InputHz / Hz;
  if (InputHz % Hz != 0)
    m += 1;
  
  n = 0;
  while (m > 16) {
    n += 1;
    m = (m + 1) / 2;
  }

  if (n > 3) {
    DEBUG((EFI_D_ERROR, "Cannot set frequency to %u\n", Hz));
    Status = EFI_UNSUPPORTED;
    ASSERT_EFI_ERROR(Status);
    goto Exit;
  }

  if (Hz <= 400000) {
    OclkDly = 0;
    SclkDly = 0;
  } else if (Hz <= 25000000) {
    OclkDly = 0;
    SclkDly = 5;
  } else if (Hz <= 52000000) {
    OclkDly = 3;
    SclkDly = 4;
  } else {
    OclkDly = 1;
    SclkDly = 4;
  }

  DEBUG((
    EFI_D_INFO,
    "Set mmc%u clock req %u parent %s %u n %u m %u rate %u\n",
    Desc->Data,
    Hz,
    Input == CCM_MMC_CTRL_PLL6 ? L"pll6" : L"osc24m",
    InputHz,
    1 << n,
    m,
    InputHz / (1 << n) / m
  ));

  MmioWrite32(
    CCM_BASE + CCM_OFFSET_SD0_CLK_CFG + Desc->Data * 4,
    CCM_MMC_CTRL_ENABLE
     | Input
     | CCM_MMC_CTRL_N(n)
     | CCM_MMC_CTRL_M(m)
     | CCM_MMC_CTRL_OCLK_DLY(OclkDly)
     | CCM_MMC_CTRL_SCLK_DLY(SclkDly)
  );

Exit:
  return Status;
}