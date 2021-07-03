#include "Ext.h"

STATIC CONST EFI_SHELL_DYNAMIC_COMMAND_PROTOCOL mDumpCommand = {
    L"axp_dump",
    Axp209DumpCommandHandler,
    Axp209DumpCommandGethelp
};

extern EFI_GUID gAxp209ShellExtHiiGuid;
EFI_HANDLE gAxp209DxeHiiHandle = NULL;
STATIC EFI_HANDLE mHandle = NULL;

EFI_STATUS EFIAPI ExtendShell(VOID)
{
  gAxp209DxeHiiHandle = HiiAddPackages(
    &gAxp209ShellExtHiiGuid,
    gImageHandle,
    Axp209DxeStrings,
    NULL
  );
  if (!gAxp209DxeHiiHandle)
    return EFI_DEVICE_ERROR;

  return gBS->InstallMultipleProtocolInterfaces(
    &mHandle,
    &gEfiShellDynamicCommandProtocolGuid, &mDumpCommand,
    NULL
  );
}
