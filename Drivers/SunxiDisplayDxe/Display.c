/**
 SPDX-License-Identifier: GPL-2.0+
 
 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on U-Boot driver
 (C) Copyright 2013-2014 Luc Verhaegen <libv@skynet.be>
 (C) Copyright 2014-2015 Hans de Goede <hdegoede@redhat.com>

**/

#include "Driver.h"

#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS SunxiDisplayInit(SUNXI_DISPLAY_DRIVER *Driver) {
  EFI_STATUS Status;

  Status = gBS->LocateProtocol(
    &gSunxiCcmProtocolGuid,
    NULL,
    (VOID**)&Driver->SunxiCcmProtocol
  );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to locate Sunxi CCM protocol: %r\n", Status));
    ASSERT_EFI_ERROR(Status);
    return Status;
  }

  Status = SunxiBackendInit(Driver);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to initialize display backend: %r\n", Status));
    ASSERT_EFI_ERROR(Status);
  }

  Status = SunxiDisplayInitGpio(Driver);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  // A13 does not need frontend so skip initialization
  // TODO: support frontend on platforms that need this

  return Status;
}
