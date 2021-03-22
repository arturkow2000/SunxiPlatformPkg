/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (c) Artur Kowalski. <arturkow2000@gmail.com>
**/

#pragma once

EFI_STATUS
EFIAPI
CcmGetPll6GetFrequency(
  IN  CONST CCM_PLL_DESC *Desc,
  OUT UINT32 *Hz
);

EFI_STATUS
EFIAPI
CcmMmcGetFrequency(
  IN  CONST CCM_PLL_DESC *Desc,
  OUT UINT32 *Hz
);

EFI_STATUS
EFIAPI
CcmMmcSetFrequency(
  IN  CONST CCM_PLL_DESC *Desc,
  IN  UINT32 Hz
);
