/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on code from U-Boot

 Copyright (C) 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
**/

#pragma once

#define AXP209_REG_POWER_STATUS 0x00
#define AXP209_REG_CHIP_VERSION 0x03
#define AXP209_REG_OUTPUT_CTRL 0x12
#define AXP209_REG_DCDC2_VOLTAGE 0x23
#define AXP209_REG_VRC_DCDC2_LDO3 0x25
#define AXP209_REG_DCDC3_VOLTAGE 0x27
#define AXP209_REG_LDO24_VOLTAGE 0x28
#define AXP209_REG_LDO3_VOLTAGE 0x29
#define AXP209_REG_IRQ_ENABLE1 0x40
#define AXP209_REG_IRQ_ENABLE2 0x41
#define AXP209_REG_IRQ_ENABLE3 0x42
#define AXP209_REG_IRQ_ENABLE4 0x43
#define AXP209_REG_IRQ_ENABLE5 0x44
#define AXP209_REG_IRQ_STATUS5 0x4c
#define AXP209_REG_SHUTDOWN 0x32

#define AXP209_POWER_STATUS_ON_BY_DC    (1 << 0)
#define AXP209_POWER_STATUS_VBUS_USABLE (1 << 4)

#define AXP209_CHIP_VERSION_MASK        0x0f

#define AXP209_OUTPUT_CTRL_EXTEN        (1 << 0)
#define AXP209_OUTPUT_CTRL_DCDC3        (1 << 1)
#define AXP209_OUTPUT_CTRL_LDO2         (1 << 2)
#define AXP209_OUTPUT_CTRL_LDO4         (1 << 3)
#define AXP209_OUTPUT_CTRL_DCDC2        (1 << 4)
#define AXP209_OUTPUT_CTRL_LDO3         (1 << 6)

#define AXP209_POWEROFF                 (1 << 7)
