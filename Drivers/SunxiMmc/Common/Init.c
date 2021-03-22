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

#include <Sunxi/HW/ClockSun4i.h>
#include <Sunxi/HW/Mmc.h>

#include "Driver.h"

EFI_STATUS
SunxiMmcInitDriver(
  IN SUNXI_MMC* Mmc
  )
{
  EFI_STATUS Status;

  Status = SunxiMmcInitGpio(Mmc);
  if (EFI_ERROR(Status))
    goto Exit;

  Status = SunxiMmcUnmaskGate(Mmc, Mmc->Config->AhbGate);
  if (EFI_ERROR(Status))
    goto Exit;

  Status = SunxiMmcSetModClock(Mmc, 24000000);
  if (EFI_ERROR(Status))
    goto Exit;

  Status = SunxiMmcResetHw(Mmc);
  if (EFI_ERROR(Status))
    goto Exit;

  Status = SunxiMmcInitMmc(Mmc);
  if (EFI_ERROR(Status))
    goto Exit;
    
Exit:
  return Status;
}

EFI_STATUS
SunxiMmcResetHw(
  IN SUNXI_MMC *Mmc
  )
{
  DEBUG((EFI_D_INFO, "Resetting MMC host\n"));

  MmioOr32(
    Mmc->Config->Base + SUNXI_MMC_GCTRL,
    SUNXI_MMC_GCTRL_SOFT_RESET | SUNXI_MMC_GCTRL_FIFO_RESET | SUNXI_MMC_GCTRL_DMA_RESET
  );

  // TODO add timeout
  while (MmioRead32(Mmc->Config->Base + SUNXI_MMC_GCTRL) & (SUNXI_MMC_GCTRL_SOFT_RESET | SUNXI_MMC_GCTRL_FIFO_RESET | SUNXI_MMC_GCTRL_DMA_RESET))
  {
    CpuPause();
  }

  return EFI_SUCCESS;
}

EFI_STATUS
SunxiMmcConfigureClock(
  IN SUNXI_MMC *Mmc,
  IN UINT32 Hz
  );

EFI_STATUS
SunxiMmcSetIos(
  IN SUNXI_MMC *Mmc,
  IN UINT32 Hz,
  IN UINT32 BusWidth
  )
{
  EFI_STATUS Status;
  UINT32 BusWidthRaw;

  switch (BusWidth) {
    case 1:
      BusWidthRaw = 0;
      break;
    case 4:
      BusWidthRaw = 1;
      break;
    default:
      if (BusWidth == 8 && Mmc->Config->MaxBusWidth >= 8)
        BusWidthRaw = 2;
      else
        return EFI_UNSUPPORTED;
      break;
  }

  Status = SunxiMmcConfigureClock(Mmc, Hz);
  if (EFI_ERROR(Status))
    goto Exit;

  MmioWrite32(
    Mmc->Config->Base + SUNXI_MMC_WIDTH,
    BusWidthRaw
  );

  Exit:
  return Status;
}