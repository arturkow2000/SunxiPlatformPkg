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

#include "Spl.h"

#define SPL_BASE (FixedPcdGet32(PcdSplBaseAddress))

INT32 SplGetBootSource(VOID)
{
  if (CompareMem((VOID *)(SPL_BASE + 4), "eGON.BT0", 8) != 0)
  {
    return SPL_BOOT_SOURCE_INVALID;
  }

  return *((UINT8 *)(SPL_BASE + 0x28));
}

#define SPL_BOOTED_FROM_MMC0 0
#define SPL_BOOTED_FROM_NAND 1
#define SPL_BOOTED_FROM_MMC2 2
#define SPL_BOOTED_FROM_SPI 3
#define SPL_BOOTED_FROM_MMC0_HIGH 0x10
#define SPL_BOOTED_FROM_MMC2_HIGH 0x12

UINT32 SplGetBootDevice(VOID)
{
  INT32 BootSource;

  BootSource = SplGetBootSource();

  /*
   * When booting from the SD card or NAND memory, the "eGON.BT0"
   * signature is expected to be found in memory at the address 0x0004
   * (see the "mksunxiboot" tool, which generates this header).
   *
   * When booting in the FEL mode over USB, this signature is patched in
   * memory and replaced with something else by the 'fel' tool. This other
   * signature is selected in such a way, that it can't be present in a
   * valid bootable SD card image (because the BROM would refuse to
   * execute the SPL in this case).
   *
   * This checks for the signature and if it is not found returns to
   * the FEL code in the BROM to wait and receive the main u-boot
   * binary over USB. If it is found, it determines where SPL was
   * read from.
   */

  switch (BootSource)
  {
  case SPL_BOOT_SOURCE_INVALID:
    return SPL_BOOT_DEVICE_BOARD;
  case SPL_BOOTED_FROM_MMC0:
  case SPL_BOOTED_FROM_MMC0_HIGH:
    return SPL_BOOT_DEVICE_MMC0;
  case SPL_BOOTED_FROM_MMC2:
  case SPL_BOOTED_FROM_MMC2_HIGH:
    return SPL_BOOT_DEVICE_MMC1;
  case SPL_BOOTED_FROM_NAND:
    return SPL_BOOT_DEVICE_NAND;
  case SPL_BOOTED_FROM_SPI:
    return SPL_BOOT_DEVICE_SPI;
  }

  DEBUG((EFI_D_ERROR, "Unknown boot source %d\n", BootSource));
  ASSERT(0);
  return 0;
}