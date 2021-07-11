/**
 SPDX-License-Identifier: GPL-2.0+

 Allwinner sun4i USB PHY driver

 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>
 Copyright (C) 2017 Jagan Teki <jagan@amarulasolutions.com>
 Copyright (C) 2015 Hans de Goede <hdegoede@redhat.com>
 Copyright (C) 2014 Roman Byshko <rbyshko@gmail.com>

 Modelled arch/arm/mach-sunxi/usb_phy.c to compatible with generic-phy.

**/

#pragma once

#include <PiPei.h>

#define PHY_TYPE_SUN4I_A10 0
#define PHY_TYPE_SUN6I_A31 1
#define PHY_TYPE_SUN8I_A33 2
#define PHY_TYPE_SUN8I_A83T 3
#define PHY_TYPE_SUN8I_H3 4
#define PHY_TYPE_SUN8I_R40 5
#define PHY_TYPE_SUN8I_V3S 6
#define PHY_TYPE_SUN50I_A64 7
#define PHY_TYPE_SUN50I_H6 8

typedef struct _USB_PHY_SOC_CONFIG {
  UINT32 PhyCtrlBase;
  CONST UINT32 *Pmu;
  UINT32 NumPhys;
  UINT32 Type;
  UINT32 DisconnectThreshold;
  UINT8 PhyCtlOffset;
  BOOLEAN DedicatedClocks;
  BOOLEAN EnablePmuUnk1;
  UINT32 MissingPhys;
  // If DedicatedClocks is FALSE then all PHYs have one common clock
  // this field points to clock name to enable
  //
  // If DedicatedClocks is TRUE then there is one clock per PHY
  // this field points to table of clock names
  // each entry in table MUST be non-null
  // except for PHYs marked as missing, in that case entries should be NULL
  // If NULL clock is encountered during PHY initialization then ASSERT()
  // Table size is defined by NumPhys field
  //
  // If this field is NULL then ASSERT()
  union {
    CONST CHAR16 **CcmClocks;
    CONST CHAR16 *CcmClock;
  };
  // Pointer to table of reset pins to deassert one per PHY
  // This field may be NULL
  // Entries in table this field points may be NULL
  CONST CHAR16 **CcmReset;

  // FIXME: move board specific stuff somewhere else
  CONST CHAR16 **GpioUsb0IdDetect;
  CONST CHAR16 **GpioUsb0VbusDetect;
  CONST CHAR16 **GpioUsb0VbusSupply;
} USB_PHY_SOC_CONFIG;

extern CONST USB_PHY_SOC_CONFIG gUsbPhySocConfig;
