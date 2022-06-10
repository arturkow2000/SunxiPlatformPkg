#include <PiPei.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/SunxiCcmLib.h>

#include <Ppi/SunxiCcm.h>

STATIC SUNXI_CCM_PPI *mSunxiCcmPpi = NULL;

EFI_STATUS
EFIAPI
SunxiCcmLibConstructor (
  IN       EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  EFI_STATUS Status;

  Status = PeiServicesLocatePpi(&gSunxiCcmPpiGuid, 0, NULL, (VOID**)&mSunxiCcmPpi);
  ASSERT_EFI_ERROR(Status);

  return Status;
}

EFI_STATUS SunxiCcmGetGate(CONST CHAR16 *Name, UINT32 *OutGateId) {
  return mSunxiCcmPpi->GetGate(mSunxiCcmPpi, Name, OutGateId);
}

EFI_STATUS SunxiCcmGateUnmask(UINT32 GateId) {
  return mSunxiCcmPpi->GateUnmask(mSunxiCcmPpi, GateId);
}

EFI_STATUS SunxiCcmGateMask(UINT32 GateId) {
  return mSunxiCcmPpi->GateMask(mSunxiCcmPpi, GateId);
}

EFI_STATUS SunxiCcmGateIsMasked(UINT32 GateId, BOOLEAN *OutIsMasked) {
  return mSunxiCcmPpi->GateIsMasked(mSunxiCcmPpi, GateId, OutIsMasked);
}

EFI_STATUS SunxiCcmGetPll(CONST CHAR16 *Name, UINT32 *OutPllId) {
  return mSunxiCcmPpi->GetPll(mSunxiCcmPpi, Name, OutPllId);
}

EFI_STATUS SunxiCcmPllGetFreq(UINT32 PllId, UINT32 *OutPllFreq) {
  return mSunxiCcmPpi->PllGetFreq(mSunxiCcmPpi, PllId, OutPllFreq);
}

EFI_STATUS SunxiCcmPllSetFreq(UINT32 PllId, UINT32 PllFreq) {
  return mSunxiCcmPpi->PllSetFreq(mSunxiCcmPpi, PllId, PllFreq);
}

