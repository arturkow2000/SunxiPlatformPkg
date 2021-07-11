#include "Driver.h"

EFI_STATUS UsbPhyUnmaskGate(USB_PHY_DRIVER *Driver, CONST CHAR16 *GateName) {
  EFI_STATUS Status;
  UINT32 Gate;

  Status = gSunxiCcmPpi->GetGate(gSunxiCcmPpi, GateName, &Gate);
  if (EFI_ERROR(Status))
    return Status;

  return gSunxiCcmPpi->GateUnmask(gSunxiCcmPpi, Gate);
}