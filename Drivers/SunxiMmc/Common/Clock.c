/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (c) Artur Kowalski <arturkow2000@gmail.com>

 Based on code from U-Boot
 Copyright 2008, Freescale Semiconductor, Inc
 Copyright 2020 NXP
 Andy Fleming
**/

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>

#include <Sunxi/HW/Mmc.h>

#include "Driver.h"

EFI_STATUS
SunxiMmcUpdateClock(
  IN SUNXI_MMC *Mmc
  )
{
  MmioWrite32(
    Mmc->Config->Base + SUNXI_MMC_CMD,
    SUNXI_MMC_CMD_START | SUNXI_MMC_CMD_UPCLK_ONLY
  );

  // TODO: add timeout
  while (MmioRead32(Mmc->Config->Base + SUNXI_MMC_CMD) & SUNXI_MMC_CMD_START)
  {
    CpuPause();
  }

  // clock update sets various irq status bits, clear these
  MmioWrite32(
    Mmc->Config->Base + SUNXI_MMC_RINT,
    MmioRead32(Mmc->Config->Base + SUNXI_MMC_RINT)
  );

  return EFI_SUCCESS;
}

EFI_STATUS
SunxiMmcConfigureClock(
  IN SUNXI_MMC *Mmc,
  IN UINT32 Hz
  )
{
  EFI_STATUS Status;
  UINT32 Reg;

  Reg = MmioRead32(Mmc->Config->Base + SUNXI_MMC_CLKCR);
  
  Reg &= ~SUNXI_MMC_CLKCR_EN;
  MmioWrite32(Mmc->Config->Base + SUNXI_MMC_CLKCR, Reg);
  Status = SunxiMmcSetModClock(Mmc, Hz);
  if (EFI_ERROR(Status))
    goto Exit;

  Reg &= ~SUNXI_MMC_CLKCR_DIV_MASK;
  MmioWrite32(Mmc->Config->Base + SUNXI_MMC_CLKCR, Reg);

  Reg |= SUNXI_MMC_CLKCR_EN;
  MmioWrite32(Mmc->Config->Base + SUNXI_MMC_CLKCR, Reg);

  Status = SunxiMmcUpdateClock(Mmc);

  Exit:
  return Status;
}