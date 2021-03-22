#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>

#include <Protocol/SunxiGpio.h>

#include "Driver.h"

// FIXME: move board specific config somewhere else
#if FixedPcdGet32(Mmc0Base) != 0
STATIC CONST SUNXI_GPIO_CONFIG mMmc0GpioConfig[] = {
  { L"PF00", L"mmc0", SUNXI_GPIO_PULL_UP, 30 },
  { L"PF01", L"mmc0", SUNXI_GPIO_PULL_UP, 30 },
  { L"PF02", L"mmc0", SUNXI_GPIO_PULL_UP, 30 },
  { L"PF03", L"mmc0", SUNXI_GPIO_PULL_UP, 30 },
  { L"PF04", L"mmc0", SUNXI_GPIO_PULL_UP, 30 },
  { L"PF05", L"mmc0", SUNXI_GPIO_PULL_UP, 30 },
  { NULL, NULL, 0, 0 },
};
#endif

#if FixedPcdGet32(Mmc1Base) != 0
// XW711 does not use MMC1
#error MMC1 not supported
#endif

#if FixedPcdGet32(Mmc2Base) != 0
STATIC CONST SUNXI_GPIO_CONFIG mMmc2GpioConfig[] = {
  { L"PC06", L"mmc2", SUNXI_GPIO_PULL_UP, 30 },
  { L"PC07", L"mmc2", SUNXI_GPIO_PULL_UP, 30 },
  { L"PC08", L"mmc2", SUNXI_GPIO_PULL_UP, 30 },
  { L"PC09", L"mmc2", SUNXI_GPIO_PULL_UP, 30 },
  { L"PC10", L"mmc2", SUNXI_GPIO_PULL_UP, 30 },
  { L"PC11", L"mmc2", SUNXI_GPIO_PULL_UP, 30 },
  { L"PC12", L"mmc2", SUNXI_GPIO_PULL_UP, 30 },
  { L"PC13", L"mmc2", SUNXI_GPIO_PULL_UP, 30 },
  { L"PC14", L"mmc2", SUNXI_GPIO_PULL_UP, 30 },
  { L"PC15", L"mmc2", SUNXI_GPIO_PULL_UP, 30 },
  { NULL, NULL, 0, 0 },
};
#endif

CONST SUNXI_MMC_CONFIG gMmcConfig[] = {
#if FixedPcdGet32(Mmc0Base) != 0
  {
    .GpioConfig = mMmc0GpioConfig,
    .Base = FixedPcdGet32(Mmc0Base),
    .MaxBusWidth = 4,
    .MinFreq = 400000, // 400 kHz
    .MaxFreq = 52000000, // 52 MHz
    .FifoRegOffset = 0x100,
    .SupportedVoltagesOcr = 0x300000,
    .IsInternal = FALSE,
    .AhbGate = L"ahb0-mmc0",
    .MmcClock = L"mmc0",
  },
#endif
#if FixedPcdGet32(Mmc1Base) != 0
// XW711 does not use MMC1
#error MMC1 not supported
#endif
#if FixedPcdGet32(Mmc2Base) != 0
  {
    .GpioConfig = mMmc2GpioConfig,
    .Base = FixedPcdGet32(Mmc2Base),
    .MaxBusWidth = 8,
    .MinFreq = 400000, // 400 kHz
    .MaxFreq = 52000000, // 52 MHz
    .FifoRegOffset = 0x100,
    .SupportedVoltagesOcr = 0x300000,
    .IsInternal = TRUE,
    .AhbGate = L"ahb0-mmc2",
    .MmcClock = L"mmc2",
  },
#endif
};

CONST UINT32 gNumMmcControllers = ARRAY_SIZE(gMmcConfig);