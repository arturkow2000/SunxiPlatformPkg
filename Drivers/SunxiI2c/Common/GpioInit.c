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

EFI_STATUS SunxiI2cInitGpio(
  IN I2C_DRIVER* Driver
  )
{
  EFI_STATUS Status;
  INT32 i;
  CONST GPIO_CONFIG *Config;
  UINT32 Pin;

  Status = EFI_SUCCESS;

  if (Driver->Config->GpioConfig == NULL)
    return EFI_SUCCESS;

  for (i = 0; i < Driver->Config->GpioConfigLength; i++) {
    Config = &Driver->Config->GpioConfig[i];

    Status = SunxiGpioGetPin(Config->Pin, &Pin);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "Failed to get pin %s: %r\n", Config->Pin, Status));
      ASSERT(0);
      goto Exit;
    }

    Status = SunxiGpioConfigureAsInput(Pin);
    if (EFI_ERROR(Status)) {
      ASSERT(0);
      goto Exit;
    }

    Status = SunxiGpioSetPullMode(Pin, Config->Pull);
    if (EFI_ERROR(Status)) {
      ASSERT(0);
      goto Exit;
    }

    Status = SunxiGpioSetDriveStrength(Pin, Config->Drive);
    if (EFI_ERROR(Status)) {
      ASSERT(0);
      goto Exit;
    }

    Status = SunxiGpioSetFunction(Pin, Config->Function);
    if (EFI_ERROR(Status)) {
      ASSERT(0);
      goto Exit;
    }
  }

Exit:
  return Status;
}