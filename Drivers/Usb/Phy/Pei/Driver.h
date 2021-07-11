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

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>

#include <Ppi/SunxiCcm.h>

#include "../Common/Driver.h"

#define PEI_DRIVER_SIGNATURE SIGNATURE_32('p', 'h', 'y', 'P')

#define PEI_DRIVER_FROM_COMMON_DRIVER(x) \
  CR(x, USB_PHY_PEI_DRIVER, Common, PEI_DRIVER_SIGNATURE)

typedef struct _USB_PHY_PEI_DRIVER {
  UINT32 Signature;
  USB_PHY_DRIVER Common;
} USB_PHY_PEI_DRIVER;

extern SUNXI_CCM_PPI *gSunxiCcmPpi;
