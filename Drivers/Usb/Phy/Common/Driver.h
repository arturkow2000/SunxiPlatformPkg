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

typedef struct _USB_PHY_DRIVER {
  USB_PHY_SOC_CONFIG *SocConfig;
} USB_PHY_DRIVER;

EFI_STATUS UsbPhyInit(USB_PHY_DRIVER *Driver);

EFI_STATUS UsbPhyInitGpio(USB_PHY_DRIVER *Driver);
