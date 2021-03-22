/**
 SPDX-License-Identifier: GPL-2.0-only

 Copyright (c) 2021 Artur Kowalski. <arturkow2000@gmail.com>

 Based on:
  - EDKII ARM Generic timer driver
    Copyright (c) 2011-2013 ARM Ltd. All rights reserved.<BR>

  - Linux sun4i clocksource driver
    Copyright (C) 2012 Maxime Ripard <maxime.ripard@free-electrons.com>
    Allwinner Technology Co., Ltd. <www.allwinnertech.com>
    Benn Huang <benn@allwinnertech.com>

**/

#pragma once

#include <Sunxi/HW/Timer.h>

#define INTERRUPT_TIMER 0
#define TIMER_IRQ_VECTOR 22

VOID Sun4iTimerSync(VOID);
VOID Sun4iTimerStop(VOID);
UINT32 Sun4iTimerGetCounter(VOID);
VOID Sun4iTimerStart(UINT32 Interval);
BOOLEAN Sun4iTimerGetIsInterruptActive(VOID);
VOID Sun4iTimerClearInterruptStatus(VOID);