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

EFI_STATUS GetPllFrequency(IN I2C_DRIVER *Driver, IN CONST CHAR16 *PllName, OUT UINT32 *Freq)
{
  EFI_STATUS Status;
  I2C_DXE_DRIVER *DxeDriver;
  UINT32 PllId;

  if (Freq == NULL)
    return EFI_INVALID_PARAMETER;

  DxeDriver = DXE_DRIVER_FROM_COMMON_DRIVER(Driver);
  Status = gSunxiCcmProtocol->GetPll(gSunxiCcmProtocol, PllName, &PllId);
  if (EFI_ERROR(Status))
    return Status;

  return gSunxiCcmProtocol->PllGetFreq(gSunxiCcmProtocol, PllId, Freq);
}
