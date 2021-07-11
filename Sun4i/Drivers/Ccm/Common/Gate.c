/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (c) Artur Kowalski. <arturkow2000@gmail.com>

 Based on code from Linux:
 Copyright (c) 2016 Maxime Ripard. All rights reserved.
**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>

#include "Driver.h"

#include <Sunxi/HW/ClockSun4i.h>

CONST CCM_GATE_DESC gCcmGateList[] = {
  // ahb0
  GATE(L"ahb0-otg", 0x060, 0),
  GATE(L"ahb0-ehci", 0x060, 1),
  GATE(L"ahb0-ohci", 0x060, 2),
  GATE(L"ahb0-ss", 0x060, 5),
  GATE(L"ahb0-dma", 0x060, 6),
  GATE(L"ahb0-bist", 0x060, 7),
  GATE(L"ahb0-mmc0", 0x060, 8),
  GATE(L"ahb0-mmc1", 0x060, 9),
  GATE(L"ahb0-mmc2", 0x060, 10),
  GATE(L"ahb0-nand0", 0x060, 13),
  GATE(L"ahb0-sdram", 0x060, 14),
  GATE(L"ahb0-emac", 0x060, 17),
  GATE(L"ahb0-ts0", 0x060, 18),
  GATE(L"ahb0-spi0", 0x060, 20),
  GATE(L"ahb0-spi1", 0x060, 21),
  GATE(L"ahb0-spi2", 0x060, 22),
  GATE(L"ahb0-gps", 0x060, 26),
  GATE(L"ahb0-hstimer", 0x060, 28),
  // ahb1
  GATE(L"ahb1-ve", 0x064, 0),
  GATE(L"ahb1-tve", 0x064, 2),
  GATE(L"ahb1-lcd", 0x064, 4),
  GATE(L"ahb1-csi", 0x064, 8),
  GATE(L"ahb1-hdmi", 0x064, 11),
  GATE(L"ahb1-be0", 0x064, 12),
  GATE(L"ahb1-fe0", 0x064, 14),
  GATE(L"ahb1-iep", 0x064, 19),
  GATE(L"ahb1-gpu", 0x064, 20),
  // apb0
  GATE(L"apb0-codec", 0x068, 0),
  GATE(L"apb0-spdif", 0x068, 1),
  GATE(L"apb0-i2s", 0x068, 3),
  GATE(L"apb0-pio", 0x068, 5),
  GATE(L"apb0-ir", 0x068, 6),
  GATE(L"apb0-keypad", 0x068, 10),
  // apb1
  GATE(L"apb1-i2c0", 0x06c, 0),
  GATE(L"apb1-i2c1", 0x06c, 1),
  GATE(L"apb1-i2c2", 0x06c, 2),
  GATE(L"apb1-uart0", 0x06c, 16),
  GATE(L"apb1-uart1", 0x06c, 17),
  GATE(L"apb1-uart2", 0x06c, 18),
  GATE(L"apb1-uart3", 0x06c, 19),
  // DRAM
  GATE(L"dram-be0", 0x100, 26),
  // pll-periph
  GATE(L"usb-ohci", 0x0cc, 6),
  GATE(L"usb-phy", 0x0cc, 8)
};

CONST UINT32 gCcmGateListLength = ARRAY_SIZE(gCcmGateList);

EFI_STATUS CcmGateUnmask(IN UINT32 GateIndex)
{
  CONST CCM_GATE_DESC *Gate;

  if (GateIndex >= gCcmGateListLength)
    return EFI_NOT_FOUND;

  Gate = &gCcmGateList[GateIndex];

  MmioOr32(CCM_BASE + Gate->Reg, 1u << Gate->Bit);

  return EFI_SUCCESS;
}

EFI_STATUS CcmGateMask(IN UINT32 GateIndex)
{
  CONST CCM_GATE_DESC *Gate;

  if (GateIndex >= gCcmGateListLength)
    return EFI_NOT_FOUND;

  Gate = &gCcmGateList[GateIndex];

  MmioAnd32(CCM_BASE + Gate->Reg, ~(1u << Gate->Bit));

  return EFI_SUCCESS;
}

EFI_STATUS CcmGateIsMasked(IN UINT32 GateIndex, OUT BOOLEAN *OutIsMasked)
{
  CONST CCM_GATE_DESC *Gate;
  UINT32 Reg;

  if (OutIsMasked == NULL)
    return EFI_INVALID_PARAMETER;

  if (GateIndex >= gCcmGateListLength)
    return EFI_NOT_FOUND;

  Gate = &gCcmGateList[GateIndex];

  Reg = MmioRead32(CCM_BASE + Gate->Reg);
  Reg >>= Gate->Bit;

  *OutIsMasked = !(Reg & 1);

  return EFI_SUCCESS;
}
