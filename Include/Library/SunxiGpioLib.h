/* SPDX-License-Identifier: GPL-2.0+
 *
 * Copyright (c) Artur Kowalski. <arturkow2000@gmail.com>
 * 
 * Based on code from U-Boot
 * 
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 * 
 */

#pragma once

#include <Uefi.h>

#define SUNXI_GPIO_PULL_DISABLE 0
#define SUNXI_GPIO_PULL_UP 1
#define SUNXI_GPIO_PULL_DOWN 2

#define SUNXI_GPIO_NAME_MAX_LENGTH 31

#define SUNXI_GPIO_PIN_PA(x) ((UINT32)(x))
#define SUNXI_GPIO_PIN_PB(x) ((UINT32)32 + (x))
#define SUNXI_GPIO_PIN_PC(x) ((UINT32)64 + (x))
#define SUNXI_GPIO_PIN_PD(x) ((UINT32)96 + (x))
#define SUNXI_GPIO_PIN_PE(x) ((UINT32)128 + (x))
#define SUNXI_GPIO_PIN_PF(x) ((UINT32)160 + (x))
#define SUNXI_GPIO_PIN_PG(x) ((UINT32)192 + (x))

EFI_STATUS SunxiGpioGetPin(IN CONST CHAR16 *Name, OUT UINT32 *OutPin);

EFI_STATUS SunxiGpioGetFunction(IN UINT32 Pin, OUT CONST CHAR16 **OutFunction);
EFI_STATUS SunxiGpioSetFunction(IN UINT32 Pin, IN CONST CHAR16 *Function);

EFI_STATUS SunxiGpioGetPullMode(IN UINT32 Pin, OUT UINT32 *OutPullMode);
EFI_STATUS SunxiGpioSetPullMode(IN UINT32 Pin, IN UINT32 PullMode);

EFI_STATUS SunxiGpioGetDriveStrength(IN UINT32 Pin, OUT UINT32 *OutStrength);
EFI_STATUS SunxiGpioSetDriveStrength(IN UINT32 Pin, IN UINT32 Strength);

EFI_STATUS SunxiGpioConfigureAsOutput(IN UINT32 Pin);
EFI_STATUS SunxiGpioConfigureAsInput(IN UINT32 Pin);

EFI_STATUS SunxiGpioSetLevel(IN UINT32 Pin, IN UINT8 Level);
EFI_STATUS SunxiGpioGetLevel(IN UINT32 Pin, OUT UINT8 *OutLevel);

