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

EFI_STATUS SunxiI2cInit(IN I2C_DRIVER *Driver) {
  EFI_STATUS Status;
  CHAR16 ApbGate[10];

  ASSERT(Driver);

  CopyMem(ApbGate, L"apb1-i2c", 8 * sizeof(CHAR16));
  ApbGate[8] = Driver->Config->No + L'0';
  ApbGate[9] = 0;

  Status = GateUnmask(Driver, ApbGate);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = SunxiI2cInitGpio(Driver);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  MmioWrite32(Driver->Config->Base + SUNXI_I2C_SWRST, SUNXI_I2C_SWRST_RESET);
  MicroSecondDelay(100);

  if (Driver->Config->No == 0) {
    Status = SunxiI2cSetBusClock(Driver, 400000); // 400Khz
    Driver->BusClockHz = 400000;
  }
  else {
    Status = SunxiI2cSetBusClock(Driver, 100000); // 100Khz
    Driver->BusClockHz = 100000;
  }
  ASSERT_EFI_ERROR(Status);

  MmioOr32(Driver->Config->Base + SUNXI_I2C_CTL, SUNXI_I2C_CTL_BUSEN);

  return Status;
}

EFI_STATUS SunxiI2cDeInit(IN I2C_DRIVER *Driver) {
  EFI_STATUS Status;
  CHAR16 ApbGate[10];

  ASSERT(Driver);

  // Disable I2C bus clock
  MmioAnd32(Driver->Config->Base + SUNXI_I2C_CTL, ~SUNXI_I2C_CTL_BUSEN);

  // mask APB gate
  CopyMem(ApbGate, L"apb1-i2c", 8 * sizeof(CHAR16));
  ApbGate[8] = Driver->Config->No + L'0';
  ApbGate[9] = 0;

  Status = GateMask(Driver, ApbGate);
  ASSERT_EFI_ERROR(Status);

  return Status;
}
