#pragma once

#include "../Common/Driver.h"

extern SUNXI_GPIO_PROTOCOL *gSunxiGpioProtocol;

typedef struct _SUNXI_MMC_DRIVER {
  EFI_SD_MMC_PASS_THRU_PROTOCOL Protocol;
  SUNXI_MMC Mmc;
} SUNXI_MMC_DRIVER;

EFI_STATUS SunxiMmcCreateDriver(
  IN CONST SUNXI_MMC_CONFIG *Config,
  OUT SUNXI_MMC_DRIVER **Driver
);

VOID SunxiMmcDestroyDriver(
  IN SUNXI_MMC_DRIVER *Driver
);
