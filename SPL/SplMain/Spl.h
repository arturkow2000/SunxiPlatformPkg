// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2021, Artur Kowalski.
 * 
 * Based on code from U-Boot
 * 
 * (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 */

#pragma once

#include <PiPei.h>

#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/ArmLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>

#define SPL_BOOT_SOURCE_INVALID (-1)

#define SPL_BOOT_DEVICE_BOARD 1
#define SPL_BOOT_DEVICE_MMC0 2
#define SPL_BOOT_DEVICE_MMC1 3
#define SPL_BOOT_DEVICE_NAND 4
#define SPL_BOOT_DEVICE_SPI 5

INT32 SplGetBootSource(VOID);
UINT32 SplGetBootDevice(VOID);

VOID SplTimerInit(VOID);
UINT32 SplTimerRead(VOID);
VOID SplUdelay(UINT64 MicroSeconds);

UINT32 SplDramInit(VOID);
