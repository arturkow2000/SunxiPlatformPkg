#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include "ComponentName.h"

#include <Protocol/DevicePath.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/SunxiGpio.h>

STATIC EFI_HANDLE mDevice;
EFI_DRIVER_BINDING_PROTOCOL gSunxiGpioDriverBinding;

extern SUNXI_GPIO_PROTOCOL gSunxiGpioProtocol;

typedef struct {
  VENDOR_DEVICE_PATH Path;
  EFI_DEVICE_PATH End;
} SUNXI_GPIO_CONTROLLER_PATH;

STATIC SUNXI_GPIO_CONTROLLER_PATH mControllerPath =
{
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8)(sizeof (VENDOR_DEVICE_PATH)),
        (UINT8)((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    EFI_CALLER_ID_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      sizeof(EFI_DEVICE_PATH_PROTOCOL),
      0
    }
  }
};

EFI_STATUS
EFIAPI
SunxiGpioInitialize (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;

  Status = gBS->InstallMultipleProtocolInterfaces(
    &mDevice,
    &gEfiDevicePathProtocolGuid, &mControllerPath,
    &gEfiCallerIdGuid, NULL,
    NULL
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = EfiLibInstallDriverBindingComponentName2(
    ImageHandle,
    SystemTable,
    &gSunxiGpioDriverBinding,
    ImageHandle,
    &gComponentName,
    &gComponentName2
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
DriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  )
{
  VOID *Temp;

  if (Controller != mDevice) {
    return EFI_UNSUPPORTED;
  }

  if (gBS->HandleProtocol(Controller, &gSunxiGpioProtocolGuid, &Temp) == EFI_SUCCESS) {
    return EFI_ALREADY_STARTED;
  }

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
  VOID *Dummy;

  Status = gBS->OpenProtocol (
    Controller,
    &gEfiCallerIdGuid,
    &Dummy,
    This->DriverBindingHandle,
    Controller,
    EFI_OPEN_PROTOCOL_BY_DRIVER
  );
  if (EFI_ERROR (Status))
    return Status;

  Status = gBS->InstallMultipleProtocolInterfaces(
    &Controller,
    &gSunxiGpioProtocolGuid, &gSunxiGpioProtocol,
    NULL
  );

  if (EFI_ERROR(Status)) {
    gBS->CloseProtocol(
      Controller,
      &gEfiCallerIdGuid,
      This->DriverBindingHandle,
      Controller
    );
  }
  return Status;
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

  Status = gBS->UninstallMultipleProtocolInterfaces(
    Controller,
    &gSunxiGpioProtocolGuid,
    &gSunxiGpioProtocol,
    NULL
  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  gBS->CloseProtocol(
    Controller,
    &gEfiCallerIdGuid,
    This->DriverBindingHandle,
    Controller
  );

  return Status;
}

EFI_DRIVER_BINDING_PROTOCOL gSunxiGpioDriverBinding = {
  DriverSupported,
  DriverStart,
  DriverStop,
  0x10,
  NULL,
  NULL
};
