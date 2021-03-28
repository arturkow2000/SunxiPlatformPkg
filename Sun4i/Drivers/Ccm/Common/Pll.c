/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (c) Artur Kowalski. <arturkow2000@gmail.com>
**/

#include "Driver.h"
#include "Pll.h"

CONST CCM_PLL_DESC gCcmPllList[] = {
  PLL(L"pll1", CcmPll1GetFrequency, NULL),
  PLL(L"pll6", CcmGetPll6GetFrequency, NULL),
  PLL(L"cpu", CcmGetCpuFrequency, NULL),
  PLL(L"axi", CcmGetAxiFrequency, NULL),
  PLL(L"ahb", CcmGetAhbFrequency, NULL),
  PLL(L"apb0", CcmGetApb0Frequency, NULL),
  PLL(L"apb1", CcmGetApb1Frequency, NULL),
  PLL_D(L"mmc0", CcmMmcGetFrequency, CcmMmcSetFrequency, 0),
  PLL_D(L"mmc1", CcmMmcGetFrequency, CcmMmcSetFrequency, 1),
  PLL_D(L"mmc2", CcmMmcGetFrequency, CcmMmcSetFrequency, 2),
};

CONST UINT32 gCcmPllListLength = ARRAY_SIZE(gCcmPllList);
