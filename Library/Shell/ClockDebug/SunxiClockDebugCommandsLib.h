/** @file
  (C) Copyright 2015 Hewlett-Packard Development Company, L.P.<BR>
  Copyright (c) 2011 - 2018, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2021, Artur Kowalski.

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

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

#include <Protocol/SunxiCcm.h>

extern EFI_GUID gSunxiClockDebugHiiGuid;
extern EFI_HII_HANDLE gSunxiClockDebugShellLevel1HiiHandle;

SHELL_STATUS EFIAPI SunxiCommandRunGateGet(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable);

SHELL_STATUS EFIAPI SunxiCommandRunGateSet(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable);

SHELL_STATUS EFIAPI SunxiCommandRunPllDump(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
);