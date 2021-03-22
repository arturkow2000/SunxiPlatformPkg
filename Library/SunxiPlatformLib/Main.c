#include <Uefi.h>
#include <Library/ArmPlatformLib.h>

VOID
ArmPlatformGetPlatformPpiList (
  OUT UINTN                   *PpiListSize,
  OUT EFI_PEI_PPI_DESCRIPTOR  **PpiList
  )
{
  *PpiListSize = 0;
  *PpiList = NULL;
}

UINTN
ArmPlatformGetCorePosition (
  IN UINTN MpId
  )
{
  return 1;
}

RETURN_STATUS
ArmPlatformInitialize (
  IN  UINTN                     MpId
  )
{
  return RETURN_SUCCESS;
}

EFI_BOOT_MODE
ArmPlatformGetBootMode (
  VOID
  )
{
  return BOOT_WITH_FULL_CONFIGURATION;
}