/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (c) Artur Kowalski. <arturkow2000@gmail.com>
**/

#pragma once

#include <Uefi.h>

typedef struct _CCM_GATE_DESC CCM_GATE_DESC;
typedef struct _CCM_PLL_DESC CCM_PLL_DESC;

struct _CCM_GATE_DESC {
  CONST CHAR16 *Name;
  UINT16 Reg;
  UINT8 Bit;
};

#define GATE(_Name, _Reg, _Bit)          \
  {                                      \
    .Name = (_Name),                     \
    .Reg = (_Reg),                       \
    .Bit = (_Bit),                       \
  }

extern CONST CCM_GATE_DESC gCcmGateList[];
extern CONST UINT32 gCcmGateListLength;

EFI_STATUS CcmGateUnmask(IN UINT32 GateIndex);
EFI_STATUS CcmGateMask(IN UINT32 GateIndex);
EFI_STATUS CcmGateIsMasked(IN UINT32 GateIndex, OUT BOOLEAN *OutIsMasked);

typedef
EFI_STATUS
(EFIAPI *CCM_PLL_GET_FREQ)(
  IN  CONST CCM_PLL_DESC *Desc,
  OUT UINT32 *Hz
);

typedef
EFI_STATUS
(EFIAPI *CCM_PLL_SET_FREQ)(
   IN  CONST CCM_PLL_DESC *Desc,
   IN  UINT32 Hz
);

struct _CCM_PLL_DESC {
  CONST CHAR16 *Name;
  UINT32 Data;
  CCM_PLL_GET_FREQ GetFreq;
  CCM_PLL_SET_FREQ SetFreq;
};

#define PLL_D(_Name, _Get, _Set, _Data)      \
  {                                          \
    .Name = (_Name),                         \
    .GetFreq = (_Get),                       \
    .SetFreq = (_Set),                       \
    .Data = (_Data),                         \
  }

#define PLL(_Name, _Get, _Set)        \
  PLL_D(_Name, _Get, _Set, 0)

extern CONST CCM_PLL_DESC gCcmPllList[];
extern CONST UINT32 gCcmPllListLength;
