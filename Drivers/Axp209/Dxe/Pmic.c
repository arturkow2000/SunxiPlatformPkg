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
  DRIVER_DATA *Data = (DRIVER_DATA*)This;
  AXP209_PACKET Packet;
  UINT8 Buf[2];

  Buf[0] = AXP209_REG_SHUTDOWN;
  Buf[1] = AXP209_POWEROFF;

  Packet.OperationCount = 1;
  Packet.Operation[0].Buffer = Buf;
  Packet.Operation[0].Flags = 0;
  Packet.Operation[0].LengthInBytes = 2;

  Status = Data->I2cIo->QueueRequest(
    Data->I2cIo,
    0,
    NULL,
    (EFI_I2C_REQUEST_PACKET*)&Packet,
    NULL
  );
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
