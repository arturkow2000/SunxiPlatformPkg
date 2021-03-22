/**
  (C) Copyright 2013 Hewlett-Packard Development Company, L.P.<BR>
  Copyright (c) 2009 - 2018, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2020, Artur Kowalski.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SunxiClockDebugCommandsLib.h"

STATIC CONST CHAR16 mFileName[] = L"ShellCommands";
EFI_HII_HANDLE gSunxiClockDebugShellLevel1HiiHandle = NULL;

/**
  Return the help text filename.  Only used if no HII information found.

  @retval the filename.
**/
CONST CHAR16 *EFIAPI
    SunxiClockDebugShellCommandGetManFileNameLevel1(VOID)
{
  return (mFileName);
}

EFI_STATUS EFIAPI SunxiClockDebugCommandsLibConstructor(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
  gSunxiClockDebugShellLevel1HiiHandle = HiiAddPackages(&gSunxiClockDebugHiiGuid, gImageHandle, SunxiClockDebugCommandsLibStrings, NULL);
  if (gSunxiClockDebugShellLevel1HiiHandle == NULL)
  {
    return (EFI_DEVICE_ERROR);
  }

  //
  // install our shell command handlers that are always installed
  //
  ShellCommandRegisterCommandName(L"sunxi_gate_get", SunxiCommandRunGateGet, SunxiClockDebugShellCommandGetManFileNameLevel1, 1, L"", FALSE, gSunxiClockDebugShellLevel1HiiHandle, (EFI_STRING_ID)(PcdGet8(PcdShellSupportLevel) < 3 ? 0 : STRING_TOKEN(STR_GET_HELP_CLOCK_GET)));
  ShellCommandRegisterCommandName(L"sunxi_gate_set", SunxiCommandRunGateSet, SunxiClockDebugShellCommandGetManFileNameLevel1, 1, L"", FALSE, gSunxiClockDebugShellLevel1HiiHandle, (EFI_STRING_ID)(PcdGet8(PcdShellSupportLevel) < 3 ? 0 : STRING_TOKEN(STR_GET_HELP_CLOCK_SET)));
  ShellCommandRegisterCommandName(L"sunxi_pll_dump", SunxiCommandRunPllDump, SunxiClockDebugShellCommandGetManFileNameLevel1, 1, L"", FALSE, gSunxiClockDebugShellLevel1HiiHandle, (EFI_STRING_ID)(PcdGet8(PcdShellSupportLevel) < 3 ? 0 : STRING_TOKEN(STR_GET_HELP_PLL_DUMP)));

  return (EFI_SUCCESS);
}

EFI_STATUS EFIAPI SunxiClockDebugCommandsLibDestructor(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
  if (gSunxiClockDebugShellLevel1HiiHandle != NULL)
  {
    HiiRemovePackages(gSunxiClockDebugShellLevel1HiiHandle);
  }
  return (EFI_SUCCESS);
}