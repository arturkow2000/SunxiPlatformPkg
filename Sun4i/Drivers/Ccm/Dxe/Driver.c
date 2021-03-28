/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (c) Artur Kowalski. <arturkow2000@gmail.com>
**/

#include "Driver.h"

EFI_STATUS
EFIAPI
SunxiCcmInitialize(
  EFI_HANDLE          ImageHandle,
  EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_HANDLE Handle = NULL;
  EFI_STATUS Status;

  Status = gBS->InstallMultipleProtocolInterfaces(
    &Handle,
    &gSunxiCcmProtocolGuid, &gSunxiCcmProtocol,
    NULL
  );
  ASSERT_EFI_ERROR(Status);

  return Status;
}
