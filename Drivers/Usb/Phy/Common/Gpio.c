#include "Driver.h"

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

EFI_STATUS UsbPhyInitGpio(USB_PHY_DRIVER *Driver, UINT32 Phy) {
  EFI_STATUS Status;

  if (Driver->SocConfig->GpioUsb0VbusSupply[Phy]) {
    Status = UsbPhyConfigureVbusSupply(Driver->SocConfig->GpioUsb0VbusSupply[Phy]);
    if (EFI_ERROR(Status))
      return Status;
  }

  if (Driver->SocConfig->GpioUsb0VbusDetect[Phy]) {
    Status = UsbPhyConfigureVbusDetect(Driver->SocConfig->GpioUsb0VbusDetect[Phy]);
    if (EFI_ERROR(Status))
      return Status;
  }

  if (Driver->SocConfig->GpioUsb0IdDetect[Phy]) {
    Status = UsbPhyConfigureIdDetect(Driver->SocConfig->GpioUsb0IdDetect[Phy]);
    if (EFI_ERROR(Status))
      return Status;
  }

  return EFI_SUCCESS;
}