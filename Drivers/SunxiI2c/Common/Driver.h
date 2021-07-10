/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on code Linux code
 Copyright (C) 2010-2015 Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 Pan Nan <pannan@allwinnertech.com>
 Tom Cubie <tanglaing@allwinnertech.com>
 Victor Wei <weiziheng@allwinnertech.com>

**/

#pragma once

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/SunxiGpioLib.h>

#include "Hw.h"

#define SUNXI_I2C_MAX_CONTROLLERS 5

typedef struct _GPIO_CONFIG {
  CONST CHAR16 *Pin;
  CONST CHAR16 *Function;
  UINT32 Pull;
  UINT32 Drive;
} GPIO_CONFIG;

typedef struct _SUNXI_I2C_CONFIG {
  CONST GPIO_CONFIG *GpioConfig;
  UINT32 GpioConfigLength;
  UINT32 Base;
  UINT32 No;
} SUNXI_I2C_CONFIG;

extern SUNXI_I2C_CONFIG gSunxiI2cConfig[];
extern UINT32 gSunxiI2cNumControllers;

typedef struct _I2C_DRIVER {
  CONST SUNXI_I2C_CONFIG *Config;
  UINT32 BusClockHz;
} I2C_DRIVER;

EFI_STATUS SunxiI2cInit(IN I2C_DRIVER *Driver);
EFI_STATUS SunxiI2cDeInit(IN I2C_DRIVER *Driver);
EFI_STATUS SunxiI2cSetBusClock(IN I2C_DRIVER *Driver, IN UINT32 Hz);

EFI_STATUS GateUnmask(IN I2C_DRIVER *Driver, IN CONST CHAR16 *GateName);
EFI_STATUS GateMask(IN I2C_DRIVER *Driver, IN CONST CHAR16 *GateName);
EFI_STATUS GetPllFrequency(IN I2C_DRIVER *Driver, IN CONST CHAR16 *PllName, OUT UINT32 *Freq);

EFI_STATUS SunxiI2cInitGpio(IN I2C_DRIVER *Driver);

EFI_STATUS SunxiI2cStart(IN I2C_DRIVER *Driver, BOOLEAN Restart);
EFI_STATUS SunxiI2cStop(IN I2C_DRIVER *Driver);

BOOLEAN SunxiI2cPoll(IN I2C_DRIVER *Driver, OUT UINT32 *OutStatus);
VOID SunxiI2cWriteByte(IN I2C_DRIVER *Driver, IN UINT8 Byte);