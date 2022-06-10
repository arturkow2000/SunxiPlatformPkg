#include "Phy.h"

STATIC EFI_STATUS UsbPhyConfigureVbusSupply(CONST CHAR16 *PinName) {
  EFI_STATUS Status;
  UINT32 Pin;

  ASSERT(PinName);

  Status = SunxiGpioGetPin(PinName, &Pin);
  if (EFI_ERROR(Status))
    return Status;

  Status = SunxiGpioSetLevel(Pin, 0);
  if (EFI_ERROR(Status))
    return Status;

  return SunxiGpioConfigureAsOutput(Pin);
}

STATIC EFI_STATUS UsbPhyConfigureVbusDetect(CONST CHAR16 *PinName) {
  EFI_STATUS Status;
  UINT32 Pin;

  ASSERT(PinName);

  Status = SunxiGpioGetPin(PinName, &Pin);
  if (EFI_ERROR(Status))
    return Status;

  return SunxiGpioConfigureAsInput(Pin);
}

STATIC EFI_STATUS UsbPhyConfigureIdDetect(CONST CHAR16 *PinName) {
  EFI_STATUS Status;
  UINT32 Pin;

  ASSERT(PinName);

  Status = SunxiGpioGetPin(PinName, &Pin);
  if (EFI_ERROR(Status))
    return Status;

  Status = SunxiGpioConfigureAsInput(Pin);
  if (EFI_ERROR(Status))
    return Status;

  return SunxiGpioSetPullMode(Pin, SUNXI_GPIO_PULL_UP);
}

EFI_STATUS UsbPhyInitGpio(UINT32 Phy) {
  CONST USB_PHY_SOC_CONFIG *Config = &gUsbPhySocConfig;
  EFI_STATUS Status;

  if (Config->GpioUsb0VbusSupply[Phy]) {
    Status = UsbPhyConfigureVbusSupply(Config->GpioUsb0VbusSupply[Phy]);
    if (EFI_ERROR(Status))
      return Status;
  }

  if (Config->GpioUsb0VbusDetect[Phy]) {
    Status = UsbPhyConfigureVbusDetect(Config->GpioUsb0VbusDetect[Phy]);
    if (EFI_ERROR(Status))
      return Status;
  }

  if (Config->GpioUsb0IdDetect[Phy]) {
    Status = UsbPhyConfigureIdDetect(Config->GpioUsb0IdDetect[Phy]);
    if (EFI_ERROR(Status))
      return Status;
  }

  return EFI_SUCCESS;
}