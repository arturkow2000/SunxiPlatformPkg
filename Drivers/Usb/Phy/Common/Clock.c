#include"Driver.h"

EFI_STATUS UsbPhyInitClocks(USB_PHY_DRIVER *Driver, UINT32 Phy) {
  EFI_STATUS Status;

  if (Driver->SocConfig->DedicatedClocks) {
    ASSERT(Driver->SocConfig->CcmClocks);
    ASSERT(Driver->SocConfig->CcmClocks[Phy]);
    Status = UsbPhyUnmaskGate(Driver, Driver->SocConfig->CcmClocks[Phy]);
  } else {
    ASSERT(Driver->SocConfig->CcmClock);
    Status = UsbPhyUnmaskGate(Driver, Driver->SocConfig->CcmClock);
  }
  if (EFI_ERROR(Status))
    return Status;

  if (Driver->SocConfig->CcmReset && Driver->SocConfig->CcmReset[Phy]) {
    // TODO: support deasserting reset pins for platform that need this
    ASSERT(0);
    return EFI_UNSUPPORTED;
  }
  
  return EFI_SUCCESS;
}
