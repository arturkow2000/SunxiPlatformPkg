/**
 SPDX-License-Identifier: GPL-2.0+
 
 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on U-Boot driver
 (C) Copyright 2013-2014 Luc Verhaegen <libv@skynet.be>
 (C) Copyright 2014-2015 Hans de Goede <hdegoede@redhat.com>

**/

#include "Driver.h"
#include "Backend.h"

EFI_STATUS SunxiBackendInit(SUNXI_DISPLAY_DRIVER *Driver) {
  EFI_STATUS Status;
  UINT32 Gate;
  UINT32 i;

  Status = Driver->SunxiCcmProtocol->GetGate(
    Driver->SunxiCcmProtocol,
    L"ahb1-be0",
    &Gate
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = Driver->SunxiCcmProtocol->GateUnmask(
    Driver->SunxiCcmProtocol,
    Gate
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = Driver->SunxiCcmProtocol->GetGate(
    Driver->SunxiCcmProtocol,
    L"dram-be0",
    &Gate
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = Driver->SunxiCcmProtocol->GateUnmask(
    Driver->SunxiCcmProtocol,
    Gate
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = SunxiBackendSetClock(Driver, 300000000);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  for (i = 0x0800; i < 0x1000; i += 4)
    MmioWrite32(SUNXI_BE0_BASE + i, 0);

  MmioOr32(SUNXI_BE0_BASE + SUNXI_DE_BE_MODE, SUNXI_DE_BE_MODE_ENABLE);

  return EFI_SUCCESS;
}

EFI_STATUS SunxiBackendSetMode(SUNXI_DISPLAY_DRIVER *Driver, SUNXI_VIDEO_MODE *Mode, UINT32 FramebufferBase) {
  MmioWrite32(SUNXI_BE0_BASE + SUNXI_DE_BE_DISP_SIZE, SUNXI_DE_BE_WIDTH(Mode->Width) | SUNXI_DE_BE_HEIGHT(Mode->Height));
  MmioWrite32(SUNXI_BE0_BASE + SUNXI_DE_BE_LAYER0_SIZE, SUNXI_DE_BE_WIDTH(Mode->Width) | SUNXI_DE_BE_HEIGHT(Mode->Height));
  MmioWrite32(SUNXI_BE0_BASE + SUNXI_DE_BE_LAYER0_STRIDE, SUNXI_DE_BE_LAYER_STRIDE(Mode->Width));
  MmioWrite32(SUNXI_BE0_BASE + SUNXI_DE_BE_LAYER0_ADDR_LOW32B, FramebufferBase << 4);
  MmioWrite32(SUNXI_BE0_BASE + SUNXI_DE_BE_LAYER0_ADDR_HIGH4B, FramebufferBase >> 29);
  MmioWrite32(SUNXI_BE0_BASE + SUNXI_DE_BE_LAYER0_ATTR0_CTRL, 0);
  MmioWrite32(SUNXI_BE0_BASE + SUNXI_DE_BE_LAYER0_ATTR1_CTRL, SUNXI_DE_BE_LAYER_ATTR1_FMT_XRGB8888);
  MmioOr32(SUNXI_BE0_BASE + SUNXI_DE_BE_MODE, SUNXI_DE_BE_MODE_LAYER0_ENABLE);

  return EFI_SUCCESS;
}

EFI_STATUS SunxiBackendEnable(SUNXI_DISPLAY_DRIVER *Driver) {
  MmioOr32(SUNXI_BE0_BASE + SUNXI_DE_BE_REG_CTRL, SUNXI_DE_BE_REG_CTRL_LOAD_REGS);
  MmioOr32(SUNXI_BE0_BASE + SUNXI_DE_BE_MODE, SUNXI_DE_BE_MODE_START);

  return EFI_SUCCESS;
}
