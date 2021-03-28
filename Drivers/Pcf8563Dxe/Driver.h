/** @file

  SPDX-License-Identifier: BSD-2-Clause-Patent

  Copyright (c) 2021, Artur Kowalski.

  Based on PCF8563 library from edk2-platforms
  Copyright (c) 2017, Linaro, Ltd. All rights reserved.<BR>

**/

#pragma once

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/DriverBinding.h>
#include <Protocol/DevicePath.h>
#include <Protocol/ComponentName.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/I2cIo.h>
#include <Protocol/Pcf8563.h>

extern EFI_DRIVER_BINDING_PROTOCOL gPcf8563DriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL gComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL gComponentName2;
extern PCF8563_PROTOCOL gPcf8563Protocol;

typedef struct _DRIVER_DATA {
  PCF8563_PROTOCOL Protocol;
  EFI_I2C_IO_PROTOCOL *I2cIo;
} DRIVER_DATA;
