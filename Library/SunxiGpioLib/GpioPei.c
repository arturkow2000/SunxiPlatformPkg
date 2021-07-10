#include <PiPei.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>

#include <Ppi/SunxiGpio.h>

STATIC SUNXI_GPIO_PPI *mSunxiGpioPpi = NULL;

EFI_STATUS
EFIAPI
SunxiGpioLibConstructor (
  IN       EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  EFI_STATUS Status;

  Status = PeiServicesLocatePpi(&gSunxiGpioPpiGuid, 0, NULL, (VOID**)&mSunxiGpioPpi);
  ASSERT_EFI_ERROR(Status);

  return Status;
}

EFI_STATUS SunxiGpioGetPin(IN CONST CHAR16 *Name, OUT UINT32 *OutPin) {
  return mSunxiGpioPpi->GetPin(mSunxiGpioPpi, Name, OutPin);
}

EFI_STATUS SunxiGpioGetFunction(IN UINT32 Pin, OUT CONST CHAR16 **OutFunction) {
  return mSunxiGpioPpi->GetFunction(mSunxiGpioPpi, Pin, OutFunction);
}

EFI_STATUS SunxiGpioSetFunction(IN UINT32 Pin, IN CONST CHAR16 *Function) {
  return mSunxiGpioPpi->SetFunction(mSunxiGpioPpi, Pin, Function);
}

EFI_STATUS SunxiGpioGetPullMode(IN UINT32 Pin, OUT UINT32 *OutPullMode) {
  return mSunxiGpioPpi->GetPullMode(mSunxiGpioPpi, Pin, OutPullMode);
}

EFI_STATUS SunxiGpioSetPullMode(IN UINT32 Pin, IN UINT32 PullMode) {
  return mSunxiGpioPpi->SetPullMode(mSunxiGpioPpi, Pin, PullMode);
}

EFI_STATUS SunxiGpioGetDriveStrength(IN UINT32 Pin, OUT UINT32 *OutStrength) {
  return mSunxiGpioPpi->GetDriveStrength(mSunxiGpioPpi, Pin, OutStrength);
}

EFI_STATUS SunxiGpioSetDriveStrength(IN UINT32 Pin, IN UINT32 Strength) {
  return mSunxiGpioPpi->SetDriveStrength(mSunxiGpioPpi, Pin, Strength);
}

EFI_STATUS SunxiGpioConfigureAsOutput(IN UINT32 Pin) {
  return mSunxiGpioPpi->ConfigureAsOutput(mSunxiGpioPpi, Pin);
}

EFI_STATUS SunxiGpioConfigureAsInput(IN UINT32 Pin) {
  return mSunxiGpioPpi->ConfigureAsInput(mSunxiGpioPpi, Pin);
}

EFI_STATUS SunxiGpioSetLevel(IN UINT32 Pin, IN UINT8 Level) {
  return mSunxiGpioPpi->SetLevel(mSunxiGpioPpi, Pin, Level);
}

EFI_STATUS SunxiGpioGetLevel(IN UINT32 Pin, OUT UINT8 *OutLevel) {
  return mSunxiGpioPpi->GetLevel(mSunxiGpioPpi, Pin, OutLevel);
}
