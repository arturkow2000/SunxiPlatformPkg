/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (c) Artur Kowalski. <arturkow2000@gmail.com>
**/

#pragma once

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/DevicePath.h>
#include <Protocol/ComponentName.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/SunxiCcm.h>

extern EFI_COMPONENT_NAME_PROTOCOL gComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL gComponentName2;
extern SUNXI_CCM_PROTOCOL gSunxiCcmProtocol;
