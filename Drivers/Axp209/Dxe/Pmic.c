/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on code from U-Boot

 Copyright (C) 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
**/

#include "Driver.h"

STATIC
EFI_STATUS
EFIAPI
Poweroff(
  IN  PMIC_PROTOCOL     *This
  )
{
  EFI_STATUS Status;
  AXP209_DXE_DRIVER *Driver = (AXP209_DXE_DRIVER*)This;

  Status = AxpWrite8(&Driver->Common, AXP209_REG_SHUTDOWN, AXP209_POWEROFF);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "AXP209: failed to request shutdown\n"));
    return Status;
  }

  MicroSecondDelay(1000000);
  DEBUG((EFI_D_ERROR, "AXP209: shutdown timed out\n"));

  return EFI_DEVICE_ERROR;
}

PMIC_PROTOCOL gPmicProtocol = {
  Poweroff
};
