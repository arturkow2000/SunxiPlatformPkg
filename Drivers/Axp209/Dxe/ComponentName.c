/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on code from U-Boot

 Copyright (C) 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
**/

#include "Driver.h"

STATIC EFI_UNICODE_STRING_TABLE mDriverName[] = {
  {
    "eng;en",
    (CHAR16 *)L"AXP209 Driver",
  },
  { NULL, NULL },
};

STATIC EFI_UNICODE_STRING_TABLE mDeviceName[] = {
  {
    "eng;en",
    (CHAR16 *)L"AXP209 PMIC"
  },
  { NULL, NULL },
};

STATIC
EFI_STATUS
EFIAPI
ComponentNameGetDriverName(
  IN EFI_COMPONENT_NAME_PROTOCOL *This,
  IN CHAR8 *Language,
  OUT CHAR16 **DriverName
  )
{
  return LookupUnicodeString2(
    Language,
    This->SupportedLanguages,
    mDriverName,
    DriverName,
    (BOOLEAN)(This == &gComponentName)
  );
}

STATIC
EFI_STATUS
EFIAPI
ComponentNameGetControllerName(
  IN EFI_COMPONENT_NAME_PROTOCOL *This,
  IN EFI_HANDLE Controller,
  IN EFI_HANDLE ChildHandle,
  IN CHAR8 *Language,
  OUT CHAR16 **ControllerName
  )
{
  EFI_STATUS Status;

  if (ChildHandle != NULL)
    return EFI_UNSUPPORTED;

  Status = EfiTestManagedDevice(Controller, gAxp209DriverBinding.DriverBindingHandle, &gEfiI2cIoProtocolGuid);
  if (EFI_ERROR(Status))
    return EFI_UNSUPPORTED;

  return LookupUnicodeString2(
    Language,
    This->SupportedLanguages,
    mDeviceName,
    ControllerName,
    (BOOLEAN)(This == &gComponentName)
  );
}

EFI_COMPONENT_NAME_PROTOCOL gComponentName = {
  ComponentNameGetDriverName,
  ComponentNameGetControllerName,
  "eng",
};

EFI_COMPONENT_NAME2_PROTOCOL gComponentName2 = {
  (EFI_COMPONENT_NAME2_GET_DRIVER_NAME)ComponentNameGetDriverName,
  (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME)ComponentNameGetControllerName,
  "en",
};
