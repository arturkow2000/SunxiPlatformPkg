// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2021, Artur Kowalski.
 * 
 * Based on code from U-Boot
 * 
 * (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 */

#include "Spl.h"

#define TIMER_BASE (FixedPcdGet32(TimerBase))
#define TIMER_CTL_OFFSET 16
#define TIMER_INTER_OFFSET 20
#define TIMER_VAL_OFFSET 24

#define TIMER_MODE (0x0 << 7)   /* continuous mode */
#define TIMER_DIV (0x0 << 4)    /* pre scale 1 */
#define TIMER_SRC (0x1 << 2)    /* osc24m */
#define TIMER_RELOAD (0x1 << 1) /* reload internal value */
#define TIMER_EN (0x1 << 0)     /* enable timer */

#define TIMER_CLOCK 240000000
#define TIMER_LOAD_VAL 0xffffffff

VOID SplTimerInit(VOID)
{
  MmioWrite32(TIMER_BASE + TIMER_INTER_OFFSET, TIMER_LOAD_VAL);
  MmioWrite32(TIMER_BASE + TIMER_CTL_OFFSET, TIMER_MODE | TIMER_DIV | TIMER_SRC | TIMER_RELOAD | TIMER_EN);
}

UINT32 SplTimerRead(VOID)
{
  /*
   * The hardware timer counts down, therefore we invert to
   * produce an incrementing timer.
   */
  return 0xffffffff - MmioRead32(TIMER_BASE + TIMER_VAL_OFFSET);
}

VOID SplUdelay(UINT64 MicroSeconds)
{
  INT64 Timeout;
  UINT32 Now;
  UINT32 Last;

  Timeout = MultU64x32(MicroSeconds, 24);
  Last = SplTimerRead();

  while (Timeout > 0)
  {
    Now = SplTimerRead();

    if (Now > Last)
      Timeout -= Now - Last;
    else
      Timeout -= TIMER_LOAD_VAL - Last + Now;

    Last = Now;
  }
}