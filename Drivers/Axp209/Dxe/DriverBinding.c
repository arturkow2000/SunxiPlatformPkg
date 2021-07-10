/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on code from U-Boot

 Copyright (C) 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
**/

#include "Driver.h"

extern EFI_GUID gI2cAxp209Guid;
// Global pointer to private driver structure
// used by shell extensions
AXP209_DXE_DRIVER *gAxp209DxeDriver = NULL;

STATIC
EFI_STATUS
EFIAPI
DriverSupported(
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN EFI_HANDLE                             Controller,
  IN EFI_DEVICE_PATH_PROTOCOL               *RemainingDevicePath OPTIONAL
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

  if (CompareGuid(I2cIo->DeviceGuid, &gI2cAxp209Guid))
    Status = EFI_SUCCESS;
  else
    Status = EFI_UNSUPPORTED;

  gBS->CloseProtocol(Controller, &gEfiI2cIoProtocolGuid, This->DriverBindingHandle, Controller);
  if (EFI_ERROR(Status))
    return Status;

  Status = gBS->HandleProtocol(Controller, &gPmicProtocolGuid, &Dummy);
  if (Status == EFI_SUCCESS)
    return EFI_ALREADY_STARTED;

  // support only one device
  if (gAxp209DxeDriver != NULL) {
    ASSERT(0);
    return EFI_ALREADY_STARTED;
  }
  
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
DriverStart(
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN EFI_HANDLE                             Controller,
  IN EFI_DEVICE_PATH_PROTOCOL               *RemainingDevicePath OPTIONAL
  )
{
  EFI_STATUS Status;
  EFI_I2C_IO_PROTOCOL *I2cIo;
  AXP209_DXE_DRIVER *Driver;

  Driver = AllocatePool(sizeof(AXP209_DXE_DRIVER));
  if (Driver == NULL)
    return EFI_OUT_OF_RESOURCES;

  CopyMem(&Driver->PmicProto, &gPmicProtocol, sizeof(PMIC_PROTOCOL));

  Status = gBS->OpenProtocol(
    Controller,
    &gEfiI2cIoProtocolGuid,
    (VOID**)&I2cIo,
    This->DriverBindingHandle,
    Controller,
    EFI_OPEN_PROTOCOL_BY_DRIVER
  );
  if (EFI_ERROR(Status))
    goto Exit;

  Driver->I2cIo = I2cIo;

  Status = Axp209Init(I2cIo);
  if (EFI_ERROR(Status))
    goto Exit;

  Status = gBS->InstallMultipleProtocolInterfaces(
    &Controller,
    &gPmicProtocolGuid, &Driver->PmicProto,
    NULL
  );
  if (EFI_ERROR(Status))
    goto Exit;

  Exit:
  if (EFI_ERROR(Status)) {
    gBS->CloseProtocol(Controller, &gEfiI2cIoProtocolGuid, This->DriverBindingHandle, Controller);
    FreePool(Driver);
  } else
    gAxp209DxeDriver = Driver;

  return Status;
}

STATIC
EFI_STATUS
EFIAPI
DriverStop(
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN EFI_HANDLE                            Controller,
  IN UINTN                                 NumberOfChildren,
  IN EFI_HANDLE                            *ChildHandleBuffer OPTIONAL
  )
{
  EFI_STATUS Status;
  AXP209_DXE_DRIVER *Driver;
  
  Status = gBS->HandleProtocol(Controller, &gPmicProtocolGuid, (VOID**)&Driver);
  if (EFI_ERROR(Status))
    return Status;

  Status = gBS->UninstallMultipleProtocolInterfaces(
    Controller,
    &gPmicProtocolGuid, &Driver->PmicProto,
    NULL
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = gBS->CloseProtocol(Controller, &gEfiI2cIoProtocolGuid, This->DriverBindingHandle, Controller);
  ASSERT_EFI_ERROR(Status);

  FreePool(Driver);
  gAxp209DxeDriver = NULL;
  
  return Status;
}

EFI_DRIVER_BINDING_PROTOCOL gAxp209DriverBinding = {
  DriverSupported,
  DriverStart,
  DriverStop,
  0x10,
  NULL,
  NULL
};

EFI_STATUS EFIAPI ExtendShell(VOID);

EFI_STATUS
EFIAPI
Axp209Initialize(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;

  Status = ExtendShell();
  ASSERT_EFI_ERROR(Status);

  return EfiLibInstallDriverBindingComponentName2(
    ImageHandle,
    SystemTable,
    &gAxp209DriverBinding,
    ImageHandle,
    &gComponentName,
    &gComponentName2
  );
}