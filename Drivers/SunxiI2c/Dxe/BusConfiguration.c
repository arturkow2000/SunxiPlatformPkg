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

STATIC
EFI_STATUS
EFIAPI
EnableConfiguration(
  IN CONST EFI_I2C_BUS_CONFIGURATION_MANAGEMENT_PROTOCOL *This,
  IN UINTN                                               I2cBusConfiguration,
  IN EFI_EVENT                                           Event      OPTIONAL,
  IN EFI_STATUS                                          *I2cStatus OPTIONAL
  )
{
  EFI_STATUS Status;

  // There is only one bus confiuguration
  if (I2cBusConfiguration == 0)
    Status = EFI_SUCCESS;
  else
    Status = EFI_NO_MAPPING;

  if (I2cStatus)
    *I2cStatus = Status;
  
  if (Event)
    gBS->SignalEvent(Event);

  return Status;
}

EFI_I2C_BUS_CONFIGURATION_MANAGEMENT_PROTOCOL gSunxiI2cBusConfigurationProto = {
  EnableConfiguration
};
