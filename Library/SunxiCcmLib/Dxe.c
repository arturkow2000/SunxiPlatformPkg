#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/SunxiCcmLib.h>

#include <Protocol/SunxiCcm.h>

STATIC SUNXI_CCM_PROTOCOL *mSunxiCcmProtocol = NULL;

EFI_STATUS
EFIAPI
SunxiCcmLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;

  Status = gBS->LocateProtocol(&gSunxiCcmProtocolGuid, NULL, (VOID**)&mSunxiCcmProtocol);
  ASSERT_EFI_ERROR(Status);

  return Status;
}

EFI_STATUS SunxiCcmGetGate(CONST CHAR16 *Name, UINT32 *OutGateId) {
  return mSunxiCcmProtocol->GetGate(mSunxiCcmProtocol, Name, OutGateId);
}

EFI_STATUS SunxiCcmGateUnmask(UINT32 GateId) {
  return mSunxiCcmProtocol->GateUnmask(mSunxiCcmProtocol, GateId);
}

EFI_STATUS SunxiCcmGateMask(UINT32 GateId) {
  return mSunxiCcmProtocol->GateMask(mSunxiCcmProtocol, GateId);
}

EFI_STATUS SunxiCcmGateIsMasked(UINT32 GateId, BOOLEAN *OutIsMasked) {
  return mSunxiCcmProtocol->GateIsMasked(mSunxiCcmProtocol, GateId, OutIsMasked);
}

EFI_STATUS SunxiCcmGetPll(CONST CHAR16 *Name, UINT32 *OutPllId) {
  return mSunxiCcmProtocol->GetPll(mSunxiCcmProtocol, Name, OutPllId);
}

EFI_STATUS SunxiCcmPllGetFreq(UINT32 PllId, UINT32 *OutPllFreq) {
  return mSunxiCcmProtocol->PllGetFreq(mSunxiCcmProtocol, PllId, OutPllFreq);
}

EFI_STATUS SunxiCcmPllSetFreq(UINT32 PllId, UINT32 PllFreq) {
  return mSunxiCcmProtocol->PllSetFreq(mSunxiCcmProtocol, PllId, PllFreq);
}

