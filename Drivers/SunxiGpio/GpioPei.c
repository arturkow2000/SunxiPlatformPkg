#include <PiPei.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/SunxiGpioLib.h>

#include <Ppi/SunxiGpio.h>

STATIC
EFI_STATUS
EFIAPI
GetPin(
  IN  SUNXI_GPIO_PPI *This,
  IN  CONST CHAR16 *Name,
  OUT UINT32 *OutPin
  )
{
  return SunxiGpioGetPin(Name, OutPin);
}

STATIC
EFI_STATUS
EFIAPI
GetFunction(
  IN  SUNXI_GPIO_PPI *This,
  IN  UINT32 Pin,
  OUT CONST CHAR16 **OutFunction
  )
{
  return SunxiGpioGetFunction(Pin, OutFunction);
}

STATIC
EFI_STATUS
EFIAPI
SetFunction(
  IN SUNXI_GPIO_PPI *This,
  IN UINT32 Pin,
  IN CONST CHAR16 *Function
  )
{
  return SunxiGpioSetFunction(Pin, Function);
}

STATIC
EFI_STATUS
EFIAPI
GetPullMode(
  IN  SUNXI_GPIO_PPI *This,
  IN  UINT32 Pin,
  OUT UINT32 *OutPullMode
  )
{
  return SunxiGpioGetPullMode(Pin, OutPullMode);
}

STATIC
EFI_STATUS
EFIAPI
SetPullMode(
  IN SUNXI_GPIO_PPI *This,
  IN UINT32 Pin,
  IN UINT32 PullMode
  )
{
  return SunxiGpioSetPullMode(Pin, PullMode);
}

STATIC
EFI_STATUS
EFIAPI
GetDriveStrength(
  IN  SUNXI_GPIO_PPI *This,
  IN  UINT32 Pin,
  OUT UINT32 *OutDriveStrength
  )
{
  return SunxiGpioGetDriveStrength(Pin, OutDriveStrength);
}

STATIC
EFI_STATUS
EFIAPI
SetDriveStrength(
  IN SUNXI_GPIO_PPI *This,
  IN UINT32 Pin,
  IN UINT32 DriveStrength
  )
{
  return SunxiGpioSetDriveStrength(Pin, DriveStrength);
}

STATIC
EFI_STATUS
EFIAPI
ConfigureAsOutput(
  IN SUNXI_GPIO_PPI *This,
  IN UINT32 Pin
  )
{
  return SunxiGpioConfigureAsOutput(Pin);
}

STATIC
EFI_STATUS
EFIAPI
ConfigureAsInput(
  IN SUNXI_GPIO_PPI *This,
  IN UINT32 Pin
  )
{
  return SunxiGpioConfigureAsInput(Pin);
}

STATIC
EFI_STATUS
SetLevel(
  IN SUNXI_GPIO_PPI *This,
  IN UINT32 Pin,
  IN UINT8 Level
  )
{
  return SunxiGpioSetLevel(Pin, Level);
}

STATIC
EFI_STATUS
GetLevel(
  IN SUNXI_GPIO_PPI *This,
  IN UINT32 Pin,
  OUT UINT8 *OutLevel
  )
{
  return SunxiGpioGetLevel(Pin, OutLevel);
}

STATIC CONST SUNXI_GPIO_PPI mSunxiGpioPpi = {
  GetPin,
  GetFunction,
  SetFunction,
  GetPullMode,
  SetPullMode,
  GetDriveStrength,
  SetDriveStrength,
  ConfigureAsOutput,
  ConfigureAsInput,
  SetLevel,
  GetLevel,
};

STATIC CONST EFI_PEI_PPI_DESCRIPTOR mPpiList = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gSunxiGpioPpiGuid,
  (VOID*)&mSunxiGpioPpi
};

EFI_STATUS
EFIAPI
SunxiGpioInitialize(
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS Status;

  Status = PeiServicesInstallPpi(&mPpiList);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;
  
  return EFI_SUCCESS;
}
