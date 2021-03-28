/** @file

  SPDX-License-Identifier: BSD-2-Clause-Patent

  Copyright (c) 2021, Artur Kowalski.

  Based on PCF8563 library from edk2-platforms
  Copyright (c) 2017, Linaro, Ltd. All rights reserved.<BR>

**/

#include "Driver.h"

extern EFI_GUID gI2cPcf8563Guid;

STATIC
EFI_STATUS
EFIAPI
DriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  )
{
  EFI_STATUS Status;
  EFI_I2C_IO_PROTOCOL *I2cIo;
  VOID *Dummy;

  Status = gBS->OpenProtocol(
    Controller,
    &gEfiI2cIoProtocolGuid,
    (VOID**)&I2cIo,
    This->DriverBindingHandle,
    Controller,
    EFI_OPEN_PROTOCOL_BY_DRIVER
  );
  if (EFI_ERROR(Status))
    return EFI_UNSUPPORTED;

  if (CompareGuid(I2cIo->DeviceGuid, &gI2cPcf8563Guid))
    Status = EFI_SUCCESS;
  else
    Status = EFI_UNSUPPORTED;

  gBS->CloseProtocol(Controller, &gEfiI2cIoProtocolGuid, This->DriverBindingHandle, Controller);
  if (EFI_ERROR(Status))
    return Status;

  Status = gBS->HandleProtocol(Controller, &gPcf8563ProtocolGuid, &Dummy);
  if (Status == EFI_SUCCESS)
    return EFI_ALREADY_STARTED;

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
DriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  )
{
  EFI_STATUS Status;
  DRIVER_DATA *Data;

  Data = AllocatePool(sizeof(DRIVER_DATA));
  if (Data == NULL)
    return EFI_OUT_OF_RESOURCES;

  CopyMem(&Data->Protocol, &gPcf8563Protocol, sizeof(PCF8563_PROTOCOL));

  Status = gBS->OpenProtocol(
    Controller,
    &gEfiI2cIoProtocolGuid,
    (VOID**)&Data->I2cIo,
    This->DriverBindingHandle,
    Controller,
    EFI_OPEN_PROTOCOL_BY_DRIVER
  );
  if (EFI_ERROR(Status)) {
    FreePool(Data);
    return Status;
  }

  Status = gBS->InstallMultipleProtocolInterfaces(
    &Controller,
    &gPcf8563ProtocolGuid, &Data->Protocol,
    NULL
  );
  if (EFI_ERROR(Status)) {
    gBS->CloseProtocol(Controller, &gEfiI2cIoProtocolGuid, This->DriverBindingHandle, Controller);
    FreePool(Data);
    return Status;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
DriverStop (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN UINTN                       NumberOfChildren,
  IN EFI_HANDLE                  *ChildHandleBuffer
  )
{
  EFI_STATUS Status;
  DRIVER_DATA *Data;

  Status = gBS->HandleProtocol(Controller, &gPcf8563ProtocolGuid, (VOID**)&Data);
  if (EFI_ERROR(Status))
    return Status;

  Status = gBS->UninstallMultipleProtocolInterfaces(
    Controller,
    &gPcf8563ProtocolGuid, &Data->Protocol,
    NULL
  );
  if (EFI_ERROR(Status))
    return Status;

  gBS->CloseProtocol(Controller, &gEfiI2cIoProtocolGuid, This->DriverBindingHandle, Controller);

  FreePool(Data);

  return EFI_SUCCESS;
}

EFI_DRIVER_BINDING_PROTOCOL gPcf8563DriverBinding = {
  DriverSupported,
  DriverStart,
  DriverStop,
  0x10,
  NULL,
  NULL
};

EFI_STATUS
EFIAPI
Pcf8536DxeInitialize(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;

  Status = EfiLibInstallDriverBindingComponentName2(
    ImageHandle,
    SystemTable,
    &gPcf8563DriverBinding,
    ImageHandle,
    &gComponentName,
    &gComponentName2
  );
  ASSERT_EFI_ERROR(Status);

  return Status;
}
