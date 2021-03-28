#include "Driver.h"

EFI_STATUS SunxiMmcInitGpio(
  IN SUNXI_MMC* Mmc
  )
{
  EFI_STATUS Status;
  INT32 i;
  CONST SUNXI_GPIO_CONFIG *Config;
  SUNXI_GPIO_PIN Pin;

  Status = EFI_SUCCESS;

  if (Mmc->Config->GpioConfig == NULL)
    return EFI_SUCCESS;

  for (i = 0; Mmc->Config->GpioConfig[i].Pin != NULL; i++) {
    Config = &Mmc->Config->GpioConfig[i];

    Status = gSunxiGpioProtocol->GetPin(gSunxiGpioProtocol, Config->Pin, &Pin);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "Failed to get pin %s: %r\n", Config->Pin, Status));
      ASSERT(0);
      goto Exit;
    }

    Status = gSunxiGpioProtocol->SetFunction(gSunxiGpioProtocol, Pin, L"gpio_in");
    if (EFI_ERROR(Status)) {
      ASSERT(0);
      goto Exit;
    }

    Status = gSunxiGpioProtocol->SetPullMode(gSunxiGpioProtocol, Pin, Config->Pull);
    if (EFI_ERROR(Status)) {
      ASSERT(0);
      goto Exit;
    }

    Status = gSunxiGpioProtocol->SetDriveStrength(gSunxiGpioProtocol, Pin, Config->DriveStrength);
    if (EFI_ERROR(Status)) {
      ASSERT(0);
      goto Exit;
    }

    Status = gSunxiGpioProtocol->SetFunction(gSunxiGpioProtocol, Pin, Config->Function);
    if (EFI_ERROR(Status)) {
      ASSERT(0);
      goto Exit;
    }
  }

  Exit:
  return Status;
}
