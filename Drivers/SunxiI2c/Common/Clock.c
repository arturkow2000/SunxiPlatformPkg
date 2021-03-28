/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on code Linux code
 Copyright (C) 2010-2015 Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 Pan Nan <pannan@allwinnertech.com>
 Tom Cubie <tanglaing@allwinnertech.com>
 Victor Wei <weiziheng@allwinnertech.com>

**/

#include "Driver.h"

EFI_STATUS
SunxiI2cSetBusClock(
  IN I2C_DRIVER *Driver,
  IN UINT32 RequestedFreq
  )
{
  EFI_STATUS Status;
  UINT32 Input;
  UINT32 Div;
  UINT32 n;
  UINT32 m;
  UINT32 p;
  UINT32 RealFreq;

  Status = GetPllFrequency(Driver, L"apb1", &Input);
  if (EFI_ERROR(Status))
    return Status;

  Div = Input / RequestedFreq;
  n = 0;
  m = 0;
  p = 1;

  while (n < 8) {
    m = (Div / p) - 1;
    while (m < 16) {
      RealFreq = Input / (m + 1) / p;
      if (RealFreq <= RequestedFreq)
        goto SetClock;
      else
        m += 1;
    }
    n += 1;
    p <<= 1;
  }
SetClock:
  DEBUG((EFI_D_INFO, "i2c%d set clock %u req %u n %u m %u input %u\n", Driver->Config->No, RealFreq, RequestedFreq, n, m, Input));

  MmioAndThenOr32(Driver->Config->Base + SUNXI_I2C_CLK, ~0x7F, (m << 3) | n);
  return EFI_SUCCESS;
}