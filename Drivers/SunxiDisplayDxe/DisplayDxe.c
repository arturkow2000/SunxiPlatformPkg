/**
 SPDX-License-Identifier: GPL-2.0+
 
 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on U-Boot driver
 (C) Copyright 2013-2014 Luc Verhaegen <libv@skynet.be>
 (C) Copyright 2014-2015 Hans de Goede <hdegoede@redhat.com>

**/

#include "Driver.h"

#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>

STATIC VOID *mFramebufferBase;
STATIC SUNXI_VIDEO_MODE mLcdMode;

STATIC VOID *mPmicRegistration;
STATIC VOID EnableLcd(
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS Status;
  SUNXI_DISPLAY_DRIVER *Driver = Context;

  Status = gBS->LocateProtocol(
    &gPmicProtocolGuid,
    NULL,
    (VOID**)&Driver->Pmic
  );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to locate PMIC protocol: %r\n", Status));
    return;
  }

  SunxiLcdBacklightControl(Driver, FALSE);
  SunxiLcdPanelPowerControl(Driver, TRUE);

  SunxiBackendSetMode(Driver, &mLcdMode, (UINT32)mFramebufferBase - (UINT32)FixedPcdGet64(PcdSystemMemoryBase));
  SunxiLcdSetMode(Driver, &mLcdMode);
  SunxiBackendEnable(Driver);
  SunxiLcdEnable(Driver);
  SunxiLcdBacklightControl(Driver, TRUE);
}

EFI_STATUS EFIAPI DisplayDxeInitialize(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS Status;
  SUNXI_DISPLAY_DRIVER *Driver;
  UINT32 FramebufferReserved;
  UINT32 FramebufferSize;

  FramebufferReserved = FixedPcdGet32(FramebufferReserved);
  mFramebufferBase = (VOID*)FixedPcdGet64(PcdSystemMemoryBase);
  gBS->SetMem(mFramebufferBase, FramebufferReserved, 0);

  Driver = AllocatePool(sizeof(SUNXI_DISPLAY_DRIVER));
  if (!Driver) {
    Status = EFI_OUT_OF_RESOURCES;
    ASSERT_EFI_ERROR(Status);
    return Status;
  }

  Status = SunxiDisplayInit(Driver);
  ASSERT_EFI_ERROR(Status);

  if (FixedPcdGet32(LcdFlags) & LCD_PRESENT) {
    mLcdMode.Width = FixedPcdGet32(LcdHorizontalResolution);
    mLcdMode.Height = FixedPcdGet32(LcdVerticalResolution);
    mLcdMode.RefreshRate = FixedPcdGet32(LcdRefreshRate);
    mLcdMode.Pixclock = FixedPcdGet32(LcdPixclock);
    mLcdMode.PixclockKHz = FixedPcdGet32(LcdPixclockKHz);
    mLcdMode.HBackPorch = FixedPcdGet32(LcdHBackPorch);
    mLcdMode.HFrontPorch = FixedPcdGet32(LcdHFrontPorch);
    mLcdMode.VBackPorch = FixedPcdGet32(LcdVBackPorch);
    mLcdMode.VFrontPorch = FixedPcdGet32(LcdVFrontPorch);
    mLcdMode.HSync = FixedPcdGet32(LcdHSync);
    mLcdMode.VSync = FixedPcdGet32(LcdVSync);
    mLcdMode.Flags = FixedPcdGet32(LcdModeFlags);
    mLcdMode.Depth = FixedPcdGet32(LcdDepth);

    FramebufferSize = mLcdMode.Width * mLcdMode.Height * 4;
    if (FramebufferSize > FramebufferReserved) {
      DEBUG((
        EFI_D_ERROR,
        "Not enough framebuffer memory reserved, need at least %u, only %u is reserved.\n",
        FramebufferSize,
        FramebufferReserved
      ));
      ASSERT(0);
      return EFI_OUT_OF_RESOURCES;
    }

    SunxiLcdInit(Driver);
    EfiCreateProtocolNotifyEvent(
      &gPmicProtocolGuid,
      TPL_CALLBACK,
      EnableLcd,
      Driver,
      &mPmicRegistration
    );

    Status = SunxiDisplayInitGop(Driver, mFramebufferBase, &mLcdMode);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status))
      return Status;
  }

  return EFI_SUCCESS;
}
