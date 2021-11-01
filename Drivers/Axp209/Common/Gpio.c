#include "Driver.h"

STATIC EFI_STATUS AxpGpioGetReg(UINT32 Pin, UINT8 *OutReg) {
  switch (Pin) {
  case 0: *OutReg = AXP209_GPIO0_CTL; break;
  case 1: *OutReg = AXP209_GPIO1_CTL; break;
  case 2: *OutReg = AXP209_GPIO2_CTL; break;
  default: return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

EFI_STATUS AxpGpioOutput(
  IN AXP209_DRIVER *Driver,
  IN UINT32 Pin,
  IN UINT32 Value
) {
  EFI_STATUS Status;
  UINT8 Reg;

  Status = AxpGpioGetReg(Pin, &Reg);
  if (EFI_ERROR(Status))
    return Status;

  return AxpWrite8(Driver, Reg, !!(Value));
}

EFI_STATUS AxpGpioInput(
  IN AXP209_DRIVER *Driver,
  IN UINT32 Pin
) {
  EFI_STATUS Status;
  UINT8 Reg;

  Status = AxpGpioGetReg(Pin, &Reg);
  if (EFI_ERROR(Status))
    return Status;

  return AxpWrite8(Driver, Reg, AXP209_GPIO_CTL_INPUT);
}
