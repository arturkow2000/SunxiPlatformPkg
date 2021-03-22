#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>

#include "ComponentName.h"

#include <Protocol/SunxiGpio.h>

extern EFI_DRIVER_BINDING_PROTOCOL gSunxiGpioDriverBinding;

STATIC EFI_UNICODE_STRING_TABLE mDriverName[] = {
  {
    "eng;en",
    (CHAR16 *)L"Sunxi GPIO Driver",
  },
  { NULL, NULL },
};

STATIC EFI_UNICODE_STRING_TABLE mControllerName[] = {
  {
    "eng;en",
    (CHAR16 *)L"Sunxi GPIO controller",
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
  IN EFI_HANDLE ControllerHandle,
  IN EFI_HANDLE ChildHandle,
  IN CHAR8 *Language,
  OUT CHAR16 **ControllerName
  )
{
  EFI_STATUS Status;

  //
  // ChildHandle must be NULL for a Device Driver
  //
  if (ChildHandle != NULL)
  {
    return EFI_UNSUPPORTED;
  }

  //
  // Make sure this driver is currently managing ControllerHandle
  //
  Status = EfiTestManagedDevice(
    ControllerHandle,
    gSunxiGpioDriverBinding.DriverBindingHandle,
    &gSunxiGpioProtocolGuid
    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  return LookupUnicodeString2(
    Language,
    This->SupportedLanguages,
    mControllerName,
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