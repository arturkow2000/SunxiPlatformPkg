#pragma once

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

#include <Protocol/DriverBinding.h>
#include <Protocol/ComponentName.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/SdMmcPassThru.h>
#include <Protocol/SunxiGpio.h>
#include <Protocol/SunxiCcm.h>

#include "../Common/Driver.h"

extern EFI_COMPONENT_NAME_PROTOCOL gComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL gComponentName2;
extern EFI_DRIVER_BINDING_PROTOCOL gSunxiMmcDriverBinding;
extern EFI_SD_MMC_PASS_THRU_PROTOCOL gSunxiMmcProtocol;

extern SUNXI_GPIO_PROTOCOL *gSunxiGpioProtocol;
extern SUNXI_CCM_PROTOCOL *gSunxiCcmProtocol;

typedef struct _SUNXI_MMC_DRIVER {
  EFI_SD_MMC_PASS_THRU_PROTOCOL Protocol;
  SUNXI_MMC Mmc;
} SUNXI_MMC_DRIVER;

EFI_STATUS SunxiMmcCreateDriver(
  IN CONST SUNXI_MMC_CONFIG *Config,
  OUT SUNXI_MMC_DRIVER **Driver
);

EFI_STATUS SunxiMmcDestroyDriver(
  IN SUNXI_MMC_DRIVER *Driver
);
