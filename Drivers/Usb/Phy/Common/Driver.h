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

#include "Config.h"
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/SunxiGpioLib.h>

typedef struct _USB_PHY_DRIVER {
  CONST USB_PHY_SOC_CONFIG *SocConfig;
} USB_PHY_DRIVER;

// Driver interface
EFI_STATUS UsbPhyInit(USB_PHY_DRIVER *Driver);

// Driver internal functions
EFI_STATUS UsbPhyInitGpio(USB_PHY_DRIVER *Driver, UINT32 Phy);
EFI_STATUS UsbPhyInitClocks(USB_PHY_DRIVER *Driver, UINT32 Phy);
EFI_STATUS UsbPhyInitHw(USB_PHY_DRIVER *Driver, UINT32 Phy);

UINT32 UsbPhyRead(USB_PHY_DRIVER *Driver, UINT32 Phy, UINT32 Addr, UINT32 Len);
VOID UsbPhyWrite(USB_PHY_DRIVER *Driver, UINT32 Phy, UINT32 Addr, UINT32 Data, UINT32 Len);
VOID UsbPhyPassby(USB_PHY_DRIVER *Driver, UINT32 Phy, BOOLEAN Enable);
VOID UsbPhy0Route(USB_PHY_DRIVER *Driver, BOOLEAN Gadget);

EFI_STATUS UsbPhyUnmaskGate(USB_PHY_DRIVER *Driver, CONST CHAR16 *GateName);
