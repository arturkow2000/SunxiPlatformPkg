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

#define SUNXI_GPIO_PIN_INVALID 0xffffffff

#define SUNXI_GPIO_DIRECTION_INPUT 1
#define SUNXI_GPIO_DIRECTION_OUTPUT 2

#define SUNXI_GPIO_PULL_DISABLE 0
#define SUNXI_GPIO_PULL_UP 1
#define SUNXI_GPIO_PULL_DOWN 2

#define SUNXI_GPIO_NAME_MAX_LENGTH 31

typedef UINT32 SUNXI_GPIO_PIN;

#define SUNXI_GPIO_PIN_PA(x) ((SUNXI_GPIO_PIN)(x))
#define SUNXI_GPIO_PIN_PB(x) ((SUNXI_GPIO_PIN)32 + (x))
#define SUNXI_GPIO_PIN_PC(x) ((SUNXI_GPIO_PIN)64 + (x))
#define SUNXI_GPIO_PIN_PD(x) ((SUNXI_GPIO_PIN)96 + (x))
#define SUNXI_GPIO_PIN_PE(x) ((SUNXI_GPIO_PIN)128 + (x))
#define SUNXI_GPIO_PIN_PF(x) ((SUNXI_GPIO_PIN)160 + (x))
#define SUNXI_GPIO_PIN_PG(x) ((SUNXI_GPIO_PIN)192 + (x))

SUNXI_GPIO_PIN SunxiGpioNameToPin(IN CONST CHAR16 *Name);
BOOLEAN SunxiGpioIsPinValid(IN SUNXI_GPIO_PIN Pin);
UINTN SunxiGpioGetPinCount(VOID);
BOOLEAN SunxiGpioGetPinName(IN SUNXI_GPIO_PIN Pin, IN CHAR16 *Buffer, IN UINTN BufferLength, OUT UINTN *OutBufferLength OPTIONAL);

CONST CHAR16 *SunxiGpioMuxGetFunction(IN SUNXI_GPIO_PIN Pin);
BOOLEAN SunxiGpioMuxSetFunction(IN SUNXI_GPIO_PIN Pin, IN CONST CHAR16 *Function);
CONST CHAR16 *SunxiGpioMuxGetFunctionName(IN SUNXI_GPIO_PIN Pin, IN INTN Index);

UINT32 SunxiGpioPullGet(IN SUNXI_GPIO_PIN Pin);
VOID SunxiGpioPullSet(IN SUNXI_GPIO_PIN Pin, IN UINT32 Pull);

UINT32 SunxiGpioGetDriveStrength(IN SUNXI_GPIO_PIN Pin);
VOID SunxiGpioSetDriveStrength(IN SUNXI_GPIO_PIN Pin, IN UINT32 Strength);

UINT32 SunxiGpioRead(IN SUNXI_GPIO_PIN Pin);
UINT32 SunxiGpioWrite(IN SUNXI_GPIO_PIN Pin, IN UINT32 Value);
