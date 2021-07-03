#include <PiPei.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/SunxiGpioLib.h>

#include <Ppi/SunxiGpio.h>

// TODO: improve error handling

STATIC
EFI_STATUS
EFIAPI
GetPin(
  IN  SUNXI_GPIO_PPI *This,
  IN  CONST CHAR16 *Name,
  OUT SUNXI_GPIO_PIN *OutPin
  )
{
  SUNXI_GPIO_PIN Pin;

  if (Name == NULL || OutPin == NULL)
    return EFI_INVALID_PARAMETER;

  Pin = SunxiGpioNameToPin(Name);
  if (Pin == SUNXI_GPIO_PIN_INVALID)
    return EFI_NOT_FOUND;
  
  *OutPin = Pin;

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
GetFunction(
  IN  SUNXI_GPIO_PPI *This,
  IN  SUNXI_GPIO_PIN Pin,
  OUT CONST CHAR16 **OutFunction
  )
{
  if (OutFunction == NULL)
    return EFI_INVALID_PARAMETER;

  *OutFunction = SunxiGpioMuxGetFunction(Pin);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
SetFunction(
  IN SUNXI_GPIO_PPI *This,
  IN SUNXI_GPIO_PIN Pin,
  IN CONST CHAR16 *Function
  )
{
  if (Function == NULL)
    return EFI_INVALID_PARAMETER;

  if (!SunxiGpioMuxSetFunction(Pin, Function))
    return EFI_UNSUPPORTED;

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
GetPullMode(
  IN  SUNXI_GPIO_PPI *This,
  IN  SUNXI_GPIO_PIN Pin,
  OUT SUNXI_GPIO_PULL_MODE *OutPullMode
  )
{
  if (OutPullMode == NULL)
    return EFI_INVALID_PARAMETER;

  *OutPullMode = SunxiGpioPullGet(Pin);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
SetPullMode(
  IN SUNXI_GPIO_PPI *This,
  IN SUNXI_GPIO_PIN Pin,
  IN SUNXI_GPIO_PULL_MODE Mode
  )
{
  SunxiGpioPullSet(Pin, Mode);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
GetDriveStrength(
  IN  SUNXI_GPIO_PPI *This,
  IN  SUNXI_GPIO_PIN Pin,
  OUT UINT32 *OutDriveStrength
  )
{
  if (OutDriveStrength == NULL)
    return EFI_INVALID_PARAMETER;

  *OutDriveStrength = SunxiGpioGetDriveStrength(Pin);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
SetDriveStrength(
  IN SUNXI_GPIO_PPI *This,
  IN SUNXI_GPIO_PIN Pin,
  IN UINT32 DriveStrength
  )
{
  SunxiGpioSetDriveStrength(Pin, DriveStrength);

  return EFI_SUCCESS;
}

STATIC CONST SUNXI_GPIO_PPI mSunxiGpioPpi = {
  GetPin,
  GetFunction,
  SetFunction,
  GetPullMode,
  SetPullMode,
  GetDriveStrength,
  SetDriveStrength,
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