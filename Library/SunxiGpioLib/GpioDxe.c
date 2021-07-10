#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Protocol/SunxiGpio.h>

STATIC SUNXI_GPIO_PROTOCOL *mSunxiGpioProtocol = NULL;

EFI_STATUS
EFIAPI
SunxiGpioLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;

  Status = gBS->LocateProtocol(&gSunxiGpioProtocolGuid, NULL, (VOID**)&mSunxiGpioProtocol);
  ASSERT_EFI_ERROR(Status);

  return Status;
}

EFI_STATUS SunxiGpioGetPin(IN CONST CHAR16 *Name, OUT UINT32 *OutPin) {
  return mSunxiGpioProtocol->GetPin(mSunxiGpioProtocol, Name, OutPin);
}

EFI_STATUS SunxiGpioGetFunction(IN UINT32 Pin, OUT CONST CHAR16 **OutFunction) {
  return mSunxiGpioProtocol->GetFunction(mSunxiGpioProtocol, Pin, OutFunction);
}

EFI_STATUS SunxiGpioSetFunction(IN UINT32 Pin, IN CONST CHAR16 *Function) {
  return mSunxiGpioProtocol->SetFunction(mSunxiGpioProtocol, Pin, Function);
}

EFI_STATUS SunxiGpioGetPullMode(IN UINT32 Pin, OUT UINT32 *OutPullMode) {
  return mSunxiGpioProtocol->GetPullMode(mSunxiGpioProtocol, Pin, OutPullMode);
}

EFI_STATUS SunxiGpioSetPullMode(IN UINT32 Pin, IN UINT32 PullMode) {
  return mSunxiGpioProtocol->SetPullMode(mSunxiGpioProtocol, Pin, PullMode);
}

EFI_STATUS SunxiGpioGetDriveStrength(IN UINT32 Pin, OUT UINT32 *OutStrength) {
  return mSunxiGpioProtocol->GetDriveStrength(mSunxiGpioProtocol, Pin, OutStrength);
}

EFI_STATUS SunxiGpioSetDriveStrength(IN UINT32 Pin, IN UINT32 Strength) {
  return mSunxiGpioProtocol->SetDriveStrength(mSunxiGpioProtocol, Pin, Strength);
}

EFI_STATUS SunxiGpioConfigureAsOutput(IN UINT32 Pin) {
  return mSunxiGpioProtocol->ConfigureAsOutput(mSunxiGpioProtocol, Pin);
}

EFI_STATUS SunxiGpioConfigureAsInput(IN UINT32 Pin) {
  return mSunxiGpioProtocol->ConfigureAsInput(mSunxiGpioProtocol, Pin);
}

EFI_STATUS SunxiGpioSetLevel(IN UINT32 Pin, IN UINT8 Level) {
  return mSunxiGpioProtocol->SetLevel(mSunxiGpioProtocol, Pin, Level);
}

EFI_STATUS SunxiGpioGetLevel(IN UINT32 Pin, OUT UINT8 *OutLevel) {
  return mSunxiGpioProtocol->GetLevel(mSunxiGpioProtocol, Pin, OutLevel);
}

