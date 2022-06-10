#include "Phy.h"

EFI_STATUS UsbPhyUnmashGate(CONST CHAR16 *Name) {
  EFI_STATUS Status;
  UINT32 Id;

  Status = SunxiCcmGetGate(Name, &Id);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;
  
  Status = SunxiCcmGateUnmask(Id);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  return EFI_SUCCESS;
}

EFI_STATUS UsbPhyInitClocks(UINT32 Phy) {
  CONST USB_PHY_SOC_CONFIG *Config = &gUsbPhySocConfig;
  EFI_STATUS Status;

  if (Config->DedicatedClocks) {
    ASSERT(Config->CcmClocks);
    ASSERT(Config->CcmClocks[Phy]);
    Status = UsbPhyUnmashGate(Config->CcmClocks[Phy]);
  } else {
    ASSERT(Config->CcmClock);
    Status = UsbPhyUnmashGate(Config->CcmClock);
  }
  if (EFI_ERROR(Status))
    return Status;

  if (Config->CcmReset && Config->CcmReset[Phy]) {
    // TODO: support deasserting reset pins for platform that need this
    ASSERT(0);
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}
