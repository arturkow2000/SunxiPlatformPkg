/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Configuration settings for the Allwinner A10-evb board.
 */
#pragma once

#define SUNXI_TIMER_BASE (FixedPcdGet32(TimerBase))

#define SUNXI_TIMER_NUM_TIMERS 6

#define SUNXI_TIMER_IRQEN 0x000
#define SUNXI_TIMER_IRQSTA 0x004
#define SUNXI_TIMER_CTL(Timer) (0x010 + 0x010 * Timer)
#define SUNXI_TIMER_INTERVAL(Timer) (0x014 + 0x010 * Timer)
#define SUNXI_TIMER_VAL(Timer) (0x018 + 0x010 * Timer)

#define SUNXI_TIMER_CTL_EN (1 << 0)
#define SUNXI_TIMER_CTL_RELOAD (1 << 1)
#define SUNXI_TIMER_CTL_SRC_OSC32K (0 << 2)
#define SUNXI_TIMER_CTL_SRC_OSC24M (1 << 2)
// TODO: determine which PLL is that
#define SUNXI_TIMER_CTL_SRC_PLL (2 << 3)
#define SUNXI_TIMER_CTL_DIV1 (0 << 4)
#define SUNXI_TIMER_CTL_DIV2 (1 << 4)
#define SUNXI_TIMER_CTL_DIV4 (2 << 4)
#define SUNXI_TIMER_CTL_DIV8 (3 << 4)
#define SUNXI_TIMER_CTL_DIV16 (4 << 4)
#define SUNXI_TIMER_CTL_DIV32 (5 << 4)
#define SUNXI_TIMER_CTL_DIV64 (6 << 4)
#define SUNXI_TIMER_CTL_DIV128 (7 << 4)
#define SUNXI_TIMER_CTL_ONESHOT (1 << 7)