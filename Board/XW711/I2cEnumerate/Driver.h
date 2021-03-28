#pragma once

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/DriverBinding.h>
#include <Protocol/DevicePath.h>
#include <Protocol/ComponentName.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/I2cMaster.h>
#include <Protocol/I2cEnumerate.h>

extern EFI_COMPONENT_NAME_PROTOCOL gComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL gComponentName2;

#define EFI_I2C_DEVICE_TO_I2C_DEVICE(x) \
  CR(x, I2C_DEVICE, Device, I2C_DEVICE_SIGNATURE)
#define I2C_DEVICE_SIGNATURE SIGNATURE_32('i', '2', 'd', 'e')

typedef struct _I2C_DEVICE I2C_DEVICE;
struct _I2C_DEVICE {
  UINT32 Signature;
  EFI_I2C_DEVICE Device;
  I2C_DEVICE *Next;
};

extern CONST I2C_DEVICE *gI2cDeviceListHeadArray[];
extern CONST UINT32 gI2cDeviceListHeadArrayLength;
extern EFI_I2C_ENUMERATE_PROTOCOL gI2cEnumerateProtocol;

typedef struct _I2C_ENUMERATOR_DRIVER {
  EFI_I2C_ENUMERATE_PROTOCOL Protocol;
  CONST I2C_DEVICE *Head;
} I2C_ENUMERATOR_DRIVER;
