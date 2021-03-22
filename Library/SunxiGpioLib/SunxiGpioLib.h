/*
 * SPDX-License-Identifier: GPL-2.0+
 *
 * Copyright (C) 2021, Artur Kowalski. <arturkow2000@gmail.com>
 * 
 * Based on Linux driver
 * 
 * Copyright (C) 2014-2016 Maxime Ripard <maxime.ripard@free-electrons.com>
 * Copyright (C) 2016 Mylene Josserand <mylene.josserand@free-electrons.com>
 * 
 */

#pragma once

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/IoLib.h>
#include <Library/SunxiGpioLib.h>

#include "Hw.h"

#define _GPIO_PINS_PER_BANK 32

typedef struct _SUNXI_GPIO_MUX
{
  CONST CHAR16 *Name;
  UINT16 Irq;
  UINT8 Id;
} SUNXI_GPIO_MUX;

#define _GPIO_PIN_NONEXISTENT \
  {                           \
    .MuxTable = NULL,         \
    .MuxTableSize = 0,        \
  }

#define GPIO_MUX_IRQ(_Id, _Irq) \
  {                             \
    .Name = L"irq",             \
    .Id = _Id,                  \
    .Irq = _Irq,                \
  }

#define GPIO_MUX(_Id, _Name) \
  {                          \
    .Id = _Id,               \
    .Name = _Name,           \
  }

typedef struct _SUNXI_GPIO_PIN_INTERNAL
{
  CONST SUNXI_GPIO_MUX *MuxTable;
  UINT32 MuxTableSize;
} SUNXI_GPIO_PIN_INTERNAL;

#define GPIO_PIN(_MuxTable)                \
  {                                        \
    .MuxTable = _MuxTable,                 \
    .MuxTableSize = ARRAY_SIZE(_MuxTable), \
  }

extern CONST SUNXI_GPIO_PIN_INTERNAL gSunxiGpioPinList[];
extern CONST UINTN gSunxiGpioPinListLength;