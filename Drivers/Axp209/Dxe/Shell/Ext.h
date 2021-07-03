#pragma once

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/HiiLib.h>
#include <Library/ShellLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>

#include <Protocol/Shell.h>
#include <Protocol/ShellDynamicCommand.h>

#include "../Driver.h"

extern EFI_HANDLE gAxp209DxeHiiHandle;
extern AXP209_DXE_DRIVER *gAxp209DxeDriver;

SHELL_STATUS
EFIAPI
Axp209DumpCommandHandler (
  IN EFI_SHELL_DYNAMIC_COMMAND_PROTOCOL  *This,
  IN EFI_SYSTEM_TABLE                    *SystemTable,
  IN EFI_SHELL_PARAMETERS_PROTOCOL       *ShellParameters,
  IN EFI_SHELL_PROTOCOL                  *Shell
  );

CHAR16*
EFIAPI
Axp209DumpCommandGethelp (
  IN EFI_SHELL_DYNAMIC_COMMAND_PROTOCOL  *This,
  IN CONST CHAR8                         *Language
  );
