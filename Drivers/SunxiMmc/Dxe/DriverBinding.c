#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/DriverBinding.h>
#include <Protocol/SdMmcPassThru.h>
#include <Protocol/SunxiGpio.h>
#include <Protocol/SunxiCcm.h>

#include "ComponentName.h"
#include "Driver.h"

STATIC EFI_HANDLE *mDevices = NULL;
EFI_DRIVER_BINDING_PROTOCOL gSunxiMmcDriverBinding;
SUNXI_GPIO_PROTOCOL *gSunxiGpioProtocol = NULL;
SUNXI_CCM_PROTOCOL *gSunxiCcmProtocol = NULL;

extern CONST SUNXI_MMC_CONFIG gMmcConfig[];
extern CONST UINT32 gNumMmcControllers;

typedef struct
{
  VENDOR_DEVICE_PATH Vendor;
  MEMMAP_DEVICE_PATH MemoryMapped;
  EFI_DEVICE_PATH End;
} SUNXI_MMC_DEVICE_PATH;

STATIC SUNXI_MMC_DEVICE_PATH mSunxiMmcPathTemplate = {
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
    {
      HARDWARE_DEVICE_PATH,
      HW_MEMMAP_DP,
      {
        (UINT8)(sizeof(MEMMAP_DEVICE_PATH)),
        (UINT8)(sizeof(MEMMAP_DEVICE_PATH) >> 8),
      },
    },
    EfiMemoryMappedIO,
    (EFI_PHYSICAL_ADDRESS)0,
    (EFI_PHYSICAL_ADDRESS)0
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

EFI_STATUS
EFIAPI
SunxiMmcInitialize(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;
  INT32 i;
  SUNXI_MMC_DEVICE_PATH *DevicePath;

  mDevices = AllocateZeroPool(sizeof(EFI_HANDLE) * gNumMmcControllers);
  if (mDevices == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    ASSERT_EFI_ERROR(Status);
    return Status;
  }

  for (i = 0; i < gNumMmcControllers; i++) {
    DevicePath = AllocateCopyPool(sizeof(SUNXI_MMC_DEVICE_PATH), &mSunxiMmcPathTemplate);
    if (!DevicePath) {
      DEBUG((EFI_D_ERROR, "Failed to allocate device path\n"));
      Status = EFI_OUT_OF_RESOURCES;
      ASSERT_EFI_ERROR(Status);
      return Status;
    }
    DevicePath->MemoryMapped.StartingAddress = gMmcConfig[i].Base;
    DevicePath->MemoryMapped.EndingAddress = DevicePath->MemoryMapped.StartingAddress + 0xfff;
    Status = gBS->InstallMultipleProtocolInterfaces(
      &mDevices[i],
      &gEfiCallerIdGuid, NULL,
      &gEfiDevicePathProtocolGuid, DevicePath,
      NULL
    );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  Status = EfiLibInstallDriverBindingComponentName2(
    ImageHandle,
    SystemTable,
    &gSunxiMmcDriverBinding,
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
DriverSupported(
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL *RemainingDevicePath OPTIONAL
  )
{
  EFI_STATUS Status;
  VOID *Temp;

  //if (ControllerHandle != mDevice)
  //  return EFI_UNSUPPORTED;

  Status = gBS->HandleProtocol(ControllerHandle, &gEfiCallerIdGuid, &Temp);
  if (EFI_ERROR(Status))
    return EFI_UNSUPPORTED;

  if (gBS->HandleProtocol(ControllerHandle, &gEfiSdMmcPassThruProtocolGuid, &Temp) == EFI_SUCCESS)
    return EFI_ALREADY_STARTED;

  Status = gBS->LocateProtocol(
    &gSunxiGpioProtocolGuid,
    NULL,
    (VOID**)&gSunxiGpioProtocol
  );
  if (EFI_ERROR(Status))
    return EFI_NOT_READY;
  
  Status = gBS->LocateProtocol(
    &gSunxiCcmProtocolGuid,
    NULL,
    (VOID**)&gSunxiCcmProtocol
  );
  if (EFI_ERROR(Status))
    return EFI_NOT_READY;

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
DriverStart(
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL *RemainingDevicePath OPTIONAL
  )
{
  EFI_STATUS Status;
  VOID *Dummy;
  SUNXI_MMC_DRIVER *Driver;
  SUNXI_MMC_DEVICE_PATH *DevicePath;
  CONST SUNXI_MMC_CONFIG *Config;
  INT32 i;

  Driver = NULL;

  Status = gBS->OpenProtocol(
    ControllerHandle,
    &gEfiCallerIdGuid,
    &Dummy,
    This->DriverBindingHandle,
    ControllerHandle,
    EFI_OPEN_PROTOCOL_BY_DRIVER
  );
  if (EFI_ERROR(Status))
    return Status;

  Status = gBS->HandleProtocol(
    ControllerHandle,
    &gEfiDevicePathProtocolGuid,
    (VOID**)&DevicePath
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    goto Exit;

  Config = NULL;

  for (i = 0; i < gNumMmcControllers; i++) {
    if (gMmcConfig[i].Base == DevicePath->MemoryMapped.StartingAddress) {
      Config = &gMmcConfig[i];
      break;
    }
  }

  if (!Config) {
    DEBUG((EFI_D_ERROR, "No config found for mmc@%llx\n", DevicePath->MemoryMapped.StartingAddress));
    Status = EFI_DEVICE_ERROR;
    goto Exit;
  }

  Status = SunxiMmcCreateDriver(Config, &Driver);
  if (EFI_ERROR(Status))
    goto Exit;

  Status = gBS->InstallMultipleProtocolInterfaces(
    &ControllerHandle,
    &gEfiSdMmcPassThruProtocolGuid, &Driver->Protocol,
    NULL
  );

Exit:
  if (EFI_ERROR(Status)) {
    if (Driver)
      SunxiMmcDestroyDriver(Driver);

    gBS->CloseProtocol(ControllerHandle, &gEfiCallerIdGuid, This->DriverBindingHandle, ControllerHandle);
  }
  
  return Status;
}

STATIC
EFI_STATUS
EFIAPI
DriverStop(
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE ControllerHandle,
  IN UINTN NumberOfChildren,
  IN EFI_HANDLE *ChildHandleBuffer OPTIONAL
  )
{
  ASSERT(0);
  return EFI_UNSUPPORTED;
}

EFI_DRIVER_BINDING_PROTOCOL gSunxiMmcDriverBinding = {
  DriverSupported,
  DriverStart,
  DriverStop,
  0x10,
  NULL,
  NULL,
};
