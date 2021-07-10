#pragma once

#include <PiPei.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>

#include <Pi/PiI2c.h>
#include <Ppi/I2cEnumerate.h>
#include <Ppi/I2cMaster.h>

#include "../Common/Driver.h"

typedef struct _AXP209_PACKET {
  UINTN OperationCount;
  EFI_I2C_OPERATION Operation[2];
} AXP209_PACKET;

#define AXP209_COMMON_DRIVER_TO_DXE_DRIVER(x)  \
  BASE_CR(x, AXP209_PEI_DRIVER, Common)

typedef struct {
  AXP209_DRIVER Common;
  CONST EFI_I2C_DEVICE *Device;
  EFI_PEI_I2C_MASTER_PPI *Master;
} AXP209_PEI_DRIVER;
