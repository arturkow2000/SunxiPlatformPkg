#include "Driver.h"

EFI_STATUS
EFIAPI
OtgPeiInitialize(
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS Status;
  USB_DRIVER Driver;
  ZeroMem(&Driver, sizeof(USB_DRIVER));
  Driver.Base = 0x01c13000;
  Driver.Enabled = FALSE;

  Status = UsbInit(&Driver);
  ASSERT_EFI_ERROR(Status);

  ASSERT(0);
  return EFI_SUCCESS;
}
