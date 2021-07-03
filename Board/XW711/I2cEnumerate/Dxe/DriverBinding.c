#include "Driver.h"

extern EFI_GUID gSunxiI2cDriverGuid;

STATIC
EFI_STATUS
EFIAPI
DriverSupported(
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN EFI_HANDLE                             ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL               *RemainingDevicePath OPTIONAL
  )
{
  EFI_STATUS Status;
  VOID *Dummy;
  EFI_DEVICE_PATH *DevicePath;
  VENDOR_DEVICE_PATH *Vendor;
  UINTN AdditionalDataLength;
  UINT8 ControllerNo;

  Status = gBS->OpenProtocol(
    ControllerHandle,
    &gSunxiI2cDriverGuid,
    &Dummy,
    This->DriverBindingHandle,
    ControllerHandle,
    EFI_OPEN_PROTOCOL_GET_PROTOCOL
  );
  if (EFI_ERROR(Status))
    return EFI_UNSUPPORTED;

  gBS->CloseProtocol(ControllerHandle, &gSunxiI2cDriverGuid, This->DriverBindingHandle, ControllerHandle);

  Status = gBS->OpenProtocol(
    ControllerHandle,
    &gEfiDevicePathProtocolGuid,
    (VOID**)&DevicePath,
    This->DriverBindingHandle,
    ControllerHandle,
    EFI_OPEN_PROTOCOL_BY_DRIVER
  );
  if (EFI_ERROR(Status))
    return EFI_UNSUPPORTED;

  Status = EFI_UNSUPPORTED;

  if (DevicePathType(DevicePath) != HARDWARE_DEVICE_PATH || DevicePathSubType(DevicePath) != HW_VENDOR_DP)
    goto Exit;

  Vendor = (VENDOR_DEVICE_PATH*)DevicePath;
  if (!CompareGuid(&Vendor->Guid, &gSunxiI2cDriverGuid))
    goto Exit;

  AdditionalDataLength = DevicePathNodeLength(DevicePath) - sizeof(VENDOR_DEVICE_PATH);
  if (AdditionalDataLength > 1) {
    ControllerNo = *(UINT8*)((UINTN)DevicePath + sizeof(VENDOR_DEVICE_PATH));
    if (ControllerNo < gI2cDeviceListHeadArrayLength) {
      if (gI2cDeviceListHeadArray[ControllerNo] != NULL)
        Status = EFI_SUCCESS;
    }
  }
  
Exit:
  gBS->CloseProtocol(ControllerHandle, &gEfiDevicePathProtocolGuid, This->DriverBindingHandle, ControllerHandle);
  if (Status == EFI_SUCCESS) {
    Status = gBS->HandleProtocol(
      ControllerHandle,
      &gEfiI2cEnumerateProtocolGuid,
      &Dummy
    );
    if (Status == EFI_SUCCESS)
      return EFI_ALREADY_STARTED;

    Status = EFI_SUCCESS;
  }

  return Status;
}

STATIC
EFI_STATUS
EFIAPI
DriverStart(
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN EFI_HANDLE                             ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL               *RemainingDevicePath OPTIONAL
  )
{
  EFI_STATUS Status;
  EFI_DEVICE_PATH *DevicePath;
  UINT8 ControllerNo;
  CONST I2C_DEVICE *Head;
  I2C_ENUMERATOR_DRIVER *Driver;

  Status = gBS->HandleProtocol(
    ControllerHandle,
    &gEfiDevicePathProtocolGuid,
    (VOID**)&DevicePath
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  ControllerNo = *(UINT8*)((UINTN)DevicePath + sizeof(VENDOR_DEVICE_PATH));

  ASSERT(ControllerNo < gI2cDeviceListHeadArrayLength);
  Head = gI2cDeviceListHeadArray[ControllerNo];

  Driver = AllocatePool(sizeof(I2C_ENUMERATOR_DRIVER));
  if (!Driver)
    return EFI_OUT_OF_RESOURCES;

  CopyMem(&Driver->Protocol, &gI2cEnumerateProtocol, sizeof(EFI_I2C_ENUMERATE_PROTOCOL));
  Driver->Head = Head;

  Status = gBS->InstallMultipleProtocolInterfaces(
    &ControllerHandle,
    &gEfiI2cEnumerateProtocolGuid, &Driver->Protocol,
    NULL
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    FreePool(Driver);

  return Status;
}

STATIC
EFI_STATUS
EFIAPI
DriverStop(
  IN EFI_DRIVER_BINDING_PROTOCOL            *This,
  IN EFI_HANDLE                            ControllerHandle,
  IN UINTN                                 NumberOfChildren,
  IN EFI_HANDLE                            *ChildHandleBuffer OPTIONAL
  )
{
  EFI_STATUS Status;
  VOID *Interface;

  Status = gBS->HandleProtocol(ControllerHandle, &gEfiI2cEnumerateProtocolGuid, &Interface);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = gBS->UninstallMultipleProtocolInterfaces(
    ControllerHandle,
    &gEfiI2cEnumerateProtocolGuid, Interface,
    NULL
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  FreePool(Interface);

  return Status;
}

STATIC EFI_DRIVER_BINDING_PROTOCOL mSunxiI2cEnumeratorBinding = {
  DriverSupported,
  DriverStart,
  DriverStop,
  0x10,
  NULL,
  NULL
};

EFI_STATUS
EFIAPI
I2cEnumeratorInitialize(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  return EfiLibInstallDriverBindingComponentName2(
    ImageHandle,
    SystemTable,
    &mSunxiI2cEnumeratorBinding,
    ImageHandle,
    &gComponentName,
    &gComponentName2
  );
}