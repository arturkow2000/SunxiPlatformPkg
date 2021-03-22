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

#include <Library/BaseLib.h>

VOID _ReturnToFel();

extern UINT32 fel_stash[];

VOID SplMain()
{
  UINT32 BootDevice;
  UINT32 DramSize;

  DEBUG((EFI_D_INFO, "SPL starting ...\n"));

  BootDevice = SplGetBootDevice();
  switch (BootDevice)
  {
  case SPL_BOOT_DEVICE_BOARD:
    DEBUG((EFI_D_INFO, "Booting from FEL\n"));
    break;
  case SPL_BOOT_DEVICE_MMC0:
    DEBUG((EFI_D_INFO, "Booting from MMC0\n"));
    break;
  case SPL_BOOT_DEVICE_MMC1:
    DEBUG((EFI_D_INFO, "Booting from MMC1\n"));
    break;
  case SPL_BOOT_DEVICE_SPI:
    DEBUG((EFI_D_INFO, "Booting from SPI\n"));
    break;
  case SPL_BOOT_DEVICE_NAND:
    DEBUG((EFI_D_INFO, "Booting from NAND\n"));
    break;
  }

  DEBUG((EFI_D_INFO, "Initializing DRAM\n"));
  DramSize = SplDramInit();
  DEBUG((EFI_D_INFO, "DRAM size: %u\n", DramSize));

  DEBUG((EFI_D_INFO, "SPL done\n"));

  if (BootDevice == SPL_BOOT_DEVICE_BOARD)
  {
    DEBUG((EFI_D_INFO, "Returning to FEL SP=0x%08x LR=0x%08x\n", fel_stash[0], fel_stash[1]));
    _ReturnToFel();
  }
  else
  {
    DEBUG((EFI_D_ERROR, "Unimplemented\n"));
    ASSERT(0);
    while (TRUE)
      CpuPause();
  }
}
