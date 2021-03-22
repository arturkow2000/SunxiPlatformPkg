// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

#include "Spl.h"

UINT64 DramDetect(UINT64 *Base, UINT64 MaxSize)
{
  volatile UINT64 *Addr;
  UINT64 Save[63];
  UINT64 SaveBase;
  UINT64 Cnt;
  UINT64 Val;
  UINT64 Size;
  INT32 i;

  for (Cnt = (MaxSize / sizeof(UINT64)) >> 1; Cnt > 0; Cnt >>= 1)
  {
    Addr = Base + Cnt; /* pointer arith! */
    Save[i++] = *Addr;
    *Addr = ~Cnt;
  }

  Addr = Base;
  SaveBase = *Addr;
  *Addr = 0;

  if ((Val = *Addr) != 0)
  {
    /* Restore the original data before leaving the function. */
    *Base = SaveBase;
    for (Cnt = 1; Cnt < MaxSize / sizeof(UINT64); Cnt <<= 1)
    {
      Addr = Base + Cnt;
      *Addr = Save[--i];
    }
    return (0);
  }

  for (Cnt = 1; Cnt < MaxSize / sizeof(UINT64); Cnt <<= 1)
  {
    Addr = Base + Cnt; /* pointer arith! */
    Val = *Addr;
    *Addr = Save[--i];
    if (Val != ~Cnt)
    {
      Size = Cnt * sizeof(UINT64);
      /*
       * Restore the original data
       * before leaving the function.
       */
      for (Cnt <<= 1;
           Cnt < MaxSize / sizeof(long);
           Cnt <<= 1)
      {
        Addr = Base + Cnt;
        *Addr = Save[--i];
      }
      /* warning: don't restore save_base in this case,
       * it is already done in the loop because
       * base and base+size share the same physical memory
       * and *base is saved after *(base+size) modification
       * in first loop
       */
      return (Size);
    }
  }

  *Base = SaveBase;
  return MaxSize;
}