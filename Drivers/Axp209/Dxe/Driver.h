/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on code from U-Boot

 Copyright (C) 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
**/

#pragma once

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TimerLib.h>

#include <Protocol/DriverBinding.h>
#include <Protocol/DevicePath.h>
#include <Protocol/ComponentName.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/I2cIo.h>
#include <Protocol/Pmic.h>

#include "../Common/Driver.h"

extern EFI_COMPONENT_NAME_PROTOCOL gComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL gComponentName2;

extern EFI_DRIVER_BINDING_PROTOCOL gAxp209DriverBinding;
extern PMIC_PROTOCOL gPmicProtocol;

#define AXP209_COMMON_DRIVER_TO_DXE_DRIVER(x)  \
  BASE_CR(x, AXP209_DXE_DRIVER, Common)

typedef struct {
  PMIC_PROTOCOL PmicProto;
  EFI_I2C_IO_PROTOCOL *I2cIo;
  AXP209_DRIVER Common;
} AXP209_DXE_DRIVER;

typedef struct _AXP209_PACKET {
  UINTN OperationCount;
  EFI_I2C_OPERATION Operation[2];
} AXP209_PACKET;

EFI_STATUS Axp209Init(IN EFI_I2C_IO_PROTOCOL *Io);
