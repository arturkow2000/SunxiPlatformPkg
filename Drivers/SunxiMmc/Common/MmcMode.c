/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (c) Artur Kowalski <arturkow2000@gmail.com>

 Based on code from U-Boot
 Copyright 2008, Freescale Semiconductor, Inc
 Copyright 2020 NXP
 Andy Fleming
**/

#include <Uefi.h>

#include <Library/DebugLib.h>
#include <Library/TimerLib.h>

#include "Driver.h"
#include "MmcDef.h"

STATIC
EFI_STATUS
MmcSwitch(
  IN SUNXI_MMC *Mmc,
  IN UINT8 Set,
  IN UINT8 Index,
  IN UINT8 Value
  )
{
  EFI_STATUS Status;
  UINT32 Timeout;
  UINT32 Resp;
  INT32 i;

  Timeout = 100;

  for (i = 0; i < 3; i++) {
    Status = SunxiMmcExecuteCommand(
      Mmc,
      MMC_CMD_SWITCH,
      (MMC_SWITCH_MODE_WRITE_BYTE << 24)
        | (Index << 16)
        | (Value << 8),
      MMC_RSP_R1b,
      NULL
    );
    if (Status == EFI_SUCCESS)
      break;
  }

  if (EFI_ERROR(Status))
    return Status;
  
  while (Timeout > 0) {
    Status = SunxiMmcExecuteCommand(
      Mmc,
      MMC_CMD_SEND_STATUS,
      Mmc->State.CardInfo.Rca << 16,
      MMC_RSP_R1,
      &Resp
    );
    if (EFI_ERROR(Status))
      return Status;

    if (Resp & MMC_STATUS_SWITCH_ERROR) {
      DEBUG((EFI_D_ERROR, "MMC switch failed\n"));
      return EFI_DEVICE_ERROR;
    }

    if (Resp & MMC_STATUS_RDY_FOR_DATA)
      return EFI_SUCCESS;

    MicroSecondDelay(100);
    Timeout--;
  };

  DEBUG((EFI_D_ERROR, "MMC switch timed out\n"));
  return EFI_TIMEOUT;
}

EFI_STATUS SdSelectModeAndWidth(IN SUNXI_MMC *Mmc)
{
  // TODO: implement
  return EFI_SUCCESS;
}

EFI_STATUS MmcSelectModeAndWidth(IN SUNXI_MMC *Mmc)
{
  EFI_STATUS Status;
  UINT32 BusWidth;
  UINT32 BusWidthCsd;

  if (Mmc->State.CardInfo.Version & SD_VERSION_SD)
    return SdSelectModeAndWidth(Mmc);

  // MMC version 4+ supports 8 bit bus width
  // Older version support only 1 bit bus width
  if (Mmc->State.CardInfo.Version >= MMC_VERSION_4) {
    BusWidth = MIN(Mmc->Config->MaxBusWidth, 8);

    switch (BusWidth) {
    case 1:
      BusWidthCsd = EXT_CSD_BUS_WIDTH_1;
      break;
    case 4:
      BusWidthCsd = EXT_CSD_BUS_WIDTH_4;
      break;
    case 8:
      BusWidthCsd = EXT_CSD_BUS_WIDTH_8;
      break;
    default:
      ASSERT(0);
      return EFI_DEVICE_ERROR;
      break;
    }

    Status = MmcSwitch(
      Mmc,
      EXT_CSD_CMD_SET_NORMAL,
      EXT_CSD_BUS_WIDTH,
      BusWidthCsd
    );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status))
      return Status;
    
    Status = SunxiMmcSetIos(Mmc, Mmc->State.ClockHz, BusWidth);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status))
      return Status;
  }

  return EFI_SUCCESS;
}