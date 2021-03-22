/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Configuration settings for the Allwinner A10-evb board.
 */

#pragma once

#define SUNXI_WATCHDOG_BASE (FixedPcdGet32(WatchdogBase))

#define SUNXI_WATCHDOG_CTL 0x000
#define SUNXI_WATCHDOG_MODE 0x004

#define SUNXI_WATCHDOG_CTL_RESTART (1 << 0)
#define SUNXI_WATCHDOG_CTL_KEY (0xa57 << 1)

#define SUNXI_WATCHDOG_MODE_EN (1 << 0)
#define SUNXI_WATCHDOG_MODE_RESET_EN (1 << 1)
