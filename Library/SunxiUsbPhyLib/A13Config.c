/**
 SPDX-License-Identifier: GPL-2.0+

 Allwinner sun4i USB PHY driver

 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>
 Copyright (C) 2017 Jagan Teki <jagan@amarulasolutions.com>
 Copyright (C) 2015 Hans de Goede <hdegoede@redhat.com>
 Copyright (C) 2014 Roman Byshko <rbyshko@gmail.com>

 Modelled arch/arm/mach-sunxi/usb_phy.c to compatible with generic-phy.

**/

#include "Phy.h"
#include "Hw.h"

STATIC CONST UINT32 PmuList[] = {
  0,
  0x01c14800
};

STATIC CONST CHAR16 *mUsb0IdDetectPins[] = {
  L"PG02",
  NULL
};

STATIC CONST CHAR16 *mUsb0VbusDetectPins[] = {
  L"PG01",
  NULL
};

STATIC CONST CHAR16 *mUsb0VbusSupplyPins[] = {
  L"PG12",
  NULL
};

CONST USB_PHY_SOC_CONFIG gUsbPhySocConfig = {
  .PhyCtrlBase = 0x01c13400,
  .Pmu = PmuList,
  .NumPhys = 2,
  .Type = PHY_TYPE_SUN4I_A10,
  .DisconnectThreshold = 2,
  .PhyCtlOffset = REG_PHYCTL_A10,
  .DedicatedClocks = FALSE,
  .EnablePmuUnk1 = FALSE,
  .MissingPhys = 0,
  .CcmClock = L"usb-phy",
  .CcmReset = NULL,
  // FIXME: move board specific stuff somewhere else
  .GpioUsb0IdDetect = mUsb0IdDetectPins,
  .GpioUsb0VbusDetect = mUsb0VbusDetectPins,
  .GpioUsb0VbusSupply = mUsb0VbusSupplyPins
};
