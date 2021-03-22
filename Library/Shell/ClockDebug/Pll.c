/** @file
  (C) Copyright 2015 Hewlett-Packard Development Company, L.P.<BR>
  Copyright (c) 2011 - 2018, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2021, Artur Kowalski.

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "SunxiClockDebugCommandsLib.h"

VOID DumpAllPlls(IN SUNXI_CCM_PROTOCOL *Ccm) {
  EFI_STATUS Status;
  INT32 i;
  UINT32 PllId;
  CONST CHAR16 *PllName;
  UINT32 Freq;

  for (i = 0; ; i++) {
    Status = Ccm->GetNextPll(Ccm, i, &PllId, &PllName);
    if (Status == EFI_SUCCESS) {
      Status = Ccm->PllGetFreq(Ccm, PllId, &Freq);
      if (EFI_ERROR(Status))
        ShellPrintEx(-1, -1, L"%H%s%N: %r\r\n", PllName, Status);
      else
        ShellPrintEx(-1, -1, L"%H%s%N: %u Hz\r\n", PllName, Freq);
    }
    else if (Status == EFI_NOT_FOUND)
      return;
    else {
      ShellPrintEx(-1, -1, L"PLL dump failed\r\n");
      break;
    }
  }
}

SHELL_STATUS EFIAPI SunxiCommandRunPllDump(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;
  LIST_ENTRY *Package;
  CHAR16 *ProblemParam;
  SHELL_STATUS ShellStatus;

  CONST CHAR16 *PllName;
  UINT32 PllId;
  SUNXI_CCM_PROTOCOL *CcmProto;
  UINT32 Freq;

  //
  // initialize the shell lib (we must be in non-auto-init...)
  //
  Status = ShellInitialize();
  ASSERT_EFI_ERROR(Status);

  Status = CommandInit();
  ASSERT_EFI_ERROR(Status);

  Status = ShellCommandLineParse(EmptyParamList, &Package, &ProblemParam, TRUE);
  if (EFI_ERROR(Status))
  {
    if (Status == EFI_VOLUME_CORRUPTED && ProblemParam != NULL)
    {
      ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GEN_PROBLEM), gSunxiClockDebugShellLevel1HiiHandle, L"sunxi_pll_dump", ProblemParam);
      FreePool(ProblemParam);
      ShellStatus = SHELL_INVALID_PARAMETER;
    }
    else
    {
      ASSERT(FALSE);
    }
  }
  else
  {
    if (ShellCommandLineGetRawValue(Package, 2) != NULL)
    {
      ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GEN_TOO_MANY), gSunxiClockDebugShellLevel1HiiHandle, L"sunxi_pll_dump");
      ShellStatus = SHELL_INVALID_PARAMETER;
    } else {
      PllName = ShellCommandLineGetRawValue(Package, 1);
      if (PllName == NULL)
      {
        ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GEN_TOO_FEW), gSunxiClockDebugShellLevel1HiiHandle, L"sunxi_pll_dump");
        ShellStatus = SHELL_INVALID_PARAMETER;
      } else {
        Status = gBS->LocateProtocol(&gSunxiCcmProtocolGuid, NULL, (VOID**)&CcmProto);
        if (EFI_ERROR(Status))
          ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_SUNXI_CCM_NOT_FOUND), gSunxiClockDebugShellLevel1HiiHandle, L"sunxi_pll_dump");
        else {
          if (StrCmp(PllName, L"ALL") == 0)
            DumpAllPlls(CcmProto);
          else {
            Status = CcmProto->GetPll(CcmProto, PllName, &PllId);
            if (EFI_ERROR(Status))
              ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_PLL_NOT_FOUND), gSunxiClockDebugShellLevel1HiiHandle);
            else {
              Status = CcmProto->PllGetFreq(CcmProto, PllId, &Freq);
              if (EFI_ERROR(Status))
                ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_FAILED_TO_GET_PLL_FREQ), gSunxiClockDebugShellLevel1HiiHandle, L"sunxi_pll_dump", Status);
              else
                ShellPrintEx(-1, -1, L"%H%s%N: %u Hz\r\n", PllName, Freq);
            }
          }
        }
      }
    }
    ShellCommandLineFreeVarList(Package);
  }

  return ShellStatus;
}