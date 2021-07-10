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

#define REG_ISCR                  0x00
#define REG_PHYCTL_A10            0x04
#define REG_PHYBIST               0x08
#define REG_PHYTUNE               0x0c
#define REG_PHYCTL_A33            0x10
#define REG_PHY_OTGCTL            0x20
#define REG_PMU_UNK1              0x10

/* Common Control Bits for Both PHYs */
#define PHY_PLL_BW                0x03
#define PHY_RES45_CAL_EN          0x0c

/* Private Control Bits for Each PHY */
#define PHY_TX_AMPLITUDE_TUNE     0x20
#define PHY_TX_SLEWRATE_TUNE      0x22
#define PHY_DISCON_TH_SEL         0x2a
#define PHY_SQUELCH_DETECT        0x3c

#define PHYCTL_DATA               (1 << 7)
#define OTGCTL_ROUTE_MUSB         (1 << 0)

#define PHY_TX_RATE               (1 << 4)
#define PHY_TX_MAGNITUDE          (1 << 2)
#define PHY_TX_AMPLITUDE_LEN       5

#define PHY_RES45_CAL_DATA        (1 << 0)
#define PHY_RES45_CAL_LEN         1
#define PHY_DISCON_TH_LEN         2

#define SUNXI_AHB_ICHR8_EN        (1 << 10)
#define SUNXI_AHB_INCR4_BURST_EN  (1 << 9)
#define SUNXI_AHB_INCRX_ALIGN_EN  (1 << 8)
#define SUNXI_ULPI_BYPASS_EN      (1 << 0)

/* A83T specific control bits for PHY0 */
#define PHY_CTL_VBUSVLDEXT        (1 << 5)
#define PHY_CTL_SIDDQ             (1 << 3)

/* A83T specific control bits for PHY2 HSIC */
#define SUNXI_EHCI_HS_FORCE       (1 << 20)
#define SUNXI_HSIC_CONNECT_INT    (1 << 16)
#define SUNXI_HSIC                (1 << 1)