/** @file
  (C) Copyright 2015 Hewlett-Packard Development Company, L.P.<BR>
  Copyright (c) 2011 - 2018, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2021, Artur Kowalski.

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "Ext.h"

STATIC CONST CHAR16 *GpioPullToString(IN UINT32 Value)
{
  switch (Value)
  {
  case SUNXI_GPIO_PULL_DISABLE:
    return L"OFF";
  case SUNXI_GPIO_PULL_UP:
    return L"UP";
  case SUNXI_GPIO_PULL_DOWN:
    return L"DOWN";
  default:
    break;
  }

  return L"UNK";
}

STATIC VOID DumpPin(IN CONST CHAR16 *Name, IN SUNXI_GPIO_PIN Pin)
{
  CONST CHAR16 *Function;
  CONST CHAR16 *Pull;
  UINT32 DriveStrength;
  UINT32 Value;

  if (Pin != SUNXI_GPIO_PIN_INVALID)
  {
    Function = SunxiGpioMuxGetFunction(Pin);
    Pull = GpioPullToString(SunxiGpioPullGet(Pin));
    DriveStrength = SunxiGpioGetDriveStrength(Pin);
    Value = SunxiGpioRead(Pin);

    ShellPrintEx(-1, -1, L"%H%s%N: %10s %8s %2umA %u\r\n", Name, Function ? Function : L"UNKNOWN", Pull, DriveStrength, Value);
  }
  else
    ASSERT(0);
}

SHELL_STATUS EFIAPI SunxiCommandRunGpioDump(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS Status;
  LIST_ENTRY *Package;
  CHAR16 *ProblemParam;
  SHELL_STATUS ShellStatus;

  CONST CHAR16 *GpioName;
  SUNXI_GPIO_PIN Pin;
  UINTN NumPins;
  UINTN i;
  CHAR16 Buffer[SUNXI_GPIO_NAME_MAX_LENGTH + 1];
  BOOLEAN BStatus;

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
      ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GEN_PROBLEM), gSunxiGpioDebugHiiHandle, L"sunxi_gpio_dump", ProblemParam);
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
      ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GEN_TOO_MANY), gSunxiGpioDebugHiiHandle, L"sunxi_gpio_dump");
      ShellStatus = SHELL_INVALID_PARAMETER;
    }
    else
    {
      GpioName = ShellCommandLineGetRawValue(Package, 1);
      if (GpioName)
      {
        if (StrCmp(GpioName, L"ALL") == 0)
        {
          NumPins = SunxiGpioGetPinCount();
          for (i = 0; i < NumPins; i++)
          {
            if (SunxiGpioIsPinValid((SUNXI_GPIO_PIN)i))
            {
              BStatus = SunxiGpioGetPinName((SUNXI_GPIO_PIN)i, Buffer, ARRAY_SIZE(Buffer), NULL);
              ASSERT(BStatus);
              DumpPin(Buffer, (SUNXI_GPIO_PIN)i);
            }
          }
        }
        else
        {
          Pin = SunxiGpioNameToPin(GpioName);
          if (Pin != SUNXI_GPIO_PIN_INVALID)
            DumpPin(GpioName, Pin);
          else
            ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_NOT_FOUND), gSunxiGpioDebugHiiHandle, L"sunxi_gpio_dump", GpioName);
        }
      }
      else
      {
        ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GEN_TOO_FEW), gSunxiGpioDebugHiiHandle, L"sunxi_gpio_dump");
        ShellStatus = SHELL_INVALID_PARAMETER;
      }
    }
    ShellCommandLineFreeVarList(Package);
  }

  return ShellStatus;
}
