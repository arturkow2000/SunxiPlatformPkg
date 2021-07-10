#include "Driver.h"

EFI_STATUS UsbPhyInitGpio(USB_PHY_DRIVER *Driver) {
  EFI_STATUS Status;
  SUNXI_GPIO_PIN Pin;
  UINT32 i;

  for (i = 0; i < Driver->SocConfig->NumPhys; i++) {
      if (Driver->SocConfig->MissingPhys & (1 << i))
        continue;

    if (Driver->SocConfig->GpioUsb0IdDetect[i]) {
      Status = gSunxiGpioPpi->GetPin(gSunxiGpioPpi, Driver->SocConfig->GpioUsb0IdDetect[i], &Pin);
      if (EFI_ERROR(Status)) {
        DEBUG((EFI_D_ERROR, "%s: %r\n", Driver->SocConfig->GpioUsb0IdDetect[i], Status));
        return Status;
      }

      Status = gSunxiGpioPpi->SetFunction(gSunxiGpioPpi, Pin, L"gpio_in");
      if (EFI_ERROR(Status)) {
        DEBUG((EFI_D_ERROR, "failed to set %s to gpio_in: %r\n", Driver->SocConfig->GpioUsb0IdDetect[i], Status));
        return Status;
      }

      Status = gSunxiGpioPpi->SetPullMode(gSunxiGpioPpi, Pin, SUNXI_GPIO_PULL_UP);
      if (EFI_ERROR(Status)) {
        DEBUG((EFI_D_ERROR, "failed to pullup %s: %r\n", Driver->SocConfig->GpioUsb0IdDetect[i], Status));
        return Status;
      }
    }

    if (Driver->SocConfig->GpioUsb0VbusDetect[i]) {
      Status = gSunxiGpioPpi->GetPin(gSunxiGpioPpi, Driver->SocConfig->GpioUsb0VbusDetect[i], &Pin);
      ASSERT_EFI_ERROR(Status);
    }
  }
}