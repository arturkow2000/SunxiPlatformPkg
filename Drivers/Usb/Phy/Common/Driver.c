#include "Driver.h"

EFI_STATUS UsbPhyInit(USB_PHY_DRIVER *Driver) {
  EFI_STATUS Status;
  UINT32 i;

  for (i = 0; i < Driver->SocConfig->NumPhys; i++) {
    if (Driver->SocConfig->MissingPhys & (1 << i))
      continue;

    Status = UsbPhyInitGpio(Driver, i);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "phy%u GPIO init failed\n", i));
      return Status;
    }

    Status = UsbPhyInitClocks(Driver, i);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "phy%u clocks init failed\n", i));
      return Status;
    }

    Status = UsbPhyInitHw(Driver, i);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "phy%u init failed\n", i));
      return Status;
    }
  }

  return EFI_SUCCESS;
}