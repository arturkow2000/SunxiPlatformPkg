#include <PiPei.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Pi/PiI2c.h>
#include <Ppi/I2cEnumerate.h>

#include "../Common/Driver.h"

STATIC
EFI_STATUS
EFIAPI
Enumerate(
  IN CONST I2C_ENUMERATE_PPI          *This,
  IN UINT32                           Controller,
  IN OUT CONST EFI_I2C_DEVICE         **EfiI2cDevice
  )
{
  I2C_DEVICE *I2cDevice;

  if (EfiI2cDevice == NULL) {
    ASSERT(0);
    return EFI_INVALID_PARAMETER;
  }

  if (*EfiI2cDevice == NULL) {
    if (Controller >= gI2cDeviceListHeadArrayLength)
      return EFI_NOT_FOUND;

    *EfiI2cDevice = &gI2cDeviceListHeadArray[Controller]->Device;
    return EFI_SUCCESS;
  } else {
    I2cDevice = EFI_I2C_DEVICE_TO_I2C_DEVICE(*EfiI2cDevice);
    if (I2cDevice->Next != NULL) {
      *EfiI2cDevice = (CONST EFI_I2C_DEVICE*)&I2cDevice->Next->Device;
      return EFI_SUCCESS;
    } else
      return EFI_NOT_FOUND;
  }
}

STATIC CONST I2C_ENUMERATE_PPI mEnumeratePpi = {
  Enumerate
};

STATIC CONST EFI_PEI_PPI_DESCRIPTOR mPpiDescriptor = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gI2cEnumeratePpiGuid,
  (VOID*)&mEnumeratePpi
};

EFI_STATUS
EFIAPI
I2cEnumeratorInitialize(
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS Status;

  Status = PeiServicesInstallPpi(&mPpiDescriptor);
  ASSERT_EFI_ERROR(Status);

  return Status;
}
