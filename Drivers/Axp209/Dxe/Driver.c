/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on code from U-Boot

 Copyright (C) 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
**/

#include "Driver.h"

EFI_STATUS Axp209Init(IN EFI_I2C_IO_PROTOCOL *Io)
{
  EFI_STATUS Status;
  AXP209_PACKET Packet;
  UINT8 Resp;
  UINT8 Reg;

  Reg = AXP209_REG_CHIP_VERSION;

  Packet.OperationCount = 2;
  Packet.Operation[0].Buffer = &Reg;
  Packet.Operation[0].Flags = 0;
  Packet.Operation[0].LengthInBytes = 1;

  Packet.Operation[1].Buffer = &Resp;
  Packet.Operation[1].Flags = I2C_FLAG_READ;
  Packet.Operation[1].LengthInBytes = 1;
  Status = Io->QueueRequest(Io, 0, NULL, (EFI_I2C_REQUEST_PACKET*)&Packet, NULL);
  if (EFI_ERROR(Status))
    return Status;

  if ((Resp & AXP209_CHIP_VERSION_MASK) != 1) {
    DEBUG((EFI_D_ERROR, "Unsupported AXP209 version 0x%x\n", Resp));
    ASSERT(0);
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}