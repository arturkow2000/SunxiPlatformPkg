/*
 * SPDX-License-Identifier: GPL-2.0+
 *
 * Copyright (C) 2021, Artur Kowalski. <arturkow2000@gmail.com>
 * 
 * Based on Linux driver
 * 
 * Copyright (C) 2014-2016 Maxime Ripard <maxime.ripard@free-electrons.com>
 * Copyright (C) 2016 Mylene Josserand <mylene.josserand@free-electrons.com>
 * 
 */
#pragma once

#define GPIO_BASE 0x01c20800

#define BANK_MEM_SIZE 0x24
#define MUX_REGS_OFFSET 0x0
#define DATA_REGS_OFFSET 0x10
#define DLEVEL_REGS_OFFSET 0x14
#define PULL_REGS_OFFSET 0x1c

#define PINS_PER_BANK 32
#define MUX_PINS_PER_REG 8
#define MUX_PINS_BITS 4
#define MUX_PINS_MASK 0x0f
#define DATA_PINS_PER_REG 32
#define DATA_PINS_BITS 1
#define DATA_PINS_MASK 0x01
#define DLEVEL_PINS_PER_REG 16
#define DLEVEL_PINS_BITS 2
#define DLEVEL_PINS_MASK 0x03
#define PULL_PINS_PER_REG 16
#define PULL_PINS_BITS 2
#define PULL_PINS_MASK 0x03