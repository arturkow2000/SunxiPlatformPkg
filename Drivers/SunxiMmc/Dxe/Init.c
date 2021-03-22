/**
  SPDX-License-Identifier: GPL-2.0+

  Copyright (c) 2021, Artur Kowalski. <arturkow2000@gmail.com>
**/

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/SdMmcPassThru.h>
#include <Protocol/SunxiGpio.h>

#include "Driver.h"

extern EFI_SD_MMC_PASS_THRU_PROTOCOL gSunxiMmcProtocol;

EFI_STATUS SunxiMmcCreateDriver(
  IN CONST SUNXI_MMC_CONFIG *Config,
  OUT SUNXI_MMC_DRIVER **OutDriver
  )
{
  EFI_STATUS Status;
  SUNXI_MMC_DRIVER *Driver;

  if (!OutDriver) {
    ASSERT(0);
    return EFI_INVALID_PARAMETER;
  }

  Driver = AllocateZeroPool(sizeof(SUNXI_MMC_DRIVER));
  Driver->Mmc.Config = Config;
  CopyMem(&Driver->Protocol, &gSunxiMmcProtocol, sizeof(EFI_SD_MMC_PASS_THRU_PROTOCOL));

  Status = SunxiMmcInitDriver(&Driver->Mmc);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "MMC driver initialization failed.\n"));
    ASSERT(0);

    FreePool(Driver);
    return Status;
  }

  *OutDriver = Driver;
  return EFI_SUCCESS;
}

VOID SunxiMmcDestroyDriver(
  IN SUNXI_MMC_DRIVER *Driver
  )
{
  // TODO: deinitialize hardware
  if (Driver)
    FreePool(Driver);
  else ASSERT(0);
}