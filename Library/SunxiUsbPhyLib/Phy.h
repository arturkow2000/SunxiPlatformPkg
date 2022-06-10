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
#include <Library/SunxiCcmLib.h>
#include <Library/SunxiUsbPhyLib.h>

// Driver internal functions
EFI_STATUS UsbPhyInitGpio(UINT32 Phy);
EFI_STATUS UsbPhyInitClocks(UINT32 Phy);
