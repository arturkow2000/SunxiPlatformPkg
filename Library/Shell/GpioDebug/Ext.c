/**
  (C) Copyright 2013 Hewlett-Packard Development Company, L.P.<BR>
  Copyright (c) 2009 - 2018, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2020, Artur Kowalski.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Ext.h"

EFI_HII_HANDLE gSunxiGpioDebugHiiHandle = NULL;

STATIC CONST CHAR16 *EFIAPI SunxiGpioGetManFileName(VOID)
{
  // TODO: implement
  return L"no_such_file";
}

EFI_STATUS EFIAPI SunxiGpioDebugShellExtConstructor(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
  gSunxiGpioDebugHiiHandle = HiiAddPackages(&gSunxiGpioDebugHiiGuid, gImageHandle, SunxiGpioDebugShellExtStrings, NULL);
  if (gSunxiGpioDebugHiiHandle == NULL)
    return EFI_DEVICE_ERROR;

  ShellCommandRegisterCommandName(L"sunxi_gpio_dump", SunxiCommandRunGpioDump, SunxiGpioGetManFileName, 1, L"", FALSE, gSunxiGpioDebugHiiHandle, (EFI_STRING_ID)(PcdGet8(PcdShellSupportLevel) < 3 ? 0 : STRING_TOKEN(STR_HELP_DUMP_GPIO)));
  ShellCommandRegisterCommandName(L"sunxi_gpio_get_function_list", SunxiCommandRunGpioGetFunctionList, SunxiGpioGetManFileName, 1, L"", FALSE, gSunxiGpioDebugHiiHandle, (EFI_STRING_ID)(PcdGet8(PcdShellSupportLevel) < 3 ? 0 : STRING_TOKEN(STR_HELP_GPIO_GET_FUNC_LIST)));
  ShellCommandRegisterCommandName(L"sunxi_gpio_set_function", SunxiCommandRunGpioSetFunction, SunxiGpioGetManFileName, 1, L"", FALSE, gSunxiGpioDebugHiiHandle, (EFI_STRING_ID)(PcdGet8(PcdShellSupportLevel) < 3 ? 0 : STRING_TOKEN(STR_HELP_GPIO_SET_FUNC)));

  return EFI_SUCCESS;
}

EFI_STATUS EFIAPI SunxiGpioDebugShellExtDestructor(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
  if (gSunxiGpioDebugHiiHandle)
    HiiRemovePackages(gSunxiGpioDebugHiiHandle);

  return EFI_SUCCESS;
}
