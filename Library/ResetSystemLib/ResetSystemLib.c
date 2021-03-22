/**
 SPDX-License-Identifier: GPL-2.0+

  Copyright (C) 2021 Artur Kowalski. <arturkow2000@gmail.com>

 (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 
 (C) Copyright 2007-2011
 Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 Tom Cubie <tangliang@allwinnertech.com>
**/
#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/I2cIo.h>
#include <Protocol/AxpPower.h>

#include <Sunxi/HW/Watchdog.h>

STATIC VOID WatchdogReset(VOID)
{
  MmioWrite32(
      SUNXI_WATCHDOG_BASE + SUNXI_WATCHDOG_MODE,
      SUNXI_WATCHDOG_MODE_EN | SUNXI_WATCHDOG_MODE_RESET_EN);
  MmioWrite32(
      SUNXI_WATCHDOG_BASE + SUNXI_WATCHDOG_CTL,
      SUNXI_WATCHDOG_CTL_KEY | SUNXI_WATCHDOG_CTL_RESTART);

  // TODO: add timeout
  while (TRUE)
  {
    MmioWrite32(
        SUNXI_WATCHDOG_BASE + SUNXI_WATCHDOG_MODE,
        SUNXI_WATCHDOG_MODE_EN | SUNXI_WATCHDOG_MODE_RESET_EN);
  }
}

VOID EFIAPI ResetCold(VOID)
{
  WatchdogReset();
}

VOID EFIAPI ResetWarm(VOID)
{
  WatchdogReset();
}

STATIC AXP_POWER_PROTOCOL *mAxpPower;

VOID EFIAPI ResetShutdown(VOID)
{
  EFI_STATUS Status;

  Status = gBS->LocateProtocol(&gAxpPowerProtocolGuid, NULL, (VOID **)&mAxpPower);
  if (Status != EFI_SUCCESS)
  {
    DebugPrint(EFI_D_ERROR, "AXP Power Protocol Missing\n");
    ASSERT(FALSE);
    return;
  }

  Status = mAxpPower->Shutdown(mAxpPower);
  if (Status != EFI_SUCCESS)
  {
    DebugPrint(EFI_D_ERROR, "Shutdown Failed\n");
    ASSERT(FALSE);
    return;
  }
}

VOID EFIAPI ResetPlatformSpecific(
    IN UINTN DataSize,
    IN VOID *ResetData)
{
  ResetCold();
}

VOID EFIAPI EnterS3WithImmediateWake(VOID)
{
}