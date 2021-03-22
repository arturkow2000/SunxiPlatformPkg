/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (c) Artur Kowalski. <arturkow2000@gmail.com>
**/

#include "Driver.h"

STATIC EFI_UNICODE_STRING_TABLE mDriverName[] = {
  {
    "eng;en",
    (CHAR16 *)L"Sunxi CCM Driver",
  },
  { NULL, NULL },
};

STATIC
EFI_STATUS
EFIAPI
ComponentNameGetDriverName(
  IN EFI_COMPONENT_NAME_PROTOCOL *This,
  IN CHAR8 *Language,
  OUT CHAR16 **DriverName)
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
  IN EFI_HANDLE ControllerHandle,
  IN EFI_HANDLE ChildHandle,
  IN CHAR8 *Language,
  OUT CHAR16 **ControllerName)
{
  return EFI_UNSUPPORTED;
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
