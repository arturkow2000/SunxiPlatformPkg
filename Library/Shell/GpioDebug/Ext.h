#pragma once

#include <Uefi.h>

#include <Protocol/Shell.h>
#include <Protocol/ShellParameters.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/UnicodeCollation.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/ShellCommandLib.h>
#include <Library/ShellLib.h>
#include <Library/SortLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HiiLib.h>
#include <Library/FileHandleLib.h>

#include <Library/SunxiGpioLib.h>

extern EFI_GUID gSunxiGpioDebugHiiGuid;
extern EFI_HII_HANDLE gSunxiGpioDebugHiiHandle;

SHELL_STATUS EFIAPI SunxiCommandRunGpioDump(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable);

SHELL_STATUS EFIAPI SunxiCommandRunGpioGetFunctionList(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable);

SHELL_STATUS EFIAPI SunxiCommandRunGpioSetFunction(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable);
