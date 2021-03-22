/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (c) Artur Kowalski. <arturkow2000@gmail.com>
**/

#include "Driver.h"

STATIC EFI_HANDLE mDevice = NULL;

typedef struct
{
  VENDOR_DEVICE_PATH Path;
  EFI_DEVICE_PATH End;
} SUNXI_CCM_DEVICE_PATH;

STATIC SUNXI_CCM_DEVICE_PATH mSunxiCcmPath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8)(sizeof(VENDOR_DEVICE_PATH)),
        (UINT8)(sizeof(VENDOR_DEVICE_PATH) >> 8),
      },
    },
    EFI_CALLER_ID_GUID,
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      sizeof(EFI_DEVICE_PATH_PROTOCOL),
      0,
    },
  },
};

STATIC
EFI_STATUS
EFIAPI
DriverSupported(
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE Controller,
  IN EFI_DEVICE_PATH_PROTOCOL *RemainingDevicePath OPTIONAL
  )
{
  VOID *Temp;

  if (Controller != mDevice)
    return EFI_UNSUPPORTED;

  if (gBS->HandleProtocol(Controller, &gSunxiCcmProtocolGuid, &Temp) == EFI_SUCCESS)
    return EFI_ALREADY_STARTED;

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
DriverStart(
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE Controller,
  IN EFI_DEVICE_PATH_PROTOCOL *RemainingDevicePath OPTIONAL
  )
{
  EFI_STATUS Status;
  VOID *Dummy;
  
  Status = gBS->OpenProtocol(
    Controller,
    &gEfiCallerIdGuid,
    &Dummy,
    This->DriverBindingHandle,
    Controller,
    EFI_OPEN_PROTOCOL_BY_DRIVER
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = gBS->InstallMultipleProtocolInterfaces(
    &Controller,
    &gSunxiCcmProtocolGuid, &gSunxiCcmProtocol,
    NULL
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    gBS->CloseProtocol(Controller, &gEfiCallerIdGuid, This->DriverBindingHandle, Controller);

  return Status;
}

STATIC
EFI_STATUS
EFIAPI
DriverStop(
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE Controller,
  IN UINTN NumberOfChildren,
  IN EFI_HANDLE *ChildHandleBuffer OPTIONAL
  )
{
  EFI_STATUS Status;

  Status = gBS->UninstallProtocolInterface(
    Controller,
    &gSunxiCcmProtocolGuid, &gSunxiCcmProtocol
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  gBS->CloseProtocol(Controller, &gEfiCallerIdGuid, This->DriverBindingHandle, Controller);

  return EFI_SUCCESS;
}

STATIC EFI_DRIVER_BINDING_PROTOCOL mDriverBinding = {
  DriverSupported,
  DriverStart,
  DriverStop,
  0x10,
  NULL,
  NULL
};

EFI_STATUS
EFIAPI
SunxiCcmInitialize(
  EFI_HANDLE          ImageHandle,
  EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS Status;

  Status = gBS->InstallMultipleProtocolInterfaces(
    &mDevice,
    &gEfiDevicePathProtocolGuid, &mSunxiCcmPath,
    &gEfiCallerIdGuid, NULL,
    NULL
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = EfiLibInstallDriverBindingComponentName2(
    ImageHandle,
    SystemTable,
    &mDriverBinding,
    ImageHandle,
    &gComponentName,
    &gComponentName2
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  return Status;
}