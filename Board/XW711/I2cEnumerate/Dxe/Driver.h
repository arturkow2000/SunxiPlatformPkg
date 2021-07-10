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

#include "../Common/Driver.h"

extern EFI_COMPONENT_NAME_PROTOCOL gComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL gComponentName2;

extern EFI_I2C_ENUMERATE_PROTOCOL gI2cEnumerateProtocol;

typedef struct _I2C_ENUMERATOR_DRIVER {
  EFI_I2C_ENUMERATE_PROTOCOL Protocol;
  CONST I2C_DEVICE *Head;
} I2C_ENUMERATOR_DRIVER;
