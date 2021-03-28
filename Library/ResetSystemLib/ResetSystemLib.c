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
#include <Protocol/Pmic.h>

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

VOID EFIAPI ResetShutdown(VOID)
{
  EFI_STATUS Status;
  PMIC_PROTOCOL *Pmic;

  Status = gBS->LocateProtocol(&gPmicProtocolGuid, NULL, (VOID **)&Pmic);
  if (EFI_ERROR(Status)) {
    DebugPrint(EFI_D_ERROR, "PMIC protocol not found\n");
    return;
  }

  Status = Pmic->Poweroff(Pmic);
  if (EFI_ERROR(Status)) {
    DebugPrint(EFI_D_ERROR, "Shutdown error: %r\n", Status);
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