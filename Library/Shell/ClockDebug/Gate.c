/** @file
  (C) Copyright 2015 Hewlett-Packard Development Company, L.P.<BR>
  Copyright (c) 2011 - 2018, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2021, Artur Kowalski.

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "SunxiClockDebugCommandsLib.h"

VOID DumpAllGates(IN SUNXI_CCM_PROTOCOL *Ccm) {
  EFI_STATUS Status;
  INT32 i;
  UINT32 GateId;
  CONST CHAR16 *GateName;
  BOOLEAN IsMasked;

  for (i = 0; ; i++) {
    Status = Ccm->GetNextGate(Ccm, i, &GateId, &GateName);
    if (Status == EFI_SUCCESS) {
      Status = Ccm->GateIsMasked(Ccm, GateId, &IsMasked);
      if (EFI_ERROR(Status))
        ShellPrintEx(-1, -1, L"%H%s%N: %r\r\n", GateName, Status);
      else
        ShellPrintEx(-1, -1, L"%H%s%N: %s\r\n", GateName, IsMasked ? L"masked" : L"unmasked");
    }
    else if (Status == EFI_NOT_FOUND)
      return;
    else {
      ShellPrintEx(-1, -1, L"Gate dump failed\r\n");
      break;
    }
  }
}

SHELL_STATUS EFIAPI SunxiCommandRunGateGet(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS Status;
  LIST_ENTRY *Package;
  CHAR16 *ProblemParam;
  SHELL_STATUS ShellStatus;
  CONST CHAR16 *GateName;
  UINT32 GateId;
  SUNXI_CCM_PROTOCOL *CcmProto;
  BOOLEAN IsMasked;

  ShellStatus = SHELL_SUCCESS;

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
      ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GEN_PROBLEM), gSunxiClockDebugShellLevel1HiiHandle, L"sunxi_gate_get", ProblemParam);
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
      ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GEN_TOO_MANY), gSunxiClockDebugShellLevel1HiiHandle, L"sunxi_gate_get");
      ShellStatus = SHELL_INVALID_PARAMETER;
    }
    else
    {
      GateName = ShellCommandLineGetRawValue(Package, 1);
      if (GateName == NULL)
      {
        ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GEN_TOO_FEW), gSunxiClockDebugShellLevel1HiiHandle, L"sunxi_gate_get");
        ShellStatus = SHELL_INVALID_PARAMETER;
      }
      else
      {
        Status = gBS->LocateProtocol(&gSunxiCcmProtocolGuid, NULL, (VOID**)&CcmProto);
        if (EFI_ERROR(Status))
          ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_SUNXI_CCM_NOT_FOUND), gSunxiClockDebugShellLevel1HiiHandle, L"sunxi_gate_get");
        else {
          if (StrCmp(GateName, L"ALL") == 0)
            DumpAllGates(CcmProto);
          else {
            Status = CcmProto->GetGate(CcmProto, GateName, &GateId);
            if (EFI_ERROR(Status))
              ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GATE_NOT_FOUND), gSunxiClockDebugShellLevel1HiiHandle);
            else {
              Status = CcmProto->GateIsMasked(CcmProto, GateId, &IsMasked);
              if (EFI_ERROR(Status))
                ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_FAILED_TO_GET_GATE_STATE), gSunxiClockDebugShellLevel1HiiHandle, L"sunxi_gate_get", Status);
              else
                ShellPrintEx(-1, -1, L"%H%s%N: %s\r\n", GateName, IsMasked ? L"masked" : L"unmasked");
            }
          }
        }
      }
    }
    ShellCommandLineFreeVarList(Package);
  }

  return ShellStatus;
}

#define OP_INVALID 0
#define OP_MASK 1
#define OP_UNMASK 2

SHELL_STATUS EFIAPI SunxiCommandRunGateSet(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS Status;
  LIST_ENTRY *Package;
  CHAR16 *ProblemParam;
  SHELL_STATUS ShellStatus;
  CONST CHAR16 *GateName;
  CONST CHAR16 *OperationStr;
  UINT32 GateId;
  UINT32 Operation;
  SUNXI_CCM_PROTOCOL *CcmProto;

  ShellStatus = SHELL_SUCCESS;

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
      ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GEN_PROBLEM), gSunxiClockDebugShellLevel1HiiHandle, L"sunxi_gate_set", ProblemParam);
      FreePool(ProblemParam);
      ShellStatus = SHELL_INVALID_PARAMETER;
    }
    else
    {
      ASSERT(0);
    }
  }
  else
  {
    if (ShellCommandLineGetRawValue(Package, 3) != NULL)
    {
      ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GEN_TOO_MANY), gSunxiClockDebugShellLevel1HiiHandle, L"sunxi_gate_set");
      ShellStatus = SHELL_INVALID_PARAMETER;
    }
    else
    {
      GateName = ShellCommandLineGetRawValue(Package, 1);
      OperationStr = ShellCommandLineGetRawValue(Package, 2);
      if (GateName == NULL || OperationStr == NULL)
      {
        ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GEN_TOO_FEW), gSunxiClockDebugShellLevel1HiiHandle, L"sunxi_gate_set");
        ShellStatus = SHELL_INVALID_PARAMETER;
      }
      else
      {
        if (StrCmp(OperationStr, L"unmask") == 0 || StrCmp(OperationStr, L"1") == 0)
        {
          Operation = OP_UNMASK;
        }
        else if (StrCmp(OperationStr, L"mask") == 0 || StrCmp(OperationStr, L"0") == 0)
        {
          Operation = OP_MASK;
        }
        else
        {
          Operation = OP_INVALID;
        }

        if (Operation == OP_INVALID)
        {
          ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_EXPECTED_ZERO_ONE_DISABLE_ENABLE), gSunxiClockDebugShellLevel1HiiHandle, L"sunxi_gate_set");
          ShellStatus = SHELL_INVALID_PARAMETER;
        }
        else
        {
          Status = gBS->LocateProtocol(&gSunxiCcmProtocolGuid, NULL, (VOID**)&CcmProto);
          if (EFI_ERROR(Status))
            ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_SUNXI_CCM_NOT_FOUND), gSunxiClockDebugShellLevel1HiiHandle, L"sunxi_gate_set");
          else {
            Status = CcmProto->GetGate(CcmProto, GateName, &GateId);
            if (EFI_ERROR(Status))
              ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GATE_NOT_FOUND), gSunxiClockDebugShellLevel1HiiHandle, L"sunxi_gate_set");
            else {
              if (Operation == OP_UNMASK)
                Status = CcmProto->GateUnmask(CcmProto, GateId);
              else if (Operation == OP_MASK)
                Status = CcmProto->GateMask(CcmProto, GateId);
              else
                ASSERT(0);
              
              if (EFI_ERROR(Status))
                ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_FAILED_TO_CHANGE_GATE_STATE), gSunxiClockDebugShellLevel1HiiHandle, L"sunxi_gate_set", Status);
            }
          }
        }
      }
    }
    ShellCommandLineFreeVarList(Package);
  }

  return ShellStatus;
}