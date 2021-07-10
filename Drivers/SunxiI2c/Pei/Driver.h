/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on code Linux code
 Copyright (C) 2010-2015 Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 Pan Nan <pannan@allwinnertech.com>
 Tom Cubie <tanglaing@allwinnertech.com>
 Victor Wei <weiziheng@allwinnertech.com>

**/

#include <PiPei.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>

#include <Ppi/SunxiCcm.h>
#include <Ppi/I2cMaster.h>

#include "../Common/Driver.h"

extern SUNXI_CCM_PPI *gSunxiCcmPpi;
extern EFI_PEI_I2C_MASTER_PPI gI2cMasterPpi;

#define PEI_DRIVER_SIGNATURE SIGNATURE_32('P', 'i', '2', 'c')

#define PEI_DRIVER_FROM_COMMON_DRIVER(x) \
  CR(x, I2C_PEI_DRIVER, Common, PEI_DRIVER_SIGNATURE)

#define PEI_DRIVER_FROM_MASTER(x) \
  CR(x, I2C_PEI_DRIVER, MasterPpi, PEI_DRIVER_SIGNATURE)

typedef struct _I2C_PEI_DRIVER {
  UINT32 Signature;
  EFI_PEI_I2C_MASTER_PPI MasterPpi;
  // This must follow MasterPpi
  // I2C enumerator driver uses this
  UINT8 ControllerNo;
  I2C_DRIVER Common;
  BOOLEAN Initialized;
} I2C_PEI_DRIVER;

EFI_STATUS
EFIAPI
SunxiI2cExecute(
  IN I2C_PEI_DRIVER *Driver,
  IN UINTN SlaveAddress,
  IN EFI_I2C_REQUEST_PACKET *RequestPacket
);

