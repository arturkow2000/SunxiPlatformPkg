#include "Driver.h"

EFI_STATUS SunxiMmcUnmaskGate(
  IN SUNXI_MMC *Mmc,
  IN CONST CHAR16 *Gate
  )
{
  EFI_STATUS Status;
  UINT32 GateId;
  
  Status = gSunxiCcmProtocol->GetGate(gSunxiCcmProtocol, Gate, &GateId);
  if (EFI_ERROR(Status))
    goto Exit;

  Status = gSunxiCcmProtocol->GateUnmask(gSunxiCcmProtocol, GateId);
  if (EFI_ERROR(Status))
    goto Exit;

  Exit:
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to ungate %s: %r\n", Gate, Status));
    ASSERT_EFI_ERROR(Status);
  }

  return Status;
}

EFI_STATUS SunxiMmcMaskGate(
  IN SUNXI_MMC *Mmc,
  IN CONST CHAR16 *Gate
  )
{
  EFI_STATUS Status;
  UINT32 GateId;
  
  Status = gSunxiCcmProtocol->GetGate(gSunxiCcmProtocol, Gate, &GateId);
  if (EFI_ERROR(Status))
    goto Exit;

  Status = gSunxiCcmProtocol->GateMask(gSunxiCcmProtocol, GateId);
  if (EFI_ERROR(Status))
    goto Exit;

  Exit:
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to ungate %s: %r\n", Gate, Status));
    ASSERT_EFI_ERROR(Status);
  }

  return Status;
}

EFI_STATUS
SunxiMmcSetModClock(
  IN SUNXI_MMC *Mmc,
  IN UINT32 Hz
  )
{
  EFI_STATUS Status;
  UINT32 PllId;
  
  Status = gSunxiCcmProtocol->GetPll(gSunxiCcmProtocol, Mmc->Config->MmcClock, &PllId);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to get %s: %r\n", Mmc->Config->MmcClock, Status));
    ASSERT(0);
    return Status;
  }

  Status = gSunxiCcmProtocol->PllSetFreq(gSunxiCcmProtocol, PllId, Hz);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to set %s to %u\n", Mmc->Config->MmcClock, Hz));
    return Status;
  }

  Mmc->State.ClockHz = Hz;
  return EFI_SUCCESS;
}