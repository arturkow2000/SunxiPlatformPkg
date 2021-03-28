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

EFI_STATUS SunxiI2cStart(IN I2C_DRIVER *Driver, BOOLEAN Restart) {
  UINT16 Timeout;

  Timeout = 0xffff;
  MmioOr32(Driver->Config->Base + SUNXI_I2C_CTL, SUNXI_I2C_CTL_STA);
  if (Restart)
    MmioAnd32(Driver->Config->Base + SUNXI_I2C_CTL, ~SUNXI_I2C_CTL_INTFLG);

  while (Timeout > 0) {
    if ((MmioRead32(Driver->Config->Base + SUNXI_I2C_CTL) & SUNXI_I2C_CTL_STA) == 0)
      return EFI_SUCCESS;

    Timeout -= 1;
  }

  return EFI_DEVICE_ERROR;
}

EFI_STATUS SunxiI2cStop(IN I2C_DRIVER *Driver) {
  UINT16 Timeout;
  
  Timeout = 0xffff;
  MmioAndThenOr32(Driver->Config->Base + SUNXI_I2C_CTL, ~SUNXI_I2C_CTL_INTFLG, SUNXI_I2C_CTL_STOP);

  while (Timeout > 0) {
    if ((MmioRead32(Driver->Config->Base + SUNXI_I2C_CTL) & SUNXI_I2C_CTL_STOP) == 0)
      break;
    Timeout -= 1;
  }

  if (Timeout == 0)
    return EFI_DEVICE_ERROR;

  Timeout = 0xffff;
  while (Timeout > 0) {
    if (MmioRead32(Driver->Config->Base + SUNXI_I2C_STA) == SUNXI_I2C_STA_IDLE)
      break;
    Timeout -= 1;
  }

  if (Timeout == 0)
    return EFI_DEVICE_ERROR;

  Timeout = 0xffff;
  while (Timeout > 0) {
    if (MmioRead32(Driver->Config->Base + SUNXI_I2C_LCR) == SUNXI_I2C_LCR_IDLE)
      return EFI_SUCCESS;
    Timeout -= 1;
  }

  return EFI_DEVICE_ERROR;
}

BOOLEAN SunxiI2cPoll(IN I2C_DRIVER *Driver, OUT UINT32 *OutStatus) {
  ASSERT(Driver);
  ASSERT(OutStatus);

  if (!(MmioRead32(Driver->Config->Base + SUNXI_I2C_CTL) & SUNXI_I2C_CTL_INTFLG))
    return FALSE;

  *OutStatus = MmioRead32(Driver->Config->Base + SUNXI_I2C_STA) & 0xff;

  return TRUE;
}

VOID SunxiI2cWriteByte(IN I2C_DRIVER *Driver, IN UINT8 Byte) {
  MmioWrite32(Driver->Config->Base + SUNXI_I2C_DATA, Byte);
  // Clear interrupt flag to trigger transfer
  MmioAnd32(Driver->Config->Base + SUNXI_I2C_CTL, ~SUNXI_I2C_CTL_INTFLG);
}