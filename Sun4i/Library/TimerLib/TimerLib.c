/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (c) Artur Kowalski. <arturkow2000@gmail.com> 

 Based on code from U-Boot

 (C) Copyright 2007-2011
 Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 Tom Cubie <tangliang@allwinnertech.com>
**/

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/TimerLib.h>
#include <Library/IoLib.h>

#include <Sunxi/HW/Timer.h>

#define DELAY_TIMER 1

EFI_STATUS
EFIAPI
SunxiTimerConstructor(VOID)
{
  if ((MmioRead32(SUNXI_TIMER_BASE + SUNXI_TIMER_CTL(DELAY_TIMER)) & SUNXI_TIMER_CTL_EN) == 0)
  {
    MmioWrite32(SUNXI_TIMER_BASE + SUNXI_TIMER_INTERVAL(DELAY_TIMER), 0xffffffff);
    MmioWrite32(
      SUNXI_TIMER_BASE + SUNXI_TIMER_CTL(DELAY_TIMER),
      SUNXI_TIMER_CTL_EN
        | SUNXI_TIMER_CTL_RELOAD
        | SUNXI_TIMER_CTL_SRC_OSC24M
        | SUNXI_TIMER_CTL_DIV1
    );
  }

  return EFI_SUCCESS;
}

/**
  Stalls the CPU for at least the given number of microseconds.

  Stalls the CPU for the number of microseconds specified by MicroSeconds.

  @param  MicroSeconds  The minimum number of microseconds to delay.

  @return The value of MicroSeconds inputted.

**/
UINTN
EFIAPI
MicroSecondDelay(
  IN UINTN MicroSeconds
  )
{
  UINTN LeftTicks;
  UINTN Last;
  UINTN Now;

  // timer runs at 24MHz which is 24 ticks per 1us
  LeftTicks = MicroSeconds * 24;
  Last = ~MmioRead32(SUNXI_TIMER_BASE + SUNXI_TIMER_VAL(DELAY_TIMER));

  while (LeftTicks > 0)
  {
    Now = ~MmioRead32(SUNXI_TIMER_BASE + SUNXI_TIMER_VAL(DELAY_TIMER));
    if (Now > Last)
    {
      // detect overflow
      if (LeftTicks - (Now - Last) > LeftTicks)
        LeftTicks = 0;
      else
        LeftTicks -= Now - Last;
    }
    else
    {
      // detect overflow
      if (LeftTicks - (0xffffffff - Last + Now))
        LeftTicks = 0;
      else
        LeftTicks -= 0xffffffff - Last + Now;
    }
    Last = Now;
  }

  return MicroSeconds;
}

/**
  Stalls the CPU for at least the given number of nanoseconds.

  Stalls the CPU for the number of nanoseconds specified by NanoSeconds.

  @param  NanoSeconds The minimum number of nanoseconds to delay.

  @return The value of NanoSeconds inputted.

**/
UINTN
EFIAPI
NanoSecondDelay(
  IN UINTN NanoSeconds
  )
{
  UINTN MicroSeconds;

  MicroSeconds = NanoSeconds / 1000;
  MicroSeconds += ((NanoSeconds % 1000) == 0) ? 0 : 1;

  MicroSecondDelay(MicroSeconds);

  return NanoSeconds;
}

/**
  Retrieves the current value of a 64-bit free running performance counter.

  The counter can either count up by 1 or count down by 1. If the physical
  performance counter counts by a larger increment, then the counter values
  must be translated. The properties of the counter can be retrieved from
  GetPerformanceCounterProperties().

  @return The current value of the free running performance counter.

**/
UINT64
EFIAPI
GetPerformanceCounter(VOID)
{
  return MmioRead32(SUNXI_TIMER_BASE + SUNXI_TIMER_VAL(DELAY_TIMER));
}

/**
  Retrieves the 64-bit frequency in Hz and the range of performance counter
  values.

  If StartValue is not NULL, then the value that the performance counter starts
  with immediately after is it rolls over is returned in StartValue. If
  EndValue is not NULL, then the value that the performance counter end with
  immediately before it rolls over is returned in EndValue. The 64-bit
  frequency of the performance counter in Hz is always returned. If StartValue
  is less than EndValue, then the performance counter counts up. If StartValue
  is greater than EndValue, then the performance counter counts down. For
  example, a 64-bit free running counter that counts up would have a StartValue
  of 0 and an EndValue of 0xFFFFFFFFFFFFFFFF. A 24-bit free running counter
  that counts down would have a StartValue of 0xFFFFFF and an EndValue of 0.

  @param  StartValue  The value the performance counter starts with when it
                      rolls over.
  @param  EndValue    The value that the performance counter ends with before
                      it rolls over.

  @return The frequency in Hz.

**/
UINT64
EFIAPI
GetPerformanceCounterProperties(
  OUT UINT64 *StartValue OPTIONAL,
  OUT UINT64 *EndValue OPTIONAL
  )
{
  if (StartValue)
    *StartValue = 0xffffffff;
  if (EndValue)
    *EndValue = 0;

  return 24000000;
}

/**
  Converts elapsed ticks of performance counter to time in nanoseconds.

  This function converts the elapsed ticks of running performance counter to
  time value in unit of nanoseconds.

  @param  Ticks     The number of elapsed ticks of running performance counter.

  @return The elapsed time in nanoseconds.

**/
UINT64
EFIAPI
GetTimeInNanoSecond(
  IN UINT64 Ticks
  )
{
  return (Ticks / 24) * 1000;
}