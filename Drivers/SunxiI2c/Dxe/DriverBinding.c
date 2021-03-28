/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on code Linux code
 Copyright (C) 2010-2015 Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 Pan Nan <pannan@allwinnertech.com>
 Tom Cubie <tanglaing@allwinnertech.com>
 Victor Wei <weiziheng@allwinnertech.com>

**/

#include "Driver.h"

STATIC CONST SUNXI_I2C_PATH mDevicePathTemplate = {
  {
    {
      {
        HARDWARE_DEVICE_PATH,
        HW_VENDOR_DP,
        {
          (UINT8)(sizeof(I2C_VENDOR_PATH)),
          (UINT8)((sizeof(I2C_VENDOR_PATH)) >> 8)
        }
      },
      EFI_CALLER_ID_GUID
    },
    0,
    0,
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
  VOID *Dummy;
  SUNXI_I2C_PATH *DevicePath;

  // Check if it's us who created controller
  Status = gBS->HandleProtocol(
    Controller,
    &gEfiCallerIdGuid,
    &Dummy
  );
  if (EFI_ERROR(Status))
    return EFI_UNSUPPORTED;

  Status = gBS->OpenProtocol(
    Controller,
    &gEfiDevicePathProtocolGuid,
    (VOID**)&DevicePath,
    This->DriverBindingHandle,
    Controller,
    EFI_OPEN_PROTOCOL_BY_DRIVER
  );
  if (EFI_ERROR(Status))
    return EFI_UNSUPPORTED;

  // Verify path
  // valid path begins vendor node with driver's guid
  // followed by encoded controller number and pointer to SUNXI_I2C_CONFIG structure
  if (CompareMem(&DevicePath->Vendor, &mDevicePathTemplate.Vendor, sizeof(VENDOR_DEVICE_PATH)) != 0)
    Status = EFI_UNSUPPORTED;

  gBS->CloseProtocol(Controller, &gEfiDevicePathProtocolGuid, This->DriverBindingHandle, Controller);
  if (EFI_ERROR(Status))
    return Status;

  // I2C driver requires CCM and GPIO driver
  Status = gBS->LocateProtocol(
    &gSunxiCcmProtocolGuid,
    NULL,
    &Dummy
  );
  if (EFI_ERROR(Status))
    return EFI_NOT_READY;

  Status = gBS->LocateProtocol(
    &gSunxiGpioProtocolGuid,
    NULL,
    &Dummy
  );
  if (EFI_ERROR(Status))
    return EFI_NOT_READY;

  Status = gBS->HandleProtocol(
    Controller,
    &gEfiI2cMasterProtocolGuid,
    &Dummy
  );
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
#if 0
  UINT32 BufferSize;
  EFI_HANDLE CcmHandle;
  EFI_HANDLE GpioHandle;
  SUNXI_CCM_PROTOCOL *CcmProto;
  SUNXI_GPIO_PROTOCOL *GpioProto;
#endif
  SUNXI_I2C_PATH *DevicePath;
  SUNXI_I2C_CONFIG *I2cConfig;
  I2C_DXE_DRIVER *Driver = NULL;
  VOID *Dummy;

  Status = gBS->OpenProtocol(Controller, &gEfiCallerIdGuid, &Dummy, This->DriverBindingHandle, Controller, EFI_OPEN_PROTOCOL_BY_DRIVER);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = gBS->OpenProtocol(Controller, &gEfiDevicePathProtocolGuid, (VOID**)&DevicePath, This->DriverBindingHandle, Controller, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    goto Exit;

  // Path already verified in DriverSupported
#if defined(MDE_CPU_AARCH64)
  I2cConfig = (SUNXI_I2C_CONFIG*)ReadUnaligned64(&DevicePath->Vendor.ConfigAddr);
#elif defined(MDE_CPU_ARM)
  I2cConfig = (SUNXI_I2C_CONFIG*)ReadUnaligned32(&DevicePath->Vendor.ConfigAddr);
#else
#error ?
#endif

  gBS->CloseProtocol(Controller, &gEfiDevicePathProtocolGuid, This->DriverBindingHandle, Controller);

  Driver = AllocatePool(sizeof(I2C_DXE_DRIVER));
  if (!Driver) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  Driver->Signature = DXE_DRIVER_SIGNATURE;
  CopyMem(&Driver->MasterProto, &gSunxiI2cMasterProto, sizeof(EFI_I2C_MASTER_PROTOCOL));
  CopyMem(&Driver->BusConfigProto, &gSunxiI2cBusConfigurationProto, sizeof(EFI_I2C_BUS_CONFIGURATION_MANAGEMENT_PROTOCOL));
  Driver->Driver.Config = I2cConfig;
  Driver->TimerEvent = NULL;
  InitializeListHead(&Driver->Queue);
  Driver->CurrentTask.Request = NULL;

  Status = SunxiI2cInit(&Driver->Driver);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    goto Exit;

  Status = gBS->CreateEvent(
    EVT_TIMER | EVT_NOTIFY_SIGNAL,
    TPL_NOTIFY, 
    ExecuteQueuedTasks,
    Driver,
    &Driver->TimerEvent
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    goto Exit;

  Status = gBS->SetTimer(Driver->TimerEvent, TimerPeriodic, EFI_TIMER_PERIOD_MILLISECONDS(10));
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    goto Exit;

  Status = gBS->InstallMultipleProtocolInterfaces(
    &Controller,
    &gEfiI2cMasterProtocolGuid, &Driver->MasterProto,
    &gEfiI2cBusConfigurationManagementProtocolGuid, &Driver->BusConfigProto,
    NULL
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    goto Exit;

  Exit:
  if (EFI_ERROR(Status)) {
    gBS->CloseProtocol(Controller, &gEfiCallerIdGuid, This->DriverBindingHandle, Controller);
    if (Driver) {
      if (Driver->TimerEvent)
        gBS->CloseEvent(Driver->TimerEvent);

      FreePool(Driver);
    }
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
  EFI_I2C_MASTER_PROTOCOL *Master;
  I2C_DXE_DRIVER *Driver;

  Status = gBS->HandleProtocol(
    Controller,
    &gEfiI2cMasterProtocolGuid,
    (VOID**)&Master
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Driver = DXE_DRIVER_FROM_MASTER(Master);

  Status = gBS->CloseEvent(Driver->TimerEvent);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  // TODO: abort all pending tasks

  Status = SunxiI2cDeInit(&Driver->Driver);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;
  
  Status = gBS->UninstallMultipleProtocolInterfaces(
    Controller,
    &gEfiI2cMasterProtocolGuid, &Driver->MasterProto,
    &gEfiI2cBusConfigurationManagementProtocolGuid, &Driver->BusConfigProto,
    NULL
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  FreePool(Driver);

  Status = gBS->CloseProtocol(Controller, &gEfiCallerIdGuid, This->DriverBindingHandle, Controller);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  return EFI_SUCCESS;
}

EFI_DRIVER_BINDING_PROTOCOL gSunxiI2cDriverBinding = {
  DriverSupported,
  DriverStart,
  DriverStop,
  0x10,
  NULL,
  NULL
};

SUNXI_CCM_PROTOCOL *gSunxiCcmProtocol;
SUNXI_GPIO_PROTOCOL *gSunxiGpioProtocol;

EFI_STATUS
EFIAPI
SunxiI2cInitialize(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;
  EFI_HANDLE Handle;
  UINT32 i;
  SUNXI_I2C_PATH *Path;

  Status = gBS->LocateProtocol(&gSunxiCcmProtocolGuid, NULL, (VOID**)&gSunxiCcmProtocol);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "CCM protocol not found\n"));
    ASSERT_EFI_ERROR(Status);
    return EFI_NOT_FOUND;
  }

  Status = gBS->LocateProtocol(&gSunxiGpioProtocolGuid, NULL, (VOID**)&gSunxiGpioProtocol);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "GPIO protocol not foun\n"));
    ASSERT_EFI_ERROR(Status);
    return EFI_NOT_FOUND;
  }

  for (i = 0; i < gSunxiI2cNumControllers; i++) {
    Handle = NULL;

    Path = AllocateCopyPool(sizeof(SUNXI_I2C_PATH), &mDevicePathTemplate);
    if (!Path) {
      ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
      return EFI_OUT_OF_RESOURCES;
    }

    Path->Vendor.ControllerNo = (UINT8)gSunxiI2cConfig[i].No;
#if defined(MDE_CPU_AARCH64)
    WriteUnaligned64((UINT64*)&Path->Vendor.ConfigAddr, (UINT64)&gSunxiI2cConfig[i]);
#elif defined(MDE_CPU_ARM)
    WriteUnaligned32((UINT32*)&Path->Vendor.ConfigAddr, (UINT32)&gSunxiI2cConfig[i]);
#else
#error ?
#endif

    Status = gBS->InstallMultipleProtocolInterfaces(
      &Handle,
      &gEfiDevicePathProtocolGuid, Path,
      &gEfiCallerIdGuid, NULL,
      NULL
    );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status))
      return Status;
  }

  Status = EfiLibInstallDriverBindingComponentName2(
    ImageHandle,
    SystemTable,
    &gSunxiI2cDriverBinding,
    ImageHandle,
    &gComponentName,
    &gComponentName2
  );
  ASSERT_EFI_ERROR(Status);

  return Status;
}